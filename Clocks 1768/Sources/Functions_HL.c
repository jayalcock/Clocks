#include "Functions_HL.h"

#define NTP_PACKET_SIZE 48

const char* NTP = "AT+CIPSTART=\"UDP\",\"0.ca.pool.ntp.org\",123\r\n";
//const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
const char* SEND = "AT+CIPSEND=48\r\n";
const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n";
const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};

uint8_t ntpPacketBuffer[NTP_PACKET_SIZE] = {0};

void time_thread(void *p)
{
    //Wait for other threads to complete on startup
    ctl_timeout_wait(ctl_current_time + 2000);
         
    rtc_start();
    
    update_rtc();    
 
    for(;;)
    {
        //Loop
    }
    
}

void getNTPtime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    ARM_DRIVER_USART * USARTdrv0 = &Driver_USART0;
    ARM_DRIVER_USART * USARTdrv1 = &Driver_USART1;

    //Connect to NTP Server
    ESP_command(NTP, 1000, 0);
   
   //Interrogate NTP
    //Send command
    ESP_command(SEND, 1000, 0);

    //Send NTP Packet
    //ESP_command(NTP_PACKET, 1000);
    ESP_command(NTP_PACKET, 1000, 48);
    USARTdrv0->Send("\n\n", 2);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
    
    ////Read NTP Packet from UART rx buffer
    readBuffer(ntpPacketBuffer, 48);

    //Calculate and print time
    char* tim = calculateTime(ntpPacketBuffer, hour, min, sec);
    USARTdrv0->Send("The current time is: ", 21);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
    
    USARTdrv0->Send(tim, 8);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
    
    USARTdrv0->Send("\n\n", 2);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
    
    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP, 1000, 0);
}

char * calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    static char timeString[10];
    char hourC[4], minC[4], secC[4];
    uint32_t timeInt;   
    uint32_t epoch;
    //uint8_t hour, min, sec;
    int8_t gmt;
    
    timeInt = dataBuffer[40] << 24 |  dataBuffer[41] << 16 | dataBuffer[42] << 8 | dataBuffer[43] << 0;

    gmt = -7;
    epoch = timeInt - 2207520000UL; 
    *hour = epoch % 86400UL / 3600;
    
    if(*hour < abs(gmt))
    {
        *hour += 24;
    }
    *hour += gmt;
    *min = epoch % 3600 / 60;
    *sec = epoch % 60;

    
    if(*hour < 10)
    {
        sprintf(hourC, "0%d", *hour);
    }
    else
    {
        sprintf(hourC, "%d", *hour);
    }
    if(*min < 10)
    {
        sprintf(minC, "0%d", *min);
    }
    else
    {
        sprintf(minC, "%d", *min);
    }
    if(*sec < 10)
    {
        sprintf(secC, "0%d", *sec);
    }
    else
    {
        sprintf(secC, "%d", *sec);
    }

    snprintf(timeString, sizeof(timeString), "%s:%s:%s", hourC, minC, secC);
    //strcpy(timeString, hourC);
    //strcpy(timeString, minC);
    //strcpy(timeString, secC);
    
    return timeString;
}


void rtc_start(void)
{
    //Enable RTC 
    CCR |= CCR_CLKEN | CCR_CCALEN;
 
}

void update_rtc(void)
{
    uint8_t hour; 
    uint8_t min;
    uint8_t sec;
    
    getNTPtime(&hour, &min, &sec);
    
    HOUR = hour;
    MIN = min;
    SEC = sec;    
    
}