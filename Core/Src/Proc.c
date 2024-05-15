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
#define SIZE 3


static uint8_t state = 0;
uint8_t status = 0;
static uint32_t startTime = 0;
static uint32_t stopTime = 0;
static uint32_t holdTime = 0;
static uint32_t currentTime = 0;
static uint32_t resetTime = 0;
static uint8_t arr[SIZE] = {1,0,1};
static uint8_t counter = 0;

uint8_t getButStatus()
{
	if (holdTime == 0)
	{
		return BUT_NOT_PRESSED;
	}
	if (holdTime < 1000)
	{
		return BUT_PRESS_SHORT;
	}
	if (holdTime >= 1000)
	{
		return BUT_PRESS_LONG;
	}
}

void ProcManualButton (void)
{
	currentTime = HAL_GetTick();

	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
	{
		startTime = currentTime;
		resetTime = currentTime - stopTime;
	}
	else if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		stopTime = currentTime;
		holdTime = stopTime - startTime;
		state = 1;
		resetTime = 0;
	}

	if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (state == 1))
	{
		state = 0;
		HAL_TIM_Base_Start_IT(&htim2);  ////////////////
		status = getButStatus();

		arr[counter] = getButStatus();
		counter++;

		holdTime = 0;
		state = 0;
	}

	if(resetTime >= 2000)
	{
		for(uint8_t i = 0; i < SIZE; i++)
		{
			arr[i] = 0;
		}
		counter = 0;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) ////////////////
{
    if (htim->Instance == TIM2)
    {
    	status = getButStatus();
    	holdTime = 0;
    	state = 0;
    	HAL_TIM_Base_Stop_IT(&htim2);
    }
}











