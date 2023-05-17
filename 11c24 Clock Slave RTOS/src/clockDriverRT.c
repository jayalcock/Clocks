#include <stdint.h>
#include "ccand_11xx.h"
#include "clockDriverRT.h"
#include "board.h"
#include "ctl_api.h"
#include "timer_11xx.h"
#include "clock_11xx.h"
#include "ccan_rom.h"
#include "gpio_11xx_2.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
// Test variables
#define TESTING 1
#define FIRSTCLOCK 0
#define NUMBEROFCLOCKS 4
#define NUMBEROFARMS 2
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 3
#define HIRESTIMER 1

// Clock numbers
#define CLOCK0          0
#define CLOCK1          1
#define CLOCK2          2
#define CLOCK3          3

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

// Clock control event trigger constsnts
#define UPDATE_ALL_CLOCKS   1<<0
#define UPDATE_CLOCK0_MIN   1<<1
#define UPDATE_CLOCK0_HOUR  1<<2
#define UPDATE_CLOCK1_MIN   1<<3
#define UPDATE_CLOCK1_HOUR  1<<4
#define UPDATE_CLOCK2_MIN   1<<5
#define UPDATE_CLOCK2_HOUR  1<<6
#define UPDATE_CLOCK3_MIN   1<<7
#define UPDATE_CLOCK3_HOUR  1<<8
#define CAN_UPDATE          1<<9
#define HOME_CLOCKS         1<<10

// Can message types
#define POSITION       0x200 
#define SPEED          0x201 
#define ACCELERATION   0x202 
#define STARTMOTION    0x203 

// Initialise objects
CTL_EVENT_SET_t clock0Event, clock1Event, clock2Event, clock3Event, clockControlEvent;
CCAN_MSG_OBJ_T can_RX_data;

// File scope variables
const uint16_t speed[] = {100, 200, 400, 800, 1600, 3200};
uint32_t timerFreq;
uint8_t localControl = 0;

uint8_t homingSpeed = 2;
uint8_t homingDir = 0;
uint8_t homingBit = 0;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

// Population of motor data structs with initial values
motorStruct motorData[] =
{
    {0, //clock num
        //minute
        { 0, 3, // pulse port/pin 
          0, 2, // direction port/pin 
          0,    // angle
          90,   // desired angle 
          0,    // direction
          0,    // start motion
          0,    // remaining steps
          1,    // at position
          2,    // speed
          2,    // accel
          2, 1 }, // hall port/pin 
        //hour
        { 0, 5, // pulse port/pin
          0, 4, // direction port/pin
          0,    // angle
          180,  // desired angle
          0,    // direction
          0,    // start motion
          0,    // remaining steps
          1,    // at position
          3,    // speed
          2,    // accel
          2, 2 }}, // hall port/pin
                
    {1, //clock num
        {0, 7, 0, 6, 0, 0, 1, 0, 0, 1, 2, 2, 2, 3}, //min
        {0, 9, 0, 8, 0, 0, 1, 0, 0, 1, 2, 2, 2, 6}}, //hour         
                            
    {2, //clock num
        {1, 5, 1, 4, 0, 0, 1, 0, 0, 1, 2, 2, 2, 7}, //min
        {1, 7, 1, 6, 0, 0, 1, 0, 0, 1, 2, 2, 2, 8}}, //hour   
                     
    {3, //clock num
        {1, 10, 1, 8, 0, 0, 1, 0, 0, 1, 2, 2, 2, 10}, //min
        {2, 0, 1, 11, 0, 0, 1, 0, 0, 1, 2, 2, 2, 11}}, //hour   
          
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
    //localControl = 1;
    //uint8_t clockNum = 2;
    //char* arm = "min";
    //uint16_t M1angle = 400;
    
    home_clocks();
    
    //motorData[2].min.speed = 3;
    //motorData[2].min.angle = 400;
    
    //motorData[2].min.start = 1;
    
    //motorData[2].hour.speed = 3;
    //motorData[2].hour.angle = 400;
    
    //motorData[2].hour.start = 1;
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
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 0);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 0, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK0].min.speed]);
        
        if(motorData[CLOCK0].min.start == 1)
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_MIN, 0);

    }
    // Clock 1 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK1].min.speed]);
        
        if(motorData[CLOCK1].min.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_MIN, 0);

    }
    // Clock 2 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 2);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 2, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK2].min.speed]);
        
        if(motorData[CLOCK2].min.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_MIN, 0);

    }
    // Clock 3 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 3);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 3, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK3].min.speed]);
        
        if(motorData[CLOCK3].min.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_MIN, 0);

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
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 0);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 0, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK0].hour.speed]);
        
        if(motorData[CLOCK0].hour.start == 1)
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 1);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK1].hour.speed]);
        
        if(motorData[CLOCK1].hour.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 2);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 2, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK2].hour.speed]);
        
        if(motorData[CLOCK2].hour.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 3);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 3, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[CLOCK3].hour.speed]);
        
        if(motorData[CLOCK3].hour.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_HOUR, 0);

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
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_MIN, 0);

    }
    // Clock 1 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK1].min.speed]);
        
        if(motorData[CLOCK1].min.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_MIN, 0);

    }
    // Clock 2 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK2].min.speed]);
        
        if(motorData[CLOCK2].min.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_MIN, 0);

    }
    // Clock 3 minute timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0,CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[CLOCK3].min.speed]);
        
        if(motorData[CLOCK3].min.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_MIN, 0);

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
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK1);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK1, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK1].hour.speed]);
        
        if(motorData[CLOCK1].hour.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK2);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK2, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK2].hour.speed]);
        
        if(motorData[CLOCK2].hour.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match interrupt clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_1, CLOCK3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_1, CLOCK3);
        Chip_TIMER_SetMatch(LPC_TIMER16_1, CLOCK3, Chip_TIMER_ReadCount(LPC_TIMER16_1) + timerFreq/speed[motorData[CLOCK3].hour.speed]);
        
        if(motorData[CLOCK3].hour.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_HOUR, 0);

    }
  
}   
#endif

/*
    @brief    GPIO IRQ handler for handling hall effect triggers. 
    
    @return   Nothing

*/
void GPIO2_IRQHandler(void)
{
    uint16_t triggeredPins;
    
    // Determine which halls have been triggered
    triggeredPins = Chip_GPIO_GetMaskedInts(LPC_GPIO, motorData[CLOCK0].min.hallPort); 
    
    if(triggeredPins && motorData[CLOCK0].min.hallPin)
    //if(triggeredPins && motorData[CLOCK0].hour.hallPin)
    //    motorData[CLOCK0].hour.start = 0;
    {
        //motorData[CLOCK0].min.start = 0;
        homingBit = 1;
    }
    //if(triggeredPins && motorData[CLOCK0].hour.hallPin)
        //motorData[CLOCK0].hour.start = 0;
    //if(triggeredPins && motorData[CLOCK1].min.hallPin)
    //    motorData[CLOCK1].min.start = 0;
    //if(triggeredPins && motorData[CLOCK1].hour.hallPin)
    //    motorData[CLOCK1].hour.start = 0;
    //if(triggeredPins && motorData[CLOCK2].min.hallPin)
    //    motorData[CLOCK2].min.start = 0;
    //if(triggeredPins && motorData[CLOCK2].hour.hallPin)
    //    motorData[CLOCK2].hour.start = 0;
    //if(triggeredPins && motorData[CLOCK3].min.hallPin)
    //    motorData[CLOCK3].min.start = 0;
    //if(triggeredPins && motorData[CLOCK3].hour.hallPin)
    //    motorData[CLOCK3].hour.start = 0;
    
    
    // Clear interrupts 
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, triggeredPins);

    
}

/*
    @brief    Pulse generation function that flips output bit to drive steppers, for motion and direction. 
    
    @param    motorNum - motor number to move
              arm - hour or minute arm to move

    @return   Nothing

*/
static void pulse_generation(const uint8_t motorNum, const char arm)
{
    // Set direction - minute arms
    if(motorData[motorNum].min.dir == 0)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    
    // Set direction - hour arms
    if(motorData[motorNum].hour.dir == 0)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    
    // Generate stepper pulse
    if(arm == 'm') // minute arm
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
        //pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
    }
    if(arm == 'h') // hour arm
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
        //pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
    }
    
    
}

/* 

    @brief    Calculate how many steps to get to desired angle
    
    @param    newAngle - desired arm angle
              angle - current arm angle

    @return   Number of steps to desired angle

*/
static uint16_t calculate_steps(uint16_t newAngle , uint16_t angle)
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

/*

    @brief    Drive clock to specific position

    @param    clockNum: clock number to drive
    @param    arm:  arm to drive
    @param    steps:  how many steps to drive

    @return

*/
static void drive_to_pos(const uint8_t clockNum, char arm, uint8_t *steps)
{
    // Move hour arm
    if(arm == 'h')
    {
        if(motorData[clockNum].hour.remainingSteps != 0)
        {
            motorData[clockNum].hour.atPosition = 0;
        }

        if(!motorData[clockNum].hour.atPosition)
        {
            pulse_generation(clockNum, 'h'); // Generate stepper pulse

            --motorData[clockNum].hour.remainingSteps;
    
            (*steps)++;
            
            // Update angle actual calculation
            if(*steps == STEPSIZE)
            {
                motorData[clockNum].hour.angle++;
        
                if(motorData[clockNum].hour.angle == 360)
                {
                    motorData[clockNum].hour.angle = 0;
                }
                *steps = 0;
            }

            // At desired position 
            if(motorData[clockNum].hour.remainingSteps <= 0)
            {
                motorData[clockNum].hour.atPosition = 1;
                motorData[clockNum].hour.start == 0;
            }

        }   
    }
    
    if(arm == 'm')
    {
        if(motorData[clockNum].min.remainingSteps != 0)
        {
            motorData[clockNum].min.atPosition = 0;
        }
    
        if(!motorData[clockNum].min.atPosition)
        {
            pulse_generation(clockNum, 'm');
    
            --motorData[clockNum].min.remainingSteps;
        
            (*steps)++;
        
            // Update angle actual calculation
            if(*steps == STEPSIZE)
            {
                motorData[clockNum].min.angle++;
            
                if(motorData[clockNum].min.angle == 360)
                {
                    motorData[clockNum].min.angle = 0;
                }
            
                *steps = 0;
            }
    
            if(motorData[clockNum].min.remainingSteps <= 0)
            {
                motorData[clockNum].min.atPosition = 1;
                motorData[clockNum].min.start == 0;
            }
        }

    }
    
}


// Drive clock at constant speed
static void drive_continuous(const uint8_t clockNum, const uint8_t speed, const uint8_t dir)
{
     
    pulse_generation(clockNum, 'm'); // Generate stepper pulse
    pulse_generation(clockNum, 'h'); // Generate stepper pulse
    
    motorData[clockNum].min.speed = speed;
    motorData[clockNum].hour.speed = speed;
    
    motorData[clockNum].min.dir = dir;
    motorData[clockNum].hour.dir = dir;
    
    motorData[clockNum].min.angle++;
    if(motorData[clockNum].min.angle == 360)
    {
        motorData[clockNum].min.angle = 0;
    }
    
    motorData[clockNum].hour.angle++;
    if(motorData[clockNum].hour.angle == 360)
    {
        motorData[clockNum].hour.angle = 0;
    }
 
}

// Homing Procedure
static void home_clocks(void)
{
    uint8_t hallBit = 1;
    uint8_t speed = 2;
    uint8_t direction = 1; 
    
    localControl = 1; 
    
    // Drive clocks CW until hall is hit
    //drive_continuous(CLOCK0, speed, direction);
    ctl_events_set_clear(&clockControlEvent, HOME_CLOCKS, 0);
    motorData[0].min.start = 1;
    motorData[1].min.start = 1;
    motorData[2].min.start = 1;
    motorData[3].min.start = 1;
    motorData[0].hour.start = 1;
    motorData[1].hour.start = 1;
    motorData[2].hour.start = 1;
    motorData[3].hour.start = 1;

    // Save position/angle
    
    // Drive past hall
    
    
    
    // Drive CCW until hall is hit
    
    // Save position/angle
    
    
    // Calculate home position
    
    
    // Drive to home
    
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

// Clock 0 control function
void clock0_func(void *p)
{  
    unsigned int v=0;
    uint8_t mSteps, hSteps;
    
    // Initialise clock 0 events
    ctl_events_init(&clock0Event, 0);
    
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
    
    // Set up GPIO    
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
   
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].min.dirPort, motorData[CLOCK0].min.dirPin);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].min.port, motorData[CLOCK0].min.pin);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].hour.dirPort, motorData[CLOCK0].hour.dirPin);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK0].hour.port, motorData[CLOCK0].hour.pin);  // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK0].hour.hallPort, motorData[CLOCK0].hour.hallPin);   // Hall hour
    
    // Set up interrupts
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin, GPIO_INT_RISING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK0].hour.hallPort, motorData[CLOCK0].hour.hallPin, GPIO_INT_RISING_EDGE);

    
    //Chip_GPIO_SetPinModeEdge(LPC_GPIO, motorData[CLOCK0].min.hallPort, 0x48A);
    //Chip_GPIO_SetPinModeEdge(LPC_GPIO, motorData[CLOCK0].hour.hallPort, 0x944);
    
    //Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, 0x48A);
    //Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].hour.hallPort, 0x944);
    
    //Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, 0x48A);
    //Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].hour.hallPort, 0x944);
    
    Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);
    Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);
    


    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock0Event, RUN_ALL_CLOCKS|RUN_CLOCK0_HOUR|RUN_CLOCK0_MIN, CTL_TIMEOUT_NONE, 0);  
                
        // Drive clock 0 hour arm until at desired position
        if(clock0Event & RUN_CLOCK0_HOUR|RUN_ALL_CLOCKS)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                //Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);
                drive_continuous(CLOCK0, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK0, 'h', &hSteps);
            }
            ctl_events_set_clear(&clock0Event, 0, RUN_CLOCK0_HOUR); // clear event when at position
        }
        
        // Drive clock 0 minute arm until at desired position
        if(clock0Event & RUN_CLOCK0_MIN|RUN_ALL_CLOCKS)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK0, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK0, 'm', &mSteps);
            }           
            
            ctl_events_set_clear(&clock0Event, 0, RUN_CLOCK0_MIN);  // clear event when at position
        }

        v++;
    }  
}

void clock1_func(void *p)
{  
    unsigned int v=0;
    uint8_t mSteps, hSteps;
     
    // Initialise clock 1 events
    ctl_events_init(&clock1Event, 0);
    
    // Set up GPIO
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    //// Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].min.dirPort, motorData[CLOCK1].min.dirPin);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].min.port, motorData[CLOCK1].min.pin);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].hour.dirPort, motorData[CLOCK1].hour.dirPin);  // Dir D 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK1].hour.port, motorData[CLOCK1].hour.pin);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK1].min.hallPort, motorData[CLOCK1].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK1].hour.hallPort, motorData[CLOCK1].hour.hallPin);   // Hall hour
    
    // Set up interrupts
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK1].min.hallPort, motorData[CLOCK1].min.hallPin, GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK1].hour.hallPort, motorData[CLOCK1].hour.hallPin, GPIO_INT_FALLING_EDGE);
    
    
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
    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock1Event, RUN_ALL_CLOCKS|RUN_CLOCK1_MIN|RUN_CLOCK1_HOUR, CTL_TIMEOUT_NONE, 0);      
        
        // Drive clock 1 hour arm until at desired position
        if(clock1Event & RUN_CLOCK1_HOUR)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK1, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK1, 'h', &hSteps);
            }            
            ctl_events_set_clear(&clock1Event, 0, RUN_CLOCK1_HOUR); // clear event when at position
        }
        
        // Drive clock 1 minute arm until at desired position
        if(clock1Event & RUN_CLOCK1_MIN)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK1, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK1, 'm', &mSteps);
            }           

            ctl_events_set_clear(&clock1Event, 0, RUN_CLOCK1_MIN);  // clear event when at position
        }

            
        v++;
    }  
}

void clock2_func(void *p)
{  
    unsigned int v=0;
    uint8_t mSteps, hSteps;

    // Initialise clock 2 events
    ctl_events_init(&clock2Event, 0);
    
    // Set up GPIO
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_4, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].min.dirPort, motorData[CLOCK2].min.dirPin);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].min.port, motorData[CLOCK2].min.pin);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].hour.dirPort, motorData[CLOCK2].hour.dirPin);  // Dir B 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK2].hour.port, motorData[CLOCK2].hour.pin);  // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK2].min.hallPort, motorData[CLOCK2].min.hallPin);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK2].hour.hallPort, motorData[CLOCK2].hour.hallPin);   // Hall hour
    
    // Set up interrupts
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK2].min.hallPort, motorData[CLOCK2].min.hallPin, GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK2].hour.hallPort, motorData[CLOCK2].hour.hallPin, GPIO_INT_FALLING_EDGE);
    
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
    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock2Event, RUN_ALL_CLOCKS|RUN_CLOCK2_MIN|RUN_CLOCK2_HOUR, CTL_TIMEOUT_NONE, 0);     
        
        // Drive clock 2 hour arm until at desired position
        if(clock2Event & RUN_CLOCK2_HOUR)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK2, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK2, 'h', &hSteps);
            }            

            ctl_events_set_clear(&clock2Event, 0, RUN_CLOCK2_HOUR); // clear event when at position
        }
        
        // Drive clock 2 minute arm until at desired position
        if(clock2Event & RUN_CLOCK2_MIN)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK2, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK2, 'm', &mSteps);
            }           
            
            ctl_events_set_clear(&clock2Event, 0, RUN_CLOCK2_MIN);  // clear event when at position
        }

   
        
        v++;
    }  
}

void clock3_func(void *p)
{  
    unsigned int v=0;
    uint8_t mSteps, hSteps;
    
    // Initialise clock 3 events
    ctl_events_init(&clock3Event, 0);
    
    // Set up GPIO
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_10, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_11, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_0, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_10, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].min.dirPort, motorData[CLOCK3].min.dirPin);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].min.port, motorData[CLOCK3].min.pin);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].hour.dirPort, motorData[CLOCK3].hour.dirPin);  // Dir D 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, motorData[CLOCK3].hour.port, motorData[CLOCK3].hour.pin);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK3].min.hallPort, motorData[CLOCK3].min.hallPin);  // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, motorData[CLOCK3].hour.hallPort, motorData[CLOCK3].hour.hallPort);  // Hall hour
    
    // Set up interrupts
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK3].min.hallPort, motorData[CLOCK3].min.hallPin, GPIO_INT_FALLING_EDGE);
    Chip_GPIO_SetupPinInt(LPC_GPIO, motorData[CLOCK3].hour.hallPort, motorData[CLOCK3].hour.hallPin, GPIO_INT_FALLING_EDGE);
    
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
    
    
    while (1)
    {      
            
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock3Event, RUN_ALL_CLOCKS|RUN_CLOCK3_HOUR|RUN_CLOCK3_MIN, CTL_TIMEOUT_NONE, 0);      
      
        // Drive clock 3 hour arm until at desired position
        if(clock3Event & RUN_CLOCK3_HOUR)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK3, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK3, 'h', &hSteps);
            }     

            ctl_events_set_clear(&clock3Event, 0, RUN_CLOCK3_HOUR); // clear event when at position
        }
        
        // Drive clock 3 minute arm until at desired position
        if(clock3Event & RUN_CLOCK3_MIN)
        {
            if(clockControlEvent & HOME_CLOCKS)
            {
                drive_continuous(CLOCK3, homingSpeed, homingDir);
            }
            else
            {
                drive_to_pos(CLOCK3, 'm', &mSteps);
            }      
                                   
            ctl_events_set_clear(&clock3Event, 0, RUN_CLOCK3_MIN);  // clear event when at position
        }

        v++;
    }  
}



// Main clock control thread
void clock_control(void *p)
{
    unsigned int v = 0;
    
    timerFreq = Chip_Clock_GetSystemClockRate();
    
    // Enable IRQs for GPIO port 2
    NVIC_EnableIRQ(EINT2_IRQn);
    
    // Set up stepper driver GPIO - stepper reset control 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_1, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN)); // Pulldown per VID6606 datasheet 
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 1);  // Reset
     
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
   
    // Initialise clock event
    ctl_events_init(&clockControlEvent, 0);
    
    // Set reset pin high 
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 1); 
    
    
    #if TESTING
        clock_testing();
    #endif
    
    // Trigger an update to all clocks on startup
    //ctl_events_set_clear(&clockControlEvent, UPDATE_ALL_CLOCKS, 0);
    

    
    while(1)
    {
        // Wait for any clock event to be triggered
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clockControlEvent, UPDATE_ALL_CLOCKS|UPDATE_CLOCK0_MIN|UPDATE_CLOCK0_HOUR|UPDATE_CLOCK1_MIN|UPDATE_CLOCK1_HOUR|
            UPDATE_CLOCK2_MIN|UPDATE_CLOCK2_HOUR|UPDATE_CLOCK3_MIN|UPDATE_CLOCK3_HOUR|CAN_UPDATE, CTL_TIMEOUT_NONE, 0);
         
        // Update clock 0 minute remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK0_MIN | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK0].min.remainingSteps = calculate_steps(motorData[CLOCK0].min.angleDesired, motorData[CLOCK0].min.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK0_MIN);
        }
        
        // Update clock 0 hour remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK0_HOUR | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK0].hour.remainingSteps = calculate_steps(motorData[CLOCK0].hour.angleDesired, motorData[CLOCK0].hour.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK0_HOUR);
        }
        
        // Update clock 1 minute remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK1_MIN | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK1].min.remainingSteps = calculate_steps(motorData[CLOCK1].min.angleDesired, motorData[CLOCK1].min.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK1_MIN);  
        }
        
        // Update clock 1 hour remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK1_HOUR | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK1].hour.remainingSteps = calculate_steps(motorData[CLOCK1].hour.angleDesired, motorData[CLOCK1].hour.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK1_HOUR);
        }
        // Update clock 2 minute remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK2_MIN | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK2].min.remainingSteps = calculate_steps(motorData[CLOCK2].min.angleDesired, motorData[CLOCK2].min.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK2_MIN);
        }
        
        // Update clock 2 hour remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK2_HOUR | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK2].hour.remainingSteps = calculate_steps(motorData[CLOCK2].hour.angleDesired, motorData[CLOCK2].hour.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK2_HOUR);
        }
        
        // Update clock 3 minute remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK3_MIN | UPDATE_ALL_CLOCKS))
        {
            motorData[CLOCK3].min.remainingSteps = calculate_steps(motorData[CLOCK3].min.angleDesired, motorData[CLOCK3].min.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK3_MIN);  
        }
        
        // Update clock 3 hour remaining steps 
        if(clockControlEvent & (UPDATE_CLOCK3_HOUR | UPDATE_ALL_CLOCKS))    
        {
            motorData[CLOCK3].hour.remainingSteps = calculate_steps(motorData[CLOCK3].hour.angleDesired, motorData[CLOCK3].hour.angle);
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_CLOCK3_HOUR);
        }
        
        // Update position of all clocks
        if(clockControlEvent & UPDATE_ALL_CLOCKS)
        {
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_ALL_CLOCKS);
        }
        
        // Update with data received from CAN bus
        if(clockControlEvent & CAN_UPDATE)
        {
            /* Message types:
            0x200 - position
            0x201 - speed
            0x202 - acceleration
            0x203 - start motion */
            
            // position update
            if(can_RX_data.mode_id == POSITION)
            {
                for(int i = FIRSTCLOCK; i < NUMBEROFCLOCKS; i++)
                {
                    if(can_RX_data.data[0] == motorData[i].clockNumber) // find which clock to update
                    {
                        motorData[i].min.angleDesired = ((can_RX_data.data[1] << 8) | (can_RX_data.data[2])); // minute
                        motorData[i].hour.angleDesired = ((can_RX_data.data[3] << 8) | (can_RX_data.data[4])); // hour
                        ctl_events_set_clear(&clockControlEvent, UPDATE_ALL_CLOCKS, 0);
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
                    motorData[0].min.start = 1;
                    motorData[1].min.start = 1;
                    motorData[2].min.start = 1;
                    motorData[3].min.start = 1;
                    motorData[0].hour.start = 1;
                    motorData[1].hour.start = 1;
                    motorData[2].hour.start = 1;
                    motorData[3].hour.start = 1;
                }
            }
        
            ctl_events_set_clear(&clockControlEvent, 0, CAN_UPDATE);
        }
        
        // Execute homing procedure
        if(clockControlEvent & HOME_CLOCKS)
        {
            //ctl_events_set_clear(&clockControlEvent, RUN_ALL_CLOCKS, 0);
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_HOUR, 0);
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_MIN, 0);
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_HOUR, 0);
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_MIN, 0);
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_HOUR, 0);
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_MIN, 0);
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_HOUR, 0);
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_MIN, 0);

            ctl_events_set_clear(&clockControlEvent, 0, HOME_CLOCKS);            
        }
            
        v++;
       
    }
    
    
}

// Receive data from CAN bus
void update_from_CAN(CCAN_MSG_OBJ_T *CANdata)
{
    can_RX_data = *CANdata;
    
    if(!localControl)
    {
        ctl_events_set_clear(&clockControlEvent, CAN_UPDATE, 0);
    }
 

}



<<<<<<< HEAD
// Drive clock at constant speed
void drive_continuous(const uint8_t clockNum, const uint8_t speed, const uint8_t dir)
{
     
    pulse_generation(clockNum, 'm'); // Generate stepper pulse
    pulse_generation(clockNum, 'h'); // Generate stepper pulse
    
    motorData[clockNum].min.speed = speed;
    motorData[clockNum].hour.speed = speed;
    
    motorData[clockNum].min.dir = dir;
    motorData[clockNum].hour.dir = dir;
    
    motorData[clockNum].min.angle++;
    if(motorData[clockNum].min.angle == 360)
    {
        motorData[clockNum].min.angle = 0;
    }
    
    motorData[clockNum].hour.angle++;
    if(motorData[clockNum].hour.angle == 360)
    {
        motorData[clockNum].hour.angle = 0;
    }
 
}

// Homing Procedure
void home_clocks(void)
{
    uint8_t hallBit = 1;
    uint8_t speed = 2;
    uint8_t direction = 1; 
    
    localControl = 1; 
    
    // Drive clocks CW until hall is hit

    //Chip_GPIO_EnableInt(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin);
    //Chip_GPIO_ClearInts(LPC_GPIO, motorData[CLOCK0].min.hallPort, motorData[CLOCK0].min.hallPin); 
    ctl_events_set_clear(&clockControlEvent, HOME_CLOCKS, 0);
    motorData[0].min.start = 1;
    motorData[1].min.start = 1;
    motorData[2].min.start = 1;
    motorData[3].min.start = 1;
    motorData[0].hour.start = 1;
    motorData[1].hour.start = 1;
    motorData[2].hour.start = 1;
    motorData[3].hour.start = 1;

    // Save position/angle
    
    // Drive past hall
    
    
    
    // Drive CCW until hall is hit
    
    // Save position/angle
    
    
    // Calculate home position
    
    
    // Drive to home
    
}
=======
>>>>>>> refs/remotes/origin/dev
    
// Receive updates from can bus and apply to motor setpoints
//void update_from_CAN(CCAN_MSG_OBJ_T *CANdata)
//{
//    /* Message types:
//            0x200 - position
//            0x201 - speed
//            0x202 - acceleration */
       
     
//    // position update
//    if(CANdata->mode_id == 0x200)
//    {
//        for(int i = 0; i < NUMBEROFCLOCKS; i++)
//        {
//            if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
//            {
//                motorData[i].min.angleDesired = ((CANdata->data[1] << 8) | (CANdata->data[2])); // minute
//                motorData[i].hour.angleDesired = ((CANdata->data[3] << 8) | (CANdata->data[4])); // hour
//                //moveComplete = 0;
//            }
//        }
//    }
//    // speed and direction update
//    if (CANdata->mode_id == 0x201)
//    {
//        for(int i = 0; i < NUMBEROFCLOCKS; i++)
//        {
//            if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
//            {
//                motorData[i].min.speed = (CANdata->data[1]);    // minute speed
//                motorData[i].hour.speed = (CANdata->data[2]);  // hour speed
//                motorData[i].min.dir = (CANdata->data[3]);    // minute direction
//                motorData[i].hour.dir = (CANdata->data[4]);  // hour direction
//            }
//        }
        
//    }
//    // acceleration update
//    if (CANdata->mode_id == 0x202)
//    {
        
//    }
    
//    // start movement
//    if (CANdata->mode_id == 0x203)
//    {
//        if(CANdata->data[0] == 200)
//        {
//            moveComplete = 0;
//        }
        
//        // Individual clock 
//        //else
//        //{
//        //    for(int i = 0; i < NUMBEROFCLOCKS; i++)
//        //    {
//        //        if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
//        //        {
//        //           motorData[i].hour.atPosition = 0;
//        //        }
//        //    }
//        //}
//    }
        
//}    