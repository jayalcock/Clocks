#ifndef _COMMS_H_
#define _COMMS_H_

#include <ctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Driver_USART.h"
#include "Time.h"


#define NEW_WIFI_CONNECTION 0
#define HOME_WIFI 0
#define PHONE_WIFI 0
#define LYNDSEY_WIFI 0
#define LAKE_WIFI 0 
#define OTHER_WIFI 1

#define BAUD 115200

#define UART0_EVENT 1<<0
#define UART0_RX_DONE 1<<1
#define UART0_TX_DONE 1<<2
#define UART1_EVENT 1<<3
#define UART1_RX_DONE 1<<4
#define UART1_TX_DONE 1<<5
#define UART1_RX_END_OF_STRING 1<<6

//WIFI Constants 
static const char* INIT = "AT\r\n";
static const char* RESET = "AT+RST\r\n";
static const char* DISCONNECT = "AT+CWQAP\r\n";
static const char* MODE = "AT+CWMODE=1\r\n";
#if HOME_WIFI
static const char* SSIDPWD = "AT+CWJAP=\"NETGEAR47\",\"phobicjungle712\"\r\n";
#elif PHONE_WIFI
static const char* SSIDPWD = "AT+CWJAP=\"Jay's iPhone\",\"shannon1\"\r\n";
#elif LYNDSEY_WIFI
static const char* SSIDPWD = "AT+CWJAP=\"L-Gav Pad\",\"hgbbs123\"\r\n";
#elif LAKE_WIFI
static const char* SSIDPWD = "AT+CWJAP=\"Gavins\",\"LakeHouse9307\"\r\n";
#elif OTHER_WIFI
static const char* SSIDPWD = "AT+CWJAP=\"Crown Mansion\",\"crownjewel\"\r\n";
#endif

void UART0_callback(uint32_t event);
void UART1_callback(uint32_t event);

void commsThread(void *p);
void rx_thread(void *p);

void UARTinit(ARM_DRIVER_USART* UART_driver_number, uint32_t baud, void(*callback)(uint32_t));
void UART0init(uint32_t baud);
void UART1init(uint32_t baud);
void UART_send(const char * data, uint8_t length, uint8_t uartNumber);

void writeBuffer(const char *tempChar);
void readBuffer(uint8_t *arr, uint8_t length);

void ESP_command(const void* command, const uint16_t delay, uint8_t size);

void ESP_reset(uint16_t delay);
void ESP_test(uint16_t delay);
void ESP_WIFI_Disconnect(uint16_t delay);
void connect_wifi();
void disconnect_wifi();


//extern ARM_DRIVER_USART *USARTdrv0;
//extern ARM_DRIVER_USART *USARTdrv1;


//extern USARTdrv0 = &Driver_USART0;
//USARTdrv1 = &Driver_USART1;

//Globals

extern CTL_EVENT_SET_t comms_event;

#endif /*COMMS_H*/