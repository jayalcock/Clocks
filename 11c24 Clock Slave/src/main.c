#include "board.h"
#include "ccan_rom.h"
#include "clockDriver.h"



uint8_t tick, clock_task;


void SysTick_Handler(void)
{
    tick = 1;
}


int main(int argc, char *argv[])
{
    tick = 0;
    clock_task = 0; 
  
    SystemCoreClockUpdate();
    Board_Init();

    can_init();
    
    
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, (IOCON_FUNC1 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, (IOCON_FUNC1 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_2, (IOCON_FUNC1 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_6, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP)); 
    
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 0);  // Pulse A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 1);  // Pulse B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 2);  // Dir A
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 5);  // Dir B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 6);  // Reset
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 3);  // Pulse C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 6);  // Pulse D
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 7);  // Dir C
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 8);  // Dir D
    
    Chip_GPIO_SetPinOutLow(LPC_GPIO, 1, 6); // Set reset pin low 
   
    
    while(1)
    {
        
        if(tick == 1)
        {
            clock_control();
            //printf("%d\n", Chip_TIMER_ReadCount(LPC_TIMER16_0));
            //Chip_TIMER_Reset(LPC_TIMER16_0);
            tick = 0;
        }
        
        if(clock_task = 1)
        {
            //clock_control();
            
        }   
    };   
}