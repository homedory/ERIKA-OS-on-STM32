#include <wiringSerial.h>
#include "uart.h"
#include "stub.h"

void sendDataUART(int serialPort, char messageId, char* data, int size) 
{
    printf("send response\n");
    
    if (size > 255)
        perror("Data too large");

    // send STX
    serialPutchar(serialPort, STX);

    // send messageID
    serialPutchar(serialPort, messageId);
    
    // send data length
    serialPutchar(serialPort, (unsigned char) size);
    
    // send data
    while(size--) {
        serialPutchar(serialPort, *data);
        data++;
    }
}

int getStringFromFunctionCall(char* buff, FunctionCall* functionCall, int offset)
{
    int index = 0;

    if (functionCall->data[index + offset].type != TYPE_CHAR)
        return -1;

    while(functionCall->data[index + offset].type == TYPE_CHAR) {
        buff[index] = functionCall->data[index + offset].value.charValue;
        if (buff[index] == '\0')
            break;
        index++;
    }

    return 0;
}

int getIntFromFunctionCall(int* value, FunctionCall* functionCall, int offset)
{
    if (functionCall->data[offset].type != TYPE_INT)
        return -1;
    
    *value = functionCall->data[offset].value.intValue;

    return 0;
}

int callFunction(FunctionCall* functionCall)
{
    int functionReturn;
    char text[36];

    switch(functionCall->functionId) {
        case FUNC_MOVE_MOTOR_ID:
            int inputValue;
            if (getIntFromFunctionCall(&inputValue, functionCall, 0) < 0)
                perror("Function Call Error (func: moveMotor)");

            functionReturn = move_motor(inputValue);
            break;
        case FUNC_DISPLAY_TEXT_ID:
            int lineNum;

            if (getIntFromFunctionCall(&lineNum, functionCall, 0) < 0)
                perror("Function Call Error (func: displayText)");

            if (getStringFromFunctionCall(text, functionCall, 1) < 0)
                perror("Function Call Error (func: displayText)");
            
            functionReturn = display_text(lineNum, text);
            break;
        
        case FUNC_TERMINATE_RPC_ID:
            if (getStringFromFunctionCall(text, functionCall, 0) < 0)
                perror("Function Call Error (func: terminateRPC)");

            functionReturn = terminate_RPC(text);
            break;
    }

    return functionReturn;
}