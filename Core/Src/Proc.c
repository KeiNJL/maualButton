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
#define BUT_PRESS_SETUP 3

#define PRESS_LIST_SIZE 3

#define DEBOUNCE_TIME 20
#define BORDER_TIME 500
#define SETUP_TIME 5000

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

static uint8_t pressList[PRESS_LIST_SIZE] = {0,0,0};
static uint8_t liftUp[PRESS_LIST_SIZE] = {1,0,2};
static uint8_t liftDown[PRESS_LIST_SIZE] = {2,0,0};
static uint8_t liftSetup[PRESS_LIST_SIZE] = {3,0,0};

static uint8_t command = 0;

uint8_t getButStatus(void)
{
	if (holdTime == 0)
	{
		return BUT_PRESS_FREE;
	}
	else if (holdTime >= SETUP_TIME)
	{
		return BUT_PRESS_SETUP;
	}
	else if (holdTime < BORDER_TIME)
	{
		return BUT_PRESS_SHORT;
	}
	else if ((holdTime >= BORDER_TIME) && (holdTime < SETUP_TIME))
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
		if(resetTime >= 1000)
		{
			resetPressList();
		}
		butCounter = 0;
	}
	else if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		holdTime = currentTime - startTime;
	}

	if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1) && (butState == 0))
	{
		butCounter++;
		if (butCounter > DEBOUNCE_TIME)
		{
			resetTime = 0;
			butState = 1;
		}
	}
	else if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) && (butState == 1))
	{
		SetPressList();
		resetTime = 0;
		stopTime = currentTime;
		holdTime = 0;
		if (resetCounter == 0)
		{
			SetPressList();
		}
		resetCounter++;
		butState = 0;
	}
}

void SetPressList (void)
{
	pressList[counter] = getButStatus();
	counter++;
	if (counter == 3)
	{
		counter = 0;
	}
	setLiftStatus();
}

void setLiftStatus(void)
{
	if ((pressList[0] == liftDown[0]) && (pressList[1] == liftDown[1]) && (pressList[2] == liftDown[2]))		//длинное нажатие ( >500 мс)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		command = LIFT_DOWN;
	}
	else if ((pressList[0] == liftUp[0]) && (pressList[1] == liftUp[1]) && (pressList[2] == liftUp[2]))	//короткое + длинное нажатие
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		command = LIFT_UP;
	}
	else if ((pressList[0] == liftSetup[0]) && (pressList[1] == liftSetup[1]) && (pressList[2] == liftSetup[2]))	//юстировка (Длинное нажатие более 5 секунд)
	{
		command = LIFT_SETUP;
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
	else if (command == LIFT_SETUP)
	{
		return LIFT_SETUP;
	}
}




