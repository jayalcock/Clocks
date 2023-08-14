#include <stdint.h>
#include "ccand_11xx.h"
#include "clockDriverRT.h"
#include "board.h"
#include "ctl_api.h"
#include "timer_11xx.h"
#include "clock_11xx.h"
#include "ccan_rom.h"
#include "gpio_11xx_2.h"
#include "ring_buffer.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
// Test variables
#define TESTING 0
#define FIRSTCLOCK 0
#define NUMBEROFCLOCKS 4
#define HIRESTIMER 1

// System constants
#define NUMBEROFARMS    2
#define STEPSIZE        12 // 1/12 degree per step
#define PULSEWIDTH      10
#define RESETPORT       0
#define RESETPIN        1

// Clock numbers
#define HOMEPOSHOUR     0
#define HOMEPOSMIN      180

// Clock specific trigger constants
#define RUN_ALL_CLOCKS  1<<0
#define RUN_CLOCK0_MIN  1<<1
#define RUN_CLOCK0_HOUR 1<<2
#define RUN_CLOCK1_MIN  1<<3
#define RUN_CLOCK1_HOUR 1<<4
#define RUN_CLOCK2_MIN  1<<5
#define RUN_CLOCK2_HOUR 1<<6
#define RUN_CLOCK3_MIN  1<<7
#define RUN_CLOCK3_HOUR 1<<8
#define HOME_CLOCKS     1<<9
#define CLOCK0_MIN_AT_POS   1<<10
#define CLOCK0_HOUR_AT_POS  1<<11
#define CLOCK1_MIN_AT_POS   1<<12
#define CLOCK1_HOUR_AT_POS  1<<13
#define CLOCK2_MIN_AT_POS   1<<14
#define CLOCK2_HOUR_AT_POS  1<<15
#define CLOCK3_MIN_AT_POS   1<<16
#define CLOCK4_HOUR_AT_POS  1<<17

// Clock control event trigger constsnts
//#define UPDATE_ALL_CLOCKS   1<<0
//#define UPDATE_CLOCK0_MIN   1<<1
//#define UPDATE_CLOCK0_HOUR  1<<2
//#define UPDATE_CLOCK1_MIN   1<<3
//#define UPDATE_CLOCK1_HOUR  1<<4
//#define UPDATE_CLOCK2_MIN   1<<5
//#define UPDATE_CLOCK2_HOUR  1<<6
//#define UPDATE_CLOCK3_MIN   1<<7
//#define UPDATE_CLOCK3_HOUR  1<<8
#define CAN_UPDATE          1<<9


// Clock Homing event trigger constants
#define HOMING_ACTIVE       1<<0
#define CLOCK0_MIN_HOME     1<<1
#define CLOCK0_HOUR_HOME    1<<2
#define CLOCK1_MIN_HOME     1<<3
#define CLOCK1_HOUR_HOME    1<<4
#define CLOCK2_MIN_HOME     1<<5
#define CLOCK2_HOUR_HOME    1<<6
#define CLOCK3_MIN_HOME     1<<7
#define CLOCK3_HOUR_HOME    1<<8



// Can message types
#define POSITION        0x200 
#define SPEED           0x201 
#define ACCELERATION    0x202 
#define STARTMOTION     0x203 
#define TRIGGERFUNC     0x204

// Clock functions
#define HOMECLOCKS      1

// Clock Numbers
enum clock_numbers
{
    CLOCK0,          
    CLOCK1,          
    CLOCK2,          
    CLOCK3,          
    ALLCLOCKS,
    MINUTEARM, 
    HOURARM, 
    BOTHARMS,
    
};   

enum arm_direction
{
    CW,
    CCW,
};

enum start_stop
{
    STOP, 
    START,
};

// Control modes
enum control_mode
{
    POS_CTRL,
    VEL_CTRL,
};

// Initialise objects
// Events
CTL_EVENT_SET_t clockControlEvent, clockHomeEvent, clockEvent; //clock0Event, clock1Event, clock2Event, clock3Event, ;
// Messages
CCAN_MSG_OBJ_T can_RX_data;

// Buffer
#define RXBUFFSIZE 5
RINGBUFF_T rxBuffer;
CCAN_MSG_OBJ_T rx_buffer[RXBUFFSIZE];


// File scope variables
const uint16_t speed[] = {100, 200, 400, 800, 1600, 3200};
uint32_t timerFreq;
uint8_t localControl = 0;

uint8_t homingSpeed = 2;
volatile uint8_t homingDir = 0;
uint8_t homingBit = 0;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

// Population of motor data structs with initial values
motorStruct motorData[] =
{
    {0, //clock num
        //hour
        { 0, 3, // pulse port/pin 
          0, 2, // direction port/pin 
          2, 2, // hall port/pin 
          0,    // angle
          0,    // desired angle 
          0,    // direction
          0,    // start motion
          0,    // steps
          0,    // remaining steps
          1,    // at position
          2,    // speed
          2,    // accel
          0},    // pos/vel control mode

        //minute
        { 0, 5, // pulse port/pin
          0, 4, // direction port/pin
          2, 1, // hall port/pin
          0,    // angle
          0,    // desired angle
          0,    // direction
          0,    // start motion
          0,    // steps
          0,    // remaining steps
          1,    // at position
          2,    // speed
          2,    // accel
          0 }}, // pos/vel control mode
                
    {1, //clock num
        {0, 7, 0, 6, 2, 6, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0}, //hour
        {0, 9, 0, 8, 2, 3, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0}}, //minute       
                            
    {2, //clock num
        {1, 5, 1, 4, 2, 8, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0}, //hour
        {1, 7, 1, 6, 2, 7, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0}}, //minute
                     
    {3, //clock num
        {1, 10, 1, 8, 2, 11, 0, 0, 0, 0, 0, 1, 2, 2, 0}, //hour
        {2, 0, 1, 11, 2, 10, 0, 0, 0, 0, 0, 1, 2, 2, 0}}, //minute   
          
};

/*****************************************************************************
 * Private functions
 ****************************************************************************/
 
 /*
 Testing function 

 */
#if TESTING
void clock_testing(void)
{
    localControl = 1;
    
    motorData[CLOCK0].min.angle = 0;
    motorData[CLOCK0].hour.angle = 0;
    motorData[CLOCK1].min.angle = 0;
    motorData[CLOCK1].hour.angle = 0;
    motorData[CLOCK2].min.angle = 0;
    motorData[CLOCK2].hour.angle = 0;
    motorData[CLOCK3].min.angle = 0;
    motorData[CLOCK3].hour.angle = 0;
    
    motorData[CLOCK0].min.angleDesired = 45;
    motorData[CLOCK0].hour.angleDesired = 45;
    motorData[CLOCK1].min.angleDesired = 45;
    motorData[CLOCK1].hour.angleDesired = 45;
    motorData[CLOCK2].min.angleDesired = 45;
    motorData[CLOCK2].hour.angleDesired = 45;
    motorData[CLOCK3].min.angleDesired = 45;
    motorData[CLOCK3].hour.angleDesired = 45;

}
#endif
  

/* 
  @brief    32-bit timer 0 interrupt handler - minutre arm speed control 
  
  @return   Nothing

  @note     Iterrupt routine is called if any of the 4 timer matches are met.
            Each match represents one clock arm. 
            Match value is set by the desired speed for the particular arm + timer current value. 
*/
#if HIRESTIMER
// Interrupt handler for 32-bit timer 0 - Controlling minute arm speeds
void CT32B0_IRQHandler(void)
{
    // Clock 0 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, CLOCK0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, CLOCK0);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, CLOCK0, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK0].min.speed]);
        
        if(motorData[CLOCK0].min.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_MIN, 0);

    }
    // Clock 1 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK1].min.speed]);
        
        if(motorData[CLOCK1].min.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_MIN, 0);

    }
    // Clock 2 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK2].min.speed]);
        
        if(motorData[CLOCK2].min.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_MIN, 0);

    }
    // Clock 3 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK3].min.speed]);
        
        if(motorData[CLOCK3].min.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_MIN, 0);

    }  
} 


/* 
  @brief    32-bit timer 1 interrupt handler - hour arm speed control 
  
  @return   Nothing

  @note     Iterrupt routine is called if any of the 4 timer matches are met.
            Each match represents one clock arm. 
            Match value is set by the desired speed for the particular arm + timer current value. 
*/
void CT32B1_IRQHandler(void)
{
    // Clock 0 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, CLOCK0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, CLOCK0);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, CLOCK0, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK0].hour.speed]);
        
        if(motorData[CLOCK0].hour.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK1].hour.speed]);
        
        if(motorData[CLOCK1].hour.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK2].hour.speed]);
        
        if(motorData[CLOCK2].hour.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK3].hour.speed]);
        
        if(motorData[CLOCK3].hour.start == START)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_HOUR, 0);

    }
}  


#else 
/* 
  16-bit timer 0 interrupt handler - Minute arm speed control - Backup in case of any 32-bit timer issues

  Iterrupt routine is called if any of the 4 timer matches are met.
  Each match represents one clock arm. 
  Match value is set by the desired speed for the particular arm + timer current value. 
*/
void CT16B0_IRQHandler(void)
{
    // Clock 0 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, CLOCK0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK0);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK0, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK0].min.speed]);
        
        if(motorData[CLOCK0].min.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_MIN, 0);

    }
    // Clock 1 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK1].min.speed]);
        
        if(motorData[CLOCK1].min.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_MIN, 0);

    }
    // Clock 2 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK2].min.speed]);
        
        if(motorData[CLOCK2].min.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_MIN, 0);

    }
    // Clock 3 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0,CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK3].min.speed]);
        
        if(motorData[CLOCK3].min.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_MIN, 0);

    }
  
}  

/* 
  16-bit timer 0 interrupt handler - Hour arm speed control - Backup in case of any 32-bit timer issues

  Iterrupt routine is called if any of the 4 timer matches are met.
  Each match represents one clock arm. 
  Match value is set by the desired speed for the particular arm + timer current value. 
*/
void CT16B1_IRQHandler(void)
{
    // Clock 0 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK0);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK0, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK0].hour.speed]);
        
        if(motorData[CLOCK0].hour.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK1].hour.speed]);
        
        if(motorData[CLOCK1].hour.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK2].hour.speed]);
        
        if(motorData[CLOCK2].hour.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK3].hour.speed]);
        
        if(motorData[CLOCK3].hour.start == 1)
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_HOUR, 0);

    }
  
}   
#endif


/*
    @brief      Start/stop clock

    @param      clockNum: clock number to modify
    @param      arm:  arm to modify
    @param      start:  start/stop

    @return     Nothing
*/

void set_start_stop(const uint8_t clockNum, const uint8_t arm, const uint8_t runCmd)
{
    if(clockNum == ALLCLOCKS)
    {
        motorData[CLOCK0].min.start = runCmd;
        motorData[CLOCK0].hour.start = runCmd;
        motorData[CLOCK1].min.start = runCmd;
        motorData[CLOCK1].hour.start = runCmd;
        motorData[CLOCK2].min.start = runCmd;
        motorData[CLOCK2].hour.start = runCmd;
        motorData[CLOCK3].min.start = runCmd;
        motorData[CLOCK3].hour.start = runCmd;
        
        if(runCmd == START)
        {
            //ctl_events_set_clear(&clockControlEvent, RUN_ALL_CLOCKS, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_HOUR, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK0_MIN, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_HOUR, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK1_MIN, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_HOUR, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK2_MIN, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_HOUR, 0);
            ctl_events_set_clear(&clockEvent, RUN_CLOCK3_MIN, 0);
        }
    }
    
    else 
    {
        if(arm == MINUTEARM | arm == BOTHARMS)
        {
            motorData[clockNum].min.start = runCmd;
        }
        if(arm == HOURARM | arm == BOTHARMS)
        {
            motorData[clockNum].hour.start = runCmd;
        }
    }

    
}

/*
    @brief    GPIO IRQ handler for handling hall effect triggers. 
    
    @return   Nothing

*/
void GPIO2_IRQHandler(void)
{
    uint16_t triggeredPins;
    
    // Determine which halls have been triggered
    triggeredPins = Chip_GPIO_GetMaskedInts(LPC_GPIO, motorData[CLOCK0].min.hallPort); 
    
    if(triggeredPins & 1 << motorData[CLOCK0].min.hallPin)
    {
         ctl_events_set_clear(&clockHomeEvent, CLOCK0_MIN_HOME, 0);
         set_start_stop(CLOCK0, MINUTEARM, STOP);
    }
    
    if(triggeredPins & 1 << motorData[CLOCK0].hour.hallPin)
    {
        ctl_events_set_clear(&clockHomeEvent, CLOCK0_HOUR_HOME, 0);
        set_start_stop(CLOCK0, HOURARM, STOP);
    }
    
    if(triggeredPins & 1 << motorData[CLOCK1].min.hallPin)
    {
         ctl_events_set_clear(&clockHomeEvent, CLOCK1_MIN_HOME, 0);
         set_start_stop(CLOCK1, MINUTEARM, STOP);
    }
    if(triggeredPins & 1 << motorData[CLOCK1].hour.hallPin)
    {
        ctl_events_set_clear(&clockHomeEvent, CLOCK1_HOUR_HOME, 0);
        set_start_stop(CLOCK1, HOURARM, STOP);
    }
    
    if(triggeredPins & 1 << motorData[CLOCK2].min.hallPin)
    {
         ctl_events_set_clear(&clockHomeEvent, CLOCK2_MIN_HOME, 0);
         set_start_stop(CLOCK2, MINUTEARM, STOP);
    }
    if(triggeredPins & 1 << motorData[CLOCK2].hour.hallPin)
    {
        ctl_events_set_clear(&clockHomeEvent, CLOCK2_HOUR_HOME, 0);
        set_start_stop(CLOCK2, HOURARM, STOP);
    }
    
    if(triggeredPins & 1 << motorData[CLOCK3].min.hallPin)
    {
         ctl_events_set_clear(&clockHomeEvent, CLOCK3_MIN_HOME, 0);
         set_start_stop(CLOCK3, MINUTEARM, STOP);
    }
    if(triggeredPins & 1 << motorData[CLOCK3].hour.hallPin)
    {
        ctl_events_set_clear(&clockHomeEvent, CLOCK3_HOUR_HOME, 0);
        set_start_stop(CLOCK3, HOURARM, STOP);
    }   

    
    
    // Clear interrupts 
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, triggeredPins);
    
}

void gpio_init(void)
{
    // Enable IRQs for GPIO port 2
    NVIC_EnableIRQ(EINT2_IRQn);
    
    // Set up GPIO - Clock 0   
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 

    // Set required pin direction - Clock 0 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].min.dirPort,  motorData[CLOCK0].min.dirPin);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].min.port,     motorData[CLOCK0].min.pin);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].hour.dirPort, motorData[CLOCK0].hour.dirPin);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].hour.port,    motorData[CLOCK0].hour.pin);  // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK0].hour.hallPort,motorData[CLOCK0].hour.hallPin);   // Hall hour

    // Set up GPIO - Clock 1
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 

    // Set required pin direction - Clock 1
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].min.dirPort,  motorData[CLOCK1].min.dirPin);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].min.port,     motorData[CLOCK1].min.pin);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].hour.dirPort, motorData[CLOCK1].hour.dirPin);  // Dir D 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].hour.port,    motorData[CLOCK1].hour.pin);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK1].min.hallPort, motorData[CLOCK1].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK1].hour.hallPort,motorData[CLOCK1].hour.hallPin);   // Hall hour

    // Set up GPIO - Clock 2
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_4, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 

    // Set required pin direction - Clock 2
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].min.dirPort,  motorData[CLOCK2].min.dirPin);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].min.port,     motorData[CLOCK2].min.pin);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].hour.dirPort, motorData[CLOCK2].hour.dirPin);  // Dir B 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].hour.port,    motorData[CLOCK2].hour.pin);  // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK2].min.hallPort, motorData[CLOCK2].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK2].hour.hallPort,motorData[CLOCK2].hour.hallPin);   // Hall hour

    // Set up GPIO - Clock 3
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8,   (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_10,  (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_11,  (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_0,   (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_10,  (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11,  (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 

    // Set required pin direction - Clock 3
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].min.dirPort,  motorData[CLOCK3].min.dirPin);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].min.port,     motorData[CLOCK3].min.pin);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].hour.dirPort, motorData[CLOCK3].hour.dirPin);  // Dir D 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].hour.port,    motorData[CLOCK3].hour.pin);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK3].min.hallPort, motorData[CLOCK3].min.hallPin);  // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO,  motorData[CLOCK3].hour.hallPort,motorData[CLOCK3].hour.hallPort);  // Hall hour
    
    // Set up interrupts
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK0].min.hallPort,     1 << motorData[CLOCK0].min.hallPin,     GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK0].hour.hallPort,    1 << motorData[CLOCK0].hour.hallPin,    GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK1].min.hallPort,     1 << motorData[CLOCK1].min.hallPin,     GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK1].hour.hallPort,    1 << motorData[CLOCK1].hour.hallPin,    GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK2].min.hallPort,     1 << motorData[CLOCK2].min.hallPin,     GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK2].hour.hallPort,    1 << motorData[CLOCK2].hour.hallPin,    GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK3].min.hallPort,     1 << motorData[CLOCK3].min.hallPin,     GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK3].hour.hallPort,    1 << motorData[CLOCK3].hour.hallPin,    GPIO_INT_FALLING_EDGE); 

    // Clear interrupts
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, 1 << motorData[CLOCK0].hour.hallPin | 1 << motorData[CLOCK0].min.hallPin);
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK1].min.hallPort, 1 << motorData[CLOCK1].hour.hallPin | 1 << motorData[CLOCK1].min.hallPin);
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK2].min.hallPort, 1 << motorData[CLOCK2].hour.hallPin | 1 << motorData[CLOCK2].min.hallPin);
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK3].min.hallPort, 1 << motorData[CLOCK3].hour.hallPin | 1 << motorData[CLOCK3].min.hallPin);
    
    // Set up stepper driver GPIO - stepper reset control 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_1, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN)); // Pulldown per VID6606 datasheet 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 1);  // Reset
    
    // Set reset pin high 
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, RESETPORT, RESETPIN); 
}

void timer_init(void)
{
    
    timerFreq = Chip_Clock_GetSystemClockRate();
    
    #if HIRESTIMER
        // Initialise timers
        Chip_TIMER_Init(LPC_TIMER32_0);
        Chip_TIMER_Init(LPC_TIMER32_1);
    
        // Clear pending interrupts   
        NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
        NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);
    
        // Enable timer interrupts
        NVIC_EnableIRQ(TIMER_32_0_IRQn);    
        NVIC_EnableIRQ(TIMER_32_1_IRQn);   
    
        // Enable timers for stepper pulses
        Chip_TIMER_Enable(LPC_TIMER32_0);
        Chip_TIMER_Enable(LPC_TIMER32_1); 

    #else
        // Initialise timers
        Chip_TIMER_Init(LPC_TIMER16_0);
        Chip_TIMER_Init(LPC_TIMER16_1);
    
        // Clear pending interrupts   
        NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
        NVIC_ClearPendingIRQ(TIMER_16_1_IRQn);
       
        // Enable timer interrupts
        NVIC_EnableIRQ(TIMER_16_0_IRQn);    
        NVIC_EnableIRQ(TIMER_16_1_IRQn);  
    
        // Enable timers for stepper pulses
        Chip_TIMER_Enable(LPC_TIMER16_0);
        Chip_TIMER_Enable(LPC_TIMER16_1);     
    #endif
    
    // Set up clock 0 timers  
    #if HIRESTIMER
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 0);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 0, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 0);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 0, timerFreq/10);
    
    #else

    Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 0);
    Chip_TIMER_SetMatch(LPC_TIMER16_0, 0, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, 0);
    Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, timerFreq/10);
    #endif

    // Set up clock 1 timers
    #if HIRESTIMER
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, timerFreq/10);
    
    #else
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 1);
    Chip_TIMER_SetMatch(LPC_TIMER16_0, 1, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER16_1, 1, timerFreq/10);
    #endif

    // Set up clock 2 timers
    #if HIRESTIMER
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 2);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 2, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 2);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 2, timerFreq/10);
    
    #else

    Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 2);
    Chip_TIMER_SetMatch(LPC_TIMER16_0, 2, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, 2);
    Chip_TIMER_SetMatch(LPC_TIMER16_1, 2, timerFreq/10);
    #endif

    // Set up clock 3 timers
    #if HIRESTIMER
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 3);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 3, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 3);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 3, timerFreq/10);
    
    #else
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 3);
    Chip_TIMER_SetMatch(LPC_TIMER16_0, 3, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, 3);
    Chip_TIMER_SetMatch(LPC_TIMER16_1, 3, timerFreq/10);
    #endif
    
    
    
}


/*
    @brief      Basic delay function. 
    
    @param      delay: cycles to delay for

    @return     Nothing

*/
void pulse_delay(const uint16_t delay)
{
    for(uint16_t i = 0; i < delay; i++)
    {
        __NOP();
    }
}

/*
    @brief      Pulse generation function that flips output bit to drive steppers, for motion and direction. 
    
    @param      motorNum: motor number to move
    @param      arm: hour or minute arm to move

    @return     Nothing

*/
static void pulse_generation(const uint8_t motorNum, const uint8_t arm)
{
    // Set direction - minute arms
    if(motorData[motorNum].min.dir == CW)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    
    // Set direction - hour arms
    if(motorData[motorNum].hour.dir == CW)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    
    // Generate stepper pulse
    if(arm == MINUTEARM) // minute arm
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
        pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
    }
    if(arm == HOURARM) // hour arm
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
        pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
    }
    
    
}


/* 

    @brief      Calculate how many steps to get to desired angle
    
    @param      newAngle:   desired arm angle
    @param      angle:  current arm angle

    @return     Number of steps to desired angle

*/
static uint16_t calculate_steps(const uint16_t newAngle ,const uint16_t angle, const uint8_t dir)
{

    if (dir == CW)
    {
        if((newAngle - angle) < 0)
        {
            return (newAngle - angle + 360) * STEPSIZE;  
        }
        else if ((newAngle - angle) >= 360)
        {
            return (newAngle - angle - 360) * STEPSIZE;
        }
        else
        {    
            return (newAngle - angle) * STEPSIZE;   
        }
    }
    else
    {
        if((angle - newAngle) < 0)
        {
            return (angle - newAngle + 360) * STEPSIZE;  
        }
        else if ((newAngle - angle) >= 360)
        {
            return (angle - newAngle- 360) * STEPSIZE;
        }
        else
        {    
            return (angle - newAngle) * STEPSIZE;   
        }
    }
}

void update_stepcount(void)
{
    for(int i = FIRSTCLOCK; i < NUMBEROFCLOCKS; i++)
    {
        motorData[i].min.remainingSteps = calculate_steps(motorData[i].min.angleDesired, motorData[i].min.angle, motorData[i].min.dir);
        motorData[i].hour.remainingSteps = calculate_steps(motorData[i].hour.angleDesired, motorData[i].hour.angle, motorData[i].hour.dir);
        
        if(motorData[i].min.remainingSteps != 0)
        {
            motorData[i].min.atPosition = 0;
        }
        
        if(motorData[i].hour.remainingSteps != 0)
        {
            motorData[i].hour.atPosition = 0;
        }
        
    }
    
}

/*
    @brief      Set clock arm direction

    @param      clockNum: clock number to modify
    @param      arm:  arm to modify
    @param      dir:  arm direction

    @return     Nothing
*/
void set_arm_direction(const uint8_t clockNum, const uint8_t arm, const uint8_t direction)
{
    if(clockNum == ALLCLOCKS)
    {
        motorData[CLOCK0].min.dir = direction;
        motorData[CLOCK0].hour.dir = direction;
        motorData[CLOCK1].min.dir = direction;
        motorData[CLOCK1].hour.dir = direction;
        motorData[CLOCK2].min.dir = direction;
        motorData[CLOCK2].hour.dir = direction;
        motorData[CLOCK3].min.dir = direction;
        motorData[CLOCK3].hour.dir = direction;    
    }
    
    else 
    {
        if(arm == MINUTEARM | arm == BOTHARMS)
        {
            motorData[clockNum].min.dir = direction;
        }
        else if(arm == HOURARM | arm == BOTHARMS)
        {
            motorData[clockNum].hour.dir = direction;
        }
    }
    
}

/*
    @brief      Set clock arm angle setpoint

    @param      clockNum: clock number to modify
    @param      arm:  arm to modify
    @param      angle:  desired arm angle in degrees

    @return     Nothing
*/
void set_arm_angle(const uint8_t clockNum, const uint8_t arm, const uint16_t angle)
{
    if(clockNum == ALLCLOCKS)
    {
        motorData[CLOCK0].min.angleDesired = angle;
        motorData[CLOCK0].hour.angleDesired = angle;
        motorData[CLOCK1].min.angleDesired = angle;
        motorData[CLOCK1].hour.angleDesired = angle;
        motorData[CLOCK2].min.angleDesired = angle;
        motorData[CLOCK2].hour.angleDesired = angle;
        motorData[CLOCK3].min.angleDesired = angle;
        motorData[CLOCK3].hour.angleDesired = angle;    
    }
    
    else
    { 
        if(arm == MINUTEARM | arm == BOTHARMS)
        {
            motorData[clockNum].min.angleDesired = angle;
        }
        if(arm == HOURARM | arm == BOTHARMS)
        {
            motorData[clockNum].hour.angleDesired = angle;
        }
    }
}

/*
    @brief      Set clock arm speed setpoint

    @param      clockNum: clock number to modify
    @param      arm:  arm to modify
    @param      speed:  desired arm speed 1-5

    @return     Nothing
*/
void set_arm_speed(const uint8_t clockNum, const uint8_t arm, const uint16_t speed)
{
    if(clockNum == ALLCLOCKS)
    {
        motorData[CLOCK0].min.speed = speed;
        motorData[CLOCK0].hour.speed = speed;
        motorData[CLOCK1].min.speed = speed;
        motorData[CLOCK1].hour.speed = speed;
        motorData[CLOCK2].min.speed = speed;
        motorData[CLOCK2].hour.speed = speed;
        motorData[CLOCK3].min.speed = speed;
        motorData[CLOCK3].hour.speed = speed;    
    }
    
    else 
    {
        if(arm == MINUTEARM | arm == BOTHARMS)
        {
            motorData[clockNum].min.speed = speed;
        }
        else if(arm == HOURARM | arm == BOTHARMS)
        {
            motorData[clockNum].hour.speed = speed;
        }
    }
    
}
/*
    @brief      Set clock control mode, position or velocity

    @param      clockNum: clock number to modify
    @param      arm:  arm to modify
    @param      mdoe:  desired control mode - position or velocity

    @return     Nothing
*/
void set_control_mode(const uint8_t clockNum, const uint8_t arm, const uint16_t mode)
{
    if(clockNum == ALLCLOCKS)
    {
        motorData[CLOCK0].min.controlMode = mode;
        motorData[CLOCK0].hour.controlMode = mode;
        motorData[CLOCK1].min.controlMode = mode;
        motorData[CLOCK1].hour.controlMode = mode;
        motorData[CLOCK2].min.controlMode = mode;
        motorData[CLOCK2].hour.controlMode = mode;
        motorData[CLOCK3].min.controlMode = mode;
        motorData[CLOCK3].hour.controlMode = mode;    
    }
    
    else 
    {
        if(arm == MINUTEARM | arm == BOTHARMS)
        {
            motorData[clockNum].min.controlMode = mode;
            if(mode == POS_CTRL)
            {
                motorData[clockNum].min.steps = 0;
            }
        }
        else if(arm == HOURARM | arm == BOTHARMS)
        {
            motorData[clockNum].hour.controlMode = mode;
            if(mode == POS_CTRL)
            {
                motorData[clockNum].hour.steps = 0;
            }
        }
    }

        
   
}
/*
    @brief      Drive clock to specific position

    @param      clockNum: clock number to drive
    @param      arm:  arm to drive

    @return     Nothing
*/
static void drive_to_pos(const uint8_t clockNum, const uint8_t arm)
{
    // Move hour arm
    if(arm == HOURARM)
    {
        if(motorData[clockNum].hour.remainingSteps != 0)
        {
            motorData[clockNum].hour.atPosition = FALSE;
        }

        if(!motorData[clockNum].hour.atPosition)
        {
            pulse_generation(clockNum, HOURARM); // Generate stepper pulse

            --motorData[clockNum].hour.remainingSteps;
    
            motorData[clockNum].hour.steps++;
            
            // Update angle actual calculation
            if(motorData[clockNum].hour.steps >= STEPSIZE)
            {
                
                if(motorData[clockNum].hour.dir == CW)
                {
                    motorData[clockNum].hour.angle++; 
                    if(motorData[clockNum].hour.angle >= 360)
                    {
                        motorData[clockNum].hour.angle = 0;
                    }
                }
                else
                {
                    motorData[clockNum].hour.angle--;   
                    if(motorData[clockNum].hour.angle <= 0)
                    {
                        motorData[clockNum].hour.angle = 360;
                    }
                }
                            
                motorData[clockNum].hour.steps = 0;
            }

            // At desired position 
            if(motorData[clockNum].hour.remainingSteps <= 0)
            {
                motorData[clockNum].hour.atPosition = TRUE;
                //motorData[clockNum].hour.start == 0;
                set_start_stop(clockNum, HOURARM, STOP);
            }

        }   
    }
    
    if(arm == MINUTEARM)
    {
        if(motorData[clockNum].min.remainingSteps != 0)
        {
            motorData[clockNum].min.atPosition = FALSE;
        }
    
        if(!motorData[clockNum].min.atPosition)
        {
            pulse_generation(clockNum, MINUTEARM);
    
            --motorData[clockNum].min.remainingSteps;
        
            motorData[clockNum].min.steps++;
        
            // Update angle actual calculation
            if(motorData[clockNum].min.steps >= STEPSIZE)
            {
                if(motorData[clockNum].min.dir == CW)
                {
                    motorData[clockNum].min.angle++; 
                    if(motorData[clockNum].min.angle >= 360)
                    {
                        motorData[clockNum].min.angle = 0;
                    }
                }
                else
                {
                    motorData[clockNum].min.angle--;   
                    if(motorData[clockNum].min.angle <= 0)
                    {
                        motorData[clockNum].min.angle = 360;
                    }
                }
            
                motorData[clockNum].min.steps = 0;
            }
    
            if(motorData[clockNum].min.remainingSteps <= 0)
            {
                motorData[clockNum].min.atPosition = TRUE;
                //motorData[clockNum].min.start == 0;
                set_start_stop(clockNum, MINUTEARM, STOP);
            }
        }

    }
    
}

/*
    @brief      Drive clock at specific speed

    @param      clockNum: clock number to drive

    @return     Nothing
*/
static void drive_continuous(const uint8_t clockNum, const uint8_t arm)
{   
    pulse_generation(clockNum, arm); // Generate stepper pulse
    
    if(arm == HOURARM)
    {
        motorData[clockNum].hour.steps++;
    }
    else
    {
        motorData[clockNum].min.steps++;
    }
    
    
    if (arm == MINUTEARM)
    {        
        if(motorData[clockNum].min.steps >= STEPSIZE)
        {
            if(motorData[clockNum].min.dir == CW)
            {
                motorData[clockNum].min.angle++; 
                
                if(motorData[clockNum].min.angle >= 360)
                {
                    motorData[clockNum].min.angle = 0;
                }
            }
            else
            {
                motorData[clockNum].min.angle--;   
                
                if(motorData[clockNum].min.angle <= 0)
                {
                    motorData[clockNum].min.angle = 360;
                }
            }
        

            motorData[clockNum].min.steps = 0;
        }
    }
    if (arm == HOURARM)
    {
        if(motorData[clockNum].hour.steps >= STEPSIZE)
        {
            if(motorData[clockNum].hour.dir == CW)
            {
                motorData[clockNum].hour.angle++; 
                
                if(motorData[clockNum].hour.angle >= 360)
                {
                    motorData[clockNum].hour.angle = 0;
                }
            }
            else
            {
                motorData[clockNum].hour.angle--;   
                
                if(motorData[clockNum].hour.angle <= 0)
                {
                    motorData[clockNum].hour.angle = 360;
                }
            }
        
    
            motorData[clockNum].hour.steps = 0;
        }
    }
    
}

/*
    @brief      Homes clocks and drives to preset position

    @param      Nothing

    @return     Nothing
*/
void home_clocks(void)
{
  
    ctl_events_set_clear(&clockHomeEvent, HOMING_ACTIVE, CLOCK0_MIN_HOME|CLOCK0_HOUR_HOME|
    CLOCK1_MIN_HOME|CLOCK1_HOUR_HOME|
    CLOCK2_MIN_HOME|CLOCK2_HOUR_HOME|
    CLOCK3_MIN_HOME|CLOCK3_HOUR_HOME);
    
    // Enable GPIO interrupts
    Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, 1 << motorData[CLOCK0].hour.hallPin | 1 << motorData[CLOCK0].min.hallPin |
        1 << motorData[CLOCK1].hour.hallPin | 1 << motorData[CLOCK1].min.hallPin |
        1 << motorData[CLOCK2].hour.hallPin | 1 << motorData[CLOCK2].min.hallPin |
        1 << motorData[CLOCK3].hour.hallPin | 1 << motorData[CLOCK3].min.hallPin);  
    
    // Stop all motion
    set_start_stop(ALLCLOCKS, BOTHARMS, STOP);  
    
    // Set control mode to position control
    set_control_mode(ALLCLOCKS, BOTHARMS, VEL_CTRL);
    
    // Set direction CW
    set_arm_direction(ALLCLOCKS, BOTHARMS, CW);
    
    // Set speed 
    set_arm_speed(ALLCLOCKS, BOTHARMS, 3);
             
    // Drive clocks CW until hall is hit
    set_start_stop(ALLCLOCKS, BOTHARMS, START); 

    // Wait for all clocks to be at home pos
    ctl_events_wait(CTL_EVENT_WAIT_ALL_EVENTS_WITH_AUTO_CLEAR, &clockHomeEvent, 
        CLOCK0_MIN_HOME|CLOCK0_HOUR_HOME|
        CLOCK1_MIN_HOME|CLOCK1_HOUR_HOME|
        CLOCK2_MIN_HOME|CLOCK2_HOUR_HOME|
        CLOCK3_MIN_HOME|CLOCK3_HOUR_HOME, CTL_TIMEOUT_NONE, 0);
    
    // Update current arm angle to 0
    motorData[CLOCK0].min.angle = 0;
    motorData[CLOCK0].hour.angle = 0;
    motorData[CLOCK1].min.angle = 0;
    motorData[CLOCK1].hour.angle = 0;
    motorData[CLOCK2].min.angle = 0;
    motorData[CLOCK2].hour.angle = 0;
    motorData[CLOCK3].min.angle = 0;
    motorData[CLOCK3].hour.angle = 0;
    
    
    // Start motion and run CW for 50 degrees to pass other side of hall sensing area        
    set_arm_angle(ALLCLOCKS, BOTHARMS, 50);
    update_stepcount();
    set_control_mode(ALLCLOCKS, BOTHARMS, POS_CTRL);
    set_start_stop(ALLCLOCKS, BOTHARMS, START);
    ctl_timeout_wait(ctl_get_current_time() + 1000);
    
    // Clear interrupts 
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].hour.hallPin | motorData[CLOCK0].min.hallPin |
        motorData[CLOCK1].hour.hallPin | motorData[CLOCK1].min.hallPin |
        motorData[CLOCK2].hour.hallPin | motorData[CLOCK2].min.hallPin |
        motorData[CLOCK3].hour.hallPin | motorData[CLOCK3].min.hallPin); 
    
    // Set direction CCW
    set_arm_direction(ALLCLOCKS, BOTHARMS, CCW);
    set_control_mode(ALLCLOCKS, BOTHARMS, VEL_CTRL);
    set_start_stop(ALLCLOCKS, BOTHARMS, START);
    
    // Drive CCW until hall is hit
    ctl_events_wait(CTL_EVENT_WAIT_ALL_EVENTS_WITH_AUTO_CLEAR, &clockHomeEvent, CLOCK0_MIN_HOME|CLOCK0_HOUR_HOME|
        CLOCK1_MIN_HOME|CLOCK1_HOUR_HOME|
        CLOCK2_MIN_HOME|CLOCK2_HOUR_HOME|
        CLOCK3_MIN_HOME|CLOCK3_HOUR_HOME, CTL_TIMEOUT_NONE, 0);
    
    
    // Calculate home position
    motorData[CLOCK0].min.angle = (motorData[CLOCK0].min.angle / 2) + 180;
    motorData[CLOCK0].hour.angle = motorData[CLOCK0].hour.angle / 2;
    motorData[CLOCK1].min.angle = (motorData[CLOCK1].min.angle / 2) + 180;
    motorData[CLOCK1].hour.angle = motorData[CLOCK1].hour.angle / 2;
    motorData[CLOCK2].min.angle = (motorData[CLOCK2].min.angle / 2) + 180;
    motorData[CLOCK2].hour.angle = motorData[CLOCK2].hour.angle / 2;
    motorData[CLOCK3].min.angle = (motorData[CLOCK3].min.angle / 2) + 180;
    motorData[CLOCK3].hour.angle = motorData[CLOCK3].hour.angle / 2;
    
    
    // Set to position control to drive arms to homed position 
    set_control_mode(ALLCLOCKS, BOTHARMS, POS_CTRL);
     
    // Set desired angle for all arms to 0/12 oclock
    set_arm_angle(CLOCK0, MINUTEARM, HOMEPOSMIN);
    set_arm_angle(CLOCK0, HOURARM, HOMEPOSHOUR);
    set_arm_angle(CLOCK1, MINUTEARM, HOMEPOSMIN);
    set_arm_angle(CLOCK1, HOURARM, HOMEPOSHOUR);
    set_arm_angle(CLOCK2, MINUTEARM, HOMEPOSMIN);
    set_arm_angle(CLOCK2, HOURARM, HOMEPOSHOUR);
    set_arm_angle(CLOCK3, MINUTEARM, HOMEPOSMIN);
    set_arm_angle(CLOCK3, HOURARM, HOMEPOSHOUR);
    
    // Set direction of arms
    set_arm_direction(ALLCLOCKS, BOTHARMS, CCW);
    
    // Calculate number of pulses to desired angle
    update_stepcount();
    
    // Disable GPIO interrupts
    Chip_GPIO_DisableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, 1 << motorData[CLOCK0].hour.hallPin | 1 << motorData[CLOCK0].min.hallPin |
        1 << motorData[CLOCK1].hour.hallPin | 1 << motorData[CLOCK1].min.hallPin |
        1 << motorData[CLOCK2].hour.hallPin | 1 << motorData[CLOCK2].min.hallPin |
        1 << motorData[CLOCK3].hour.hallPin | 1 << motorData[CLOCK3].min.hallPin);    
        
    
    // Start motion 
    set_start_stop(ALLCLOCKS, BOTHARMS, START);

    ctl_timeout_wait(ctl_get_current_time() + 2000);    

    set_arm_direction(ALLCLOCKS, BOTHARMS, CW);
        
    ctl_events_set_clear(&clockHomeEvent, 0, HOMING_ACTIVE);
    
}



/*****************************************************************************
 * Public Functions
 ****************************************************************************/
/*
    @brief      Updates local data with data from CAN bus

    @param      CANdata - data from can bus

    @return     Nothing
*/
void update_from_CAN(CCAN_MSG_OBJ_T *CANdata)
{
    RingBuffer_Insert(&rxBuffer, CANdata);
    
    //can_RX_data = *CANdata;
    //printf("%d\n", rx_buffer[0]);
    
    ctl_events_wait(CTL_EVENT_WAIT_ALL_EVENTS_WITH_AUTO_CLEAR, &clockHomeEvent, !HOMING_ACTIVE, CTL_TIMEOUT_NONE, 0);
    //if(clockHomeEvent & !HOMING_ACTIVE)
    //{
    ctl_events_set_clear(&clockControlEvent, CAN_UPDATE, 0);
    //}

}




/*****************************************************************************
 * Realtime Threads
 ****************************************************************************/
 void clock_func(void *p)
{
    unsigned int v=0;

    while (1)
    {      
        // Wait for clock event to be triggered
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockEvent, RUN_ALL_CLOCKS|
            RUN_CLOCK0_HOUR|RUN_CLOCK0_MIN|
            RUN_CLOCK1_HOUR|RUN_CLOCK1_MIN|
            RUN_CLOCK2_HOUR|RUN_CLOCK2_MIN|
            RUN_CLOCK3_HOUR|RUN_CLOCK3_MIN, CTL_TIMEOUT_NONE, 0);  
                
        // Step clock 0 hour arm
        if(clockEvent & RUN_CLOCK0_HOUR)
        {
            if(motorData[CLOCK0].hour.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK0, HOURARM);
            }
            else
            {
                drive_to_pos(CLOCK0, HOURARM);
            }
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK0_HOUR); // clear event 
        }
        
        // Step clock 0 minute arm 
        if(clockEvent & RUN_CLOCK0_MIN)
        {
            if(motorData[CLOCK0].min.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK0, MINUTEARM);
            }
            else
            {
                drive_to_pos(CLOCK0, MINUTEARM);
            }           
            
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK0_MIN);  // clear event
        }
        
        // Step clock 1 hour arm 
        if(clockEvent & RUN_CLOCK1_HOUR)
        {
            if(motorData[CLOCK1].hour.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK1, HOURARM);
            }
            else
            {
                drive_to_pos(CLOCK1, HOURARM);
            }
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK1_HOUR); // clear event
        }
        
        // Step clock 1 minute arm
        if(clockEvent & RUN_CLOCK1_MIN)
        {
            if(motorData[CLOCK1].min.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK1, MINUTEARM);
            }
            else
            {
                drive_to_pos(CLOCK1, MINUTEARM);
            }           
            
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK1_MIN);  // clear event when at position
        }
        
        // Step clock 2 hour arm
        if(clockEvent & RUN_CLOCK2_HOUR)
        {
            if(motorData[CLOCK2].hour.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK2, HOURARM);
            }
            else
            {
                drive_to_pos(CLOCK2, HOURARM);
            }
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK2_HOUR); // clear event when at position
        }
        
        // Step clock 2 minute arm
        if(clockEvent & RUN_CLOCK2_MIN)
        {
            if(motorData[CLOCK2].min.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK2, MINUTEARM);
            }
            else
            {
                drive_to_pos(CLOCK2, MINUTEARM);
            }           
            
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK2_MIN);  // clear event when at position
        }
        
        // Step clock 3 hour arm
        if(clockEvent & RUN_CLOCK3_HOUR)
        {
            if(motorData[CLOCK3].hour.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK3, HOURARM);
            }
            else
            {
                drive_to_pos(CLOCK3, HOURARM);
            }
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK3_HOUR); // clear event when at position
        }
        
        // Step clock 3 minute arm
        if(clockEvent & RUN_CLOCK3_MIN)
        {
            if(motorData[CLOCK3].min.controlMode == VEL_CTRL)
            {
                drive_continuous(CLOCK3, MINUTEARM);
            }
            else
            {
                drive_to_pos(CLOCK3, MINUTEARM);
            }           
            
            ctl_events_set_clear(&clockEvent, 0, RUN_CLOCK3_MIN);  // clear event when at position
        }

        v++;
    }  
}


// Main clock control thread
void clock_control(void *p)
{
    unsigned int v = 0;
    
    // Initialise clock control events
    ctl_events_init(&clockControlEvent, 0);
    ctl_events_init(&clockEvent, 0);
    
    // Initialise GPIO 
    gpio_init();
    
    // Initialise peripheral timers
    timer_init();
    
    // Initialise ring buffer
    RingBuffer_Init(&rxBuffer, &rx_buffer, sizeof(can_RX_data), RXBUFFSIZE);
        
    #if TESTING
        clock_testing();
    #endif
     
    while(1)
    {
             
        // Wait for any clock event to be triggered
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockControlEvent, CAN_UPDATE, CTL_TIMEOUT_NONE, 0);

        // Update with data received from CAN bus
        if(clockControlEvent & CAN_UPDATE)
        {
            /* Message types:
            0x200 - position
            0x201 - speed
            0x202 - acceleration
            0x203 - start motion 
            0x204 - trigger function
            */

            while(!RingBuffer_IsEmpty(&rxBuffer))
            {
                RingBuffer_Pop(&rxBuffer, &can_RX_data);
            
                // position update
                if(can_RX_data.mode_id == POSITION)
                {
                 
    
                    for(int i = FIRSTCLOCK; i < NUMBEROFCLOCKS; i++)
                    {
                        if(can_RX_data.data[0] == motorData[i].clockNumber) // find which clock to update
                        {
                            motorData[i].min.angleDesired = ((can_RX_data.data[1] << 8) | (can_RX_data.data[2])); // minute
                            motorData[i].hour.angleDesired = ((can_RX_data.data[3] << 8) | (can_RX_data.data[4])); // hour
                            update_stepcount();
                        }  
                    }  
      
                }
            
 
                // speed and direction update
                if (can_RX_data.mode_id == SPEED)
                {
                    for(int i = FIRSTCLOCK; i < NUMBEROFCLOCKS; i++)
                    {
                        if(can_RX_data.data[0] == motorData[i].clockNumber) // find which clock to update
                        {
                            motorData[i].min.speed = (can_RX_data.data[1]);     // minute speed
                            motorData[i].hour.speed = (can_RX_data.data[2]);    // hour speed
                            motorData[i].min.dir = (can_RX_data.data[3]);       // minute direction
                            motorData[i].hour.dir = (can_RX_data.data[4]);      // hour direction
                        }
                    }
        
                }
            
                // acceleration update
                if (can_RX_data.mode_id == ACCELERATION)
                {
                }
            
                // start motion
                if (can_RX_data.mode_id == STARTMOTION)
                {
                    if(can_RX_data.data[0] == 200)
                    {
                        set_start_stop(ALLCLOCKS, BOTHARMS, START);
                    }
                }
                
                // trigger specific clock functions 
                if (can_RX_data.mode_id == TRIGGERFUNC)
                {
                    // Trigger homing procedure
                    if(can_RX_data.data[0] == HOMECLOCKS)
                    {
                        home_clocks();
                    }
                }
            
            }

            ctl_events_set_clear(&clockControlEvent, 0, CAN_UPDATE);
        }
                 
            
        v++;
           
    }    
}


