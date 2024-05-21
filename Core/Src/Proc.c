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
#define BUT_PRESS_SETUP 3
#define SIZE 2

#define DEBOUNCE_TIME 20
#define BORDER_TIME 500
#define SETUP_TIME 5000

#define LIFT_FREE 0
#define LIFT_UP 1
#define LIFT_DOWN 2
#define LIFT_SETUP 3
#define LIFT_STOP 4

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
static uint8_t liftSetup[SIZE] = {3,0};
static uint8_t liftStop[SIZE] = {1,1};

static uint8_t butCounter = 0;
static uint8_t command = 0;

uint8_t getButStatus()
{
	if (holdTime >= SETUP_TIME)
	{
		return BUT_PRESS_SETUP;
	}
	if (holdTime < BORDER_TIME)
	{
		return BUT_PRESS_SHORT;
	}
	if ((holdTime >= BORDER_TIME) && (holdTime < SETUP_TIME))
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
	if ((pressList[0] == liftDown[0]) && (pressList[1] == liftDown[1]))		//длинное нажатие ( >500 мс)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		command = LIFT_DOWN;
		resetPressList();
	}
	else if ((pressList[0] == liftUp[0]) && (pressList[1] == liftUp[1]))	//короткое + длинное нажатие
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		command = LIFT_UP;
		resetPressList();
	}
	else if ((pressList[0] == liftSetup[0]) && (pressList[1] == liftSetup[1]))	//юстировка (Длинное нажатие более 5 секунд)
	{
		command = LIFT_SETUP;
		resetPressList();
	}
	else if ((pressList[0] == liftStop[1]) && (pressList[1] == liftStop[1]))	//cтоп (2 коротких нажатия)
	{
		command = LIFT_STOP;
		resetPressList();
	}
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
	else if (command == LIFT_STOP)
	{
		return LIFT_STOP;
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
