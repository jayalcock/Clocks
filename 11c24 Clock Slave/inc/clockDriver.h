/** @file clockDriver.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef _CLOCKDRIVER_H_
#define _CLOCKDRIVER_H_

typedef struct
{
    uint8_t number;
    uint8_t port;
    uint8_t pin;
    uint16_t minAngle;
    uint16_t hourAngle;
    uint16_t minAngleDesired;
    uint16_t hourAngleDesired;
    uint8_t minAtPosition;
    uint8_t hourAtPosition;
    
} motorStruct;

typedef struct
{
    uint8_t min;
    uint8_t hour;
    
} clockArmStruct;



void position_control(void);
void pulse_generation(const motorStruct motorNum);

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t angle, uint16_t newAngle);

void update(void);



#endif /*_CLOCKDRIVER_*/