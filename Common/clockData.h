/** @file clockData.h
 * 
 * @brief Data file for clock info
 *
 * @par       
 */ 

#ifndef _CLOCKDATA_H_
#define _CLOCKDATA_H_

#include "stdint.h"

// Clock hour representation of angular position
static const uint16_t timeAngle[13] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0};
    
    
// Matrix for storing clock specific angle data for invidiviual digits
typedef uint16_t digitData[3][6];
//typedef uint16_t digitData[][6];

// Default reset angles
//const uint16_t DEFAULT_ANGLE_MIN = 225;
//const uint16_t DEFAULT_ANGLE_HR = 45;

enum clock_constants
{
    CLOCK0,
    CLOCK1,
    CLOCK2,
    CLOCK3,    
    ALLCLOCKS,
    MINUTEARM, 
    HOURARM, 
    BOTHARMS,
    ROWS = 8,
    COLUMNS = 15,
    ARMS = 2,
    DEFAULT_ANGLE_MIN = 225,
    DEFAULT_ANGLE_HR = 45,
    UNUSED_ANGLE = 45,
};

// Clock functions
enum clock_functions
{
    HOME_CLOCKS,
    VEL_CONTROL,
    POS_CONTROL,
    
};   

                                          
const uint8_t get_clock_numbers(const uint8_t boardNo, const uint8_t clockNo);  
digitData * get_digit_data(const uint8_t digit);

#endif /*_CLOCKDATA_H_*/