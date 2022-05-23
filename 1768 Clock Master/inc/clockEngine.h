#ifndef _CLOCKENGINE_H_
#define _CLOCKENGINE_H_


//#define RTC_IRQn 17

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
void getNTPtime();//uint8_t *hour, uint8_t *min, uint8_t *sec);
void calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char* timeString);

// Initialises RTC
void rtcInit(void);

// Real time clock interrupt handler
void RTC_IRQHandler(void);

void clock_thread(void *p);

#endif /* _CLOCKENGINE_H_ */