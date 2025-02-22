#include "board.h"
#include "ctl_api.h"
#include "clockDriverRT.h"
#include "ring_buffer.h"
#include "debugio.h"
#include "clockData.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define BOARDNUMBER 2
 
 
#define TEST_CCAN_BAUD_RATE 500000
#define CAN_ERROR_NONE 0x00000000UL 
#define CAN_ERROR_PASS 0x00000001UL 
#define CAN_ERROR_WARN 0x00000002UL 
#define CAN_ERROR_BOFF 0x00000004UL 
#define CAN_ERROR_STUF 0x00000008UL 
#define CAN_ERROR_FORM 0x00000010UL 
#define CAN_ERROR_ACK 0x00000020UL 
#define CAN_ERROR_BIT1 0x00000040UL 
#define CAN_ERROR_BIT0 0x00000080UL 
#define CAN_ERROR_CRC 0x00000100UL

#define CAN_RX 1<<0
#define CAN_TX 1<<1

static CCAN_MSG_OBJ_T msg_obj;
static CTL_EVENT_SET_t canEvent;

/* Define ringbuffer constants and variables */
#define SIZEOFRXBUFF 200
static RINGBUFF_T rxRing;
static CCAN_MSG_OBJ_T rxBuff[SIZEOFRXBUFF];

static uint8_t SLAVE0;
static uint8_t SLAVE1;
static uint8_t SLAVE2;
static uint8_t SLAVE3;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
 
 

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void baudrateCalculate(uint32_t baud_rate, uint32_t *can_api_timing_cfg)
{
    uint32_t pClk, div, quanta, segs, seg1, seg2, clk_per_bit, can_sjw;
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
    pClk = Chip_Clock_GetMainClockRate();

    clk_per_bit = pClk / baud_rate;

    for (div = 0; div <= 15; div++) 
    {
        for (quanta = 1; quanta <= 32; quanta++) 
        {
            for (segs = 3; segs <= 17; segs++) 
            {
                if (clk_per_bit == (segs * quanta * (div + 1))) 
                {
                    segs -= 3;
                    seg1 = segs / 2;
                    seg2 = segs - seg1;
                    can_sjw = seg1 > 3 ? 3 : seg1;
                    can_api_timing_cfg[0] = div;
                    can_api_timing_cfg[1] =
                        ((quanta - 1) & 0x3F) | (can_sjw & 0x03) << 6 | (seg1 & 0x0F) << 8 | (seg2 & 0x07) << 12;
                    return;
                }
            }
        }
    }
}

/*	CAN receive callback */
/*	Function is executed by the Callback handler after
    a CAN message has been received */
void CAN_rx(uint8_t msg_obj_num) 
{   
    ctl_enter_isr(); 
       
    ///* Determine which CAN message has been received */
    msg_obj.msgobj = msg_obj_num;

    /* Now load up the msg_obj structure with the CAN message */
    LPC_CCAN_API->can_receive(&msg_obj);

    /* Check to see if data is relevant to this board */
    if(msg_obj.mode_id == 0x200)
    { 
        if(msg_obj.data[0] == SLAVE0 ||  msg_obj.data[0] == SLAVE1 || msg_obj.data[0] == SLAVE2 || msg_obj.data[0] == SLAVE3)
        {
             /* Load into ringbuffer */
            if(!RingBuffer_Insert(&rxRing, &msg_obj))
            {
               //asm("BKPT");
            } 
       }
    }
    else
    {
        /* Load into ringbuffer */
        RingBuffer_Insert(&rxRing, &msg_obj);
    }

    
    /* Set event flag for processing in main loop */
    ctl_events_set_clear(&canEvent, CAN_RX, 0);
    ctl_exit_isr(); 
}

/*	CAN transmit callback */
/*	Function is executed by the Callback handler after
    a CAN message has been transmitted */
void CAN_tx(uint8_t msg_obj_num) 
{
    //ctl_enter_isr();
    //ctl_exit_isr();
}

/*	CAN error callback */
/*	Function is executed by the Callback handler after
    an error has occured on the CAN bus */
void CAN_error(uint32_t error_info) 
{
    //if (error_info & CAN_ERROR_BOFF)
    //    reset_can = TRUE;
    
    asm("BKPT");
    
    //return;

}

/**
 * @brief	CCAN Interrupt Handler
 * @return	Nothing
 * @note	The CCAN interrupt handler must be provided by the user application.
 *	It's function is to call the isr() API located in the ROM
 */
void C_CAN_IRQHandler(void)
{
    //ctl_enter_isr(); 
    LPC_CCAN_API->isr();
    //ctl_exit_isr();
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main routine for CCAN_ROM example
 * @return	Nothing
 */
int can_init(void)
{
	uint32_t CanApiClkInitTable[2];
	/* Publish CAN Callback Functions */
	CCAN_CALLBACKS_T callbacks = {
		CAN_rx,
		CAN_tx,
		CAN_error,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	};

	baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);

	LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);
	/* Configure the CAN callback functions */
	LPC_CCAN_API->config_calb(&callbacks);
	/* Enable the CAN Interrupt */
	NVIC_EnableIRQ(CAN_IRQn);
        
        
	/* Send a simple one time CAN message */
	//msg_obj.msgobj  = 0;
	//msg_obj.mode_id = 0x100;
	//msg_obj.mask    = 0x0;
	//msg_obj.dlc     = 4;
	//msg_obj.data[0] = 'T';	// 0x54
	//msg_obj.data[1] = 'E';	// 0x45
	//msg_obj.data[2] = 'S';	// 0x53
	//msg_obj.data[3] = 'T';	// 0x54
	//LPC_CCAN_API->can_transmit(&msg_obj);

	/* Configure message object 1 to receive all 11-bit messages 0x400-0x4FF */
	msg_obj.msgobj = 1;
	msg_obj.mode_id = 0x200;
	msg_obj.mask = 0x3F0;
	LPC_CCAN_API->config_rxmsgobj(&msg_obj);



//	while (1) {
//		//msg_obj.msgobj = 1;
//		//msg_obj.mode_id = 0x200;
//		//msg_obj.mask = 0x700;
//		//LPC_CCAN_API->config_rxmsgobj(&msg_obj);
////
////		msg_obj.msgobj  = 1;
////		msg_obj.mode_id = 0x100;
////		msg_obj.mask    = 0x0;
////		msg_obj.dlc     = 4;
////		LPC_CCAN_API->can_transmit(&msg_obj);
//                //test = (float)rxData;

//		//i++;
//		__WFI();	/* Go to Sleep */
//	}
}


void comms_func(void *p)
{  
    unsigned int v=0;
    CCAN_MSG_OBJ_T canMSG;
    
    int testBuff = 0;
    
    // Determine clock numbers
    SLAVE0 = get_clock_numbers(BOARDNUMBER, 0);
    SLAVE1 = get_clock_numbers(BOARDNUMBER, 1);
    SLAVE2 = get_clock_numbers(BOARDNUMBER, 2);
    SLAVE3 = get_clock_numbers(BOARDNUMBER, 3);
    
    // Initialize ring buffer
    RingBuffer_Init(&rxRing, &rxBuff, sizeof(CCAN_MSG_OBJ_T), SIZEOFRXBUFF);
    
    // Initialise CAN driver
    can_init();
    
     // Initialise CAN event
    ctl_events_init(&canEvent, 0);    
    
    while (1)
    {      
        // Wait for RX from CAN bus
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &canEvent, CAN_RX, CTL_TIMEOUT_NONE, 0);    

        Board_LED_Toggle(0);  
        
        // Disbale CAN IRQ, receive from buffer, reenable
        NVIC_DisableIRQ(CAN_IRQn);
        RingBuffer_Pop(&rxRing, &canMSG); 
        NVIC_EnableIRQ(CAN_IRQn);
          
        if(canMSG.data[0] == SLAVE0 && canMSG.mode_id == 0x200)
        {
            canMSG.data[0] = 0;
        }
        if(canMSG.data[0] == SLAVE1 && canMSG.mode_id == 0x200)
        {
            canMSG.data[0] = 1;
        }
        if(canMSG.data[0] == SLAVE2 && canMSG.mode_id == 0x200)
        {
            canMSG.data[0] = 2;
        }
        if(canMSG.data[0] == SLAVE3 && canMSG.mode_id == 0x200)
        {
            canMSG.data[0] = 3;
        }
    
        // Send commands/position to clocks
        update_from_CAN(&canMSG);
        
        // Clear event flag if buffer empty
        if(RingBuffer_IsEmpty(&rxRing))
        { 
            ctl_events_set_clear(&canEvent, 0, CAN_RX);
        }
        
        //debug_printf("%d\n", canMSG.data[0]);   
        
        v++;
    }  
}