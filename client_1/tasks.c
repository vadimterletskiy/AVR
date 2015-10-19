
/*
 * tasks.c
 *
 * Created: 10/16/2015 4:45:26 PM
 *  Author: vaterlet
 */ 
#include "tasks.h"
#include "defines.h"

void RunTasks (void)
{
	SetTimerTask(TaskLedOn,1000);
	SetTimerTask(TaskUART,1000);
	SetTimerTask(TaskADC3,10);
	//SetTimerTask(TaskAlarm,12000);
	//SetTimerTask(TaskPWM,25);
	//SetTimerTask(TaskTurnOnPhone, 1000);
	//SetTimerTask(TaskSideLights,25);
	//fprintf(stdout,"### START ###\r\n");
	//fprintf(stdout,"ATE0\r\n");
	wdt_enable(WDTO_120MS);
}

void TaskLedOn (void)
{
	SetTimerTask(TaskLedOn,10);
	//PORTB = 0x05;
}
void TaskADC3 (void)
{
	adc_v[ADC_PIN_RES_PWM] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_RES_PWM);
	adc_v[ADC_PIN_12V] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_12V);
	SetTimerTask(TaskADC3,50);
}
void TaskUART (void)
{
	//fprintf(stdout, "pwm:%d\t 12v:%d\tOCR:%d\r\n", adc_v[ADC_PIN_RES_PWM], adc_v[ADC_PIN_12V],OCR1B);
	//fprintf(stdout, "U_1200mV:%d, U_1320mV:%d, U_1200mV:%d\t%d0\n", CONST_U_1200mV, CONST_U_1280mV, adc_v[ADC_PIN_12V], 50*adc_v[ADC_PIN_12V]/1023*3);
	//fprintf(stdout, "countOn: %d\t countOff:%d\n", countOn/4, countOff/4);
	SetTimerTask(TaskUART,1000);
}
void TaskPWM (void) {
	SetTimerTask(TaskPWM,100);
	//char logTmp = (char)(adc_v[ADC_PIN_RES_PWM] & 0xFF);
	//fprintf(stdout, "%c", logTmp);
	if (adc_v[ADC_PIN_RES_PWM] <= 15)
	{
		adc_v[ADC_PIN_RES_PWM] = 0;
		TCCR1A = 0;
	} else
	{
		TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
	}
	OCR1A = adc_v[ADC_PIN_RES_PWM]*ICR_MAX/1024;
	OCR1B = OCR1A;	
}
void TaskSideLightsTurnOff (void) 
{
	
}
void TaskSideLightsTurnOn (void)
{
	
}

void TaskCheckSideLights (void)
{
	if (adc_v[ADC_PIN_12V] < U_mV(1300))
	{
		//turnOFF after 60S
		LightsCounter--;
		if (adc_v[ADC_PIN_12V] < U_mV(1200))
		{
			//turnOFF after 30S
			LightsCounter--;						
		}
	} else // (U > 1300mV)
	{
		LightsCounter = (LightsCounter < 0) ? 0 : LightsCounter;
		LightsCounter = (LightsCounter > 3) ? 3 : LightsCounter+1;
		//turnON after 3S
	}		
	SetTimerTask(TaskCheckSideLights,1000);
	if (LightsCounter < -60){ //turnON after 3S
		SwithLights(OFF);
		LightsCounter = 0;
	} else if (LightsCounter >= 3) //turnON after 3S
	{
		SwithLights(ON);
		LightsCounter = 0;
	}
	
	// Check alarm mode
	if (PM_PinIsClear(ALARM_STATUS)) //in Guard mode
	{
		SwithLights(OFF);
		LightsCounter = 0;		
	}
}
void TaskCall (void) {
	switch(callTime){
		case 0:
		case 15:
			SetBit(status, ST_ALARM);
			//fprintf_P(stdout, PSTR("ATDP+79082375997;\r\n"));
		break;
	}
	callTime++;
	
	SetTimerTask(TaskCall, 1000);
}

void TaskAlarm (void) {
	
	if (PM_PinIsClear(ALARM_STATUS) && PM_PinIsClear(ALARM_PAGER))//fprintf(stdout, "Alarm in GuardMode");
	{
		
	} else if (PM_PinIsClear(ALARM_STATUS) && PM_PinIsSet(ALARM_PAGER))
	{
		//fprintf(stdout, "Alarm is CALLING");
		if (BitIsClear(status, ST_ALARM))
		{
			SetTask(TaskCall);	
		}
	} else 
	{
		/*if (PM_PinIsSet(ALARM_STATUS))
		{
			fprintf(stdout, "STATUS is High\t");

		} else
		{
			fprintf(stdout, "STATUS is low\t");
		}
		if (PM_PinIsSet(ALARM_PAGER))
		{
			fprintf(stdout, "PAGER is High\n");

		} else
		{
			fprintf(stdout, "PAGER  is low\n");
		}*/	
	}
	SetTimerTask(TaskAlarm, 1000);
}
//void (void) {}
//void (void) {}
//void (void) {}
//void (void) {}
//void (void) {}
		


