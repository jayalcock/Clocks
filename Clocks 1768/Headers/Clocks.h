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

extern CTL_MUTEX_t mutex;

void clock_Engine();

#endif