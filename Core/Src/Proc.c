/*
 * Proc.c
 *
 *  Created on: Apr 25, 2024
 *      Author: niksh
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"

#define BUT_ERROR 0
#define BUT_PRESS_SHORT 1
#define BUT_PRESS_LONG 2
#define SIZE 2
#define DEBOUNCE_TIME 20
#define BORDER_TIME 500

static uint8_t state = 0;
static uint32_t startTime = 0;
static uint32_t stopTime = 0;
static uint32_t holdTime = 0;
static uint32_t currentTime = 0;
static uint32_t resetTime = 0;
static uint8_t pressList[SIZE] = {0,0};
static uint8_t counter = 0;
static uint8_t liftUp[SIZE] = {1,2};
static uint8_t liftDown[SIZE] = {2,0};
static uint8_t butCounter = 0;

uint8_t getButStatus()
{
	if (holdTime >= 3000)
	{
		return BUT_ERROR;
	}
	if (holdTime < BORDER_TIME)
	{
		return BUT_PRESS_SHORT;
	}
	if (holdTime >= BORDER_TIME)
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
		butCounter = 0;
	}
	else if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		butCounter++;
		if (butCounter > DEBOUNCE_TIME)
		{
			stopTime = currentTime;
			holdTime = stopTime - startTime;
			resetTime = 0;
			state = 1;
		}
	}

	if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (state == 1))
	{
		state = 0;
		pressList[counter] = getButStatus();
		counter++;

		setLiftStatus();
		if (counter == 2)
		{
			counter = 0;
		}

		holdTime = 0;
		state = 0;
	}

	if(resetTime >= 1000)
	{
		resetPressList();
	}
}

void setLiftStatus(void)
{
	if ((pressList[0] == liftDown[0]) && (pressList[1] == liftDown[1]))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);	//действие 1
		resetPressList();
	}
	else if ((pressList[0] == liftUp[0]) && (pressList[1] == liftUp[1]))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);	//действие 2
		resetPressList();
	}
}

void resetPressList (void)
{
	for(uint8_t i = 0; i < SIZE; i++)
	{
		pressList[i] = 0;
	}
	counter = 0;
}
