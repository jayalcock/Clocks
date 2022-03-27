#include "clockEngine.h"



CTL_EVENT_SET_t clockEvent;

static const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
static const char* SEND = "AT+CIPSEND=48\r\n";
static const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n";
static const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};

// Sets home position of arm
int setHome(void)
{
    int atPosition;
    
    return 1;
    
}

// Prints time from RTC 
void printTime(void)
{
    printf("%d:%d:%d\n", HOUR, MIN, SEC);
}

// Sets time
void setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    HOUR = hour;
    MIN = min;
    SEC = sec; 

}

// Initialises RTC
void rtcInit(void)
{
    
    // Power on RTC peripheral 
    PCONP |= PCONP_PCRTC; 
    
    //Reset RTC
    do 
    {
        /* Reset RTC clock*/
        CCR |= CCR_CTCRST;
    } while ((CCR & CCR_CTCRST) != CCR_CTCRST);
        
    do 
    {
        /* Finish resetting RTC clock */
        CCR &= (~CCR_CTCRST) & CCR_CTCRST_MASK;
    } while (CCR & CCR_CTCRST);
    
    //Enable RTC 
    do 
    {
        CCR |= CCR_CLKEN;
        
    } while ((CCR & CCR_CLKEN) == 0);
    
    // Disable RTC Calibration
    do 
    {
        CCR |= CCR_CCALEN;
        
    } while ((CCR & CCR_CCALEN) == 0);
    
	
}
    
void getNTPtime()//uint8_t *hour, uint8_t *min, uint8_t *sec)
{

    char timeString[9] = "00:00:00";

    //Connect to NTP Server
    ESP_command(NTP);//, ONE_SEC_DELAY, 0);
   
    ctl_timeout_wait(ctl_current_time + 1000);
   
    //Interrogate NTP
    //Send command
    ESP_command(SEND);//, ONE_SEC_DELAY, 0);
    
    ctl_timeout_wait(ctl_current_time + 1000);

    //Send NTP Packet
    ESP_command(NTP_PACKET);//, ONE_SEC_DELAY, 48);
    ctl_timeout_wait(ctl_current_time + 1000);
    
    //Chip_UART_SendByte(LPC_UART1, "\n\n");

    //UART_send("\n\n", 2, 0);
        
    //Read NTP Packet from UART rx buffer
    //readBuffer(ntpPacketBuffer, 48);

    //Extrapolate time data from bufffer
    //calculateTime(ntpPacketBuffer, hour, min, sec, timeString);
   
    //Print formatted time string
    //UART_send("The current time is: ", 21, 0);
    //UART_send(timeString, 9, 0);
    //UART_send("\n\n", 2, 0);

    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP);//, ONE_SEC_DELAY, 0);
}


void clock_thread(void *p)
{
    //RTC_TIME_T fullTime;
 
    ctl_events_init(&clockEvent, 0);
 
    // Initialise and start the RTC
    rtcInit();
    setTime(10, 5, 55);
    
    
    ctl_timeout_wait(ctl_current_time + 5000);
    getNTPtime();
    
    
    
 
    while(1)
    {
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        __asm volatile ("nop"); 
    }
}