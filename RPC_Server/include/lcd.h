void toggleLCDEnable(int eightBits);
void sendBitsToLCD(int eightBits, int mode);
void displayText(const char *stringPointer);
void changeLine(int line);
int initializeLCD();