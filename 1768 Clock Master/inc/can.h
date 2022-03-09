/*
 * can.h
 *
 *  Created on: Feb. 26, 2022
 *      Author: jayalcock
 */

#ifndef CAN_H_
#define CAN_H_

static void PrintCANErrorInfo(uint32_t Status);
/* Print CAN Message */
static void PrintCANMsg(CAN_MSG_T *pMsg);
/* Reply remote message received */
static void ReplyRemoteMessage(CAN_MSG_T *pRcvMsg);
/* Reply message received */
static void ReplyNormalMessage(CAN_MSG_T *pRcvMsg);
void CAN_IRQHandler(void);
void CAN_Thread(void *p);



#endif /* CAN_H_ */
