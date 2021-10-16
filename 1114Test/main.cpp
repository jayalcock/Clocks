//
// main.cpp : Defines entry point for an LPC1100 C/C++ application.
//

#include <mcu.h>

#define LED (1<<10) //LED on DIP pin 1/P0.8

int main(int argc, char *argv[])
{
    
    LPC_SYSCON->CLKOUTCLKSEL = 0x1;
    LPC_SYSCON->CLKOUTUEN = 0x1;
    LPC_IOCON->PIO0_1 = 0x1; //Set to clock out

    LPC_GPIO1->DIR |= LED;
    LPC_GPIO1->DATA &= ~LED;


      
      
    for(;;)
    {    
          
        for(int i=0; i<1000000; i++)
        {
        // delay
        }
    
        LPC_GPIO1->DATA |= LED;

        for(int i=0; i<1000000; i++)
        {
        // delay 
        }
        
        LPC_GPIO1->DATA &= ~LED;
    }
      
  return 0;
}
