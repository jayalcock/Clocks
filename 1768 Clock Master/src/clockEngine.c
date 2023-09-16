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
#include "clockData.h"
//#include "can_17xx_40xx.h"
#include "can.h"

//#include "uart_17xx_40xx.h"

#define RESET_WIFI      0
#define UNUSED_ANGLE    45
#define ONE_SECOND      1000U
#define GMT             -7
#define UNIX_EPOCH      2208988800UL //Seconds between 01Jan1900 and 01Jan1970 2207520000UL
#define SECONDS_PER_MIN 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400
#define CLOCK_COLUMNS   15
#define CLOCK_ROWS      8

// Offsets for each physical position in digital representation 
#define POS_A 1
#define POS_B 4
#define POS_C 8
#define POS_D 11

// CAN bus constants
#define POS_ID      0x200
#define POS_DL      5
#define SPEED_ID    0x201
#define SPEED_DL    5
#define ACCEL_ID    0x202
#define START_ID    0x203
#define FUNC_ID     0x204
#define ALL_CLOCKS  200

CTL_EVENT_SET_t clockEvent;

// Clock functions
enum clock_functions
{
    HOMECLOCKS,          
    
};    
  

static const char* RESET_CHIP = "AT+RESTORE\r\n";
static const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
static const char* SEND = "AT+CIPSEND=48\r\n";
static const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n"; 
static const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};
static const char* MODE = "AT+CWMODE=1\r\n";
static const char* SSIDPWD = "AT+CWJAP=\"NETGEAR47\",\"phobicjungle712\"\r\n";


/* Called in clockData.h
    // Clock hour representation of angular position
    static const uint16_t timeAngle[13] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0};
*/

// Initialise clock matrix - 1x hour, 1x minute
clockData clockMatrix[2] = {0};
    
static uint16_t clockTemp;  
                            
void writeClockValue(const uint8_t pos, const digitData *val)
{
    //uint8_t x_offset;
    
    /* Replaced by #define offsets
    // Determine offset in x direction given digit position
    if(pos == 0)
    {
        x_offset = 1;
    }
    else if (pos == 1)
    {
        x_offset = 4;
    }
    else if (pos == 2)
    {
        x_offset = 8;
    }
    else if (pos == 3)
    {
        x_offset = 11;
    }
    */
    
    // Populate clock matirx with desired position
    for(int k = 0; k < 2; k++) // hr/min
    {
        for(int i = 0; i < 3; i++) // column
        {
            for(int j = 0; j < 6; j++) // row
            {
                clockMatrix[k][i+pos][j+1] = val[k][i][j];
            }
        }
    } 
    
}
    

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

    // Print current time 
    Chip_UART_SendBlocking(LPC_UART0, "The current time is: ", 21);
    Chip_UART_SendBlocking(LPC_UART0, timeString, 9);
    Chip_UART_SendBlocking(LPC_UART0, "\n\n", 2);    
    

    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP, ONE_SECOND, 0);
}

// Calculates readable time value from NTP data
void calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char* timeString)
{
    char hourC[4], minC[4], secC[4];
    uint32_t timeInt;   
    uint32_t epoch;
    
    
    //Move time data from rx buffer into local variable for interpretation
    timeInt = dataBuffer[40] << 24 |  dataBuffer[41] << 16 | dataBuffer[42] << 8 | dataBuffer[43] << 0;

    epoch = timeInt - 2207520000UL; 
    *hour = epoch % SECONDS_PER_DAY / SECONDS_PER_HOUR;
    
    //Adjust for GMT
    if(*hour < abs(GMT))
    {
        *hour += 24;
    }
    *hour += GMT;
    *min = epoch % SECONDS_PER_HOUR / SECONDS_PER_MIN;
    *sec = epoch % SECONDS_PER_MIN;

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

void update_position(const uint8_t clockNum, const uint16_t minuteAngle, const uint16_t hourAngle, CTL_MESSAGE_QUEUE_t *msgQueuePtr)
{
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = POS_ID;
    sendMsgBuff.DLC = POS_DL;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = clockNum;
    sendMsgBuff.Data[1] = minuteAngle >> 8;
    sendMsgBuff.Data[2] = minuteAngle & 0xFF;
    sendMsgBuff.Data[3] = hourAngle >> 8;
    sendMsgBuff.Data[4] = hourAngle & 0xFF;

    ctl_message_queue_post(msgQueuePtr, &sendMsgBuff, CTL_TIMEOUT_NONE, 0);
    
    startCanTx();
}

// Update speed and direction of clocks via can bus
void update_speed_dir(const uint8_t clockNum, const uint8_t minuteSpeed, const uint8_t hourSpeed, const uint8_t minDir, const uint8_t hourDir, CTL_MESSAGE_QUEUE_t *msgQueuePtr)
{
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = SPEED_ID;
    sendMsgBuff.DLC = SPEED_DL;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = clockNum;
    sendMsgBuff.Data[1] = minuteSpeed;
    sendMsgBuff.Data[2] = hourSpeed;
    sendMsgBuff.Data[3] = minDir;
    sendMsgBuff.Data[4] = hourDir;

    ctl_message_queue_post(msgQueuePtr, &sendMsgBuff, CTL_TIMEOUT_NONE, 0);
    
    startCanTx();
}

// UStart movement of clocks via can bus
void start_movement(const uint8_t clockNum, CTL_MESSAGE_QUEUE_t *msgQueuePtr)
{
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = START_ID;
    sendMsgBuff.DLC = 1;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = clockNum;
    
    ctl_message_queue_post(msgQueuePtr, &sendMsgBuff, CTL_TIMEOUT_NONE, 0);
    
    startCanTx();
}

void trigger_slave_func(const uint8_t clockNum, const uint8_t funcNum, CTL_MESSAGE_QUEUE_t *msgQueuePtr)
{
    /* Function 1 - Home clocks
       Function 2 - 
    
    */
    
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = FUNC_ID;
    sendMsgBuff.DLC = 1;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = funcNum;
    
    ctl_message_queue_post(msgQueuePtr, &sendMsgBuff, CTL_TIMEOUT_NONE, 0);
    
    startCanTx();
}
    

void clock_thread(void *msgQueuePtr)
{

    uint8_t clockNode0 = 0;
    uint8_t clockNode1 = 1;
    uint8_t clockNode2 = 2;
    uint8_t clockNode3 = 3;
    
     
    ctl_events_init(&clockEvent, 0);
    
    
    trigger_slave_func(ALL_CLOCKS, HOMECLOCKS, msgQueuePtr);
    ctl_timeout_wait(ctl_get_current_time() + 3000);
 
 
    // Initialise and start the RTC
    rtcInit();
    setTime(10, 5, 55);
    
    #if RESET_WIFI
        ESP_command(RESET_CHIP, ONE_SECOND, 0);
        ctl_timeout_wait(ctl_current_time + 5000);
    #endif
    
    // Update time from NTP server
    getNTPtime();
    
    writeClockValue(POS_A, ONE);
    writeClockValue(POS_B, TWO);
    writeClockValue(POS_C, THREE);
    writeClockValue(POS_D, FOUR);
    
    uint16_t min0 = 180;
    uint16_t hour0 = 0;
    uint16_t min1 = 180;
    uint16_t hour1 = 0;
    uint16_t min2 = 180;
    uint16_t hour2 = 0;
    uint16_t min3 = 180;
    uint16_t hour3 = 0;
    uint16_t min4 = 180;
    uint16_t hour4 = 0;
    
    uint8_t speed0m = 3;
    uint8_t speed0h = 3;
    uint8_t speed1m = 3;
    uint8_t speed1h = 3;
    uint8_t speed2m = 3;
    uint8_t speed2h = 3;
    uint8_t speed3m = 3;
    uint8_t speed3h = 3;
    uint8_t dir0m = 0;
    uint8_t dir0h = 0;
    uint8_t dir1m = 0;
    uint8_t dir1h = 0;
    uint8_t dir2m = 0;
    uint8_t dir2h = 0;
    uint8_t dir3m = 0;
    uint8_t dir3h = 0;
    
    update_speed_dir(clockNode0, speed0m, speed0h, dir0m, dir0h, msgQueuePtr);
    update_speed_dir(clockNode1, speed1m, speed1h, dir1m, dir1h, msgQueuePtr);   
    update_speed_dir(clockNode2, speed2m, speed2h, dir2m, dir3h, msgQueuePtr);
    update_speed_dir(clockNode3, speed3m, speed3h, dir3m, dir3h, msgQueuePtr);   
    
 
    while(1)
    {
        //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        
        
        min0 += 45;
        hour0 += 45;
        min1 += 45;
        hour1 += 45;
        min2 += 45;
        hour2 += 45;
        min3 += 45;
        hour3 += 45;
        min4 += 45;
        hour4 += 45;
        
        
        //min0 = rand()/91;
        //hour0 = rand()/91;
        //min1 = rand()/91;
        //hour1 = rand()/91;
        //speed0m = rand()/6554.4;
        //speed0h = rand()/6553.4;
        //speed1m = rand()/6553.4;
        //speed1h = rand()/6553.4;
        
        //dir0m = rand()/16383.5;
        //dir0h = rand()/16383.5;
        //dir1m = rand()/16383.5;
        //dir1h = rand()/16383.5;
        
                    
        if(min0 >= 360)
        {
            min0 -= 360;
        }
        if(min0 <=0)
        {
            min0 += 360;
        }
        if(hour0 >= 360)
        {   
            hour0 -= 360;
        }
        if(hour0 <=0)
        {
            hour0 += 360;
        }
        
        if(min1 >= 360)
        {    
            min1 -= 360;
        }
        if(min1 <= 0)
        {
            min1 += 360;
        }
        
        if(hour1 >= 360)
        {    
            hour1 -= 360;
        }
        if(hour1 <= 0)
        {
            hour1 += 360;
        }
        
        if(min2 >= 360)
        {
            min2 -= 360;
        }
        if(min2 <= 0)
        {
            min2 += 360;
        }
        
        if(hour2 >= 360)
        {   
            hour2 -= 360;
        }
        if(hour2 <= 0)
        {
            hour2 += 360;
        }
        
        if(min3 >= 360)
        {    
            min3 -= 360;
        }
        if(min3 <= 0)
        {
            min3 += 360;
        }
        
        if(hour3 >= 360)
        {    
            hour3 -= 360;
        }
        if(hour3 <= 0)
        {
            hour3 += 360;
        }
        
        if(min4 >= 360)
        {    
            min4 -= 360;
        }
        if(min4 <= 0)
        {
            min4 += 360;
        }
        
        if(hour4 >= 360)
        {    
            hour4 -= 360;
        }
        if(hour4 <= 0)
        {
            hour4 += 360;
        }
            
        //update_speed_dir(clockNode0, speed0m, speed0h, dir0m, dir0h, msgQueuePtr);
        //update_speed_dir(clockNode1, speed1m, speed1h, dir1m, dir1h, msgQueuePtr);   
        //update_speed_dir(clockNode2, speed2m, speed2h, dir2m, dir3h, msgQueuePtr);
        //update_speed_dir(clockNode3, speed3m, speed3h, dir3m, dir3h, msgQueuePtr);   
            
        update_position(clockNode0, min0, hour0, msgQueuePtr);
        //ctl_timeout_wait(ctl_get_current_time() + 1);
        update_position(clockNode1, min1, hour1, msgQueuePtr);
        //ctl_timeout_wait(ctl_get_current_time() + 1);
        update_position(clockNode2, min2, hour2, msgQueuePtr);
        //ctl_timeout_wait(ctl_get_current_time() + 1);

        //start_movement(ALL_CLOCKS, msgQueuePtr);
        
        //ctl_timeout_wait(ctl_get_current_time() + 2000);
        
        
        update_position(clockNode3, min3, hour3, msgQueuePtr);
        
        //update_position(4, min4, hour4, msgQueuePtr);
        
        //update_position(clockNode0, min0, hour0, msgQueuePtr);
        //update_position(clockNode1, min1, hour1, msgQueuePtr);
        //update_position(clockNode2, min2, hour2, msgQueuePtr);  
        //update_position(clockNode3, min3, hour3, msgQueuePtr);

        //ctl_timeout_wait(ctl_get_current_time() + 1);
        //ctl_timeout_wait(ctl_get_current_time() + 10);
        start_movement(ALL_CLOCKS, msgQueuePtr);
        
        ctl_timeout_wait(ctl_get_current_time() + 2000);
       
        
    }
}