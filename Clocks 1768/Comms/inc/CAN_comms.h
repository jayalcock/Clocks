#ifndef CAN_COMMS_H
#define CAN_COMMS_H

#include "Driver_CAN.h"
#include "LPC1768.h"


static void Error_Handler (void);
void CAN_SignalUnitEvent (uint32_t event);
void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event);

void CAN_init(void);


#endif /*CAN_COMMS_H*/