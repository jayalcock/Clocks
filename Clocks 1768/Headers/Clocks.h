#ifndef CLOCKS_H
#define CLOCKS_H

#include <string.h>
#include <ctl_api.h>
#include <stdio.h>

#include "LPC17xx.h"
#include "Comms.h"
#include "Functions_HL.h"
#include "Clocks.h"

#define STACKSIZE 64        
#define COMMS_STACKSIZE 128

//unsigned time_task_stack[1+TIME_STACKSIZE+1];

extern CTL_MUTEX_t mutex;
//extern CTL_EVENT_SET_t start_seq_event;

void clock_Engine();

#endif