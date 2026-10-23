#ifndef _CLOCKENGINE_H_
#define _CLOCKENGINE_H_
    


//void writeClockValue(const uint8_t pos, const digitData *val);
//int setHome(void);
//void getTime(void);
//void setTime(uint8_t hour, uint8_t min, uint8_t sec);
//void getNTPtime();//uint8_t *hour, uint8_t *min, uint8_t *sec);
//void calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec, char* timeString);

//// Initialises RTC
//void rtcInit(void);

//// Real time clock interrupt handler
//void RTC_IRQHandler(void);

//// Update remote clock arm positions 
//void update_position(const uint8_t clockNum, const uint16_t minuteAngle, const uint16_t hourAngle, CTL_MESSAGE_QUEUE_t *msgQueuePtr);

//// Update clock arm speeds and directions
//void update_speed_dir(const uint8_t clockNum, const uint8_t minuteSpeed, const uint8_t hourSpeed, const uint8_t minDir, const uint8_t hourDir, CTL_MESSAGE_QUEUE_t *msgQueuePtr);

//// Start clock arm movemet
//void start_movement(const uint8_t clockNum, CTL_MESSAGE_QUEUE_t *msgQueuePtr);

// RT thread for calculating clock positions
void clock_main_thread(void *msgQueuePtr);

#endif /* _CLOCKENGINE_H_ */