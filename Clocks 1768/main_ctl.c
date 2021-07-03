//#include <string.h>
#include <ctl_api.h>
//#include <stdio.h>

//#include "LPC17xx.h"
//#include "Comms.h"
//#include "Functions_HL.h"
#include "Clocks.h"

//#define STACKSIZE 64        
//#define COMMS_STACKSIZE 128




//CTL_TASK_t main_task, comms_task, rx_task;
//CTL_TASK_t time_task;
//extern CTL_EVENT_SET_t start_seq_event;

//unsigned comms_task_stack[1+COMMS_STACKSIZE+1];
//unsigned rx_task_stack[1+STACKSIZE+1];
//unsigned time_task_stack[1+COMMS_STACKSIZE+1];


void ctl_handle_error(CTL_ERROR_CODE_t e)
{
  while (1);
}

int main(void)
{

    //CTL_EVENT_SET_t start_seq_event;
    
    unsigned int v=0;
    
    clock_Engine();

    //SysTick_Config(SystemCoreClock / 1000);
    
    //ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    //ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 

    //ctl_events_init(&start_seq_event, 0);

    ////Comms Thread
    //memset(comms_task_stack, 0xcd, sizeof(comms_task_stack));  // write known values into the stack
    //comms_task_stack[0]=comms_task_stack[1+COMMS_STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    //ctl_task_run(&comms_task, 50, commsThread, 0, "comms_task", COMMS_STACKSIZE, comms_task_stack+1, 0);
    
    ////Start receiv thread with higher priority than comms
    //memset(rx_task_stack, 0xcd, sizeof(rx_task_stack));  // write known values into the stack
    //rx_task_stack[0]=rx_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    //ctl_task_run(&rx_task, 51, rx_thread, 0, "rx_task", STACKSIZE, rx_task_stack+1, 0);

    ////Time thread
    //memset(time_task_stack, 0xcd, sizeof(time_task_stack));  // write known values into the stack
    //time_task_stack[0]=time_task_stack[1+COMMS_STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ////ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &start_seq_event, 1<<0, CTL_TIMEOUT_NONE, 0); //Wait for comms thread to start
    //ctl_task_run(&time_task, 40, time_thread, 0, "time_task", COMMS_STACKSIZE, time_task_stack+1, 0); 

    
    //ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.


    while (1)
    {    
      // power down can go here if supported      
      v++;

    }
   

    return 0;
}
