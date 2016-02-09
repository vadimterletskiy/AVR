/*
 * TaskLamp.c
 *
 * Created: 10/20/2015 17:36:21 PM
 *  Author: vaterlet
 */ 

#include "TaskLamp.h"
volatile char status = 0;
//volatile int8_t countOn = 0;
//volatile int8_t countOff = 0;
int8_t timeToOff = 0;
int16_t GetTimeToOff()
{
	return timeToOff;
}

/*void SwitchLight(int16_t count, int16_t timeLimit)
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
}*/

/*void TaskSideLights (void)
{
	#define U adc_v[ADC_PIN_12V]
	#define U_CONST(u) CONST_U_mV(u, adc_v[0])
	SetTimerTask(TaskSideLights,TIME_STEP);
	i2c_lcd_gotoxy(6,0);
	i2c_lcd_putch('_');	
	if (PM_PinIsClear(ALARM_STATUS))
	{
		i2c_lcd_putch('1');
		SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 2);
		return;
	}
	if (U < U_CONST(10000))
	//if (CONST_U_mV(adc_v[ADC_PIN_12V], adc_v[0]) < 10000)
	{
		//i2c_lcd_putch('2');		
		SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 4);
		return;
	}
	//fprintf_P(stdout, PSTR("Light\t"));
	//if (adc_v[ADC_PIN_12V] < CONST_U_mV(13000, adc_v[0]))
	if (U < U_CONST(13000))
	{
		i2c_lcd_putch('3');
		//if (adc_v[ADC_PIN_12V] < CONST_U_mV(11000, adc_v[0]))
		if (U < U_CONST(11000))
		{
			i2c_lcd_putch('4');
			SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S / 2);
		} else {
			i2c_lcd_putch('5');
			SwitchLight(TIME_OFF_COUNTER, TIME_LIMIT_1S * 60);
		}
	} else // (< 13000)
	{
		i2c_lcd_putch('6');
		SwitchLight(TIME_ON_COUNTER, TIME_LIMIT_1S * 3);
	}
}*/

void TaskSideLights (void)
{
	#define U adc_v[ADC_PIN_12V]
	#define U_CONST(u) CONST_U_mV(u, adc_v[0])
	
	SetTimerTask(TaskSideLights,1000);
	
	if (PM_PinIsClear(ALARM_STATUS))
	{
		timeToOff = 0;
	} else if (U>U_CONST(14000)){
		timeToOff += 3;
	} else if (U>U_CONST(13200)){
		++timeToOff;
	} else if (U>U_CONST(12800)){	
		--timeToOff;
	} else if (U>U_CONST(12000)){
		timeToOff -= 5;
	} else
	{
		timeToOff -= 10;
	}
	if (timeToOff > 99)
	{
		timeToOff = 99;
	} else if (timeToOff < -10)
	{
		timeToOff = -10;
	}	
	if (timeToOff == 0){
		TaskSideLightsTurnOff();
	} else if (timeToOff > 5)
	{
		TaskSideLightsTurnOn();
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
