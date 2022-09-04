#include <stdint.h>


#include "board.h"
#include "clockDriver.h"


#define NUMBEROFCLOCKS 2
#define NUMBEROFARMS 2
#define TIMESTEPMS 1
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 5
//#define PULSEPERIOD 10000

//CCAN_MSG_OBJ_T msgObj;

uint16_t angle[NUMBEROFCLOCKS][2] = {0}; // minute and hour angle 
uint16_t angleNew[NUMBEROFCLOCKS][2] = {0}; // desired minute and hour angle
uint8_t updatePosition = 0;
static uint8_t moveComplete = 0;

uint16_t speed[] = {500, 1000, 2500, 5000, 7500, 10000};
    

motorStruct motorData[] =
{
    {0, //motor num
        //minute
        { 1, 0, // pulse port/pin 
          1, 2, // direction port/pin 
          0,    // angle
          0,    // desired angle 
          0,    // direction
          0,    // remaining steps
          1,    // at position
          1,    // speed
          2 },  // accel
        //hour
        { 1, 1, // pulse port/pin
          1, 5, // direction port/pin
          0,    // angle
          0,    // desired angle
          0,    // direction
          0,    // remaining steps
          1,    // at position
          1,    // speed
          2 }}, // accel
                
    {1, //motor num
        {2, 3, 2, 7, 0, 0, 1, 0, 1, 1, 2}, //min
        {2, 6, 2, 8, 0, 0, 1, 0, 1, 1, 2}}, //hour         
              
};



void pulse_generation(const uint8_t motorNum, const char arm)
{
    // Set direction
    if(motorData[motorNum].min.dir == 0)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.dirPort, motorData[motorNum].min.dirPin);
    }
    if(motorData[motorNum].hour.dir == 0)
    {
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    else
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.dirPort, motorData[motorNum].hour.dirPin);
    }
    
    // Generate pulse
    if(arm == 'm')
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
        pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].min.port, motorData[motorNum].min.pin);
    }
    if(arm == 'h')
    {
        Chip_GPIO_SetPinOutHigh(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
        pulse_delay(PULSEWIDTH);
        Chip_GPIO_SetPinOutLow(LPC_GPIO, motorData[motorNum].hour.port, motorData[motorNum].hour.pin);
    }
    
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
    

void clock_control(void)
{
    uint8_t i, j;

    // calculate number of steps between desired and actual angles
    for(i = 0; i < NUMBEROFCLOCKS; i++)
    {

        motorData[i].min.remainingSteps = calculate_steps(motorData[i].min.angleDesired, motorData[i].min.angle);
        motorData[i].hour.remainingSteps = calculate_steps(motorData[i].hour.angleDesired, motorData[i].hour.angle);
        
        if(motorData[i].min.remainingSteps != 0)
        {
            motorData[i].min.atPosition = 0;
        }

        if(motorData[i].hour.remainingSteps != 0)
        {
            motorData[i].hour.atPosition = 0;
        }
    }

    
    // drive each clock motor until each arm is at desired angle
    while(!moveComplete)
    {
        moveComplete = 1;
        for(i = 0; i < NUMBEROFCLOCKS; i++)
        {
            if(!motorData[i].min.atPosition)
            {
                pulse_generation(i, 'm');
            
                --motorData[i].min.remainingSteps;
            
                if(motorData[i].min.remainingSteps <= 0)
                {
                    motorData[i].min.atPosition = 1;
                    motorData[i].min.angle = motorData[i].min.angleDesired;
                }
            
                moveComplete = 0;
            }
            if(!motorData[i].hour.atPosition)
            {
                pulse_generation(i, 'h');
            
                --motorData[i].hour.remainingSteps;
            
                if(motorData[i].hour.remainingSteps <= 0)
                {
                    motorData[i].hour.atPosition = 1;
                    motorData[i].hour.angle = motorData[i].hour.angleDesired;
                }
            
                moveComplete = 0;
            }

            //pulse_delay(PULSEPERIOD); // delay for correct pulse width
            pulse_delay(speed[motorData[i].min.speed]);
        }
        
    }
   

    
    // check for 0 > angle > 360
    
    // return at angle status
    
    //Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 6); // Set reset pin low
    
    
}

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle , uint16_t angle)
{
    if((newAngle - angle) < 0)
    {
        return (newAngle - angle + 360) * STEPSIZE;  
    }
    else
    {    
        return (newAngle - angle) * STEPSIZE;   
    }
}

void update_from_CAN(CCAN_MSG_OBJ_T *CANdata)
{
    /* Message types:
            0x200 - position
            0x201 - speed
            0x202 - acceleration */
       
     
    // position update
    if(CANdata->mode_id == 0x200)
    {
        for(int i = 0; i < NUMBEROFCLOCKS; i++)
        {
            if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
            {
                motorData[i].min.angleDesired = ((CANdata->data[1] << 8) | (CANdata->data[2])); // minute
                motorData[i].hour.angleDesired = ((CANdata->data[3] << 8) | (CANdata->data[4])); // hour
                moveComplete = 0;
                updatePosition = 1;
            }
        }
    }
    // speed and direction update
    if (CANdata->mode_id == 0x201)
    {
        for(int i = 0; i < NUMBEROFCLOCKS; i++)
        {
            if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
            {
                motorData[i].min.speed = (CANdata->data[1]);    // minute speed
                motorData[i].hour.speed = (CANdata->data[2]);  // hour speed
                motorData[i].min.dir = (CANdata->data[3]);    // minute direction
                motorData[i].hour.dir = (CANdata->data[4]);  // hour direction
            }
        }
        
    }
    // acceleration update
    if (CANdata->mode_id == 0x202)
    {
        
    }
}    