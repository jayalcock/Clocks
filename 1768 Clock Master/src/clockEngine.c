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
#include "can_17xx_40xx.h"
//#include "can.h"

//#include "uart_17xx_40xx.h"

#define RESET_WIFI 0
#define UNUSED_ANGLE 45
#define ONE_SECOND 1000U
#define GMT -7
#define UNIX_EPOCH 2208988800UL //Seconds between 01Jan1900 and 01Jan1970 2207520000UL
#define SECONDS_PER_MIN 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400
#define CLOCK_COLUMNS 15
#define CLOCK_ROWS 8

// Offsets for each physical position in digital representation 
#define POS_A 1
#define POS_B 4
#define POS_C 8
#define POS_D 11


CTL_EVENT_SET_t clockEvent;
  

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

//send position data to relevant mcu over CAN
void sendData(void)
{
    uint16_t remoteID = 0x200;
    
    CAN_MSG_T sendMsgBuff;
    CAN_BUFFER_ID_T txBuff;
   
    sendMsgBuff.ID = remoteID;
    sendMsgBuff.DLC = 4;
    sendMsgBuff.Type = 0;
    //sendMsgBuff.Data[0] = clockMatrix[0][6][0];
    sendMsgBuff.Data[0] = clockTemp;
    sendMsgBuff.Data[1] = clockMatrix[0][6][1];
    sendMsgBuff.Data[2] = clockMatrix[0][6][2];
    sendMsgBuff.Data[3] = clockMatrix[0][6][3];
    //sendMsgBuff.Data[0] = 'A';
    //sendMsgBuff.Data[1] = 'B';
    //sendMsgBuff.Data[2] = 'C';
    //sendMsgBuff.Data[3] = 'D';
    txBuff = Chip_CAN_GetFreeTxBuf(LPC_CAN1);
    
    //sendToCAN(sendMsgBuff);
    //Chip_CAN_Send(LPC_CAN1, txBuff, &sendMsgBuff);

}
    
                            
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


void clock_thread(void *p)
{
    //RTC_TIME_T fullTime;
    
    clockTemp = 0;
    
    // Generate clock matricies
    //uint16_t angleMatrix[15][8];
 
    ctl_events_init(&clockEvent, 0);
    
 
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
 
    while(1)
    {
        //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        ctl_timeout_wait(ctl_get_current_time() + 1000);
        sendData();
        
        clockTemp += 10;
        
    }
}