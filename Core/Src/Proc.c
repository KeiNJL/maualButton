/*
 * Proc.c
 *
 *  Created on: Apr 25, 2024
 *      Author: niksh
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"

uint8_t state = 0;
uint8_t status = 0;
uint32_t startTime = 0;
uint32_t stopTime = 0;
uint32_t holdTime = 0;
uint32_t currentTime = 0;

void ProcManualButton (void)
{
	currentTime = HAL_GetTick();

	if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (state == 0))
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
		if (holdTime == 0)
		{
			status = 0;
		}
		else if (holdTime < 1000)
		{
			status = 1;
		}
		else if (holdTime >= 1000)
		{
			status = 2;
		}
		holdTime = 0;
		state = 0;

	}
}







