/*
 * rpc.h
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */
#include "stdint.h"

#define FUNC_MOVE_MOTOR_ID 0x10
#define FUNC_DISPLAY_TEXT_ID 0x20
#define FUNC_TERMINATE_RPC_ID 0x70

#define FUNCTION_CALL_MSG 0x01
#define FUNCTION_RESPONSE_MSG 0x02

#define TYPE_INT 0
#define TYPE_CHAR 1

#define MAX_DATA_NUM 30

typedef union _Value{
	int intValue;
	char charValue;
}Value;

typedef struct _Data{
	char type;
	Value value;
} Data;

typedef struct _FunctionCall{
    int functionId;
    int numData;
    Data data[MAX_DATA_NUM];
} FunctionCall;

typedef struct _FunctionReturn{
	int functionId;
	int returnValue;
} FunctionReturn;
