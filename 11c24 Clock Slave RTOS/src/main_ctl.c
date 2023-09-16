#include <string.h>
#include <ctl_api.h>
#include "board.h"
#include "ccan_rom.h"
#include "clockDriverRT.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define STACKSIZE 128      

CTL_TASK_t main_task, comms_task, clock_control_task, clock_task;

// Stack memory allocation
unsigned clock_stack[1+STACKSIZE+1], comms_stack[1+STACKSIZE+1], clock_control_stack[1+STACKSIZE+1];
    

/*****************************************************************************
 * Public functions
 ****************************************************************************/
// Error handler
void ctl_handle_error(CTL_ERROR_CODE_t e)
{
    asm("BKPT");
    while (1);
}

int main(void)
{
  
    unsigned int v=0;
    
    ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
    
    Chip_GPIO_Init(LPC_GPIO); // TODO is this needed?
    
    // Main clock control thread initialization
    memset(clock_control_stack, 0xcd, sizeof(clock_control_stack));  // write known values into the stack
    clock_control_stack[0]=clock_control_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock_control_task, 70, clock_control, 0, "clock_control_task", STACKSIZE, clock_control_stack+1, 0);
    
    // Clock control thread initialization
    memset(clock_stack, 0xcd, sizeof(clock_stack));  // write known values into the stack
    clock_stack[0]=clock_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock_task, 90, clock_func, 0, "clock_task", STACKSIZE, clock_stack+1, 0);    
    
    // Communications thread initialization
    memset(comms_stack, 0xcd, sizeof(comms_stack));  // write known values into the stack
    comms_stack[0]=comms_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&comms_task, 80, comms_func, 0, "comms_task", STACKSIZE, comms_stack+1, 0);
  
    // Set test LED high
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO3_0, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 0);
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 3, 0);
    
    /* Reset 32bit timer 1 -
        Timer has an issue that sets the reset bit on startup 
        and has to be reset once to function correctly */
    //int test = 0x40018004;
    //int *TESTptr = &test;
    //*TESTptr = 0;
    
    int *TESTptr = 0x40018004;
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

