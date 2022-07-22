#ifndef _CLOCKENGINE_H_
#define _CLOCKENGINE_H_
    
// Matrix for storing clock specific angle data for invidiviual digits
typedef uint16_t digitData[3][6];

// Matrix for storing overall clock position data
typedef uint16_t clockData[15][8];    

void writeClockValue(const uint8_t pos, const digitData *val);
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