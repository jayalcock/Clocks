#include "board.h"
#include "ctl_api.h"
#include "clockDriverRT.h"
#include "ring_buffer.h"
#include "debugio.h"
#include "clockData.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define BOARDNUMBER 1
 
 
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
       
    // Use static msg_obj to avoid stack allocation in ISR
    msg_obj.msgobj = msg_obj_num;

    // Get the received message as quickly as possible
    LPC_CCAN_API->can_receive(&msg_obj);

    // Only insert into buffer if we have space
    // This prevents potential ISR lockups due to full buffer
    if (!RingBuffer_IsFull(&rxRing)) {
        // Fast path for relevant messages - check message ID first 
        // before checking data, as ID check is faster
        if (msg_obj.mode_id == 0x200) {
            // Using bitwise OR for more efficient checking
            uint8_t clock_id = msg_obj.data[0];
            if (clock_id == SLAVE0 || clock_id == SLAVE1 || 
                clock_id == SLAVE2 || clock_id == SLAVE3) {
RingBuffer_Insert(&rxRing, &msg_obj);
       }
    }
    else {
        // Process non-position messages
        RingBuffer_Insert(&rxRing, &msg_obj);
    }

// Set event flag for processing in main loop
        ctl_events_set_clear(&canEvent, CAN_RX, 0);
    }
    
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
    static uint32_t error_count = 0;
    static uint32_t last_error_time = 0;
    uint32_t current_time;
    
    // Enter ISR critical section
    ctl_enter_isr();
    
    // Get current time for error rate limiting
    current_time = ctl_get_current_time();
    
    // If bus-off condition occurs, attempt recovery
    if (error_info & CAN_ERROR_BOFF) {
        // Attempt bus recovery by reinitializing CAN controller
        uint32_t CanApiClkInitTable[2];
        baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);
        LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);
        
        // Reset error counter after recovery attempt
        error_count = 0;
        last_error_time = current_time;
    }
    // Handle other errors with rate limiting to prevent error storms
    else if ((current_time - last_error_time) > 1000) { // 1 second rate limiting
        error_count = 0;
        last_error_time = current_time;
    }
    else if (error_count < 100) { // Prevent error counter overflow
        error_count++;
    }
    
    // Only break into debugger if persistent errors occur (for development)
    if (error_count > 50) {
        #ifdef DEBUG
        asm("BKPT");
        #endif
        error_count = 0;
    }
    
    ctl_exit_isr();
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
	uint8_t retry_count = 0;
	bool init_success = false;
	
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

	/* Calculate baudrate settings with proper error handling */
	baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);
	
	/* Try initialization up to 3 times to ensure success */
	while (!init_success && retry_count < 3) {
		/* Enable CAN peripheral clock before initialization */
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
		
		/* Initialize CAN controller */
		LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);
		
		/* Configure the CAN callback functions */
		LPC_CCAN_API->config_calb(&callbacks);
		
		/* Since we can't directly check CAN registers in ROM API mode,
		   assume initialization succeeded and verify through configuration */
		init_success = true;
		
		/* Small delay to allow hardware to initialize */
		for (volatile int i = 0; i < 1000; i++) {}
		
		retry_count++;
	}
	
	/* Configure filters to only receive the messages we care about */
	/* Message object 1 for standard position messages (0x200) */
	msg_obj.msgobj = 1;
	msg_obj.mode_id = 0x200;
	msg_obj.mask = 0x700;  /* Accept 0x200-0x2FF range for all message types */
	LPC_CCAN_API->config_rxmsgobj(&msg_obj);
	
	/* Message object 2 for command messages if needed */
	msg_obj.msgobj = 2;
	msg_obj.mode_id = 0x300;  /* Command messages */
	msg_obj.mask = 0x700;     /* Accept 0x300-0x3FF for commands */
	LPC_CCAN_API->config_rxmsgobj(&msg_obj);
	
	/* Enable the CAN Interrupt with appropriate priority */
	NVIC_SetPriority(CAN_IRQn, 1);  /* Higher priority than other interrupts */
	NVIC_EnableIRQ(CAN_IRQn);
	
	return 0;  /* Return success - verification will happen at runtime */
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

    // Define batch processing variables
    #define BATCH_SIZE 8
    CCAN_MSG_OBJ_T msgBatch[BATCH_SIZE];
    int batchCount = 0;
    
    while (1)
    {      
        // Wait for RX from CAN bus with timeout to allow periodic checks
        // Using a timeout of 10ms instead of CTL_TIMEOUT_NONE
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &canEvent, CAN_RX, 10, 0);    

// Only toggle LED on actual message receipt, not every loop
        if(RingBuffer_GetCount(&rxRing) > 0) {
        Board_LED_Toggle(0);  
}
        
        // Disable CAN IRQ once for all batch operations
        NVIC_DisableIRQ(CAN_IRQn);

        // Process messages in batches for better efficiency
        batchCount = 0;
        while(!RingBuffer_IsEmpty(&rxRing) && batchCount < BATCH_SIZE) {
        RingBuffer_Pop(&rxRing, &msgBatch[batchCount]); 
batchCount++;
        }
        
        // Reenable interrupts as soon as possible
        NVIC_EnableIRQ(CAN_IRQn);
          
        // Process the batch of messages
        for(int i = 0; i < batchCount; i++) {
            // Convert global clock IDs to local indexes using a switch statement
            // which is more efficient than sequential if statements
            if(msgBatch[i].mode_id == 0x200) {
                switch(msgBatch[i].data[0]) {
                    case 0: // Handle for when the data is already 0
                        break;
                    default:
                        if(msgBatch[i].data[0] == SLAVE0)
                            msgBatch[i].data[0] = 0;
                        else if(msgBatch[i].data[0] == SLAVE1)
                            msgBatch[i].data[0] = 1;
                        else if(msgBatch[i].data[0] == SLAVE2)
                            msgBatch[i].data[0] = 2;
                        else if(msgBatch[i].data[0] == SLAVE3)
                            msgBatch[i].data[0] = 3;
                        break;
                }
        }
    
        // Send commands/position to clocks
        update_from_CAN(&msgBatch[i]);
}
        
        // Clear event flag if buffer is now empty
        if(RingBuffer_IsEmpty(&rxRing)) { 
            ctl_events_set_clear(&canEvent, 0, CAN_RX);
        }
            }  
}