#include "Clocks.h"

CTL_TASK_t main_task, comms_task, rx_task, time_task;

unsigned comms_task_stack[1+COMMS_STACKSIZE+1];
unsigned rx_task_stack[1+STACKSIZE+1];
unsigned time_task_stack[1+COMMS_STACKSIZE+1];


void clock_Engine()
{
    CTL_MUTEX_t mutex;
    
    SysTick_Config(SystemCoreClock / 1000);
    
    ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 


    //Comms Thread
    memset(comms_task_stack, 0xcd, sizeof(comms_task_stack));  // write known values into the stack
    comms_task_stack[0]=comms_task_stack[1+COMMS_STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&comms_task, 50, commsThread, &mutex, "comms_task", COMMS_STACKSIZE, comms_task_stack+1, 0);
    
    //Start receive thread with higher priority than comms
    memset(rx_task_stack, 0xcd, sizeof(rx_task_stack));  // write known values into the stack
    rx_task_stack[0]=rx_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&rx_task, 51, rx_thread, 0, "rx_task", STACKSIZE, rx_task_stack+1, 0);

    //Time thread
    memset(time_task_stack, 0xcd, sizeof(time_task_stack));  // write known values into the stack
    time_task_stack[0]=time_task_stack[1+COMMS_STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&time_task, 40, time_thread, 0, "time_task", COMMS_STACKSIZE, time_task_stack+1, 0); 

    ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
 
    for(;;)
    {
        //infinte loop
    }
 
}

