#ifndef _CLOCKDRIVERRT_H_
#define _CLOCKDRIVERRT_H_

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
};

typedef struct
{
    uint8_t clockNumber;
    struct clockArmData min;
    struct clockArmData hour;
    
} motorStruct;

void pulse_generation(const uint8_t motorNum, const char arm);
void clock0_func(void *p);
void clock1_func(void *p);
void clock2_func(void *p);
void clock3_func(void *p);

// Calculate how many steps to get to desired angle
uint16_t calculate_steps(uint16_t newAngle , uint16_t angle);

void clock_control(void *p);

void update_from_CAN(CCAN_MSG_OBJ_T *CANdata);

#endif /* _CLOCKDRIVERRT_H_ */