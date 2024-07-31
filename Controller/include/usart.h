/*
 * usart.h
 *
 *  Created on: 2024. 7. 17.
 *      Author: homedory
 */

#ifndef USART_H_
#define USART_H_

#include "stm32f4xx_usart.h"

#define STX 0xFD

#define RECEIVE_STX 0x00
#define RECEIVE_MSG_ID 0x01
#define RECEIVE_DATALENGTH 0x02
#define RECEIVE_DATA 0x03

void USART2_Initialize(void);
void USART2_SendMessage(char messageId, char* data, int dataSize);
void USART2_IRQHandler(void);
void copyFunctionResponse(void);
void USART_SendByte(USART_TypeDef* USARTx, char byte);

#endif /* USART_H_ */
