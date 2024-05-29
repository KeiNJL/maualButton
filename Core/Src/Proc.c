/*
 * ProcManualButton.c
 *
 *  Created on: Apr 23, 2024
 *      Author: niksh
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"

#define BUT_PRESS_FREE 0
#define BUT_PRESS_SHORT 1
#define BUT_PRESS_LONG 2
#define PRESS_LIST_SIZE 2

#define DEBOUNCE_TIME 20
#define BORDER_TIME 500

#define LIFT_FREE 0
#define LIFT_UP 1
#define LIFT_DOWN 2
#define LIFT_SETUP 3

static uint8_t butState = 0;

static uint32_t startTime = 0;
static uint32_t stopTime = 0;
static uint32_t holdTime = 0;
static uint32_t currentTime = 0;
static uint32_t resetTime = 0;

static uint8_t counter = 0;
static uint8_t resetCounter = 0;
static uint8_t butCounter = 0;

static uint8_t pressList[PRESS_LIST_SIZE] = {0,0};
static uint8_t liftUp[PRESS_LIST_SIZE] = {1,2};
static uint8_t liftDown[PRESS_LIST_SIZE] = {2,0};

static uint8_t command = 0;

uint8_t getButStatus(void)
{
	if (holdTime == 0)
	{
		return BUT_PRESS_FREE;
	}
	else if (holdTime < BORDER_TIME)
	{
		return BUT_PRESS_SHORT;
	}
	else if (holdTime >= BORDER_TIME)
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
		holdTime = currentTime - startTime;
	}

	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		butCounter++;
		if (butCounter > DEBOUNCE_TIME)
		{
			resetTime = 0;
			SetPressList();
			butState = 1;
		}
	}
	else if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (butState == 1))
	{
		stopTime = currentTime;
		butState = 0;
		SetPressList();
	}

	if(resetTime >= 1000)
	{
		resetPressList();
	}

	setLiftStatus();
}

void SetPressList (void)
{
		if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
		{
			pressList[counter] = getButStatus();
		}
		else if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
		{
			holdTime = 0;
			resetTime = 0;
			counter++;
			if (counter >= 3)
			{
				counter = 0;
			}
			pressList[counter] = getButStatus();
		}
}

void setLiftStatus(void)
{
	if ((pressList[0] == liftDown[0]) && (pressList[1] == liftDown[1]))			//длинное нажатие ( >500 мс)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		command = LIFT_DOWN;
		if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
		{
			resetPressList();
		}
	}
	else if ((pressList[0] == liftUp[0]) && (pressList[1] == liftUp[1]))		//короткое + длинное нажатие
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		command = LIFT_UP;
		if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
		{
			resetPressList();
		}
	}
	else
	{
		command = LIFT_FREE;
	}
}

void resetPressList (void)
{
	for(uint8_t i = 0; i < PRESS_LIST_SIZE; i++)
	{
		pressList[i] = 0;
	}
	counter = 0;
	resetCounter = 0;
}

uint8_t GetButCommand(void)
{
	if (command == LIFT_DOWN)
	{
		return LIFT_DOWN;
	}
	else if (command == LIFT_UP)
	{
		return LIFT_UP;
	}
	else if (command == LIFT_FREE)
	{
		return LIFT_FREE;
	}
}

