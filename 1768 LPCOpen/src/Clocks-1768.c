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


//#include <cr_section_macros.h>

// TODO: insert other include files here
#include <ctl_api.h>
#include <string.h>
#include "can.h"


//#include "uart_rb.h"
// TODO: insert other definitions and declarations here
//int __sys_write(int iFileHandle, char *pcBuffer, int iLength);
#define STACKSIZE 64
CTL_TASK_t main_task, can_task;

unsigned can_task_stack[1+STACKSIZE+1];


void ctl_handle_error(CTL_ERROR_CODE_t e)
{
    __asm("BKPT");
    while (1);
   
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
    // TODO: insert code here
    
    SysTick_Config(SystemCoreClock / 1000);
    
    ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
    ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
    
    //Comms Thread
    memset(can_task_stack, 0xcd, sizeof(can_task_stack));  // write known values into the stack
    can_task_stack[0]=can_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&can_task, 50, CAN_Thread, 0, "can_task", STACKSIZE, can_task_stack+1, 0);
    
    //UART_Thread();
    
    //uint8_t temp[] = "Test";
    //Chip_UART_SendByte(LPC_UART0, temp);
    //CAN_Thread();

    ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
        // "Dummy" NOP to allow source level single
        // stepping of tight while() loop
        __asm volatile ("nop");
    }
    return 0 ;
}
