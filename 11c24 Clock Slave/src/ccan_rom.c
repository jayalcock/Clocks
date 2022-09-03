
#include "board.h"
#include "clockDriver.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


#define TEST_CCAN_BAUD_RATE 500000

CCAN_MSG_OBJ_T msg_obj;

uint8_t clockNumbers[] = {0, 1, 2, 3};
uint8_t rxData;
int i; 

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

	for (div = 0; div <= 15; div++) {
		for (quanta = 1; quanta <= 32; quanta++) {
			for (segs = 3; segs <= 17; segs++) {
				if (clk_per_bit == (segs * quanta * (div + 1))) {
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
    uint8_t i;
    /* Determine which CAN message has been received */
    msg_obj.msgobj = msg_obj_num;
    /* Now load up the msg_obj structure with the CAN message */
    LPC_CCAN_API->can_receive(&msg_obj);
    Board_LED_Toggle(0);

    // Only process data applicable to nodes under this MCU
    for(i = 0; i < (sizeof(clockNumbers) / sizeof(clockNumbers[0])); i++)
    {
        if(msg_obj.data[0] == clockNumbers[i])
        {
            update_from_CAN(&msg_obj);
        }
    }

//       msg_obj.msgobj = 1;
    //msg_obj.mode_id = 0x204;
    //msg_obj.mask = 0x700;
    //LPC_CCAN_API->config_rxmsgobj(&msg_obj);
    //if (msg_obj_num == 1) {
    //	/* Simply transmit CAN frame (echo) with with ID +0x100 via buffer 2 */
    //	msg_obj.msgobj = 2;
    //	msg_obj.mode_id += 0x100;
    //	LPC_CCAN_API->can_trans`mit(&msg_obj);
    //}
        
    return;
       
}

/*	CAN transmit callback */
/*	Function is executed by the Callback handler after
    a CAN message has been transmitted */
void CAN_tx(uint8_t msg_obj_num) {}

/*	CAN error callback */
/*	Function is executed by the Callback handler after
    an error has occured on the CAN bus */
void CAN_error(uint32_t error_info) {
    //if (error_info & CAN_ERROR_BOFF)
    //    reset_can = TRUE;

    //return;

}

/**
 * @brief	CCAN Interrupt Handler
 * @return	Nothing
 * @note	The CCAN interrupt handler must be provided by the user application.
 *	It's function is to call the isr() API located in the ROM
 */
//void CAN_IRQHandler(void) 
void C_CAN_IRQHandler(void){
	LPC_CCAN_API->isr();
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
	msg_obj.mask = 0x3F8;
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
