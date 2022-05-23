/*
 * uart_rb.h
 *
 *  Created on: Feb. 26, 2022
 *      Author: jayalcock
 */

#ifndef _UART_RB_H_
#define _UART_RB_H_

void ESP_command(const void* command, const uint16_t delay, const uint8_t size);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void uart_thread(void *p);
void uartRX_thread(void *p);
/**
 * @brief	Copies data from uart1 receive ring buffer
 * @param       data: pointer to data array
 * @param       size: number of elements to be copied
 * @return	Nothing
 */
void ringBuffer1Copy(uint8_t *data, const uint8_t size);



#endif /* UART_RB_H_ */
