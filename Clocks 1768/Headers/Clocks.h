#ifndef CLOCKS_H
#define CLOCKS_H

#include <string.h>
#include <ctl_api.h>
#include <stdio.h>

#include "LPC17xx.h"
#include "Clocks.h"
#include "Comms.h"
#include "Time.h"


#define STACKSIZE 64        
#define COMMS_STACKSIZE 128

void clock_Engine();

#endif