#include "Time.h"

uint8_t ntpPacketBuffer[NTP_PACKET_SIZE] = {0};
CTL_EVENT_SET_t time_event;

void time_thread(void *p)
{
    ctl_events_init(&time_event, 0);
    
    //start real time clock
    rtc_start();
    
    for(;;)
    {
        //update RTC with NTP time
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &time_event, UPDATE_TIME_EVENT, CTL_TIMEOUT_NONE, 0);
        update_rtc();
    }
    
}

void getNTPtime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{

    char timeString[9] = "00:00:00";

    //Connect to NTP Server
    ESP_command(NTP, ONE_SEC_DELAY, 0);
   
    //Interrogate NTP
    //Send command
    ESP_command(SEND, ONE_SEC_DELAY, 0);

    //Send NTP Packet
    ESP_command(NTP_PACKET, ONE_SEC_DELAY, 48);

    UART_send("\n\n", 2, 0);
        
    //Read NTP Packet from UART rx buffer
    readBuffer(ntpPacketBuffer, 48);

    //Extrapolate time data from bufffer
    calculateTime(ntpPacketBuffer, hour, min, sec, timeString);
   
    //Print formatted time string
    UART_send("The current time is: ", 21, 0);
    UART_send(timeString, 9, 0);
    UART_send("\n\n", 2, 0);

    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP, ONE_SEC_DELAY, 0);
}

void calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char* timeString)
{
    char hourC[4], minC[4], secC[4];
    uint32_t timeInt;   
    uint32_t epoch;
    
    
    //Move time data from rx buffer into local variable for interpretation
    timeInt = dataBuffer[40] << 24 |  dataBuffer[41] << 16 | dataBuffer[42] << 8 | dataBuffer[43] << 0;

    epoch = timeInt - 2207520000UL; 
    *hour = epoch % 86400UL / 3600;
    
    //Adjust for GMT
    if(*hour < abs(GMT))
    {
        *hour += 24;
    }
    *hour += GMT;
    *min = epoch % 3600 / 60;
    *sec = epoch % 60;

    //Correctly format time string
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

    //Construct formatted time string
    snprintf(timeString, strlen(timeString)+1, "%s:%s:%s", hourC, minC, secC);
}


void rtc_start(void)
{
    //Enable RTC 
    CCR |= CCR_CLKEN | CCR_CCALEN;
 
}

//Updates RTC from NTP
void update_rtc(void)
{
    uint8_t hour; 
    uint8_t min;
    uint8_t sec;
    
    connect_wifi();
    getNTPtime(&hour, &min, &sec);
    disconnect_wifi();
    
    //Update RTC registers
    HOUR = hour;
    MIN = min;
    SEC = sec;    
    
}

void print_rtc(void)
{
    char hour[3]; 
    char min[3];
    char sec[3];
    
    sprintf(hour, "%ld", HOUR);
    sprintf(min, "%ld", MIN);
    sprintf(sec, "%ld", SEC);
    
    //Correctly format time string
    if(HOUR < 10)
    {
        sprintf(hour, "0%ld", HOUR);
    }
    if(MIN < 10)
    {
        sprintf(min, "0%ld", MIN);
    }
    if(SEC < 10)
    {
        sprintf(sec, "0%ld", SEC);
    }

    ctl_mutex_lock(&uart0_tx_mutex, CTL_TIMEOUT_NONE, 0);
    UART_send("\n", 1, 0);
    UART_send(hour, 2, 0);
    UART_send(":", 1, 0);
    UART_send(min, 2, 0);
    UART_send(":", 1, 0);
    UART_send(sec, 2, 0);       
    UART_send("\n", 1, 0);
    ctl_mutex_unlock(&uart0_tx_mutex);

}  