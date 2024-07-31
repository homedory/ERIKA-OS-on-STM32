/*
 * sensor.c
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */

#include <string.h>

#include "stm32f4xx_tim.h"
#include "sensor.h"
#include "led.h"
#include "ee.h"
#include "misc.h"
#include "swv.h"

#define TRIG_PORT GPIOB
#define TRIG_PIN GPIO_Pin_7

EE_UINT8 Is_First_Captured = 0;
EE_UINT32 IC_Val1 = 0;
EE_UINT32 IC_Val2 = 0;
EE_UINT32 Difference = 0;
EE_UINT8 Distance = 0;

TIM_ICInitTypeDef  TIM_ICInitStructure;

// for debugging
char buf[50];

void delayus(int us)
{
	TIM_SetCounter(TIM4,0);
	while(TIM_GetCounter(TIM4) < us);
}


void delayms(int ms)
{
	while(ms--) {
		delayus(1000);
	}
}


void TIM4_Initialize(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 200000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM4, ENABLE);
}


void TIM3_Initialize(void)
{
	/* enable TIM3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* enable GPIOB clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 channel 2 pin (PB.5) config */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM pins to AF2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

	/* Trigger pin (PB.7) config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* TIM3 config */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);

    /* TIM3 global Interrupt config*/
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the TIM3 CC2 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

		if (Is_First_Captured == 0) {
			IC_Val1 = TIM_GetCapture2(TIM3);
			Is_First_Captured = 1;

			/* set polarity to falling edge */
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
			TIM_ICInit(TIM3, &TIM_ICInitStructure);

		}
		else if (Is_First_Captured == 1){
			IC_Val2 = TIM_GetCapture2(TIM3);
			TIM_SetCounter(TIM3, 0);

			if (IC_Val2 > IC_Val1) {
				Difference = IC_Val2 - IC_Val1;
			}
			else if (IC_Val2 < IC_Val1) {
				Difference = (0xFFFF - IC_Val1) + IC_Val2;
			}
			else {
				Difference = 0;
			}

			Distance = Difference * 0.34 / 2;
			Is_First_Captured = 0;

			/* set polarity to rising edge */
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
			TIM_ICInit(TIM3, &TIM_ICInitStructure);

			TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
		}
	}
}


void sensorRead(void)
{
	/* trigger on */
	GPIO_SetBits(TRIG_PORT, TRIG_PIN);
	delayus(10);
	/* trigger off */
	GPIO_ResetBits(TRIG_PORT, TRIG_PIN);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}


void sensorStart(void)
{
	delayms(500);
	sensorRead();
	if (Distance <= 100)
		toggleLED();

	//for debugging
	sprintf(buf, "distance: %d\n", Distance);
	swv_write(buf, strlen(buf));
}
