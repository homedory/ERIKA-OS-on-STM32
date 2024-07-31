/*
 * sensor.h
 *
 *  Created on: 2024. 7. 9.
 *      Author: homedory
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void delayus(int us);
void delayms(int ms);
void TIM4_Initialize(void);
void TIM3_Initialize(void);
void TIM3_IRQHandler(void);
void sensorRead(void);
void sensorStart(void);

#endif /* ULTRASONIC_H_ */
