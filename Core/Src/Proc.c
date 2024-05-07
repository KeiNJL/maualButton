/*
 * Proc.c
 *
 *  Created on: Apr 25, 2024
 *      Author: niksh
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"

#define BUT_NOT_PRESSED 0
#define BUT_PRESS_SHORT 1
#define BUT_PRESS_LONG 2


static uint8_t state = 0;
static uint8_t status = 0;
static uint32_t startTime = 0;
static uint32_t stopTime = 0;
static uint32_t holdTime = 0;
static uint32_t currentTime = 0;



uint8_t getButStatus()
{
	return BUT_NOT_PRESSED;
	if (holdTime < 1000)
	{
		return BUT_PRESS_SHORT;
	}
	else if (holdTime >= 1000)
	{
		return BUT_PRESS_LONG;
	}
	else if (holdTime == 0)
	{
		return BUT_NOT_PRESSED;
	}
	holdTime = 0;
	state = 0;
	HAL_TIM_Base_Start_IT(&htim2);
}

void ProcManualButton (void)
{
	currentTime = HAL_GetTick();

	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
	{
		startTime = currentTime;
	}
	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		stopTime = currentTime;
		holdTime = stopTime - startTime;
		state = 1;
	}
	if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (state == 1))
	{
		state = 0;
		getButStatus(status);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
    	getButStatus(status);
    	HAL_TIM_Base_Stop_IT(&htim2);
    }
}











