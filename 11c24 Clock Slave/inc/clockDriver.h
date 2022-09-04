/** @file clockDriver.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef _CLOCKDRIVER_H_
#define _CLOCKDRIVER_H_


struct clockArmData
{
    uint8_t port;
    uint8_t pin;
    uint8_t dirPort;
    uint8_t dirPin;
    uint16_t angle;
    uint16_t angleDesired;
    uint8_t dir;
    uint16_t remainingSteps;
    uint8_t atPosition;    
    uint8_t speed;
    uint8_t accel;
};

typedef struct
{
    uint8_t clockNumber;
    struct clockArmData min;
    struct clockArmData hour;
    
} motorStruct;

void driver_init(void);
void pulse_delay(const uint16_t time);

// Generates pulses to stepper driver
void pulse_generation(const uint8_t motorNum, const char arm);

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle, uint16_t angle);

void clock_control(void);


void update_from_CAN(CCAN_MSG_OBJ_T *data);



#endif /*_CLOCKDRIVER_*/