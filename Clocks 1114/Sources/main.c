#include <__cross_studio_io.h>
#include "LPC11xx.h"
#include "Functions.h"


void main(void)
{
  
  LPC_GPIO1->DIR |= LED;  //Set LED pin as output
  LPC_GPIO1->DATA &= ~LED;  //Set LED pin low

  while(1)
  {
    ledFlash();
  }

  debug_printf("hello world\n");
  debug_exit(0);
}
