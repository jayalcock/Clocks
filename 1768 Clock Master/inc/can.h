/*
 * can.h
 *
 *  Created on: Feb. 26, 2022
 *      Author: jay alcock
 */

#ifndef _CAN_H_
#define _CAN_H_

/* Print CAN error */
static void PrintCANErrorInfo(uint32_t Status);

/* Print CAN Message */
static void PrintCANMsg(CAN_MSG_T *pMsg);

/* Reply remote message received */
static void ReplyRemoteMessage(CAN_MSG_T *pRcvMsg);

/* Reply message received */
static void ReplyNormalMessage(CAN_MSG_T *pRcvMsg);

/* CAN IRQ handler */
void CAN_IRQHandler(void);

/* Start CAN Tx public function */
void startCanTx();

/* CAN RT thread */ 
void CAN_Thread(void *msgQueuePtr);



#endif /* _CAN_H_ */
