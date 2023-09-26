/* 

  clockDriverRT.h - functions for driving stepper motors


*/

#ifndef _CLOCKDRIVERRT_H_
#define _CLOCKDRIVERRT_H_


// Structs

// Data for each specific clock arm 
typedef struct 
{
    uint8_t port;
    uint8_t pin;
    uint8_t dirPort;
    uint8_t dirPin;
    uint8_t hallPort;
    uint8_t hallPin;
    int16_t angle;
    int16_t angleDesired;
    uint8_t dir;
    uint8_t start;
    uint32_t steps;
    uint32_t remainingSteps;
    uint8_t atPosition;    
    uint8_t speed;
    uint8_t accel;
    uint8_t controlMode;

} clockArmData;

// Data for each specific clock
typedef struct
{
    uint8_t clockNumber;
    clockArmData hour;
    clockArmData min;
    
} motorStruct;

// Update motor parameters from received can bus data
void update_from_CAN(CCAN_MSG_OBJ_T *canData);
//void update_from_CAN(void *canData);

// Clock n control threads
void clock_func(void *p);
//void clock0_func(void *p);
//void clock1_func(void *p);
//void clock2_func(void *p);
//void clock3_func(void *p);

// Master clock control thread
void clock_control(void *p);




#endif /* _CLOCKDRIVERRT_H_ */