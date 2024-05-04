/*
 * Proc.c
 *
 *  Created on: Apr 25, 2024
 *      Author: niksh
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"


uint32_t pressTime = 0;
uint32_t currentTime = 0;
uint32_t holdTime = 0;
uint8_t counterButton = 0;


void ProcManualButton (void)

//{
//	currentTime = HAL_GetTick();
//
//
//	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
//	{
//		pressTime = currentTime;
//		HAL_TIM_Base_Start_IT(&htim2);
//	}
//
//
//	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
//	{
//		holdTime = currentTime - pressTime;
//
//		if (counterButton == 0)
//		{
//			counterButton++;
//		}
//
//		if (holdTime > 1000)
//		{
//			if (counterButton == 0)
//			{
//				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//			}
//			if (counterButton == 1)
//			{
//				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//			}
//			HAL_TIM_Base_Stop_IT(&htim2);
//		}
//	}
//}
//
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if (htim->Instance == TIM2)
//    {
//        counterButton = 0;
//    }
//}


{

	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	}
}





