#ifndef TIME_H
#define TIME_H

//#include <ctl.h>
//#include "LPC1768.h"
//#include "Comms.h"

#define GMT -7
#define NTP_PACKET_SIZE 48
#define ONE_SEC_DELAY 1000U
#define UPDATE_TIME_EVENT 1<<0

//static const char* NTP = "AT+CIPSTART=\"UDP\",\"0.ca.pool.ntp.org\",123\r\n";
static const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
static const char* SEND = "AT+CIPSEND=48\r\n";
static const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n";
static const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};


void time_thread(void *p);

void getNTPtime(uint8_t *hour, uint8_t *min, uint8_t *sec);
void calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char *timeString);

void rtc_start(void);
void update_rtc(void);
void print_rtc(void);

//Globals
extern CTL_EVENT_SET_t time_event;

#endif /* TIME_H */