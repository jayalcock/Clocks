#include <string.h>
#include <ctl_api.h>
#include "board.h"
#include "clockDriverRT.h"
#include "comms.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define STACKSIZE 64      

CTL_TASK_t main_task, clock0_task, clock1_task, clock2_task, clock3_task, comms_task, clock_control_task;

// Stack memory allocation
unsigned clock0_stack[1+STACKSIZE+1], clock1_stack[1+STACKSIZE+1], clock2_stack[1+STACKSIZE+1], 
    clock3_stack[1+STACKSIZE+1], comms_stack[1+STACKSIZE+1], clock_control_stack[1+STACKSIZE+1];


/*****************************************************************************
 * Public functions
 ****************************************************************************/
// Error handler
void ctl_handle_error(CTL_ERROR_CODE_t e)
{
    while (1);
}

int main(void)
{
  
    unsigned int v=0;
    
    ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
    
    Chip_GPIO_Init(LPC_GPIO); // TODO is this needed?
            
    // Clock0 control thread initialization
    memset(clock0_stack, 0xcd, sizeof(clock0_stack));  // write known values into the stack
    clock0_stack[0]=clock0_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock0_task, 50, clock0_func, 0, "clock0_task", STACKSIZE, clock0_stack+1, 0);
    
    // Clock1 control thread initialization
    memset(clock1_stack, 0xcd, sizeof(clock1_stack));  // write known values into the stack
    clock1_stack[0]=clock1_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock1_task, 50, clock1_func, 0, "clock1_task", STACKSIZE, clock1_stack+1, 0);
 
    // Clock2 control thread initialization
    memset(clock2_stack, 0xcd, sizeof(clock2_stack));  // write known values into the stack
    clock2_stack[0]=clock2_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock2_task, 50, clock2_func, 0, "clock2_task", STACKSIZE, clock2_stack+1, 0);

    // Clock3 control thread initialization
    memset(clock3_stack, 0xcd, sizeof(clock3_stack));  // write known values into the stack
    clock3_stack[0]=clock3_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock3_task, 50, clock3_func, 0, "clock3_task", STACKSIZE, clock3_stack+1, 0);
    
    // Communications thread initialization
    memset(comms_stack, 0xcd, sizeof(comms_stack));  // write known values into the stack
    comms_stack[0]=comms_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&comms_task, 80, comms_func, 0, "comms_task", STACKSIZE, comms_stack+1, 0);
    
    // Main clock control thread initialization
    memset(clock_control_stack, 0xcd, sizeof(clock_control_stack));  // write known values into the stack
    clock_control_stack[0]=clock_control_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock_control_task, 60, clock_control, 0, "clock_control_task", STACKSIZE, clock_control_stack+1, 0);

    // Set test LED high
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO3_0, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 0);
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 3, 0);
    
    
    /* Reset 32bit timer 1 -
        Timer has an issue that sets the reset bit on startup 
        and has to be reset once to function correctly */
    int test = 0x40018004;
    int *TESTptr = &test;
    *TESTptr = 0;
    
    
    ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.  

      
    while (1)
    {    
        //if(Chip_GPIO_GetPinState(LPC_GPIO, 1, 4) == false)
        ////if(Chip_GPIO_ReadValue(LPC_GPIO, 1, 4) == true)
        //{
            
        //    cnt++;
        //    while(Chip_GPIO_GetPinState(LPC_GPIO, 1, 4) == false)
        //    {};
        //}
        
        // power down can go here if supported      
         v++;
    }
    
    return 0;
}

