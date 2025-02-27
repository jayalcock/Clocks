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
#include "can.h"

#include "debugio.h"

// Clock control constants
#define RESET_WIFI      0
#define UNUSED_ANGLE    45
#define ONE_SECOND      1000U
#define GMT             -7
#define UNIX_EPOCH      2208988800UL //Seconds between 01Jan1900 and 01Jan1970 2207520000UL
#define SECONDS_PER_MIN 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

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
//#define ALL_CLOCKS  200

#define NUMBER_OF_SLAVES 10


#define CONTINUOUS_ROTATE 400

// Get the number of elements in any C array
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

/// Get number of columns in a 2D array
#define NUM_COLS(array_2d) ARRAY_LEN(array_2d)

/// Get number of rows in a 2D array
#define NUM_ROWS(array_2d) ARRAY_LEN(array_2d[0])


// Initialize clock event set
static CTL_EVENT_SET_t clockEvent;

//enum clock_enum
//{
//    CW,
//    CCW,
//    minuteArm,
//    hourArm,
//    bothArms,
//};

 

// ESP WIFI Commands
static const char* RESET_CHIP = "AT+RESTORE\r\n";
static const char* NTP = "AT+CIPSTART=\"UDP\",\"207.210.46.249\",123\r\n";
static const char* SEND = "AT+CIPSEND=48\r\n";
static const char* DISCONNECT_FROM_IP = "AT+CIPCLOSE\r\n"; 
static const uint8_t NTP_PACKET[48]={010,0,0,0,0,0,0,0,0};
static const char* MODE = "AT+CWMODE=1\r\n";
static const char* SSIDPWD = "AT+CWJAP=\"SloppyD\",\"phobicjungle712\"\r\n";

// Struct for storing clock data
typedef struct 
{
    uint16_t minuteAngle[COLUMNS][ROWS];
    uint16_t hourAngle[COLUMNS][ROWS];
    uint8_t minuteSpeed[COLUMNS][ROWS];
    uint8_t hourSpeed[COLUMNS][ROWS];
    Bool minuteDirection[COLUMNS][ROWS];
    Bool hourDirection[COLUMNS][ROWS];
    
} clockDataStruct;



/*****************************************************************************
 * Private functions
 ****************************************************************************/


 
 /* 
    @brief      Load clock matrix with specified digit representation
   
    @param      Pointer to clock matrix
    @param      Which position to update
    @param      Pointer to digit angle information
  
    @return     Nothing
    
*/
static void clock_matrix_number_load(clockDataStruct *clockMtxPtr, const uint8_t position, const digitData *numberPtr)
{
    const uint8_t y_offset = 1; // Offset to align digit correctly in y-axis
    
    for(size_t row = 0; row <  NUM_ROWS(*numberPtr); row++)
    {
        for(size_t col = 0; col < NUM_COLS(*numberPtr); col++)
        {
            clockMtxPtr->minuteAngle[col+position][row+y_offset] = numberPtr[0][col][row];
            clockMtxPtr->hourAngle[col+position][row+y_offset] = numberPtr[1][col][row];        
        }   
    }
    
}

/* 
    @brief  Load current time into clock matrix
    
    @param  Clock matrix pointer
  
    @return Nothing
    
*/

static void time_load_into_matrix(clockDataStruct *clockMtxPtr)
{
    uint8_t digitA, digitB, digitC, digitD, twoDigHr, twoDigMin;
    
    // Get actual time from RTC values
    twoDigHr = HOUR;
    twoDigMin = MIN;
    
    // Break hour number (HH) into two digits (H H)
    if(twoDigHr < 10)
    {
        digitA = 0;
        digitB = twoDigHr;
    }
    else if(twoDigHr < 20)
    {
        digitA = 1;
        digitB = twoDigHr - 10;
    }
    else
    {
        digitA = 2;
        digitB = twoDigHr - 20;
    }
    
    // Break minute number (MM) into two digits (M M)
    if(twoDigMin < 10)
    {
        digitC = 0;
        digitD = twoDigMin;
    }
    else if(twoDigMin < 20)
    {
        digitC = 1;
        digitD = twoDigMin - 10;
    }
    else
    {
        digitC = 2;
        digitD = twoDigMin - 20;
    }
    
    // Set all arms to "unused angle"
    for(uint8_t i = 0; i < ROWS; i++)
    {
        for(uint8_t j = 0; j < COLUMNS; j++)
        {
            clockMtxPtr->minuteAngle[j][i] = UNUSED_ANGLE;
            clockMtxPtr->hourAngle[j][i] = UNUSED_ANGLE;
        }
    }
    
    // Load each value from actual time into matrix for display    
    switch (digitA)
    {
        case 0:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(0));
            break;
        case 1:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(1));
            break;
        case 2:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(2));
            break;
        case 3:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(3));
            break;
        case 4:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(4));
            break;
        case 5:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(5));
            break;
        case 6:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(6));
            break;
        case 7:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(7));
            break;
        case 8:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(8));
            break;
        case 9:
            clock_matrix_number_load(clockMtxPtr, POS_A, get_digit_data(9));
            break;
    }
        
    switch (digitB)
    {
        case 0:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(0));
            break;
        case 1:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(1));
            break;
        case 2:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(2));
            break;
        case 3:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(3));
            break;
        case 4:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(4));
            break;
        case 5:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(5));
            break;
        case 6:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(6));
            break;
        case 7:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(7));
            break;
        case 8:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(8));
            break;
        case 9:
            clock_matrix_number_load(clockMtxPtr, POS_B, get_digit_data(9));
            break;
    }
        
    switch (digitC)
    {
        case 0:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(0));
            break;
        case 1:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(1));
            break;
        case 2:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(2));
            break;
        case 3:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(3));
            break;
        case 4:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(4));
            break;
        case 5:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(5));
            break;
        case 6:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(6));
            break;
        case 7:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(7));
            break;
        case 8:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(8));
            break;
        case 9:
            clock_matrix_number_load(clockMtxPtr, POS_C, get_digit_data(9));
            break;
    }

            
    switch (digitD)
    {
        case 0:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(0));
            break;
        case 1:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(1));
            break;
        case 2:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(2));
            break;
        case 3:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(3));
            break;
        case 4:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(4));
            break;
        case 5:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(5));
            break;
        case 6:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(6));
            break;
        case 7:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(7));
            break;
        case 8:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(8));
            break;
        case 9:
            clock_matrix_number_load(clockMtxPtr, POS_D, get_digit_data(9));
            break;
    }
    
}
    
/* 
    @brief  Initialise matrix to zeros
    
    @param  Clock matrix pointer
  
    @return Nothing
    
*/    
static void matrix_initialise(clockDataStruct *clockMtxPtr)
{
    
    for(size_t row = 0; row <  ROWS; row++)
    {
        for(size_t col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteAngle[col][row] = 0;
            clockMtxPtr->hourAngle[col][row] = 0;
            clockMtxPtr->minuteSpeed[col][row] = 0;
            clockMtxPtr->hourSpeed[col][row] = 0; 
            clockMtxPtr->minuteDirection[col][row] = 0;
            clockMtxPtr->hourDirection[col][row] = 0;         
        }   
    }
}

    
// Prints time from RTC 
static void time_printf(void)
{
    debug_printf("%d:%d:%d\n", HOUR, MIN, SEC);
}

// Sets time
static void RTC_time_set(uint8_t hour, uint8_t min, uint8_t sec)
{
    HOUR = hour;
    MIN = min;
    SEC = sec; 

}

// Initialises RTC
static void rtc_init(void)
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
    ctl_enter_isr();
    
    // Reset interrupt
    ILR |= ILR_RTCCIF;
    
    ctl_exit_isr();
    
}
 
// Calculates readable time value from NTP data
static void NTP_conversion(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char* timeString)
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
   
static void uart_ntp_rx()//uint8_t *hour, uint8_t *min, uint8_t *sec)
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
    NTP_conversion(ntpTemp, &hour, &min, &sec, timeString);

    // Print current time 
    Chip_UART_SendBlocking(LPC_UART0, "The current time is: ", 21);
    Chip_UART_SendBlocking(LPC_UART0, timeString, 9);
    Chip_UART_SendBlocking(LPC_UART0, "\n\n", 2);    
    

    //Disconnect from NTP
    ESP_command(DISCONNECT_FROM_IP, ONE_SECOND, 0);
}

// Send updated position of slave clocks over CAN bus
static void slave_position_tx(const uint8_t clockNum, const uint16_t minuteAngle, const uint16_t hourAngle)
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
    
    startCanTx(&sendMsgBuff);
}

// Send updated speed and direction of clocks via can bus
static void slave_speed_direction_tx(const uint8_t clockNum, const uint8_t minuteSpeed, const uint8_t hourSpeed, const uint8_t minDir, const uint8_t hourDir)
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
    
    startCanTx(&sendMsgBuff);
}

// Send start movement command via can bus
static void motion_start_tx(const uint8_t clockNum)
{
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = START_ID;
    sendMsgBuff.DLC = 1;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = clockNum;
        
    startCanTx(&sendMsgBuff);
}

//TODO TEST THIS FUNCTION
static void matrix_update_clock_angle(clockDataStruct *clockMtxPtr, const uint8_t *clockPosRowPtr, const uint8_t *clockPosColPtr, const bool *armPtr, const uint16_t *anglePtr)
{
    if(*clockPosRowPtr == ALLCLOCKS)
    {
        if(*armPtr == MINUTEARM)
        {
            for(size_t row = 0; row <  ROWS; row++)
            {
                for(size_t col = 0; col < COLUMNS; col++)
                {
                    clockMtxPtr->minuteAngle[col][row] = *anglePtr;
                        
                }   
            }
        }
        if(*armPtr == HOURARM)
        {
            for(size_t row = 0; row <  ROWS; row++)
            {
                for(size_t col = 0; col < COLUMNS; col++)
                {
                    clockMtxPtr->hourAngle[col][row] = *anglePtr;
                        
                }   
            }
        }    
         
     }
     else
     {
        if(*armPtr == MINUTEARM)
        {
            clockMtxPtr->minuteAngle[*clockPosColPtr][*clockPosRowPtr] = *anglePtr;
        }
        if(*armPtr == HOURARM)
        {
            clockMtxPtr->hourAngle[*clockPosColPtr][*clockPosRowPtr] = *anglePtr;
        }
    }
    
}

// Trigger a function in slave node
static void slave_function_trigger_tx(const uint8_t clockNum, const uint8_t funcNum)
{
    /* Function 1 - Home clocks
       Function 2 - Velocity control 
       Function 3 - Position control 
    
    */
    
    CAN_MSG_T sendMsgBuff;
    
    sendMsgBuff.ID = FUNC_ID;
    sendMsgBuff.DLC = 1;
    sendMsgBuff.Type = 0;
    sendMsgBuff.Data[0] = funcNum;
        
    startCanTx(&sendMsgBuff);
}

static void pattern_continuous_rotation(clockDataStruct *clockMtxPtr, const uint8_t speed, const uint8_t direction, CTL_MESSAGE_QUEUE_t *msgQueuePtr)
{

    // Load continuous rotation angle into matrix
    for(uint8_t row = 0; row <  ROWS; row++)
    {
        for(uint8_t col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteAngle[col][row] = CONTINUOUS_ROTATE;
            clockMtxPtr->hourAngle[col][row] = CONTINUOUS_ROTATE;        
        }   
    }
    
    // Update speed
    for(uint8_t row = 0; row <  ROWS; row++)
    {
        for(size_t col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteSpeed[col][row] = speed;
            clockMtxPtr->hourSpeed[col][row] = speed;        
        }   
    }
    
    // Update direction
    for(uint8_t row = 0; row <  ROWS; row++)
    {
        for(int col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteDirection[col][row] = direction;
            clockMtxPtr->hourDirection[col][row] = direction;        
        }   
    }
    
    // Send data to slaves
    for(uint8_t i = 0; i < NUMBER_OF_SLAVES; i++)
    {
        //slave_speed_direction_tx(i, clockMtxPtr->minuteSpeed[0][i], clockMtxPtr->hourSpeed[0][i], clockMtxPtr->minuteDirection[0][i], clockMtxPtr->hourDirection[0][i], msgQueuePtr);
        slave_position_tx(i, clockMtxPtr->minuteAngle[0][i], clockMtxPtr->minuteAngle[0][i]);
    }
    
    slave_function_trigger_tx(0, VEL_CONTROL);
    
    // Start motion
    motion_start_tx(ALLCLOCKS);   
    
}

static void position_reset(clockDataStruct *clockMtxPtr)
{
    for(uint8_t row = 0; row <  ROWS; row++)
    {
        for(uint8_t col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteAngle[col][row] = DEFAULT_ANGLE_MIN;
            clockMtxPtr->hourAngle[col][row] = DEFAULT_ANGLE_HR;        
        }   
    }
    // Send data to slaves
    for(uint8_t i = 0; i < NUMBER_OF_SLAVES; i++)
    {
        slave_position_tx(i, clockMtxPtr->minuteAngle[0][i], clockMtxPtr->hourAngle[0][i]);  
    }
    
    
}

void test_routine(clockDataStruct *clockMtxPtr)
{
      
    static uint16_t min0, min1, min2, min3, hour0, hour1, hour2, hour3;
    
    min0 += 45;
    hour0 += 45;
    min1 += 45;
    hour1 += 45;
    min2 += 45;
    hour2 += 45;
    min3 += 45;
    hour3 += 45;
        
                    
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
    

    
    clockMtxPtr->minuteAngle[0][0] = min0;
    clockMtxPtr->minuteAngle[0][1] = min1;
    clockMtxPtr->minuteAngle[0][2] = min2;
    clockMtxPtr->minuteAngle[0][3] = min3;
    clockMtxPtr->hourAngle[0][0] = hour0;
    clockMtxPtr->hourAngle[0][1] = hour1;
    clockMtxPtr->hourAngle[0][2] = hour2;
    clockMtxPtr->hourAngle[0][3] = hour3;
    
    
    
    
    //clockMtxPtr->minuteSpeed[0][0] = speed0m;
    //clockMtxPtr->minuteSpeed[0][1] = speed1m;
    //clockMtxPtr->minuteSpeed[0][2] = speed2m;
    //clockMtxPtr->minuteSpeed[0][3] = speed3m;
    //clockMtxPtr->hourSpeed[0][0] = speed0h;
    //clockMtxPtr->hourSpeed[0][1] = speed1h;
    //clockMtxPtr->hourSpeed[0][2] = speed2h;
    //clockMtxPtr->hourSpeed[0][3] = speed3h;
    
    slave_position_tx(0, clockMtxPtr->minuteAngle[0][0], clockMtxPtr->hourAngle[0][0]);
    motion_start_tx(0); 
    ctl_timeout_wait(ctl_get_current_time() + 150);
    slave_position_tx(1, clockMtxPtr->minuteAngle[0][1], clockMtxPtr->hourAngle[0][1]);
    motion_start_tx(1); 
    ctl_timeout_wait(ctl_get_current_time() + 150);
    slave_position_tx(2, clockMtxPtr->minuteAngle[0][2], clockMtxPtr->hourAngle[0][2]);
    motion_start_tx(2); 
    ctl_timeout_wait(ctl_get_current_time() + 150);
    slave_position_tx(3, clockMtxPtr->minuteAngle[0][3], clockMtxPtr->hourAngle[0][3]);
    motion_start_tx(3); 
    ctl_timeout_wait(ctl_get_current_time() + 150);
    
}


void clock_main_thread(void *msgQueuePtr)
{
    
    // Initialise clock matrix - 1x hour, 1x minute
    static clockDataStruct clockMatrix;
    uint16_t clockNum = 0;
    
    // Initialise event set
    ctl_events_init(&clockEvent, 0);
    
    // Initialise matrix to zeors
    matrix_initialise(&clockMatrix);  
    
    // Home clocks
    slave_function_trigger_tx(ALLCLOCKS, HOME_CLOCKS); 
 
    // Initialise and start the RTC
    rtc_init();
    
    RTC_time_set(9, 35, 55);
    
    
    
    #if RESET_WIFI
        ESP_command(RESET_CHIP, ONE_SECOND, 0);
        ctl_timeout_wait(ctl_current_time + 5000);
    #endif
    
    //TODO get time sync working
    // Update time from NTP server
    //uart_ntp_rx();
    
    //ctl_timeout_wait(ctl_get_current_time() + 10000);
    
    position_reset(&clockMatrix);
    
    motion_start_tx(ALLCLOCKS);
    
    ctl_timeout_wait(ctl_get_current_time() + 3000);
    
    time_load_into_matrix(&clockMatrix);
    
    for(uint8_t i = 0; i < ROWS; i++)
    {
        for(uint8_t j = 0; j < COLUMNS; j++)
        {
            slave_position_tx(clockNum++, clockMatrix.minuteAngle[j][i], clockMatrix.hourAngle[j][i]);
        }
    }
    //clockNum = 0;
    
    
    motion_start_tx(ALLCLOCKS);
    
    
    
    //pattern_continuous_rotation(&clockMatrix, 2, CW, msgQueuePtr);  
    
    while(1)
    {
        //ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, 0x0, CTL_TIMEOUT_NONE, 0);
        
        //test_routine(&clockMatrix);
          
        
    
        
        ctl_timeout_wait(ctl_get_current_time() + 1000);
           
    }
}