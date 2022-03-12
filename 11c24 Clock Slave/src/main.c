#include "board.h"
#include "ccan_rom.h"


int main(int argc, char *argv[])
{
    SystemCoreClockUpdate();
    Board_Init();

    can_init();
    
    
    
    while(1){};   
}