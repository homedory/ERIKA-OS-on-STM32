#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#include "lcd.h"

#define Detected_DEVICE_ID_BY_I2C       0x27        // Device ID detected by I2C 
                                                    // Seems that it maps to the device's address
#define LCD_BACKLIGHT_ON                0x08        // On             "0000 1000"
#define LCD_BACKLIGHT_OFF               0x00        // Off            "0000 0000"
#define LCD_ENABLE                      0x04        // Enable         "0000 0100"
#define LCD_DISABLE                     0x00        // Disable        "0000 0000"
#define LCD_RW_READ                     0x02        // Read           "0000 0010" 
#define LCD_RW_WRITE                    0x00        // Write          "0000 0000"
#define LCD_RS_DATA                     0x01        // Data           "0000 0001"
#define LCD_RS_INST                     0x00        // Instruction    "0000 0000"


int deviceHandle;

void toggleLCDEnable(int eightBits)
{
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CWrite(deviceHandle, (eightBits | LCD_ENABLE));
  delayMicroseconds(500);
  wiringPiI2CWrite(deviceHandle, (eightBits | LCD_DISABLE));
  delayMicroseconds(500);
}

void sendBitsToLCD(int eightBits, int mode)
{
  // Send data or instruction to LCD I/O register
  // eightBits = the data or instruction to send
  // mode can be OR'ed with LCD_BACKLIGH_ON/OFF, LCD_ENABLE/DISABLE, LCD_RW_READ/WRITE, LCD_RS_DATA/INST
  // Since we use 4-bit mode, we first send high 4 bits (highFourBits), and then send low 4 bits (lowFourBits)
  int highFourBits;
  int lowFourBits;

  // The LCD's I/O register reads only the four high order data pins each time the Enable pin is pulsed
  // Let 'xxxx yyyy' be "eightBits" received as the parameter of sendBitsToLCD() function
  // We will first send highFourBits in the form of 'xxxx abcd', and then send lowFourBits in the form of 'yyyy abcd'  
  // We found that wiringPi uses the remaining 4-bit 'abcd' to control LCD pins as follows  
  //    'a': LCD_BACKLIGHT_ON or LCD_BACKLIGHT_OFF 
  //    'b': LCD_ENABLE or LCD_DISABLE
  //    'c': LCD_RW_READ or LCD_RW_WRITE
  //    'd': LCD_RS_DATA or LCD_RS_INST
  highFourBits = mode | (eightBits & 0xF0) | LCD_BACKLIGHT_ON;
  lowFourBits = mode | ((eightBits << 4) & 0xF0) | LCD_BACKLIGHT_ON;          
  
  // First, send highFourBits and pulse the enable pin
  wiringPiI2CWrite(deviceHandle, highFourBits);  // 'xxxx abcd': xxxx is the high 4 bits of eightBits
  toggleLCDEnable(highFourBits);

  // Next, send lowFourBits and pulse the enable pin
  wiringPiI2CWrite(deviceHandle, lowFourBits);   // 'yyyy abcd': yyyy is the low 4 bits of eightBits
  toggleLCDEnable(lowFourBits);
}


// Display text string 
void displayText(const char *stringPointer)
{   
    while(*stringPointer != '\0') {
        sendBitsToLCD((int)(*stringPointer), LCD_RS_DATA | LCD_RW_WRITE);
        stringPointer++;
    }
}

// Change the text line
void changeLine(int line)
{
    if (line == 1) {
        sendBitsToLCD(0x80, LCD_RS_INST | LCD_RW_WRITE);
    }
    else if (line == 2) {
        sendBitsToLCD(0x80 | 0x40, LCD_RS_INST | LCD_RW_WRITE);
    }
    else {
        perror("Line Number Larger than 2");
    }
}

// Initialize wiringPI, I2C, and LCD
int initializeLCD()
{
    wiringPiSetupGpio();
    deviceHandle = wiringPiI2CSetup(Detected_DEVICE_ID_BY_I2C);      
    
    // Instruction: Function Set '001D NF--' 
    sendBitsToLCD(0x33, LCD_RS_INST | LCD_RW_WRITE);       // "0011 0011" (8-bit mode, 1 line, 5x8 dots)

    // Instruction: Function Set '001D NF--'
    sendBitsToLCD(0x32, LCD_RS_INST | LCD_RW_WRITE);       // "0011 0010" (8-bit mode, 1 line, 5x8 dots)
    
    // Instruction: Entry Mode Set '0000 01IS'
    sendBitsToLCD(0x06, LCD_RS_INST | LCD_RW_WRITE);       // "0000 0110" (cursor move direction is increment, normal shift operation)

    // Instruction: Display ON/OFF Control '0000 1DCB'
    sendBitsToLCD(0x0C, LCD_RS_INST | LCD_RW_WRITE);       // "0000 1100" (display on, cursor Off, blink off)

    // Instruction: FUnction Set '001D NF--'
    sendBitsToLCD(0x28, LCD_RS_INST | LCD_RW_WRITE);       // "0010 1000" (4-bit mode, 2 lines, 5x8 dots)

    // Instruction: Clear Display
    sendBitsToLCD(0x01, LCD_RS_INST | LCD_RW_WRITE);       // "0000 0001" (clear display)

    sendBitsToLCD(0x80, LCD_RS_INST | LCD_RW_WRITE);       

    delayMicroseconds(500);

    return 0;
}
