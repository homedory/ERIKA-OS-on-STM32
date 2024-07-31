/*
 * led.c
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */
#include <stdio.h>

#include "ee.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#include "led.h"

int lightOn = 0;

void delay(unsigned int nCount)
{
	unsigned int i, j;

	for (int i = 0; i < nCount; i++)
		for (j = 0; j < 0x2AFF; j++);
}

void TIM2_Initialize(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = (45000-1);
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = (2000-1);
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);

	TIM_Cmd(TIM2, ENABLE);
}

void onLED(void)
{
	/* LED on */
	GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	lightOn = 1;
}

void offLED(void)
{
	/* LED off */
	GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	lightOn = 0;
}

void toggleLED(void)
{
	if (lightOn)
		offLED();
	else
		onLED();
}

void blinkLED(void)
{
	int timerValue = TIM_GetCounter(TIM2);

	if (timerValue % 2)
		onLED();
	else
		offLED();
}





