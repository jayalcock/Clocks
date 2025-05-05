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
#define UART_SRB_SIZE 64	/* Send */
#define UART_RRB_SIZE 64	/* Receive */

///* Transmit buffer */                                           
static CAN_MSG_T txbuff1[UART_SRB_SIZE];
  
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
	
	// Process all pending interrupts at once to reduce overhead
	while ((IntStatus = Chip_CAN_GetIntStatus(LPC_CAN)) != 0) {
		// Process receive interrupts first
		if (IntStatus & CAN_ICR_RI) {
			if (Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf) == SUCCESS) {
				Board_LED_Toggle(0);
				
				// Handle message processing here
				// Consider adding to a receive ring buffer for processing outside interrupt
			}
		}
		
		// Handle error conditions
		if (IntStatus & (CAN_ICR_EI | CAN_ICR_DOI | CAN_ICR_EPI | CAN_ICR_ALI | CAN_ICR_BEI)) {
			// Only log severe errors to reduce overhead
			if (IntStatus & (CAN_ICR_DOI | CAN_ICR_BEI)) {
				// Handle critical errors - data overrun or bus error
				// Consider setting an error flag for main thread to handle
				//PrintCANErrorInfo(IntStatus);
			}
		}

  #if FULL_CAN_AF_USED
		// Process FullCAN messages more efficiently
		FullCANEntryNum = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC);
		if (FullCANEntryNum > 64) {
			FullCANEntryNum = 64;
		}
		
		for (i = 0; i < FullCANEntryNum; i++) {
			if (Chip_CAN_GetFullCANIntStatus(LPC_CANAF, i)) {
				uint8_t SCC;
				if (Chip_CAN_FullCANReceive(LPC_CANAF, LPC_CANAF_RAM, i, &RcvMsgBuf, &SCC) == SUCCESS) {
					if (SCC == CAN_CTRL_NO) {
						// Consider adding to a receive ring buffer for processing outside interrupt
					}
				}
			}
		}
#endif
	}
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
    CAN_MSG_T batchMsgs[4]; // For batch processing
    int batchCount = 0;
    int i;
    
    testPtr = &SendMsgBuf;
           
    // Initialize the ring buffer with increased size for better performance
    RingBuffer_Init(&txring1, &txbuff1, sizeof(CAN_MSG_T), UART_SRB_SIZE);

    ctl_events_init(&can_event, 0);
    ctl_mutex_init(&bufferMutex);
    
    Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);

    // Use higher baud rate if hardware supports it - 500kbit is standard
    Chip_CAN_SetBitRate(LPC_CAN, 500000);
    
    // Enable only necessary interrupts to reduce overhead
    Chip_CAN_EnableInt(LPC_CAN, CAN_ICR_RI | CAN_ICR_DOI | CAN_ICR_BEI);

#if AF_LUT_USED
    SetupAFLUT();
    ChangeAFLUT();
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
        // Wait for CAN event with timeout to allow checking buffer periodically
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &can_event, (1 << 0), 10, 0);
    
        // Process messages in batches when possible
        batchCount = 0;
        
        // Lock buffer access once for multiple operations
        ctl_mutex_lock(&bufferMutex, CTL_TIMEOUT_NONE, 0);
        
        // Get up to 4 messages at once if available
        while (!RingBuffer_IsEmpty(&txring1) && batchCount < 4) {
            RingBuffer_Pop(&txring1, &batchMsgs[batchCount]);
            batchCount++;
        }
        
        ctl_mutex_unlock(&bufferMutex);
        
        // If no messages, clear event and continue
        if (batchCount == 0) {
            ctl_events_set_clear(&can_event, 0, (1 << 0));
            continue;
        }
        
        // Process the batch of messages
        for (i = 0; i < batchCount; i++) {
            // Get a free TX buffer - wait if none available
            TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
            
            // Send the message
            Chip_CAN_Send(LPC_CAN, TxBuf, &batchMsgs[i]);
            
            // For high-priority messages, wait for completion
            // For lower priority, could continue without waiting
            if (i == (batchCount - 1) || batchMsgs[i].ID == CAN_TX_MSG_STD_ID) {
                while ((Chip_CAN_GetStatus(LPC_CAN) & CAN_SR_TCS(TxBuf)) == 0) {}
            }
        }
        
        // Toggle LED to indicate transmission success
        Board_LED_Toggle(1);
        
        // If the buffer still has messages, trigger processing again
        if (!RingBuffer_IsEmpty(&txring1)) {
            ctl_events_set_clear(&can_event, (1 << 0), 0);
        }
    }
}

