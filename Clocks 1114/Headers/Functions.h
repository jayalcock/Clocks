/** @file Functions.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef FUNCTION_H
#define FUNCTION_H

#include <LPC11xx.h>

#define LED (1<<9) //LED on DIP pin 18/P1.9
#define PULSEPERDEG 12 //12 pulses per degree rotated

void delay(int t);
void ledFlash(void);
void motorCw(int deg, int spd);


#endif /* FUNCTION_H */

/*** end of file ***/