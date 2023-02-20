#include <stdint.h>
#include "ccand_11xx.h"
#include "clockDriverRT.h"
#include "board.h"
#include "ctl_api.h"
#include "timer_11xx.h"
#include "clock_11xx.h"
#include "ccan_rom.h"


#define NUMBEROFCLOCKS 2
#define NUMBEROFARMS 2
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 3

#define CLOCK0          0
#define CLOCK1          1
#define CLOCK2          2
#define CLOCK3          3

#define RUN_ALL_CLOCKS  1<<0
#define RUN_CLOCK0_MIN  1<<1
#define RUN_CLOCK0_HOUR 1<<2
#define RUN_CLOCK1_MIN  1<<3
#define RUN_CLOCK1_HOUR 1<<4
#define RUN_CLOCK2_MIN  1<<5
#define RUN_CLOCK2_HOUR 1<<6
#define RUN_CLOCK3_MIN  1<<7
#define RUN_CLOCK3_HOUR 1<<8

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

CTL_EVENT_SET_t clock0Event, clock1Event, clock2Event, clock3Event, clockControlEvent;

CCAN_MSG_OBJ_T can_RX_data;

const uint16_t speed[] = {100, 200, 400, 800, 1600, 3200};

uint32_t timerFreq;



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
          2 },  // accel
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
          2 }}, // accel
                
    {1, //clock num
        {0, 7, 0, 6, 0, 0, 1, 0, 0, 1, 2, 2}, //min
        {0, 9, 0, 8, 0, 0, 1, 0, 0, 1, 2, 2}}, //hour         
                            
    {2, //clock num
        {1, 5, 1, 4, 0, 0, 1, 0, 0, 1, 2, 2}, //min
        {1, 7, 1, 6, 0, 0, 1, 0, 0, 1, 2, 2}}, //hour   
                     
    {3, //clock num
        {1, 8, 1, 10, 0, 0, 1, 0, 0, 1, 2, 2}, //min
        {2, 0, 1, 11, 0, 0, 1, 0, 0, 1, 2, 2}}, //hour   
          
};

// Interrupt handler for 32-bit timer 0 - Controlling minute arm speeds
void CT32B0_IRQHandler(void)
{
    // Clock 0 minute timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 0);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 0, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[CLOCK0].min.speed]);
        
        if(motorData[CLOCK0].min.start == 1)
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_MIN, 0);

    }
    // Clock 1 minute timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[1].min.speed]);
        
        if(motorData[1].min.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_MIN, 0);

    }
    // Clock 2 minute timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 2);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 2, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[2].min.speed]);
        
        if(motorData[2].min.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_MIN, 0);

    }
    // Clock 3 minute timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_0, 3);
        Chip_TIMER_SetMatch(LPC_TIMER32_0, 3, Chip_TIMER_ReadCount(LPC_TIMER32_0) + timerFreq/speed[motorData[3].min.speed]);
        
        if(motorData[3].min.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_MIN, 0);

    }
  
} 

// Interrupt handler for 32-bit timer 1 - Controlling hour arm speeds
void CT32B1_IRQHandler(void)
{
    // Clock 0 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 0);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 0, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[0].hour.speed]);
        
        if(motorData[0].hour.start == 1)
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 1);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[1].hour.speed]);
        
        if(motorData[1].hour.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 2);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 2, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[2].hour.speed]);
        
        if(motorData[2].hour.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER32_1, 3);
        Chip_TIMER_SetMatch(LPC_TIMER32_1, 3, Chip_TIMER_ReadCount(LPC_TIMER32_1) + timerFreq/speed[motorData[3].hour.speed]);
        
        if(motorData[3].hour.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_HOUR, 0);

    }
  
}   

// Interrupt handler for 32-bit timer 1 - Controlling hour arm speeds
void CT16B0_IRQHandler(void)
{
    // Clock 0 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, 0)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, 0);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, 0, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[0].hour.speed]);
        
        if(motorData[0].hour.start == 1)
            ctl_events_set_clear(&clock0Event, RUN_CLOCK0_HOUR, 0);

    }
    // Clock 1 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, 1)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, 1);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, 1, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[1].hour.speed]);
        
        if(motorData[1].hour.start == 1)
            ctl_events_set_clear(&clock1Event, RUN_CLOCK1_HOUR, 0);

    }
    // Clock 2 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, 2)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, 2);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, 2, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[2].hour.speed]);
        
        if(motorData[2].hour.start == 1)
            ctl_events_set_clear(&clock2Event, RUN_CLOCK2_HOUR, 0);

    }
    // Clock 3 hour timer - match clear and match value reset
    if (Chip_TIMER_MatchPending(LPC_TIMER16_0, 3)) 
    {
        Chip_TIMER_ClearMatch(LPC_TIMER16_0, 3);
        Chip_TIMER_SetMatch(LPC_TIMER16_0, 3, Chip_TIMER_ReadCount(LPC_TIMER16_0) + timerFreq/speed[motorData[3].hour.speed]);
        
        if(motorData[3].hour.start == 1)
            ctl_events_set_clear(&clock3Event, RUN_CLOCK3_HOUR, 0);

    }
  
}   
void pulse_generation(const uint8_t motorNum, const char arm)
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

// Clock 0 control function 
void clock0_func(void *p)
{  
    unsigned int v=0;
    const uint8_t clockNum = 0;
    uint8_t mSteps, hSteps;
    
    // Initialise clock 0 events
    ctl_events_init(&clock0Event, 0);
      
    
    // Set up GPIO
    // Driver 1
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_1, (IOCON_FUNC0 | IOCON_MODE_INACT)); // Pulldown per VID6606 datasheet 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
   
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 1);  // Reset
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 2);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 3);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 4);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 5);  // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 1);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 2);   // Hall hour
    
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 1); // Set reset pin high 
    
    //Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 11);
    //Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);

    timerFreq = Chip_Clock_GetSystemClockRate();
    Chip_TIMER_Init(LPC_TIMER32_0);
    //Chip_TIMER_Init(LPC_TIMER32_1);
    Chip_TIMER_Init(LPC_TIMER16_0);


    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 0);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 0, timerFreq/10);
    
    //Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 0);
    //Chip_TIMER_SetMatch(LPC_TIMER32_1, 0, timerFreq/10);
    Chip_TIMER_SetMatch(LPC_TIMER16_0, 0, timerFreq/10);

    // Enable timers for stepper pulses
    Chip_TIMER_Enable(LPC_TIMER32_0);
    //Chip_TIMER_Enable(LPC_TIMER32_1);
    Chip_TIMER_Enable(LPC_TIMER16_0);
    
    /* Enable timer interrupt */
    NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
    NVIC_EnableIRQ(TIMER_32_0_IRQn);

    //NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);
    //NVIC_EnableIRQ(TIMER_32_1_IRQn);
    NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
    NVIC_EnableIRQ(TIMER_16_0_IRQn);

    
    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock0Event, RUN_ALL_CLOCKS|RUN_CLOCK0_HOUR|RUN_CLOCK0_MIN, CTL_TIMEOUT_NONE, 0);  
                
        // Drive clock 0 hour arm until at desired position
        if(clock0Event & RUN_CLOCK0_HOUR)
        {
       
            if(motorData[clockNum].hour.remainingSteps != 0)
            {
                motorData[clockNum].hour.atPosition = 0;
            }
        
            if(!motorData[clockNum].hour.atPosition)
            {
                pulse_generation(clockNum, 'h');
            
                --motorData[clockNum].hour.remainingSteps;
                
                hSteps++;
                
                if(hSteps == STEPSIZE)
                {
                    motorData[clockNum].hour.angle++;
                    
                    if(motorData[clockNum].hour.angle == 360)
                    {
                        motorData[clockNum].hour.angle = 0;
                    }
                    hSteps = 0;
                }
            
                if(motorData[clockNum].hour.remainingSteps <= 0)
                {
                    motorData[clockNum].hour.atPosition = 1;
                    motorData[clockNum].hour.start == 0;
                }

            }
            
            ctl_events_set_clear(&clock0Event, 0, RUN_CLOCK0_HOUR); // clear event when at position
        }
        
        // Drive clock 0 minute arm until at desired position
        if(clock0Event & RUN_CLOCK0_MIN)
        {
                       
            if(motorData[clockNum].min.remainingSteps != 0)
            {
                motorData[clockNum].min.atPosition = 0;
            }
            
            if(!motorData[clockNum].min.atPosition)
            {
                pulse_generation(clockNum, 'm');
            
                --motorData[clockNum].min.remainingSteps;
                
                mSteps++;
                
                if(mSteps == STEPSIZE)
                {
                    motorData[clockNum].min.angle++;
                    
                    if(motorData[clockNum].min.angle == 360)
                    {
                        motorData[clockNum].min.angle = 0;
                    }
                    
                    mSteps = 0;
                }
            
                if(motorData[clockNum].min.remainingSteps <= 0)
                {
                    motorData[clockNum].min.atPosition = 1;
                    motorData[clockNum].min.start == 0;
                }

            }
            
            ctl_events_set_clear(&clock0Event, 0, RUN_CLOCK0_MIN);  // clear event when at position
        }

        v++;
    }  
}

void clock1_func(void *p)
{  
    unsigned int v=0;
    const uint8_t clockNum = 1;
    uint8_t mSteps, hSteps;
     
    // Initialise clock 1 events
    ctl_events_init(&clock1Event, 0);
    
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 6);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 7);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 8);  // Dir D
    //Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 3);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 6);   // Hall hour
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, timerFreq/10);
   
    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock1Event, RUN_ALL_CLOCKS|RUN_CLOCK1_MIN|RUN_CLOCK1_HOUR, CTL_TIMEOUT_NONE, 0);      
        
        // Drive clock 1 hour arm until at desired position
        if(clock1Event & RUN_CLOCK1_HOUR)
        {
            if(motorData[clockNum].hour.remainingSteps != 0)
            {
                motorData[clockNum].hour.atPosition = 0;
            }
        
            if(!motorData[clockNum].hour.atPosition)
            {
                pulse_generation(clockNum, 'h');
            
                --motorData[clockNum].hour.remainingSteps;
                
                hSteps++;
                
                if(hSteps == STEPSIZE)
                {
                    motorData[clockNum].hour.angle++;
                    
                    if(motorData[clockNum].hour.angle == 360)
                    {
                        motorData[clockNum].hour.angle = 0;
                    }
                    hSteps = 0;
                }
            
                if(motorData[clockNum].hour.remainingSteps <= 0)
                {
                    motorData[clockNum].hour.atPosition = 1;
                    motorData[clockNum].hour.start == 0;
                }
            
            }
            
            ctl_events_set_clear(&clock1Event, 0, RUN_CLOCK1_HOUR); // clear event when at position
        }
        
        // Drive clock 1 minute arm until at desired position
        if(clock1Event & RUN_CLOCK1_MIN)
        {
                       
            if(motorData[clockNum].min.remainingSteps != 0)
            {
                motorData[clockNum].min.atPosition = 0;
            }
            
            if(!motorData[clockNum].min.atPosition)
            {
                pulse_generation(clockNum, 'm');
            
                --motorData[clockNum].min.remainingSteps;
                
                mSteps++;
                
                if(mSteps == STEPSIZE)
                {
                    motorData[clockNum].min.angle++;
                    
                    if(motorData[clockNum].min.angle == 360)
                    {
                        motorData[clockNum].min.angle = 0;
                    }
                    
                    mSteps = 0;
                }
            
                if(motorData[clockNum].min.remainingSteps <= 0)
                {
                    motorData[clockNum].min.atPosition = 1;
                    motorData[clockNum].min.start == 0;
                }

            }
            
            ctl_events_set_clear(&clock1Event, 0, RUN_CLOCK1_MIN);  // clear event when at position
        }

            
        v++;
    }  
}

void clock2_func(void *p)
{  
    unsigned int v=0;
    const uint8_t clockNum = 2;
    uint8_t mSteps, hSteps;

    // Initialise clock 2 events
    ctl_events_init(&clock2Event, 0);
    
    // Driver 2
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_4, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 4);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 5);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 6);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 7); // Pulse B
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 7);   // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 8);   // Hall hour
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 2);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 2, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 2);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 2, timerFreq/10);
    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock2Event, RUN_ALL_CLOCKS|RUN_CLOCK2_MIN|RUN_CLOCK2_HOUR, CTL_TIMEOUT_NONE, 0);     
        
        // Drive clock 2 hour arm until at desired position
        if(clock2Event & RUN_CLOCK2_HOUR)
        {
            if(motorData[clockNum].hour.remainingSteps != 0)
            {
                motorData[clockNum].hour.atPosition = 0;
            }
        
            if(!motorData[clockNum].hour.atPosition)
            {
                pulse_generation(clockNum, 'h');
            
                --motorData[clockNum].hour.remainingSteps;
                
                hSteps++;
                
                if(hSteps == STEPSIZE)
                {
                    motorData[clockNum].hour.angle++;
                    
                    if(motorData[clockNum].hour.angle == 360)
                    {
                        motorData[clockNum].hour.angle = 0;
                    }
                    hSteps = 0;
                }
            
                if(motorData[clockNum].hour.remainingSteps <= 0)
                {
                    motorData[clockNum].hour.atPosition = 1;
                    motorData[clockNum].hour.start == 0;
                }
            
            }
            
            ctl_events_set_clear(&clock2Event, 0, RUN_CLOCK2_HOUR); // clear event when at position
        }
        
        // Drive clock 2 minute arm until at desired position
        if(clock2Event & RUN_CLOCK2_MIN)
        {
                       
            if(motorData[clockNum].min.remainingSteps != 0)
            {
                motorData[clockNum].min.atPosition = 0;
            }
            
            if(!motorData[clockNum].min.atPosition)
            {
                pulse_generation(clockNum, 'm');
            
                --motorData[clockNum].min.remainingSteps;
                
                mSteps++;
                
                if(mSteps == STEPSIZE)
                {
                    motorData[clockNum].min.angle++;
                    
                    if(motorData[clockNum].min.angle == 360)
                    {
                        motorData[clockNum].min.angle = 0;
                    }
                    
                    mSteps = 0;
                }
            
                if(motorData[clockNum].min.remainingSteps <= 0)
                {
                    motorData[clockNum].min.atPosition = 1;
                    motorData[clockNum].min.start == 0;
                }

            }
            
            ctl_events_set_clear(&clock2Event, 0, RUN_CLOCK2_MIN);  // clear event when at position
        }

   
        
        v++;
    }  
}

void clock3_func(void *p)
{  
    unsigned int v=0;
    const uint8_t clockNum = 3;
    uint8_t mSteps, hSteps;
    
    // Initialise clock 3 events
    ctl_events_init(&clock3Event, 0);
    
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_10, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_11, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_0, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_10, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    // Set required pin direction
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 8);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10); // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 11); // Dir D
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 0);  // Pulse D
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 10);  // Hall min
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 11);  // Hall hour
    
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 3);
    Chip_TIMER_SetMatch(LPC_TIMER32_0, 3, timerFreq/10);
    
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 3);
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 3, timerFreq/10);

    
    while (1)
    {      
            
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &clock3Event, RUN_ALL_CLOCKS|RUN_CLOCK3_HOUR|RUN_CLOCK3_MIN, CTL_TIMEOUT_NONE, 0);      
      
        // Drive clock 3 hour arm until at desired position
        if(clock3Event & RUN_CLOCK3_HOUR)
        {
            if(motorData[clockNum].hour.remainingSteps != 0)
            {
                motorData[clockNum].hour.atPosition = 0;
            }
        
            if(!motorData[clockNum].hour.atPosition)
            {
                pulse_generation(clockNum, 'h');
            
                --motorData[clockNum].hour.remainingSteps;
                
                hSteps++;
                
                if(hSteps == STEPSIZE)
                {
                    motorData[clockNum].hour.angle++;
                    
                    if(motorData[clockNum].hour.angle == 360)
                    {
                        motorData[clockNum].hour.angle = 0;
                    }
                    hSteps = 0;
                }
            
                if(motorData[clockNum].hour.remainingSteps <= 0)
                {
                    motorData[clockNum].hour.atPosition = 1;
                    motorData[clockNum].hour.start == 0;
                }
            
            }
            
            ctl_events_set_clear(&clock3Event, 0, RUN_CLOCK3_HOUR); // clear event when at position
        }
        
        // Drive clock 3 minute arm until at desired position
        if(clock3Event & RUN_CLOCK3_MIN)
        {
                       
            if(motorData[clockNum].min.remainingSteps != 0)
            {
                motorData[clockNum].min.atPosition = 0;
            }
            
            if(!motorData[clockNum].min.atPosition)
            {
                pulse_generation(clockNum, 'm');
            
                --motorData[clockNum].min.remainingSteps;
                
                mSteps++;
                
                if(mSteps == STEPSIZE)
                {
                    motorData[clockNum].min.angle++;
                    
                    if(motorData[clockNum].min.angle == 360)
                    {
                        motorData[clockNum].min.angle = 0;
                    }
                    
                    mSteps = 0;
                }
            
                if(motorData[clockNum].min.remainingSteps <= 0)
                {
                    motorData[clockNum].min.atPosition = 1;
                    motorData[clockNum].min.start == 0;
                }

            }
            
            ctl_events_set_clear(&clock3Event, 0, RUN_CLOCK3_MIN);  // clear event when at position
        }

        v++;
    }  
}

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle , uint16_t angle)
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

void clock_control(void *p)
{
    unsigned int v = 0;  
    
    ctl_events_init(&clockControlEvent, 0);
    
    // Initialise CAN driver
    can_init();
    
    // Trigger an update to all clocks on startup
    ctl_events_set_clear(&clockControlEvent, UPDATE_ALL_CLOCKS, 0);
    
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
        
        if(clockControlEvent & UPDATE_ALL_CLOCKS)
        {
            ctl_events_set_clear(&clockControlEvent, 0, UPDATE_ALL_CLOCKS);
        }
        
        
        if(clockControlEvent & CAN_UPDATE)
        {
            /* Message types:
            0x200 - position
            0x201 - speed
            0x202 - acceleration
            0x203 - start motion */
            
            // position update
            if(can_RX_data.mode_id == 0x200)
            {
                for(int i = 0; i < NUMBEROFCLOCKS; i++)
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
            if (can_RX_data.mode_id == 0x201)
            {
                for(int i = 0; i < NUMBEROFCLOCKS; i++)
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
            if (can_RX_data.mode_id == 0x202)
            {
            }
            
            // start motion
            if (can_RX_data.mode_id == 0x203)
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
        v++;
       
    }
    
    
}

// Receive data from CAN bus
void update_from_CAN(CCAN_MSG_OBJ_T *CANdata)
{
    can_RX_data = *CANdata;
    
    ctl_events_set_clear(&clockControlEvent, CAN_UPDATE, 0);
   
 

}
    


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