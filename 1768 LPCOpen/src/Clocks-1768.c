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
#include "can.h"
//#include "uart_rb.h"
// TODO: insert other definitions and declarations here
int __sys_write(int iFileHandle, char *pcBuffer, int iLength);

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
    //UART_Thread();
    
    //uint8_t temp[] = "Test";
    //Chip_UART_SendByte(LPC_UART0, temp);
    CAN_Thread();

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
