#include "comms.h"
#include "ctl_api.h"

#define CAN_RX 0

CTL_EVENT_SET_t canEvent;


void comms_func(void *p)
{  
    unsigned int v=0;
    


    
    while (1)
    {      
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &canEvent, CAN_RX, CTL_TIMEOUT_NONE, 0);       
        v++;
    }  
}