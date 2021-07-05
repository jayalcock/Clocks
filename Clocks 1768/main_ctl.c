#include <ctl_api.h>

#include "Clocks.h"


void ctl_handle_error(CTL_ERROR_CODE_t e)
{
  while (1);
}

int main(void)
{
    
    clock_Engine();

    return 0;
}
