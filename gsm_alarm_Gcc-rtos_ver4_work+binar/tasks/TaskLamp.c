/*
 * TaskLamp.c
 *
 * Created: 10/20/2015 17:36:21 PM
 *  Author: vaterlet
 */ 

#include "TaskLamp.h"
volatile char status = 0;
volatile int8_t countOn = 0;
volatile int8_t countOff = 0;

void SwitchLight(int16_t count, int16_t timeLimit)
{
	if (count == TIME_ON_COUNTER) //ON
	{
		countOff--; //decrement
		if (LightIsOff(status))
		{
			countOn++; //increment
		}
		if (countOff < 0)
		{
			countOff = 0;
		}
		if (countOn > timeLimit)
		{
			countOn = 0;
			countOff = 0;
			NeedTurnOn_true(status);
			SetTask(TaskSideLightsTurnOn);
		}
	} else //OFF
	{
		if (LightIsOn(status))
		{
			countOff++; //increment
		}
		countOn--; //decrement
		if (countOn < 0)
		{
			countOn = 0;
		}
		if (countOff > timeLimit)
		{
			countOn = 0;
			countOff = 0;
			NeedTurnOff_true(status);
			SetTask(TaskSideLightsTurnOff);
		}
	}
}

void TaskSideLights (void)
{
	SetTimerTask(TaskSideLights,TIME_STEP);
	if (PM_PinIsClear(ALARM_STATUS))
	{
		SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 2);
		return;
	}
	if (adc_v[ADC_PIN_12V] < CONST_U(1000))
	{
		SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 4);
		return;
	}
	//fprintf_P(stdout, PSTR("Light\t"));
	if (adc_v[ADC_PIN_12V] < CONST_U(1300))
	{
		if (adc_v[ADC_PIN_12V] < CONST_U(1100))
		{
			SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 2);
		} else {
			SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S * 60);
		}
	} else // (U_1200mV>CONST_U_1280mV)
	{
		SwitchLight(TIME_ON_COUNTER, TIME_LIMIT_1S * 3);
	}
}

void TaskSideLightsTurnOff (void)
{
	//fprintf_P(stdout, PSTR("\n\tTaskSideLightsTurnOff"));
	if (NeedTurnOff(status))
	{
		//fprintf(stdout, "\t######TurnOff\n");
		TurnOffLight(status);//PM_OffPin(SIDE_LIGHTS);
	}
	NeedTurnOff_false(status);
	NeedTurnOn_false(status);
}

void TaskSideLightsTurnOn (void)
{
	//fprintf_P(stdout, PSTR("\n\tTaskSideLightsTurnOn"));
	if (NeedTurnOn(status))
	{
		//fprintf(stdout, "\t######TurnOn\n");
		//PM_OnPin(SIDE_LIGHTS);
		TurnOnLight(status);
	}
	NeedTurnOff_false(status);
	NeedTurnOn_false(status);
}
