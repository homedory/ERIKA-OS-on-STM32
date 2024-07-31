/*
 * led.h
 *
 *  Created on: 2024. 7. 9.
 *      Author: homedory
 */

#ifndef LED_H_
#define LED_H_

void delay(unsigned int nCount);
void TIM2_Initialize(void);
void onLED(void);
void offLED(void);
void toggleLED(void);
void blinkLED(void);


#endif /* LED_H_ */
