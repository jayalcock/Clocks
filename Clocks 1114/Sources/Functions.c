#include "Functions.h"

//1us delay function
//inputs: t - int - number of us to delay
void delay(int t)
{
  while(t>0)
 {
    asm ("nop");
    asm ("nop");
    asm ("nop");
    asm ("nop");
    t--;
  }
}

//LED flash function
void ledFlash(void)
{
    LPC_GPIO1->DATA |= LED; //Set pin high
    delay(3000000);
    LPC_GPIO1->DATA &= ~LED; //Set pin low
    delay(3000000);
}

void ledToggle(void)
{
  if(LPC_GPIO1->DATA & LED)
   LPC_GPIO1->DATA &= ~LED; //Set pin low
  else
   LPC_GPIO1->DATA |= LED; //Set pin high
}

void motorCwAngle(int deg, int spd)
{
  int pulses;

  //assuming 12 pulses/deg
  pulses = deg*PULSEPERDEG;

  
  deg = spd;

}
