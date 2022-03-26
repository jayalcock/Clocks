/*
 * @brief UART interrupt example with ring buffers
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

#include "chip.h"
#include "board.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define UART_SELECTION 	LPC_UART0
#define IRQ_SELECTION 	UART0_IRQn
#define HANDLER_NAME 	UART0_IRQHandler


/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

const char inst1[] = "Startup\r\n";
const char inst2[] = "\r\n";

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	UART 0 interrupt handler using ring buffers
 * @return	Nothing
 */
void UART0_IRQHandler(void)
{
        //TODO error handling
	Chip_UART_IRQRBHandler(LPC_UART0, &rxring, &txring);
}
    
/**
 * @brief	UART 1 interrupt handler using ring buffers
 * @return	Nothing
 */
void UART1_IRQHandler(void)
{
	//TODO error handling
	Chip_UART_IRQRBHandler(LPC_UART1, &rxring, &txring);
}   

/**
 * @brief	Main UART program body
 * @return	void
 */
void uart_thread(void *p)
{
	uint8_t key;
	int bytes;

	//Board_UART_Init(UART_SELECTION);

	//* Setup UART0 for 115.2K8N1 */
	Chip_UART_Init(LPC_UART0);
	Chip_UART_SetBaud(LPC_UART0, 115200);
	Chip_UART_ConfigData(LPC_UART0, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART0, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART0);
        
        //* Setup UART1 for 115.2K8N1 */
	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 115200);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART1);
        

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_UART0, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | 
							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UART0, (UART_IER_RBRINT | UART_IER_RLSINT));
        
        
        /* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | 
							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(IRQ_SELECTION, 1);
	NVIC_EnableIRQ(IRQ_SELECTION);

	/* Send initial messages */
	Chip_UART_SendRB(LPC_UART0, &txring, inst1, sizeof(inst1) - 1);
	Chip_UART_SendRB(LPC_UART0, &txring, inst2, sizeof(inst2) - 1);

	/* Poll the receive ring buffer for the ESC (ASCII 27) key */
	while(1)
        {
            key = 0;
            while (key != 27) {
                    bytes = Chip_UART_ReadRB(LPC_UART0, &rxring, &key, 1);
                    if (bytes > 0) {
                            /* Wrap value back around */
                            if (Chip_UART_SendRB(LPC_UART0, &txring, (const uint8_t *) &key, 1) != 1) {
                                    Board_LED_Toggle(0);/* Toggle LED if the TX FIFO is full */
                            }
                    }
            }
        }
}

/**
 * @}
 */
