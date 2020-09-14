#include <__cross_studio_io.h>
#include "LPC11xx.h"

#define LED (1<<8) //LED on DIP pin 1/P0.8

void delay(int t)
{
  while(t>0)
    t--;
}


void main(void)
{

  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);  /* enable clock for IOCON */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);   /* enable clock for GPIO  */

 //USE Internal Oscillator TO GENERATE INTERNAL SYSTEM CLOCK
  //LPC_SYSCON->SYSAHBCLKDIV             = 0x1;       //set clock divider for core to 1
  //LPC_SYSCON->MAINCLKSEL               &= ~(0x03);  //set "main clock" to IRC oscillator, if not system will lock up when PLL turns off!(sec. 3.5.11)
  //LPC_SYSCON->MAINCLKUEN               &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
  //LPC_SYSCON->MAINCLKUEN               |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
  //LPC_SYSCON->SYSPLLCLKSEL             = 0x00;      //connect system oscillator to SYSTEM PLL (sec. 3.5.9)
  //LPC_SYSCON->SYSPLLCLKUEN             &= ~(1);     //write a zero to SYSPLLUEN register (sec. 3.5.10), necessary for SYSPLLCLKSEL to update
  //LPC_SYSCON->SYSPLLCLKUEN             |= 1;        //write a one to SYSPLLUEN register (sec. 3.5.10), necessary for SYSPLLCLKSEL to update
  //LPC_SYSCON->PDRUNCFG                 |= (1<<7);   //power down the PLL before changing divider values (sec 3.5.35)
  //LPC_SYSCON->SYSPLLCTRL               = 0x23;      //set MSEL = 0x00011 and PSEL = 0x01 (sec 3.5.3 and table 46 of sec. 3.11.4.1)
  //LPC_SYSCON->PDRUNCFG                 &= ~(1<<7);  //power up PLL after divider values changed (sec. 3.5.35)
  //while((LPC_SYSCON->SYSPLLSTAT & 1) == 0);         //wait for PLL to lock
  //LPC_SYSCON->MAINCLKSEL               = 0x03;      //set system oscillator to the output of the PLL (sec. 3.5.11)
  //LPC_SYSCON->MAINCLKUEN               &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
  //LPC_SYSCON->MAINCLKUEN               |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12)

  //SystemInit();
  //SystemCoreClockUpdate();



  LPC_IOCON->PIO0_8 &= ~(0x10);; //Configure P0.8 as GPIO


  
  //LPC_GPIO0->MASKED_ACCESS[LED] = 0;

  LPC_GPIO0->DIR |= LED;
  LPC_GPIO0->DATA &= ~LED;
  

  while(1)
  {
    LPC_GPIO0->DATA |= LED;
    delay(100000);
    LPC_GPIO0->DATA &= ~LED;
    delay(100000);
  }

  debug_printf("hello world\n");
  debug_exit(0);
}
