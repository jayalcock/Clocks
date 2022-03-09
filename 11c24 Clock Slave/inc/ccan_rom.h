/** @file ccan_rom.h
 * 
 * @brief Functions for clock control
 *
 * @par       
 */ 

#ifndef _CCAN_ROM_H_
#define _CCAN_ROM_H_

void baudrateCalculate(uint32_t baud_rate, uint32_t *can_api_timing_cfg);
void CAN_rx(uint8_t msg_obj_num);
void CAN_tx(uint8_t msg_obj_num);
void CAN_error(uint32_t error_info);
void C_CAN_IRQHandler(void);
int can_init(void);




#endif /*_CCAN_ROM_H_*/