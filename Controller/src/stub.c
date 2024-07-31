/*
 * stub.c
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */

#include <stdio.h>

#include "rpc.h"
#include "response.h"
#include "stdint.h"
#include "string.h"
#include "usart.h"
#include "swv.h"


int moveMotor(int inputValue)
{
	FunctionCall functionCall;
	functionCall.functionId = FUNC_MOVE_MOTOR_ID;
	functionCall.numData = 1;
	functionCall.data[0].type = TYPE_INT;
	functionCall.data[0].value.intValue = inputValue;

	/* send the function call message */
	USART2_SendMessage(FUNCTION_CALL_MSG, (char*)&functionCall, sizeof(FunctionCall));

	/* wait for a response */
	while(move_motor_response.response_ready == 0);
	move_motor_response.response_ready = 0;

	return move_motor_response.response_data;
}

int displayText(int lineNum, char *text)
{
	/* check text length */
	int textLength = 0;
	int index = 0;
	while(text[index] != '\0') {
		textLength++;
		index++;
	}
	if (textLength >= MAX_DATA_NUM - 1)
		perror("Too Long Text");

	FunctionCall functionCall;
	functionCall.functionId = FUNC_DISPLAY_TEXT_ID;
	functionCall.numData = 2;
	functionCall.data[0].type = TYPE_INT;
	functionCall.data[0].value.intValue = lineNum;

	/* copy Text */
	index = 0;
	while(text[index] != '\0') {
		functionCall.data[index + 1].type = TYPE_CHAR;
		functionCall.data[index + 1].value.charValue = text[index];
		index++;
	}
	/* add null char */
	functionCall.data[index + 1].value.charValue = text[index];
	functionCall.data[index + 1].type = TYPE_CHAR;

	/* send the function call message */
	USART2_SendMessage(FUNCTION_CALL_MSG, (char*)&functionCall, sizeof(FunctionCall));

	/* wait for a response */
	while(display_text_response.response_ready == 0);
	display_text_response.response_ready = 0;

	return display_text_response.response_data;
}

int terminateRPC(char *text)
{
	/* check text length */
	int textLength = 0;
	int index = 0;
	while(text[index] != '\0') {
		textLength++;
		index++;
	}
	if (textLength >= MAX_DATA_NUM)
		perror("Too Long Text");

	FunctionCall functionCall;
	functionCall.functionId = FUNC_TERMINATE_RPC_ID;
	functionCall.numData = 1;

	/* copy Text */
	index = 0;
	while (text[index] != '\0') {
		functionCall.data[index].type = TYPE_CHAR;
		functionCall.data[index].value.charValue = text[index];
		index++;
	}
	/* add null char */
	functionCall.data[index + 1].value.charValue = text[index];
	functionCall.data[index + 1].type = TYPE_CHAR;

	/* send the function call message */
	USART2_SendMessage(FUNCTION_CALL_MSG, (char*)&functionCall, sizeof(FunctionCall));

	/* wait for a response */
	while(terminate_rpc_response.response_ready == 0);
	terminate_rpc_response.response_ready = 0;

	return terminate_rpc_response.response_data;
}

