#include "HAL.h"
#include "EERTOS.h"
#include <string.h>
unsigned int EEMEM  eeprom_array[5];
unsigned int my_eeprom_array[5];

// RTOS Interrupt
ISR(RTOS_ISR) { TimerService();}
// Прототипы задач ============================================================
void TaskLedOn (void);
void TaskLedOff (void);
void TaskLedBlink (void);
void TaskUART (void);
void TaskADC3 (void);
void TaskADC_12V (void);
void TaskPWM (void);
void TaskADCButton (void);
void TaskMaxPWM (void);
void TaskAlarmByBattery (void);

void TaskTurnOnPhone (void);
void TaskParseUart (void);

void TaskAlarm (void);
void TaskSideLights (void);

void TaskBinarOn (void);

volatile u08 callTime = 0;
volatile u08 tmp = 0;
#define COUNT2 16
volatile int8_t timeCount = 0;
volatile uint16_t ADCButtonCount = 0;
volatile short action1 = 0;//1 - alarm
volatile short error1 = ER_OK;//1 - alarm
volatile short binar = 0;//1 - binar
volatile short prevPWM = 0;
volatile uint8_t batteryAlarm = 0;


//============================================================================
// Область задач
//============================================================================
void TaskLedBlink (void)
{
	PM_OnPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
	SetTimerTask(TaskLedOff,200);	
}
void TaskLedOn (void)
{	
	PM_OnPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
}		  
void TaskLedOff (void)
{
	PM_OffPin(LED_RED);	//гасим светодиод //LED_PORT  &= ~(1<<LED1);
}

void TaskUART (void)
{
	fprintf(stdout, "pwm:%d\t 12v:%d\tOCR:%d\r\n", adc_v[ADC_PIN_RES_PWM], adc_v[ADC_PIN_12V],OCR1B);
    //fprintf(stdout, "U_1200mV:%d, U_1320mV:%d, U_1200mV:%d\t%d0\n", CONST_U_1200mV, CONST_U_1280mV, adc_v[ADC_PIN_12V], 50*adc_v[ADC_PIN_12V]/1023*3);
	//fprintf(stdout, "countOn: %d\t countOff:%d\n", countOn/4, countOff/4);
	SetTimerTask(TaskUART,1000);
}
void TaskMaxPWM (void)
{	
	if (OCR1A++ <= ICR_MAX){
		SetTimerTask(TaskMaxPWM, 50);
	}
	OCR1B = OCR1A;
}


void ButtonHandler(uint8_t timeStep50mS, uint8_t pressed)
{
	if (pressed == 1) {
		if (timeStep50mS == 1500/25) // after 1500 mSec
		{
			my_eeprom_array[EE_OCR] = prevPWM;
			// write to eeprom array
			write_eeprom_word(&eeprom_array[EE_OCR], my_eeprom_array[EE_OCR]);  // write value 1 to position 0 of the eeprom array
			SetTask(TaskLedOn);
			SetTimerTask(TaskMaxPWM, 50);
		}
	} else
	{
		if ((timeStep50mS >= 100/25) && (timeStep50mS <= 1500/25))
		{
			my_eeprom_array[EE_C_ON_OFF] = (my_eeprom_array[EE_C_ON_OFF]) ? PWM_OFF : PWM_ON ;
			write_eeprom_word(&eeprom_array[EE_C_ON_OFF], my_eeprom_array[EE_C_ON_OFF]);  // write value 1 to position 0 of the eeprom array				
		} else if (timeStep50mS >= 1500/25)
		{
			SetTask(TaskLedOff);
		}
	}
}

void TaskADCButton (void)
{
	SetTimerTask(TaskADCButton, 25);
	if (adc_v[ADC_PIN_RES_PWM] < 10)
	{
		ButtonHandler(ADCButtonCount++, 1);
	} else {
		if (ADCButtonCount > 0)
		{
			ButtonHandler(ADCButtonCount, 0);
			ADCButtonCount = 0;
		}		
	}
}

void TaskPWM (void)
{
	SetTimerTask(TaskPWM,100);
	//char logTmp = (char)(adc_v[ADC_PIN_RES_PWM] & 0xFF);
	//fprintf(stdout, "%c", logTmp);
	if (adc_v[ADC_PIN_RES_PWM] <= 10 )
	{
		return;
	} else
	{
		prevPWM = OCR1A;
	}
	if (ALARM_IN_GUARD) //in  guard
	{
		OCR1A = my_eeprom_array[EE_OCR];
		OCR1B = OCR1A;
		TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
	} else if (my_eeprom_array[EE_C_ON_OFF] == PWM_ON)
	{
		OCR1A = adc_v[ADC_PIN_RES_PWM]*ICR_MAX/1024;
		OCR1B = OCR1A;
		TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
	} else
	{
		TCCR1A = 0; // Отключаем PWM	
	} 
}

void TaskADC3 (void)
{
	adc_v[ADC_PIN_RES_PWM] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_RES_PWM);
	adc_v[ADC_PIN_12V] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_12V);
	SetTimerTask(TaskADC3,50);
}

#define checkAlarm 1000
#define timeToCall 30000

void TaskAlarmByBattery (void)
{
	/*if (ALARM_IN_GUARD && batteryAlarm == 1)
	{
		batteryAlarm = 1;
	} else if (batteryAlarm < 55){
		++batteryAlarm;
		
	} else
	{
		batteryAlarm = 1;			//call
		SetTimerTask(TaskAlarmByBattery, 60000); //call again after 1 min
		return;				
	}
	SetTimerTask(TaskAlarmByBattery, 250);*/
}
void TaskAlarm (void)
{
	if (ALARM_IN_GUARD)//fprintf(stdout, "Alarm is On");
	{		
		error1 = ER_OK;
		SetBit(action1,AT_PING);
		//SetTask(TaskLedBlink);
		
	} else if (ALARM_IN_CALLING)//fprintf(stdout, "Alarm is CALLING");
	{		
		if (BitIsClear(action1, AT_CALL))
		{
			SetBit(action1, AT_CALL);
			if (error1 != ER_BUSY)
			{
				ClearBit(action1, AT_PING);
				error1 = ER_OK;
				fprintf_P(stdout, PSTR("ATDP+79082375997;\r\n"));
				callTime = 0;
			}
		}
		//fprintf(stdout, "STATUS is High\t");
	} else 
	{
		error1 = ER_OK;
		SetBit(action1,AT_PING);
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
	SetTask(TaskAlarmByBattery);
}

void TaskTurnOnPhone(void)
{
	static u08 timeToTurnOn = 0;
	if (BitIsSet(action1, AT_PHONE_IS_ON))
	{
		ClearBit(action1, AT_PHONE_IS_ON);		
		timeToTurnOn = 0;
		SetTimerTask(TaskTurnOnPhone, 5000);
		return;
	}
	SetTimerTask(TaskTurnOnPhone, 1000);
	switch (++timeToTurnOn)
	{
		case 5:
		case 10:
			if (BitIsSet(action1, AT_PING))
			{	
				fprintf_P(stdout, PSTR("ATE0\r\n"));
			}				
		break;
		case 16:
			POWER_PUSH_DOWN;				
		break;
		case 18:
			POWER_PUSH_UP;
			action1 = AT_PING;
			error1 = ER_OK;			
		break;
		case 19:
			SetBit(action1, AT_PING);
			timeToTurnOn = 0;
		break;		
	}
}

void TaskParseUart(void)
{
	SetBit(action1, AT_PHONE_IS_ON);
	char str[64];	
	//gets(str);
	//fscanf(stdin,"%60s", str);
	fscanf(stdin,"%s", str);
	if (strstr(str, "RING"))
	{
		//SetTask(TaskLedBlink);
		fprintf_P(stdout, PSTR("AT+CLCC\r\n"));
	} else if (strstr(str, "2375997") || strstr(str, "6088689"))
	{
		SetTask(TaskBinarOn);
		fprintf_P(stdout, PSTR("ATA\r\n"));
		unsigned int c = 0;
		do 
		{
			c = uart_getc();
		} while (c != (UART_NO_DATA&c));
	} else if (strstr(str, "NO"))
	{
		ClearBit(action1, AT_CALL);
		//fprintf_P(stdout, PSTR("_1"));		
		error1 = ER_NO_DIALTONE;
	} else if (strstr(str, "BUSY"))
	{
		ClearBit(action1, AT_CALL);
		error1 = ER_BUSY;						
		//fprintf_P(stdout, PSTR("_2"));
	} else	if (strstr(str, "ERROR"))
	{
		//fprintf_P(stdout, PSTR("_3"));
		ClearBit(action1, AT_CALL);
		error1 = ER_ERROR;		
	}	
}

void TaskBinarOn (void) {
	if (binar == 0)
	{
		binar = 1;
		SetTask(TaskLedOn);
		PM_OnPin(BINAR);
		SetTimerTask(TaskBinarOn, 1500);		
	} else if (binar == 1)
	{
		binar = 2;
		SetTask(TaskLedOff);
		PM_OffPin(BINAR);
		SetTimerTask(TaskBinarOn, 10000);		
	} else if (binar == 2)
	{
		binar = 0;
	}
}

void uartHendler(unsigned char c)
{
	if (c == '\n' || c == '\r')
	{
		SetTimerTask(TaskParseUart, 100);
	}
}

void initFromEE(){
	my_eeprom_array[EE_OCR] = read_eeprom_word(&eeprom_array[EE_OCR]);
	my_eeprom_array[EE_C_ON_OFF] = read_eeprom_word(&eeprom_array[EE_C_ON_OFF]) == PWM_OFF ? PWM_OFF : PWM_ON;
}
//==============================================================================
int main(void)
{
	SetBit(action1, AT_PING);
    InitAll();			// Инициализируем периферию
	
	/*for (short i = 0 ; i < 10 ; i++)
	{
		PM_OnPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
		_delay_ms(100);
		PM_OffPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
		_delay_ms(400);
	}*/
	
    InitRTOS();			// Инициализируем ядро
    RunRTOS();			// Старт ядра. 
	initFromEE();

    // Запуск фоновых задач.
	SetTimerTask(TaskADC3,10);
	SetTimerTask(TaskAlarm,12000);
	SetTimerTask(TaskPWM,25);
	SetTimerTask(TaskTurnOnPhone, 1000);
	SetTimerTask(TaskSideLights,25);
	SetTimerTask(TaskADCButton, 50);
	SetTimerTask(TaskAlarmByBattery, 250);
	//SetTimerTask(TaskBinarOn, 5000);
	//fprintf(stdout,"### START ###\r\n");
	fprintf(stdout,"ATE0\r\n");	
	
	
	wdt_enable(WDTO_1S);
    while(1) 		// Главный цикл диспетчера
    {
        wdt_reset();	// Сброс собачьего таймера
        TaskManager();	// Вызов диспетчера
    }
    return 0;
}
