/*
 * @brief CAN example
 * This example show how to use CAN peripheral
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <stdint.h>
#include "board.h"
#include "ctl_api.h"
#include "can.h"
#include "can_17xx_40xx.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if defined (CHIP_LPC175X_6X)
#define CAN_CTRL_NO			0
#else
#define CAN_CTRL_NO         0
#endif
#if (CAN_CTRL_NO == 0)
#define LPC_CAN             (LPC_CAN1)
#else
#define LPC_CAN             (LPC_CAN2)
#endif
#define AF_LUT_USED         0
#if AF_LUT_USED
#define FULL_CAN_AF_USED    1
#endif
#define CAN_TX_MSG_STD_ID (0x200)
//#define CAN_TX_MSG_REMOTE_STD_ID (0x300)
//#define CAN_TX_MSG_EXT_ID (0x10000200)
#define CAN_RX_MSG_ID (0x100)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
static char WelcomeMenu[] = "\n\rHello NXP Semiconductors \r\n"
							"CAN DEMO : Use CAN to transmit and receive Message from CAN Analyzer\r\n"
							"CAN bit rate : 500kBit/s\r\n";

                                             
CAN_MSG_T SendMsgBuf;
CTL_EVENT_SET_t can_event;
CTL_MUTEX_t bufferMutex;
CAN_MSG_T *testPtr;

///* Transmit and receive ring buffers */
static RINGBUFF_T txring1, rxring1;

///* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

///* Transmit and receive buffers */
//static uint8_t rxbuff1[UART_RRB_SIZE], txbuff1[UART_SRB_SIZE];                                           
static CAN_MSG_T txbuff1[30];
  
#if AF_LUT_USED
#if FULL_CAN_AF_USED
CAN_STD_ID_ENTRY_T FullCANSection[] = {
	{CAN_CTRL_NO, 0, 0x03},
	{CAN_CTRL_NO, 0, 0x05},
	{CAN_CTRL_NO, 0, 0x07},
	{CAN_CTRL_NO, 0, 0x09},
};
#endif
CAN_STD_ID_ENTRY_T SFFSection[] = {
	{CAN_CTRL_NO, 0, 0x30},
	{CAN_CTRL_NO, 0, 0x50},
	{CAN_CTRL_NO, 0, 0x70},
	{CAN_CTRL_NO, 0, 0x90},
	{CAN_CTRL_NO, 0, 0xB0},
};
CAN_STD_ID_RANGE_ENTRY_T SffGrpSection[] = {
	{{CAN_CTRL_NO, 0, 0x300}, {CAN_CTRL_NO, 0, 0x400}},
	{{CAN_CTRL_NO, 0, 0x500}, {CAN_CTRL_NO, 0, 0x600}},
	{{CAN_CTRL_NO, 0, 0x700}, {CAN_CTRL_NO, 0, 0x780}},
};
CAN_EXT_ID_ENTRY_T EFFSection[] = {
	{CAN_CTRL_NO, ((1 << 11) | 0x03)},
	{CAN_CTRL_NO, ((1 << 11) | 0x05)},
	{CAN_CTRL_NO, ((1 << 11) | 0x07)},
	{CAN_CTRL_NO, ((1 << 11) | 0x09)},
};
CAN_EXT_ID_RANGE_ENTRY_T EffGrpSection[] = {
	{{CAN_CTRL_NO, ((1 << 11) | 0x300)}, {CAN_CTRL_NO, ((1 << 11) | 0x400)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x500)}, {CAN_CTRL_NO, ((1 << 11) | 0x600)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x700)}, {CAN_CTRL_NO, ((1 << 11) | 0x780)}},
};
CANAF_LUT_T AFSections = {
#if FULL_CAN_AF_USED
	FullCANSection, sizeof(FullCANSection) / sizeof(CAN_STD_ID_ENTRY_T),
#else
	NULL, 0,
#endif
	SFFSection, sizeof(SFFSection) / sizeof(CAN_STD_ID_ENTRY_T),
	SffGrpSection, sizeof(SffGrpSection) / sizeof(CAN_STD_ID_RANGE_ENTRY_T),
	EFFSection, sizeof(EFFSection) / sizeof(CAN_EXT_ID_ENTRY_T),
	EffGrpSection, sizeof(EffGrpSection) / sizeof(CAN_EXT_ID_RANGE_ENTRY_T),
};
#endif /*AF_LUT_USED*/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Print error */
static void PrintCANErrorInfo(uint32_t status)
{
	if (status & CAN_ICR_EI) {
		DEBUGOUT("Error Warning!\r\n");
	}
	if (status & CAN_ICR_DOI) {
		DEBUGOUT("Data Overrun!\r\n");
	}
	if (status & CAN_ICR_EPI) {
		DEBUGOUT("Error Passive!\r\n");
	}
	if (status & CAN_ICR_ALI) {
		DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n", CAN_ICR_ALCBIT_VAL(status));
	}
	if (status & CAN_ICR_BEI) {

		DEBUGOUT("Bus error !!!\r\n");

		if (status & CAN_ICR_ERRDIR_RECEIVE) {
			DEBUGOUT("\t Error Direction: Transmiting\r\n");
                        //DEBUGSTR("\t Error Direction: Transmiting\r\n");
		}
		else {
			DEBUGOUT("\t Error Direction: Receiving\r\n");
                        //DEBUGSTR("\t Error Direction: Receiving\r\n");
		}

		DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(status));
		DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(status));
	}
}

/* Print CAN Message */
static void PrintCANMsg(CAN_MSG_T *pMsg)
{
	uint8_t i;
	DEBUGOUT("\t**************************\r\n");
	DEBUGOUT("\tMessage Information: \r\n");
	DEBUGOUT("\tMessage Type: ");
	if (pMsg->ID & CAN_EXTEND_ID_USAGE) {
		DEBUGOUT(" Extend ID Message");
	}
	else {
		DEBUGOUT(" Standard ID Message");
	}
	if (pMsg->Type & CAN_REMOTE_MSG) {
		DEBUGOUT(", Remote Message");
	}
	DEBUGOUT("\r\n");
	DEBUGOUT("\tMessage ID :0x%x\r\n", (pMsg->ID & (~CAN_EXTEND_ID_USAGE)));
	DEBUGOUT("\tMessage Data :");
	for (i = 0; i < pMsg->DLC; i++)
		DEBUGOUT("%x ", pMsg->Data[i]);
	DEBUGOUT("\r\n\t**************************\r\n");
}

/* Reply remote message received */
static void ReplyRemoteMessage(CAN_MSG_T *pRcvMsg)
{
	CAN_MSG_T SendMsgBuf;
	CAN_BUFFER_ID_T   TxBuf;
	uint8_t i;
	SendMsgBuf.ID  = pRcvMsg->ID;
	SendMsgBuf.DLC = pRcvMsg->DLC;
	SendMsgBuf.Type = 0;
	for (i = 0; i < pRcvMsg->DLC; i++)
		SendMsgBuf.Data[i] = '0' + i;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
	DEBUGOUT("Message Replied!!!\r\n");
	PrintCANMsg(&SendMsgBuf);
}

/* Reply message received */
static void ReplyNormalMessage(CAN_MSG_T *pRcvMsg)
{
	CAN_MSG_T SendMsgBuf = *pRcvMsg;
	CAN_BUFFER_ID_T   TxBuf;
	SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
	DEBUGOUT("Message Replied!!!\r\n");
	PrintCANMsg(&SendMsgBuf);
}

#if AF_LUT_USED
/* Print entries in AF LUT */
static void PrintAFLUT(void)
{
	uint16_t i, num;
	CAN_STD_ID_ENTRY_T StdEntry;
	CAN_EXT_ID_ENTRY_T ExtEntry;
	CAN_STD_ID_RANGE_ENTRY_T StdGrpEntry;
	CAN_EXT_ID_RANGE_ENTRY_T ExtGrpEntry;
#if FULL_CAN_AF_USED
	/* Full CAN Table */
	DEBUGOUT("\tFULL CAN Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x, Dis: %1d\r\n",
				 i, StdEntry.CtrlNo, StdEntry.ID_11, StdEntry.Disable);
	}
#endif
	/* Standard ID Table */
	DEBUGOUT("\tIndividual Standard ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadSTDEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x, Dis: %1d\r\n",
				 i, StdEntry.CtrlNo, StdEntry.ID_11, StdEntry.Disable);
	}
	/* Group Standard ID Table */
	DEBUGOUT("\tGroup Standard ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdGrpEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x-0x%x, Dis: %1d\r\n",
				 i, StdGrpEntry.LowerID.CtrlNo, StdGrpEntry.LowerID.ID_11,
				 StdGrpEntry.UpperID.ID_11, StdGrpEntry.LowerID.Disable);
	}
	/* Extended ID Table */
	DEBUGOUT("\tExtended ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadEXTEntry(LPC_CANAF, LPC_CANAF_RAM, i, &ExtEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x,\r\n",
				 i, ExtEntry.CtrlNo, ExtEntry.ID_29);
	}
	/* Group Extended ID Table */
	DEBUGOUT("\tGroup Extended ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, i, &ExtGrpEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x-0x%x\r\n",
				 i, ExtGrpEntry.LowerID.CtrlNo, ExtGrpEntry.LowerID.ID_29,
				 ExtGrpEntry.UpperID.ID_29);
	}

}

/* Setup AF LUT */
static void SetupAFLUT(void)
{
	DEBUGOUT("Setup AF LUT... \r\n");
	Chip_CAN_SetAFLUT(LPC_CANAF, LPC_CANAF_RAM, &AFSections);
	//PrintAFLUT();
}

/* Insert/Remove entries to/from AF LUT */
static void ChangeAFLUT(void)
{
#if FULL_CAN_AF_USED
	CAN_STD_ID_ENTRY_T FullEntry = {CAN_CTRL_NO, 0, 0x0A};
#endif
	CAN_STD_ID_ENTRY_T StdEntry = {CAN_CTRL_NO, 0, 0xC0};
	CAN_EXT_ID_ENTRY_T ExtEntry = {CAN_CTRL_NO, ((1 << 11) | 0x0A)};
	CAN_STD_ID_RANGE_ENTRY_T StdGrpEntry = {{CAN_CTRL_NO, 0, 0x7A0}, {CAN_CTRL_NO, 0, 0x7B0}};
	CAN_EXT_ID_RANGE_ENTRY_T ExtGrpEntry = {{CAN_CTRL_NO, ((1 << 11) | 0x7A0)}, {CAN_CTRL_NO, ((1 << 11) | 0x7B0)}};

	//DEBUGOUT("Insert new entries into the current LUT... \r\n");
#if FULL_CAN_AF_USED
	/* Edit Full CAN Table */
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
	FullEntry.ID_11 = 2;
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
	FullEntry.ID_11 = 4;
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
#endif /*FULL_CAN_AF_USED*/

	/* Edit Individual STD ID Table */
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);
	StdEntry.ID_11 = 0x20;
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);
	StdEntry.ID_11 = 0x40;
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);

	/* Edit Individual EXT ID Table */
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);
	ExtEntry.ID_29 = (1 << 11) | 0x02;
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);
	ExtEntry.ID_29 = (1 << 11) | 0x04;
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);

	/* Edit STD ID Group Table */
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);
	StdGrpEntry.LowerID.ID_11 = 0x200;
	StdGrpEntry.UpperID.ID_11 = 0x300;
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);
	StdGrpEntry.LowerID.ID_11 = 0x400;
	StdGrpEntry.UpperID.ID_11 = 0x500;
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);

	/* Edit EXT ID Group Table */
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);
	ExtGrpEntry.LowerID.ID_29 = (1 << 11) | 0x200;
	ExtGrpEntry.UpperID.ID_29 = (1 << 11) | 0x300;
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);
	ExtGrpEntry.LowerID.ID_29 = (1 << 11) | 0x400;
	ExtGrpEntry.UpperID.ID_29 = (1 << 11) | 0x500;
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);

	//PrintAFLUT();

	//DEBUGOUT("Remove entries into the current LUT... \r\n");
	/* Remove entries from the current LUT */
#if FULL_CAN_AF_USED
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC) - 1);
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC) / 2);
#endif
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC) - 1);
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC) / 2);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC) - 1);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC) / 2);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC) - 1);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC) / 2);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC) - 1);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC) / 2);
	//PrintAFLUT();
}

#endif
/*****************************************************************************
 * Public functions
 ****************************************************************************/
void CAN_IRQHandler(void)
{
#if FULL_CAN_AF_USED
	uint16_t i = 0, FullCANEntryNum = 0;
#endif
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);

	//PrintCANErrorInfo(IntStatus);

	/* New Message came */
	if (IntStatus & CAN_ICR_RI) {
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		//DEBUGOUT("Message Received!!!\r\n");
                //DEBUGSTR("RX Success\r\n");
                Board_LED_Toggle(0);
               
		//PrintCANMsg(&RcvMsgBuf);

		//if (RcvMsgBuf.Type & CAN_REMOTE_MSG) {
		//	ReplyRemoteMessage(&RcvMsgBuf);
		//}
		//else {
		//	ReplyNormalMessage(&RcvMsgBuf);
		//}

	}
#if FULL_CAN_AF_USED
	FullCANEntryNum = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC);
	if (FullCANEntryNum > 64) {
		FullCANEntryNum = 64;
	}
	for (i = 0; i < FullCANEntryNum; i++)
		if (Chip_CAN_GetFullCANIntStatus(LPC_CANAF, i)) {
			uint8_t SCC;
			Chip_CAN_FullCANReceive(LPC_CANAF, LPC_CANAF_RAM, i, &RcvMsgBuf, &SCC);
			if (SCC == CAN_CTRL_NO) {
				DEBUGOUT("FullCAN Message Received!!!\r\n");
				PrintCANMsg(&RcvMsgBuf);
				if (RcvMsgBuf.Type & CAN_REMOTE_MSG) {
					ReplyRemoteMessage(&RcvMsgBuf);
				}
				else {
					ReplyNormalMessage(&RcvMsgBuf);
				}
			}
		}

#endif /*FULL_CAN_AF_USED*/
}

// Start CAN Tx public function
void startCanTx(CAN_MSG_T *sendMsgBuffPtr)
{    
    ctl_mutex_lock(&bufferMutex, CTL_TIMEOUT_NONE, 0);
    RingBuffer_Insert(&txring1, sendMsgBuffPtr);
    ctl_mutex_unlock(&bufferMutex);
    ctl_events_set_clear(&can_event, 1<<0, 0);
}

// CAN Thread
void CAN_Thread(void *msgQueuePtr)
{
	CAN_BUFFER_ID_T TxBuf;
	CAN_MSG_T SendMsgBuf;
        //void* msgPtr;
        testPtr = &SendMsgBuf;
               
        RingBuffer_Init(&txring1, &txbuff1, sizeof(CAN_MSG_T), 30);

        ctl_events_init(&can_event, 0);
        ctl_mutex_init(&bufferMutex);
        
	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);

	Chip_CAN_SetBitRate(LPC_CAN, 500000);
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

#if AF_LUT_USED
	SetupAFLUT();
	ChangeAFLUT();
	PrintAFLUT();
#if FULL_CAN_AF_USED
	Chip_CAN_ConfigFullCANInt(LPC_CANAF, ENABLE);
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_FULL_MODE);
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);
#endif /*FULL_CAN_AF_USED*/
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
#endif /*AF_LUT_USED*/
	NVIC_EnableIRQ(CAN_IRQn);       

	while (1)
        {
            // Wait for can event to be triggered
            ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &can_event, 1<<0, CTL_TIMEOUT_NONE, 0);
            
            // Receive message from message queue
            //ctl_message_queue_receive((CTL_MESSAGE_QUEUE_t*) msgQueuePtr, &msgPtr, CTL_TIMEOUT_NONE, 0);
            //RingBuffer_Pop(&txring1, msgPtr);
            
           
            
            //wait = 1;
            ctl_mutex_lock(&bufferMutex, CTL_TIMEOUT_NONE, 0);
            RingBuffer_Pop(&txring1, &SendMsgBuf);
            //wait = 0;
            ctl_mutex_unlock(&bufferMutex);
            
            // Copy message to local buffer
            //SendMsgBuf = *(CAN_MSG_T*) msgPtr;
            
            // Get free tx buffer 
            TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
            
            // Send message and wait for ACK 
            Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
            while ((Chip_CAN_GetStatus(LPC_CAN) & CAN_SR_TCS(TxBuf)) == 0) {}
            //DEBUGSTR("TX Success\r\n");
            Board_LED_Toggle(1);
            
            // Clear can event
            if(RingBuffer_IsEmpty(&txring1))
            {
                ctl_events_set_clear(&can_event, 0, 1<<0);
            }

        }
}

