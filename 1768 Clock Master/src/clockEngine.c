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

#define NUMBER_OF_SLAVES 120


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
    const digitData* digitCache[10];  // Cache for digit representations
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
    const uint8_t numRows = NUM_ROWS(*numberPtr);
    const uint8_t numCols = NUM_COLS(*numberPtr);
    
    // Pre-calculate position offsets
    const uint8_t positionOffset = position;
    
    for(uint8_t row = 0; row < numRows; row++)
    {
        const uint8_t rowWithOffset = row + y_offset;
        
        for(uint8_t col = 0; col < numCols; col++)
        {
            const uint8_t colWithOffset = col + positionOffset;
            
            // Direct indexing is faster than multiple array dereferences
            clockMtxPtr->minuteAngle[colWithOffset][rowWithOffset] = numberPtr[0][col][row];
            clockMtxPtr->hourAngle[colWithOffset][rowWithOffset] = numberPtr[1][col][row];        
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
    // Get actual time from RTC values
    const uint8_t twoDigHr = HOUR;
    const uint8_t twoDigMin = MIN;
    
    // Break hour number (HH) into two digits (H H) - optimized calculations
    const uint8_t digitA = twoDigHr / 10;
    const uint8_t digitB = twoDigHr % 10;
    const uint8_t digitC = twoDigMin / 10;
    const uint8_t digitD = twoDigMin % 10;
    
    // Fast memory clear for unused arms - use memset for larger blocks
    for(uint8_t i = 0; i < ROWS; i++) {
        for(uint8_t j = 0; j < COLUMNS; j++) {
            clockMtxPtr->minuteAngle[j][i] = UNUSED_ANGLE;
            clockMtxPtr->hourAngle[j][i] = UNUSED_ANGLE;
        }
    }
    
    // Load each value from actual time into matrix for display - using pre-cached digits
    clock_matrix_number_load(clockMtxPtr, POS_A, clockMtxPtr->digitCache[digitA]);
    clock_matrix_number_load(clockMtxPtr, POS_B, clockMtxPtr->digitCache[digitB]);
    clock_matrix_number_load(clockMtxPtr, POS_C, clockMtxPtr->digitCache[digitC]);
    clock_matrix_number_load(clockMtxPtr, POS_D, clockMtxPtr->digitCache[digitD]);
}
    
/* 
    @brief  Initialise matrix to zeros
    
    @param  Clock matrix pointer
  
    @return Nothing
    
*/    
static void matrix_initialise(clockDataStruct *clockMtxPtr)
{
    // Initialize angle and speed matrices
    for(uint8_t row = 0; row < ROWS; row++)
    {
        for(uint8_t col = 0; col < COLUMNS; col++)
        {
            clockMtxPtr->minuteAngle[col][row] = 0;
            clockMtxPtr->hourAngle[col][row] = 0;
            clockMtxPtr->minuteSpeed[col][row] = 0;
            clockMtxPtr->hourSpeed[col][row] = 0; 
            clockMtxPtr->minuteDirection[col][row] = 0;
            clockMtxPtr->hourDirection[col][row] = 0;         
        }   
    }
    
    // Populate the digit cache
    for(uint8_t digit = 0; digit <= 9; digit++) {
        clockMtxPtr->digitCache[digit] = get_digit_data(digit);
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

// Optimized function to batch-send position updates to multiple clocks
static void slave_position_batch_tx(const clockDataStruct *clockMtxPtr, uint8_t startClockNum, uint8_t endClockNum)
{
    CAN_MSG_T sendMsgBuff;
    const uint32_t batchDelay = 5; // Small delay between messages to prevent bus saturation
    
    sendMsgBuff.ID = POS_ID;
    sendMsgBuff.DLC = POS_DL;
    sendMsgBuff.Type = 0;
    
    // Limit to valid range
    if (endClockNum > NUMBER_OF_SLAVES - 1) {
        endClockNum = NUMBER_OF_SLAVES - 1;
    }
    
    // Send position data for a group of clocks with minimal bus overhead
    for (uint8_t i = startClockNum; i <= endClockNum; i++) {
        uint8_t col = i % COLUMNS;
        uint8_t row = i / COLUMNS;
        
        sendMsgBuff.Data[0] = i;
        sendMsgBuff.Data[1] = clockMtxPtr->minuteAngle[col][row] >> 8;
        sendMsgBuff.Data[2] = clockMtxPtr->minuteAngle[col][row] & 0xFF;
        sendMsgBuff.Data[3] = clockMtxPtr->hourAngle[col][row] >> 8;
        sendMsgBuff.Data[4] = clockMtxPtr->hourAngle[col][row] & 0xFF;
        
        startCanTx(&sendMsgBuff);
        
        // Small delay to prevent bus saturation
        if (i < endClockNum) {
            ctl_timeout_wait(ctl_get_current_time() + batchDelay);
        }
    }
}

// Optimized function to send start commands to a range of clocks
static void motion_start_batch_tx(uint8_t startClockNum, uint8_t endClockNum)
{
    CAN_MSG_T sendMsgBuff;
    const uint32_t batchDelay = 2; // Smaller delay for simpler commands
    
    sendMsgBuff.ID = START_ID;
    sendMsgBuff.DLC = 1;
    sendMsgBuff.Type = 0;
    
    // Limit to valid range
    if (endClockNum > NUMBER_OF_SLAVES - 1) {
        endClockNum = NUMBER_OF_SLAVES - 1;
    }
    
    // Send start commands to a group of clocks
    for (uint8_t i = startClockNum; i <= endClockNum; i++) {
        sendMsgBuff.Data[0] = i;
        startCanTx(&sendMsgBuff);
        
        // Small delay to prevent bus saturation
        if (i < endClockNum) {
            ctl_timeout_wait(ctl_get_current_time() + batchDelay);
        }
    }
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

// Helper function to normalize an angle to keep it within 0-359 degrees
static inline uint16_t normalize_angle(int16_t angle)
{
    // Using modulo to bring the value in range
    int16_t normalized = angle % 360;
    
    // Handle negative angles
    if (normalized < 0) {
        normalized += 360;
    }
    
    return (uint16_t)normalized;
}

void test_routine(clockDataStruct *clockMtxPtr)
{
    static uint16_t min0, min1, min2, min3, hour0, hour1, hour2, hour3;
    
    // Increment all angles by 45 degrees
    min0 = normalize_angle(min0 + 45);
    hour0 = normalize_angle(hour0 + 45);
    min1 = normalize_angle(min1 + 45);
    hour1 = normalize_angle(hour1 + 45);
    min2 = normalize_angle(min2 + 45);
    hour2 = normalize_angle(hour2 + 45);
    min3 = normalize_angle(min3 + 45);
    hour3 = normalize_angle(hour3 + 45);
    
    // Update the clock matrix with the new angles
    clockMtxPtr->minuteAngle[0][0] = min0;
    clockMtxPtr->minuteAngle[0][1] = min1;
    clockMtxPtr->minuteAngle[0][2] = min2;
    clockMtxPtr->minuteAngle[0][3] = min3;
    clockMtxPtr->hourAngle[0][0] = hour0;
    clockMtxPtr->hourAngle[0][1] = hour1;
    clockMtxPtr->hourAngle[0][2] = hour2;
    clockMtxPtr->hourAngle[0][3] = hour3;
    
    // Send positions to clocks and start motion with optimized delays between each command
    const uint32_t delay = 150;
    for (uint8_t i = 0; i < 4; i++) {
        slave_position_tx(i, clockMtxPtr->minuteAngle[0][i], clockMtxPtr->hourAngle[0][i]);
        motion_start_tx(i);
        ctl_timeout_wait(ctl_get_current_time() + delay);
    }
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