/*
===============================================================================
 Name        : Clocks-1768.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif


#include <ctl_api.h>
#include <string.h>
#include "can.h"


//#include "uart_rb.h"
#define STACKSIZE 64
CTL_TASK_t main_task, can_task, test_task;

unsigned can_task_stack[1+STACKSIZE+1];
unsigned test_task_stack[1+STACKSIZE+1];

// CTL Error Handler
void ctl_handle_error(CTL_ERROR_CODE_t e)
{
    __asm("BKPT");
    while (1);
   
}

//Testing motor control
void test_thread(void *p)
{
        int desired_angle = 90; 
        int ang = desired_angle * 12;
        int steps = 0;
        bool dir = false; 
        
        
        //Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 3, true); //Motor driver reset control - low to activate
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 3, false); //Motor driver reset control - low to activate
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 4, true); //Motor driver dir
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 5, true); //Motor driver pulse as output
        

	while (1)
        {
            if(steps == ang)
            {
                //dir = !dir;
                //Chip_GPIO_WritePortBit(LPC_GPIO, 2, 4, dir);
                //steps = 0;
                ctl_timeout_wait(ctl_get_current_time() + 1000);
                steps = 0;
            }
                
            for(int i = 0; i < 8000; i++)
            {
                __asm volatile ("nop");
            }
           
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, true);
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, false);
            steps++;
            sendToCAN(ang);
        
        }
}

int main(void) { 
  
#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    //Board_LED_Set(0, true);
#endif
#endif   
  
    SysTick_Config(SystemCoreClock / 1000);
    
    ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
    
    //CAN Thread
    memset(can_task_stack, 0xcd, sizeof(can_task_stack));  // write known values into the stack
    can_task_stack[0]=can_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&can_task, 50, CAN_Thread, 0, "can_task", STACKSIZE, can_task_stack+1, 0);
    
    //TEST Thread
    memset(test_task_stack, 0xcd, sizeof(test_task_stack));  // write known values into the stack
    test_task_stack[0]=test_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&test_task, 45, test_thread, 0, "test_task", STACKSIZE, test_task_stack+1, 0);
    
    //TODO - Develop UART thread and comms
    //UART_Thread();
    
    bool state = false;
    
    

    ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
    // Force the counter to be placed into memory
    volatile static int i = 0, j = 0;
    // Enter an infinite loop, just incrementing a counter
    while(1) 
    {
        i++ ;
        // "Dummy" NOP to allow source level single
        // stepping of tight while() loop
        __asm volatile ("nop");  
        
    }
    return 0 ;
}
