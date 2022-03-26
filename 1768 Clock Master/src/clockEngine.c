#include "clockEngine.h"

CTL_EVENT_SET_t clockEvent;

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


void clock_thread(void *p)
{
    //RTC_TIME_T fullTime;
 
    ctl_events_init(&clockEvent, 0);
 
    // Initialise and start the RTC
    rtcInit();
    setTime(10, 5, 55);
    
    
    
    
 
    while(1)
    {
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        __asm volatile ("nop"); 
    }
}