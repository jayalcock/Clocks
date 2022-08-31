#include <stdint.h>


#include "board.h"
#include "clockDriver.h"


#define NUMBEROFCLOCKS 4
#define NUMBEROFARMS 2
#define TIMESTEPMS 1
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 500UL

uint16_t angle[NUMBEROFCLOCKS][2] = {0}; // minute and hour angle 
uint16_t angleNew[NUMBEROFCLOCKS][2] = {0}; // desired minute and hour angle
uint8_t updatePosition = 0;
static uint8_t moveComplete = 0;

motorStruct motorData[] =
{
    {1, 0, 1, 2, 0, 360, 0, 0, 1}, // motor 0 - minute
    {1, 1, 1, 5, 0, 360, 1, 0, 1}, // motor 1 - hour
    {2, 3, 2, 7, 0, 360, 1, 0, 1}, // motor 2 - minute
    {2, 6, 2, 8, 0, 360, 1, 0, 1}, // motor 3 - hour
    
};


clockArmStruct remainingSteps[NUMBEROFCLOCKS];


void pulse_generation(const uint8_t motorNum)
{
    // Set direction
    if(motorData[motorNum].dir == 0)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].dirPort, motorData[motorNum].dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].dirPort, motorData[motorNum].dirPin);
    }
    
    // Generate pulse
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].port, motorData[motorNum].pin);
    pulse_delay(5);
    Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].port, motorData[motorNum].pin);
    
    //Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 8);
    //pulse_delay(10);
    //Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 3);
}

// TODO - clean this up - devleop proper function 
void pulse_delay(const uint16_t time)
{
    // small delay for correct pulse width
    for(int i = 0; i < time; i++)
    {
        __asm volatile ("nop");
    }
    
}
    

void position_control(void)
{
    uint8_t i, j;
    //clockArmStruct remainingSteps[NUMBEROFCLOCKS];

    // calculate number of steps between desirec and actual angles
    for(i = 0; i < NUMBEROFCLOCKS; i++)
    {

        motorData[i].remainingSteps = calculate_steps(motorData[i].angleDesired, motorData[i].angle);

           if(motorData[i].remainingSteps != 0)
        {
            motorData[i].atPosition = 0;
        }

    }    

    
    // drive each clock motor until each arm is at desired angle
    while(!moveComplete)
    {
        moveComplete = 1;
        for(i = 0; i < NUMBEROFCLOCKS; i++)
        {
            if(!motorData[i].atPosition)
            {
                pulse_generation(i);
            
                --motorData[i].remainingSteps;
            
                if(motorData[i].remainingSteps == 0)
                {
                    motorData[i].atPosition = 1;
                }
            
                moveComplete = 0;
            }

            pulse_delay(PULSEWIDTH); // delay for correct pulse width

        }
        
    }
   

    
    // check for 0 > angle > 360
    
    // return at angle status
    
    //Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 6); // Set reset pin low
    
    
}

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle , uint16_t angle)
{
    return (newAngle - angle) * STEPSIZE;   
}

void update(void)
{
    updatePosition = 1;
}    