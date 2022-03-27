/*
 * uart_rb.h
 *
 *  Created on: Feb. 26, 2022
 *      Author: jayalcock
 */

#ifndef UART_RB_H_
#define UART_RB_H_

void ESP_command(const void* command);
void UART0_IRQHandler(void);

void uart_thread(void *p);
void uartRX_thread(void *p);


#endif /* UART_RB_H_ */
