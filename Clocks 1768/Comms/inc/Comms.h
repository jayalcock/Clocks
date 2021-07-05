#ifndef COMMS_H
#define COMMS_H

#include <ctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Driver_USART.h"
#include "Time.h"

#define BAUD 115200

#define UART0_EVENT 1<<0
#define UART0_RX_DONE 1<<1
#define UART0_TX_DONE 1<<2
#define UART1_EVENT 1<<3
#define UART1_RX_DONE 1<<4
#define UART1_TX_DONE 1<<5
#define UART1_RX_END_OF_STRING 1<<6


void UART0_callback(uint32_t event);
void UART1_callback(uint32_t event);

void commsThread(void *p);
void rx_thread(void *p);

void UARTinit(ARM_DRIVER_USART* UART_driver_number, uint32_t baud, void(*callback)(uint32_t));
void UART0init(uint32_t baud);
void UART1init(uint32_t baud);

void writeBuffer(const char *tempChar);
void readBuffer(uint8_t *arr, uint8_t length);

void ESP_command(const void* command, const uint16_t delay, uint8_t size);

void ESP_reset(uint16_t delay);
void ESP_test(uint16_t delay);
void ESP_WIFI_Disconnect(uint16_t delay);
void connect_wifi();
void disconnect_wifi();

//Globals
extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART1;
extern CTL_EVENT_SET_t comms_event;

#endif /*COMMS_H*/