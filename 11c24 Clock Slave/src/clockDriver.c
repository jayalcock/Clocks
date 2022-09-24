#include <stdint.h>

#include "board.h"
#include "clockDriver.h"

#define NUMBEROFCLOCKS 4
#define NUMBEROFARMS 2
#define TIMESTEPMS 1
#define STEPSIZE 12 // 1/12 degree per step
#define PULSEWIDTH 1
//#define PULSEPERIOD 10000

uint8_t moveComplete = 0;

//const uint16_t speed[] = {500, 1000, 2500, 5000, 7500, 10000};
//const uint16_t speed[] = {250, 500, 1500, 2500, 5000, 10000};
const uint16_t speed[] = {100, 200, 500, 1000, 2500, 5000};
    
motorStruct motorData[] =
{
    {0, //motor num
        //minute
        { 0, 3, // pulse port/pin 
          0, 2, // direction port/pin 
          0,    // angle
          0,    // desired angle 
          0,    // direction
          0,    // remaining steps
          1,    // at position
          1,    // speed
          2 },  // accel
        //hour
        { 0, 5, // pulse port/pin
          0, 4, // direction port/pin
          0,    // angle
          0,    // desired angle
          0,    // direction
          0,    // remaining steps
          1,    // at position
          1,    // speed
          2 }}, // accel
                
    {1, //motor num
        {0, 7, 0, 6, 0, 0, 1, 0, 1, 1, 2}, //min
        {0, 9, 0, 8, 0, 0, 1, 0, 1, 1, 2}}, //hour         
                            
    {2, //motor num
        {1, 7, 1, 6, 0, 0, 1, 0, 1, 1, 2}, //min
        {1, 10, 1, 8, 0, 0, 1, 0, 1, 1, 2}}, //hour   
                     
    {3, //motor num
        {2, 0, 1, 11, 0, 0, 1, 0, 1, 1, 2}, //min
        {2, 3, 2, 2, 0, 0, 1, 0, 1, 1, 2}}, //hour   
          
};

void driver_gpio_init(void)
{
    
    // Set up GPIO
    // Driver 1
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_1, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, (IOCON_FUNC0 | IOCON_STDI2C_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 1);  // Reset
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 2);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 3);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 4);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 5);  // Pulse B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 6);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 7);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 8);  // Dir D
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);  // Pulse D
    
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 0, 1); // Set reset pin low 
    
    // Driver 2
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_10, (IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGMODE_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_11, (IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGMODE_EN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_0, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    //Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 6);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 7);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 8);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);  // Pulse B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 11);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 0);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 1);  // Dir D
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 2);  // Pulse D
}


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
    uint8_t i;

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
            // keep driving minute arm until at position
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
            
            // keep driving hour arm until at position
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

            // delay to control speed
            //TODO develop speed control for each arm
            pulse_delay(speed[motorData[i].min.speed]);
        }
        
    }
   

    
    // check for 0 > angle > 360
    
    // return at angle status  
    
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

// Receive updates from can bus and apply to motor setpoints
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
                //moveComplete = 0;
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
    
    // start movement
    if (CANdata->mode_id == 0x203)
    {
        if(CANdata->data[0] == 200)
        {
            moveComplete = 0;
        }
        
        // Individual clock 
        //else
        //{
        //    for(int i = 0; i < NUMBEROFCLOCKS; i++)
        //    {
        //        if(CANdata->data[0] == motorData[i].clockNumber) // find which clock to update
        //        {
        //           motorData[i].hour.atPosition = 0;
        //        }
        //    }
        //}
    }
        
}    