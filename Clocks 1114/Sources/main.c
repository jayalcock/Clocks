#include <__cross_studio_io.h>
#include "LPC11xx.h"

#define LED (1<<2) //LED on DIP pin 11/P1.2

void main(void)
{

  LPC_IOCON->PIO1_2 |= 0x01; //Configure P1.2 as GPIO
  
  LPC_GPIO1->DIR |= LED;
  LPC_GPIO1->DATA &= ~LED;
  
  while(1)
  {
    LPC_GPIO0->DATA |= LED;
    
  }

  debug_printf("hello world\n");
  debug_exit(0);
}
