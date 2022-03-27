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

#include <ctl.h>
#include "chip.h"
#include "board.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define UART_SELECTION 	LPC_UART0
#define IRQ_SELECTION 	UART0_IRQn
#define HANDLER_NAME 	UART0_IRQHandler


#define UART0_EVENT 1<<0
#define UART0_RX_DONE 1<<1
#define UART0_TX_DONE 1<<2
#define UART1_EVENT 1<<3
#define UART1_RX_DONE 1<<4
#define UART1_TX_DONE 1<<5
#define UART1_RX_END_OF_STRING 1<<6


CTL_EVENT_SET_t uartEvent;

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring0, rxring0, txring1, rxring1;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff0[UART_RRB_SIZE], txbuff0[UART_SRB_SIZE], rxbuff1[UART_RRB_SIZE], txbuff1[UART_SRB_SIZE];;

const char inst1[] = "Startup\r\n";
const char inst2[] = "\r\n";

const char SSIDPWD[] = "AT+CWJAP=\"NETGEAR47\",\"phobicjungle712\"\r\n";
const char INIT[] = "AT\r\n";
const char RESET_CHIP[] = "AT+RST\r\n";

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

//Send command to ESP8266
void ESP_command(const void* command)//, const uint16_t delay, uint8_t size)
{
    
    Chip_UART_SendRB(LPC_UART1, &txring1, command, strlen(command));
    //Chip_UART_SendBlocking(LPC_UART1, command, strlen(command));
    
    //if(size > 0)
    //{
    //    //USARTdrv1->Send(command, size);
    //}
    //else
    //{
    //    //USARTdrv1->Send(command, strlen(command));
    //}
    //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &uartEvent, UART1_TX_DONE, CTL_TIMEOUT_NONE, 0);
    //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &uartEvent, UART1_RX_END_OF_STRING, CTL_TIMEOUT_NONE, 0);
    //ctl_timeout_wait(ctl_current_time + 10);// + delay);
}
 
 
 
/**
 * @brief	UART 0 interrupt handler using ring buffers
 * @return	Nothing
 */
void UART0_IRQHandler(void)
{
        //TODO error handling
	Chip_UART_IRQRBHandler(LPC_UART0, &rxring0, &txring0);
}
    
/**
 * @brief	UART 1 interrupt handler using ring buffers
 * @return	Nothing
 */
void UART1_IRQHandler(void)
{
        //char woops[] = "Hola\n";
	//TODO error handling
	Chip_UART_IRQRBHandler(LPC_UART1, &rxring1, &txring1);
        if(rxring1.count > 0)
        {
            //Chip_UART_SendRB(LPC_UART0, &txring0,(const uint8_t *) &rxbuff1, rxring0.count);
            //ctl_events_set_clear(&uartEvent, 1<<0, 0);
            
        }
            
}

/**
 * @brief	Main UART program body
 * @return	void
 */
void uart_thread(void *p)
{
	//uint8_t key, rx;
	//int bytes0, bytes1;

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
	RingBuffer_Init(&rxring0, rxbuff0, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring0, txbuff0, 1, UART_SRB_SIZE);
        RingBuffer_Init(&rxring1, rxbuff1, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring1, txbuff1, 1, UART_SRB_SIZE);

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
	NVIC_SetPriority(UART0_IRQn, 2);
	NVIC_EnableIRQ(UART0_IRQn);
        NVIC_SetPriority(UART1_IRQn, 1);
	NVIC_EnableIRQ(UART1_IRQn);

	/* Send initial messages */
	//Chip_UART_SendRB(LPC_UART0, &txring, inst1, sizeof(inst1) - 1);
	//Chip_UART_SendRB(LPC_UART0, &txring, inst2, sizeof(inst2) - 1);
        
        //Ensure connection to ESP
        //ESP_command(INIT);//, 200U, 0);
        //ESP_command(RESET_CHIP);//, 5000U, 0);
        
        //Chip_UART_SendRB(LPC_UART1, &txring, RESET_CHIP, sizeof(RESET_CHIP) - 1);
 
	/* Poll the receive ring buffer for the ESC (ASCII 27) key */
        uint8_t temp = 0;
        //char temp2[] = "";
	uint8_t temp2;
        while(1)
        
        {
    
            //key = 0;
           //temp2 = 0; 
            //while (key != 27) 
            //{
                if(temp == 0)
                {     
                    //Ensure connection to ESP
                    //ESP_command(INIT);//, 200U, 0);
                    //ctl_timeout_wait(ctl_current_time + 1000);
                    ESP_command(RESET_CHIP);//, 5000U, 0);
                    //ctl_timeout_wait(ctl_current_time + 5000);
                    
                    temp = 1;
                }   

 
                //bytes0 = Chip_UART_ReadRB(LPC_UART0, &rxring0, &key, 1);
                ////Chip_UART_ReadRB(LPC_UART0, &rxring0, &key, 1);
                ////Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1);
             
                //bytes1 = Chip_UART_ReadRB(LPC_UART1, &rxring1, &rx, 1);
                
                //if (bytes0 > 0) 
                //{
                //    /* Wrap value back around */
                //    //if (Chip_UART_SendRB(LPC_UART0, &txring0, (const uint8_t *) &key, 1) != 1) 
                //    if (Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1) != 1) 
                //    {
                //        Board_LED_Toggle(0);/* Toggle LED if the TX FIFO is full */
                //    }
                //    //Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1);

                //    //Chip_UART_SendRB(LPC_UART0, &txring0,(const uint8_t *) &rxring0, rxring0.count);
                //    //Chip_UART_ReadRB(LPC_UART1, &rxring1, &temp, 1);
                //    //Chip_UART_SendRB(LPC_UART0, &txring0, temp, sizeof(temp) - 1);
                //}
                //if(bytes1 > 0)
                //{
                //    Chip_UART_SendRB(LPC_UART0, &txring0, (const uint8_t *) &rx, 1);
                //}
                    
            //}
            ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &uartEvent, 1<<1, CTL_TIMEOUT_NONE, 0);
        }
}
        
 

/**
 * @}
 */
void uartRX_thread(void *p)
{
    uint8_t key, rx;
    int bytes0, bytes1;
        
    while(1)
    {
        //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &uartEvent, 1<<0, CTL_TIMEOUT_NONE, 0);
        
        bytes0 = Chip_UART_ReadRB(LPC_UART0, &rxring0, &key, 1);
        //Chip_UART_ReadRB(LPC_UART0, &rxring0, &key, 1);
        //Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1);
 
        bytes1 = Chip_UART_ReadRB(LPC_UART1, &rxring1, &rx, 1);
    
        if (bytes0 > 0) 
        {
            /* Wrap value back around */
            //if (Chip_UART_SendRB(LPC_UART0, &txring0, (const uint8_t *) &key, 1) != 1) 
            if (Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1) != 1) 
            {
                Board_LED_Toggle(0);/* Toggle LED if the TX FIFO is full */
            }
            //Chip_UART_SendRB(LPC_UART1, &txring1, (const uint8_t *) &key, 1);

            //Chip_UART_SendRB(LPC_UART0, &txring0,(const uint8_t *) &rxring0, rxring0.count);
            //Chip_UART_ReadRB(LPC_UART1, &rxring1, &temp, 1);
            //Chip_UART_SendRB(LPC_UART0, &txring0, temp, sizeof(temp) - 1);
        }
        if(bytes1 > 0)
        {
            Chip_UART_SendRB(LPC_UART0, &txring0, (const uint8_t *) &rx, 1);
        }
        
        
    }
    
}