#include <ctl_api.h>

#include "Clocks.h"


void ctl_handle_error(CTL_ERROR_CODE_t e)
{
  while (1);
}

int main(void)
{
    
    unsigned int v=0;
    
    clock_Engine();

    return 0;
}