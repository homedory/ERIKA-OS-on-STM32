/*
 * usart.c
 *
 *  Created on: 2024. 7. 17.
 *      Author: homedory
 */

#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "usart.h"
#include "rpc.h"
#include "response.h"
#include "swv.h"

// for debugging
#include <string.h>

int rxState;
unsigned char dataLen;
int bufferIndex;
char receivedMessageId;
char USART_buffer[256];
FunctionReturn functionReturn;

// for debugging
char buff[50];

void USART_SendByte(USART_TypeDef* USARTx, char byte) {
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, byte);
}

void USART2_SendMessage(char messageId, char* data, int dataSize)
{
	if (dataSize > 255)
		perror("Data too large");

	USART_SendByte(USART2, STX);

	//for debugging
	sprintf(buff, "%02X\n", STX);
	swv_write(buff, strlen(buff));

	USART_SendByte(USART2, messageId);

	//for debugging
	sprintf(buff, "%02X\n", messageId);
	swv_write(buff, strlen(buff));

	unsigned char dataLength = (unsigned char) dataSize;

	USART_SendByte(USART2, dataLength);

	//for debugging
	sprintf(buff, "dataLen: %02X\n", dataLength);
	swv_write(buff, strlen(buff));

	while(dataLength--) {
		sprintf(buff, "%02X\n", *data);
		swv_write(buff, strlen(buff));
		USART_SendByte(USART2, *data);
		data++;
	}
}

void USART2_Initialize(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* USART2_TX pin (PA.2) configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2_RX pin (PA.3) configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PA2, PA3 pins to AF7 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);

	/* Interrupt Config */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART2 */
	USART_Cmd(USART2, ENABLE);

	/* Enable the USART2 Interrupt Request */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}



void USART2_IRQHandler(void)
{
	/* receive data */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
		char receivedByte = USART_ReceiveData(USART2);

		//for debugging
		sprintf(buff, "Byte: %02X state:%d len:%02X idx:%02X\n", receivedByte, rxState, dataLen, bufferIndex);
		swv_write(buff, strlen(buff));

		switch(rxState) {
		case RECEIVE_STX:
			if (receivedByte == STX)
				rxState = RECEIVE_MSG_ID;
			break;
		case RECEIVE_MSG_ID:
			receivedMessageId = receivedByte;
			rxState = RECEIVE_DATALENGTH;
			break;
		case RECEIVE_DATALENGTH:
			dataLen = receivedByte;
			rxState = RECEIVE_DATA;
			break;
		case RECEIVE_DATA:
			if (bufferIndex < dataLen) {
				USART_buffer[bufferIndex++] = receivedByte;
			}
			if (bufferIndex == dataLen) {
				//for debugging
				sprintf(buff, "receive complete\n");
				swv_write(buff, strlen(buff));
				if (receivedMessageId == FUNCTION_RESPONSE_MSG)
					copyFunctionResponse();
				bufferIndex = 0;
				rxState = RECEIVE_STX;
			}
			break;
		}

		 USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

void copyFunctionResponse(void)
{
	memcpy(&functionReturn, &USART_buffer, sizeof(FunctionReturn));

	//for debugging
	sprintf(buff, "functionid: %d value: %d\n", functionReturn.functionId, functionReturn.returnValue);
	swv_write(buff, strlen(buff));

	switch(functionReturn.functionId) {
	case FUNC_MOVE_MOTOR_ID:
		move_motor_response.response_data = functionReturn.returnValue;
		move_motor_response.response_ready = 1;
		break;
	case FUNC_DISPLAY_TEXT_ID:
		display_text_response.response_data = functionReturn.returnValue;
		display_text_response.response_ready = 1;
		break;
	case FUNC_TERMINATE_RPC_ID:
		terminate_rpc_response.response_data = functionReturn.returnValue;
		terminate_rpc_response.response_ready = 1;
		break;
	}
}
