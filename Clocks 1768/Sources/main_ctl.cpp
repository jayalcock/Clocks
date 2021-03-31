#include <string.h>
#include <ctl_api.h>
#include "LPC17xx.h"
#include "../Headers/Comms.h"
#include <stdio.h>

//#include <vector>

//WIFI Constants 
const char* SSID = "netgear47";
const char* PASSWORD = "phobicjungle712";


CTL_TASK_t main_task, new_task;

#define CALLSTACKSIZE 16 // this is only required for AVR builds
#define STACKSIZE 64          
unsigned new_task_stack[1+STACKSIZE+1];

#define PIN00 (1<<0)

void new_task_code(void *p)
{  
  unsigned int v=0;

  LPC_PINCON->PINSEL0 = 0x000000;

  LPC_GPIO0->FIODIR |= PIN00; //Set pin 0.0 as output

  while (1)
    {      
      // task logic goes here      
      LPC_GPIO0->FIOSET |= PIN00; //Set pin 0.0 high
      ctl_timeout_wait(ctl_get_current_time()+100); //Wait 100ms
      LPC_GPIO0->FIOCLR |= PIN00; //Set pin 0.0 low
      ctl_timeout_wait(ctl_get_current_time()+100); //Wait 100ms
      v++;
    }  
}

void ctl_handle_error(CTL_ERROR_CODE_t e)
{
  while (1);
}

int main(void)
{
  unsigned int v=0;
  unsigned int w=0;


  UART wifi(SSID, PASSWORD); 
  wifi.printData();
  


  ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
  ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
  memset(new_task_stack, 0xcd, sizeof(new_task_stack));  // write known values into the stack
  new_task_stack[0]=new_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
  ctl_task_run(&new_task, 1, new_task_code, 0, "new_task", STACKSIZE, new_task_stack+1, CALLSTACKSIZE);
  ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
  while (1)
    {    
      // power down can go here if supported      
      v++;
    }

   

  return 0;
}
