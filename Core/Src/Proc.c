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
#define SIZE 2


static uint8_t state = 0;
static uint32_t startTime = 0;
static uint32_t stopTime = 0;
static uint32_t holdTime = 0;
static uint32_t currentTime = 0;
static uint32_t resetTime = 0;
static uint8_t listPress[SIZE] = {0,0};
static uint8_t counter = 0;
static uint8_t liftUp[SIZE] = {1,2};
static uint8_t liftDown[SIZE] = {2,0};

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
		listPress[counter] = getButStatus();
		counter++;
		setLiftStatus();
		if (counter == 2)
		{
			counter = 0;
		}

		holdTime = 0;
		state = 0;
	}

	if(resetTime >= 2000)
	{
		for(uint8_t i = 0; i < SIZE; i++)
		{
			listPress[i] = 0;
		}
		counter = 0;
	}
}

void setLiftStatus(void)
{
	if ((listPress[0] == liftDown[0]) && (listPress[1] == liftDown[1]))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	}
	else if ((listPress[0] == liftUp[0]) && (listPress[1] == liftUp[1]))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
}











