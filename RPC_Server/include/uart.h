#include "rpc.h"

#define STX 0xFD

#define RECEIVE_STX 0x00
#define RECEIVE_MSG_ID 0x01
#define RECEIVE_DATALENGTH 0x02
#define RECEIVE_DATA 0x03

void sendDataUART(int serialPort, char messageId, char* data, int size); 
int getStringFromFunctionCall(char* buff, FunctionCall* functionCall, int offset);
int getIntFromFunctionCall(int* value, FunctionCall* functionCall, int offset);
int callFunction(FunctionCall* functionCall);
