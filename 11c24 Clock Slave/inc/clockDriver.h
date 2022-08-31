/** @file clockDriver.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef _CLOCKDRIVER_H_
#define _CLOCKDRIVER_H_

//typedef struct
//{
//    uint8_t number;
//    uint8_t port;
//    uint8_t pin;
//    uint16_t minAngle;
//    uint16_t hourAngle;
//    uint16_t minAngleDesired;
//    uint16_t hourAngleDesired;
//    uint16_t minRemainingSteps;
//    uint16_t hourRemainingSteps;
//    uint8_t minAtPosition;
//    uint8_t hourAtPosition;
    
//} motorStruct;


typedef struct
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
    
} motorStruct;

typedef struct
{
    uint8_t min;
    uint8_t hour;
    
} clockArmStruct;



void pulse_delay(const uint16_t time);
void pulse_generation(const uint8_t motorNum);
//void pulse_delay(void);

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle, uint16_t angle);

void position_control(void);


void update(void);



#endif /*_CLOCKDRIVER_*/