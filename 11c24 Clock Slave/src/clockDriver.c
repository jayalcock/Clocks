#include <stdint.h>


#include "board.h"
#include "clockDriver.h"


#define NUMBEROFCLOCKS 4
#define NUMBEROFARMS 2
#define TIMESTEPMS 1
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 8000UL

uint16_t angle[NUMBEROFCLOCKS][2] = {0}; // minute and hour angle 
uint16_t angleNew[NUMBEROFCLOCKS][2] = {0}; // desired minute and hour angle
uint8_t updatePosition = 0;
uint8_t moveComplete = 0;

static const motorStruct motorData[] =
{
    {0, 1, 0, 0, 0, 1}, // motor 0
    {1, 1, 1, 0, 0, 1}, // motor 1
    {2, 1, 2, 0, 0, 1}, // motor 2
    {3, 1, 5, 0, 0, 1}, // motor 3
};

void pulse_generation(const motorStruct motorNum)
{

    Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorNum.port, motorNum.pin);
    Chip_GPIO_SetPinOutLow(LPC_GPIO, motorNum.port, motorNum.pin);
}

// TODO - clean this up - devleop proper function 
void pulse_delay(void)
{
    // small delay for correct pulse width
    for(int i = 0; i < 8000; i++)
    {
        __asm volatile ("nop");
    }
    
}
    

void position_control(void)
{
    uint8_t i, j;
    //uint16_t remainingSteps[NUMBEROFCLOCKS][NUMBEROFARMS];
    clockArmStruct remainingSteps[NUMBEROFCLOCKS];

    // Calculate steps to traverse for each arm of each clock
    //for(i = 0; i < NUMBEROFCLOCKS; i++)
    //{
    //    for(j = 0; j < NUMBEROFARMS; j++)
    //    {
    //        remainingSteps[i][j] = calculate_steps(angle[i][j], angleNew[i][j]);
    //    }
    //}
    
    // calculate number of steps between angles
    for(i = 0; i < NUMBEROFCLOCKS; i++)
    {
        remainingSteps[i].min = calculate_steps(motorData[i].minAngleDesired, motorData[i].minAngle);
        remainingSteps[i].hour = calculate_steps(motorData[i].hourAngleDesired, motorData[i].hourAngle);
    }    
    
    // drive each clock motor until angle == angleNew
    while(!moveComplete)
    {
        if(!motorData[i].minAtPosition)
        {
            pulse_generation(motorData[i]);
            --remainingSteps[i].min;
        }
        
        if(!motorData[i].minAtPosition)
        {
            pulse_generation(motorData[i]);
            --remainingSteps[i].min;
        }
        
    }
    
    // check for 0 > angle > 360
    
    // return at angle status
    
}

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t angle, uint16_t newAngle)
{
    return (newAngle - angle) * STEPSIZE;   
}

void update(void)
{
    updatePosition = 1;
}    