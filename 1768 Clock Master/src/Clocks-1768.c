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
#include <stdint.h>
#include "core_cm3.h"
#include "can.h"
#include "clockEngine.h"
#include "cmsis.h"

#include "uart_rb.h"
#define STACKSIZE 64
CTL_TASK_t main_task, can_task, test_task, clock_task, uart_task, uartRX_task;

unsigned can_task_stack[1+STACKSIZE+1];
unsigned test_task_stack[1+STACKSIZE+1];
//unsigned clock_task_stack[1+STACKSIZE+1];
unsigned clock_task_stack[200];
unsigned uart_task_stack[1+STACKSIZE+1];
unsigned uartRX_task_stack[1+STACKSIZE+1];

// CTL Error Handler
void ctl_handle_error(CTL_ERROR_CODE_t e)
{
    __asm("BKPT");
    while (1);
   
}

//Testing motor control
void test_thread(void *p)
{
        int desired_angle = 45;
        int current_angle = 0;
        int step_size = 12;
        int steps_remaining = 0;
        bool dir = false; 
        
         Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 2, true); //Motor driver dir
         Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 2, false); //Motor driver dir
        
        
        //Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 3, true); //Motor driver reset control - low to activate
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 3, false); //Motor driver reset control - low to activate
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 4, true); //Motor driver dir
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 5, true); //Motor driver pulse as output
        
        //shaft 2
        //Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 2);
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 2, false); //Motor driver dir
        Chip_GPIO_WriteDirBit(LPC_GPIO, 2, 1, true); //Motor driver pulse as output
        

	while (1)
        {
            
            // Generate steps to achieve desired ange
            if(steps_remaining == 0)
            {
                //dir = !dir;
                //Chip_GPIO_WritePortBit(LPC_GPIO, 2, 4, dir);
                //steps = 0;
                sendToCAN(1);
                ctl_timeout_wait(ctl_get_current_time() + 1000);
                steps_remaining = (desired_angle - current_angle) * step_size;
            }
            
            // small delay for correct pulse width
            for(int i = 0; i < 8000; i++)
            {
                __asm volatile ("nop");
            }
           
            // Pulse generation
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, true);
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 5, false);
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 1, true);
            Chip_GPIO_WritePortBit(LPC_GPIO, 2, 1, false);
            steps_remaining--;
         
            // At angle - sent to master over CAN
            //sendToCAN(1);
        
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
    //ctl_task_run(&can_task, 50, CAN_Thread, 0, "can_task", STACKSIZE, can_task_stack+1, 0);
    
    //TEST Thread
    memset(test_task_stack, 0xcd, sizeof(test_task_stack));  // write known values into the stack
    test_task_stack[0]=test_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    //ctl_task_run(&test_task, 45, test_thread, 0, "test_task", STACKSIZE, test_task_stack+1, 0);
    
    //Clock Thread
    memset(clock_task_stack, 0xcd, sizeof(clock_task_stack));  // write known values into the stack
    clock_task_stack[0]=clock_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&clock_task, 55, clock_thread, 0, "clock_task", STACKSIZE, clock_task_stack+1, 0);
    
    //TODO In development
    //UART Thread
    memset(uart_task_stack, 0xcd, sizeof(uart_task_stack));  // write known values into the stack
    uart_task_stack[0]=uart_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    ctl_task_run(&uart_task, 65, uart_thread, 0, "uart_task", STACKSIZE, uart_task_stack+1, 0);
    
    //UARTrx Thread
    //memset(uartRX_task_stack, 0xcd, sizeof(uartRX_task_stack));  // write known values into the stack
    //uartRX_task_stack[0]=uartRX_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
    //ctl_task_run(&uartRX_task, 64, uartRX_thread, 0, "uartRX_task", STACKSIZE, uartRX_task_stack+1, 0);
   
    
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
