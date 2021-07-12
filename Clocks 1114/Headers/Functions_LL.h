/** @file Functions.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef _FUNCTIONS_LL_H_
#define _FUNCTIONS_LL_H_

#include <LPC11xx.h>

typedef unsigned char U8;
typedef signed char N8;
typedef unsigned short U16;
typedef signed short N16;
typedef unsigned int U32;
typedef signed int N32;
typedef _Bool BOOL;

#define FALSE 0;
#define TRUE 1;

//#define LED (1<<9) //LED on DIP pin 18/P1.9
#define LED (1<<0) //LED on DIP pin 9/P1.0
#define PULSEPERDEG 12 //12 pulses per degree rotated

void delay(int t);
void ledFlash(void);
void ledToggle(void);
void motorCw(int deg, int spd);


#endif /* _FUNCTIONS_LL_H */

/*** end of file ***/