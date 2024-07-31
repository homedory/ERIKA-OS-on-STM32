/*
 * can.c
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */

#include <string.h>

#include "stdio.h"
#include "stm32f4xx_can.h"
#include "misc.h"
#include "can.h"
#include "response.h"

void CAN1_Initialize(void)
{
	/* initialize GPIO */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	/*
	GPIO_InitStructure.GPIO_Speed =
	*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Connect CAN pins to AF2 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);


	/* initialize CAN1 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	CAN_InitTypeDef CAN_InitStructure;

	CAN_StructInit(&CAN_InitStructure);

	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	/*
	CAN_InitStructure.CAN_BS1 =
	CAN_InitStructure.CAN_BS2 =
	CAN_InitStructure.CAN_Prescaler =
	*/
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_NART = DISABLE;

	CAN_Init(CAN1, &CAN_InitStructure);

	/* initialize CAN filter */
	CAN_FilterInitTypeDef filterInitStructure;

	filterInitStructure.CAN_FilterActivation = ENABLE;
	filterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	filterInitStructure.CAN_FilterIdHigh = 0x0000;
	filterInitStructure.CAN_FilterIdLow = 0x0000;
	filterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	filterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	filterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	filterInitStructure.CAN_FilterNumber = 0;
	filterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;

	CAN_FilterInit(&filterInitStructure);

	/* initialize NVIC */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void CAN1_SendMessage(uint32_t id, uint8_t *data, uint8_t dataLength)
{
	if (dataLength > 8)
		perror("Too Long Data Length");

	CanTxMsg TxMessage;
	TxMessage.DLC = dataLength;
	TxMessage.ExtId = 0x00;
	TxMessage.IDE = CAN_Id_Standard;
	TxMessage.RTR = CAN_RTR_Data;
	TxMessage.StdId = id;

	for (uint8_t i = 0; i < dataLength; i++) {
		TxMessage.Data[i] = data[i];
	}

	CAN_Transmit(CAN1, &TxMessage);
}

void CAN1_ReceiveMessage(CanRxMsg* RXMsg)
{
	switch(RXMsg->StdId) {
	case FUNC_MOVE_MOTOR_ID:
		move_motor_response.response_ready = 1;
		move_motor_response.response_data = (int)RXMsg->Data;
		break;
	case FUNC_DISPLAY_TEXT_ID:
		display_text_response.response_ready = 1;
		display_text_response.response_data = (int)RXMsg->Data;
		break;
	case FUNC_TERMINATE_RPC_ID:
		terminate_rpc_response.response_ready = 1;
		terminate_rpc_response.response_data = (int)RXMsg->Data;
		break;
	}
}

void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET) {
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);

		/* ISR */
		CAN1_ReceiveMessage(&RxMessage);
	}
}

void CAN_SendFunctionCall(FunctionCall *functionCall)
{
	uint8_t data[8];
	uint8_t dataLength;
	uint32_t id = functionCall->functionId;

	/* send function id */
	memcpy(data, &(functionCall->functionId), sizeof(int));
	dataLength = sizeof(int);
	CAN1_SendMessage(id++, data, dataLength);

	/* send num_params */
	memcpy(data, &(functionCall->numData), sizeof(int));
	dataLength = sizeof(int);
	CAN1_SendMessage(id++, data, dataLength);

	/* send data */
	for (uint8_t i = 0; i < functionCall->numData; i++) {
		memcpy(data, &(functionCall->data[i]), sizeof(Data));
		dataLength = sizeof(Data);
		CAN1_SendMessage(id++, data, dataLength);
	}
}

