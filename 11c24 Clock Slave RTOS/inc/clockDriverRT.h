#ifndef _CLOCKDRIVERRT_H_
#define _CLOCKDRIVERRT_H_


// Structs
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

typedef struct
{
    uint8_t clockNumber;
    struct clockArmData min;
    struct clockArmData hour;
    
} motorStruct;

// Public functions
void clock0_func(void *p);
void clock1_func(void *p);
void clock2_func(void *p);
void clock3_func(void *p);
void clock_control(void *p);
void update_from_CAN(CCAN_MSG_OBJ_T *CANdata);

// Private functions

// Generates pulses to drive steppers
static void pulse_generation(const uint8_t motorNum, const char arm);
// Calculate how many steps to get to desired angle
static uint16_t calculate_steps(uint16_t newAngle , uint16_t angle);
// Drive clock to specific angle
static void drive_to_pos(const uint8_t clockNum, char arm, uint8_t *steps);
// Drive clock at certain speed continuously 
static void drive_continuous(const uint8_t clockNum, const uint8_t speed, const uint8_t dir);
// Home clock position
static void home_clocks(void);

#endif /* _CLOCKDRIVERRT_H_ */