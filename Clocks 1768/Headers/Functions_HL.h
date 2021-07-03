#ifndef _FUNCTIONS_HL_
#define _FUNCTIONS_HL_

#include <ctl.h>
#include "LPC1768.h"
#include "Comms.h"

void time_thread(void *p);

void getNTPtime(uint8_t *hour, uint8_t *min, uint8_t *sec);
char * calculateTime(uint8_t *dataBuffer, uint8_t *hour, uint8_t *min, uint8_t *sec);

void rtc_start();

#endif /* _FUCNTIONS_HL_ */