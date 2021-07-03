#include <__cross_studio_io.h>
#include "LPC11xx.h"
#include "Functions_LL.h"


BOOL tick = FALSE;

void SysTick_Handler(void)
{
  //tick = TRUE; 
  //ledToggle();
}


void main(void)
{
  int i = 0;
  int j = 1;
 

  LPC_IOCON->SWDIO_PIO1_3 = 0x0;
  LPC_IOCON->SWCLK_PIO0_10 = 0x0;

  //SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000));
  
  LPC_GPIO1->DIR |= LED;    //Set LED pin as output
  LPC_GPIO1->DATA &= ~LED;  //Set LED pin low

  if(SysTick_Config(8400000))   //Enable system tick timer 100ms
    LPC_GPIO1->DATA |= LED;

   if(SystemCoreClock == 84000000) //trying to find system clock value
   {
      for(i=0;i<10;i++)
        ledToggle();
    }

    
  //for(i=0;i<3;i++)
  //  ledFlash();

  while(1)
  {

    LPC_GPIO1->DATA |= LED;
    //if(tick)
    //{
    //  //LPC_GPIO1->DATA |= LED;
    //  ledToggle();
    //  //tick = FALSE;
    //}
     //if(SysTick_IRQn)
     //{
     //  //ledToggle();
     // LPC_GPIO1->DATA |= LED;
     //  delay(3000000);
     //}
     //else
     //{
     //  LPC_GPIO1->DATA &= ~LED;  //Set LED pin low
     //  delay(3000000);
     //}
      

  }

  debug_printf("hello world\n");  
  debug_exit(0);
}
