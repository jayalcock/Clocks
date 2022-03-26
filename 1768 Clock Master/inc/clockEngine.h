#ifndef CLOCKENGINE_H
#define CLOCKENGINE_H

#include <stdint.h>
#include <ctl_api.h>
#include "stdio.h"
#include "LPC1768.h"

//typedef enum {
//	RTC_SECOND,		/*!< Second */
//	RTC_MINUTE,		/*!< Month */
//	RTC_HOUR,		/*!< Hour */
//	RTC_DAYOFMONTH,	/*!< Day of month */
//	RTC_DAYOFWEEK,		/*!< Day of week */
//	RTC_DAYOFYEAR,		/*!< Day of year */
//	RTC_MONTH,		/*!< Month */
//	RTC_YEAR,		/*!< Year */
//	RTC_LAST
//} RTC_TIMEINDEX_T;

////time struct   
//typedef struct {
//    uint32_t time[RTC_LAST];
//} RTC_TIME_T;
    
    
    
int setHome(void);
void getTime(void);
void setTime(uint8_t hour, uint8_t min, uint8_t sec);

// Initialises RTC
void rtcInit(void);


void clock_thread(void *p);

#endif /* CLOCKENGINE_H */