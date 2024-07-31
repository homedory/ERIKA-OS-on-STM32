/*
 * can.h
 *
 *  Created on: 2024. 7. 9.
 *      Author: homedory
 */

#ifndef CAN_H_
#define CAN_H_

#include "stdint.h"
#include "stm32f4xx_can.h"
#include "rpc.h"

void CAN1_initialize(void);
void CAN1_SendMessage(uint32_t id, uint8_t *data, uint8_t dataLength);
void CAN1_ReceiveMessage(CanRxMsg* RXMsg);
void CAN1_RX0_IRQHandler(void);
void CAN_SendFunctionCall(FunctionCall *functionCall);

#endif /* CAN_H_ */
