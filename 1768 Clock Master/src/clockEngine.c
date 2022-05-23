#include <stdlib.h>
#include <stdint.h>
#include <ctl_api.h>
#include <stdio.h>
#include <string.h>

#include "clockEngine.h"
#include "board.h"
#include "uart_17xx_40xx.h"
#include "uart_rb.h"
#include "LPC1768.h"

//#include "uart_17xx_40xx.h"

#define RESET_WIFI 1
#define UNUSED_ANGLE 45
#define ONE_SECOND 1000U
#define GMT -7

CTL_EVENT_SET_t clockEvent;


const char RESET_CHIP[] = "AT+RESTORE\r\n";
static const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
static const char* SEND = "AT+CIPSEND=48\r\n";
static const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n";
static const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};
static const char* MODE = "AT+CWMODE=1\r\n";
static const char* SSIDPWD = "AT+CWJAP=\"NETGEAR47\",\"phobicjungle712\"\r\n";
   

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
    
    
    // Enable RTC in NVIC
    NVIC_ISER0 |= RTC_IRQn;
    //NVIC_ISER0 |= 1<<17; //TODO clean this up
    
    // Set Counter Increment Interrupt Register to generate intterupt on increment of minute 
    CIIR |= CIIR_IMMIN;
	
}

// Real time clock interrupt handler
void RTC_IRQHandler(void)
{
    // Reset interrupt
    ILR |= ILR_RTCCIF;
    
}
    
void getNTPtime()//uint8_t *hour, uint8_t *min, uint8_t *sec)
{

    char timeString[9] = "00:00:00";
    uint8_t hour; 
    uint8_t min;
    uint8_t sec;
 
    uint8_t ntpTemp[48] = {0};
    
    #if RESET_WIFI
    //Select WIFI mode
    ESP_command(MODE, 1000U, 0);
    
    //Connect to WIFI
    ESP_command(SSIDPWD, 6000U, 0);
    #endif 
  
    //Connect to NTP Server
    ESP_command(NTP, 2000U, 0);
     
    //Interrogate NTP
    ESP_command(SEND, ONE_SECOND, 0);

    //Send NTP Packet
    ESP_command(NTP_PACKET, 2000U, 48);
    
    //Copy received NTP data from ring buffer
    ringBuffer1Copy(ntpTemp, 48); 
    
    Board_UARTPutChar('\n');
    Board_UARTPutChar('\n');
    
    //Calculate time from received NTP data
    calculateTime(ntpTemp, &hour, &min, &sec, timeString);

  
    //Print formatted time string
    //UART_send("The current time is: ", 21, 0);
    //UART_send(timeString, 9, 0);
    //UART_send("\n\n", 2, 0);

    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP, ONE_SECOND, 0);
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
//void showTime(void)
//{
//    //numberPos1 = 
    
//    if(HOUR == 0)
//    {
//            // left side of digit
//            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

//            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['3']
//            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

//            // middle of digit 
//            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
//            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

//            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

//            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
//            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

//            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
//            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

//            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
//            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
//            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

//            // right side of digit
//            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
//            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

//            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
//            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

//            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
//            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']
//        }
        
    
//}


void clock_thread(void *p)
{
    //RTC_TIME_T fullTime;
    
    // Generate clock matricies
    //uint16_t angleMatrix[15][8];
 
    ctl_events_init(&clockEvent, 0);
    
 
    // Initialise and start the RTC
    rtcInit();
    setTime(10, 5, 55);
    
    #if RESET_WIFI
    ESP_command(RESET_CHIP, ONE_SECOND, 0);
    #endif
    
    ctl_timeout_wait(ctl_current_time + 5000);
    getNTPtime();
    
    
    
    
    
 
    while(1)
    {
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        __asm volatile ("nop"); 
    }
}