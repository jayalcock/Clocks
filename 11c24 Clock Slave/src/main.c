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
    driver_init();  

   
    
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