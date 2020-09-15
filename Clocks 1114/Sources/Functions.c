#include "Functions.h"

//1us delay function
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

void motorCw(int deg, int spd)
{
  deg = spd;

}
