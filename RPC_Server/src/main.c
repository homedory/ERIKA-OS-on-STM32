#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "uart.h"
#include "can.h"
#include "stub.h"
#include "motor.h"
#include "lcd.h"

int terminated = 0;

int main(void)
{   
    int receiveState = RECEIVE_STX;
    int serialPort;

    if (wiringPiSetup() == -1)
        perror("Unable to start wiringPi");

    if ((serialPort = serialOpen("/dev/ttyS0", 9600)) < 0) 
        perror("Unable to open serial device");

    unsigned char dataLen;
    char messageId;
    FunctionCall functionCall;
    char* buff = (char*) &functionCall;
    unsigned char offset = 0;
    
    printf("RPC Sserver start\n");
    
    while(!terminated) {
        if (!serialDataAvail(serialPort)) 
            continue;

        char receivedByte = serialGetchar(serialPort);
        
        switch (receiveState)
        {
        case RECEIVE_STX:
            if (receivedByte == STX)
                receiveState = RECEIVE_MSG_ID;
            break;
        case RECEIVE_MSG_ID:
            messageId = receivedByte;
            receiveState = RECEIVE_DATALENGTH;
            break;
        case RECEIVE_DATALENGTH:
            dataLen = receivedByte;
            receiveState = RECEIVE_DATA;
            offset = 0;
            break;
        case RECEIVE_DATA:
            *(buff + offset) = receivedByte;
            offset++;
            // when complete data received
            if (offset == dataLen) {
                if (messageId == FUNCTION_CALL_MSG) {
                    int returnValue = callFunction(&functionCall);
                    
                    FunctionReturn functionReturn;
                    functionReturn.functionId = functionCall.functionId;
                    functionReturn.returnValue = returnValue;
                    // send function response
                    sendDataUART(serialPort, FUNCTION_RESPONSE_MSG, (char*) &functionReturn, sizeof(FunctionReturn));    
                }
                receiveState = RECEIVE_STX;
            }
            break;
        }
    }
}