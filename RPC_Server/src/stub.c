#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stub.h"
#include "uart.h"
#include "motor.h"
#include "lcd.h"

extern int terminated;

int move_motor(int inputValue)
{
    moveMotor(inputValue);

    return 0;
}

int display_text(int lineNum, const char *text)
{
    initializeLCD();
    changeLine(lineNum);
    displayText(text);

    return 0;
}

int terminate_RPC(char *text)
{
    printf("--------------------terminate called--------------------\n");
    char *text_ = text;
    while(*text_ != '\0') {
        printf("%c", *text_);
        text_++;
    }

    char* quit = "quit";
    if (strncmp(text, quit, 4) == 0) {
        terminated = 1;
        return 0;
    }

    return -1;
}
