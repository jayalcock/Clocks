/* 

  clockDriverRT.h - functions for driving stepper motors


*/

#ifndef _CLOCKDRIVERRT_H_
#define _CLOCKDRIVERRT_H_


// Structs

// Data for each specific clock arm 
struct clockArmData
{
    uint8_t port;
    uint8_t pin;
    uint8_t dirPort;
    uint8_t dirPin;
    uint16_t angle;
    uint16_t angleDesired;
    uint8_t dir;
    uint8_t start;
    uint16_t remainingSteps;
    uint8_t atPosition;    
    uint8_t speed;
    uint8_t accel;
    uint8_t hallPort;
    uint8_t hallPin;
};

// Data for each specific clock
typedef struct
{
    uint8_t clockNumber;
    struct clockArmData min;
    struct clockArmData hour;
    
} motorStruct;


// Clock n control threads
void clock0_func(void *p);
void clock1_func(void *p);
void clock2_func(void *p);
void clock3_func(void *p);

// Master clock control thread
void clock_control(void *p);

// Update motor parameters from received can bus data
void update_from_CAN(CCAN_MSG_OBJ_T *CANdata);


#endif /* _CLOCKDRIVERRT_H_ */