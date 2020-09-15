#include <__cross_studio_io.h>
#include "LPC11xx.h"

#define LED (1<<9) //LED on DIP pin 18/P1.9

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


void main(void)
{
  SystemInit();

  //LPC_IOCON->PIO0_8 = 0x10; //Configure P0.8 as GPIO

  LPC_GPIO1->DIR |= LED;
  LPC_GPIO1->DATA &= ~LED;
  

  while(1)
  {
    LPC_GPIO1->DATA |= LED;
    delay(3000000);
    LPC_GPIO1->DATA &= ~LED;
    delay(3000000);
  }

  debug_printf("hello world\n");
  debug_exit(0);
}
