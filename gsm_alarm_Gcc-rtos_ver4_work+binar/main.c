#include "HAL.h"
#include "EERTOS.h"
#include <string.h>
unsigned int EEMEM  eeprom_array[3];
unsigned int my_eeprom_array[3];

// RTOS Interrupt
ISR(RTOS_ISR) { TimerService();}
// Прототипы задач ============================================================
void TaskLedOn (void);
void TaskLedOff (void);
void TaskLedBlink (void);
void TaskLcdBlink (void);
//void TaskUART (void);
void TaskADC3 (void);
void TaskADC_12V (void);
void TaskPWM (void);
void TaskADCButton (void);
void TaskMaxPWM (void);
void TaskAlarmByBattery (void);
void TaskLcd(void);

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

volatile uint32_t time_running = 0;
volatile uint32_t time_no_answer = 0;

//============================================================================
// Область задач
//============================================================================
#define SEC_TO_DAY(sec) sec/2/60/60/24
#define SEC_TO_HOUR(sec) sec/2/60/60%24
#define SEC_TO_MIN(sec) sec/2/260%60

void TaskLcd(void)
{
	SetTimerTask(TaskLcd, 500);
	time_running++;
	i2c_lcd_home();
	char t = ':';
	if (time_running%2 == 1){
		t = ' ';
	}
	fprintf_P(stderr, PSTR("%02ld%c%02ld%c%02ld    %03d "),
	SEC_TO_DAY(time_running),
	t,
	SEC_TO_HOUR(time_running),
	t,
	SEC_TO_MIN(time_running),
	//GetTimeToOff());
	adc_v[ADC_PIN_RED]);	
	
	i2c_lcd_gotoxy(0,1);
	fprintf_P(stderr, PSTR("%2ld.%02ldV  %+2ld,%02dA  "), 
		ADC_TO_VOLT2(adc_v[7], adc_v[ADC_PIN_12V])/1000, 
		ADC_TO_VOLT2(adc_v[7], adc_v[ADC_PIN_12V])%100, 
		ADC_TO_VOLT_I(adc_v[7], adc_v[ADC_PIN_RED])/1000,
		abs(ADC_TO_VOLT_I(adc_v[7], adc_v[ADC_PIN_RED])%100));
	if (ADC_TO_VOLT_I(adc_v[7], adc_v[ADC_PIN_RED]) < 0 &&
	ADC_TO_VOLT_I(adc_v[7], adc_v[ADC_PIN_RED]) == 0)
	{
		i2c_lcd_gotoxy(9,1);
		i2c_lcd_putch('-');		
	}
}

void TaskLcdBlink(void)
{
	static uint_least8_t status = 0;
	if (status == 0)
	{
		i2c_lcd_backlight_toggle();
		SetTimerTask(TaskLcdBlink, 200);
		status++;
	}	
}

void TaskLedBlink (void)
{
	PM_OnPin(LED_NEW);
	SetTimerTask(TaskLedOff,200);		
}
void TaskLedOn (void)
{	
	PM_OnPin(LED_NEW);
}		  
void TaskLedOff (void)
{
	PM_OffPin(LED_NEW);
}
volatile char c = 'a';
/*void TaskUART (void)
{
	fprintf(stderr, "%c.", c++);
	//fprintf(stdout, "pwm:%d\t 12v:%d\tOCR:%d\r\n", adc_v[ADC_PIN_RES_PWM], adc_v[ADC_PIN_12V],OCR1B);
    //fprintf(stdout, "U_1200mV:%d, U_1320mV:%d, U_1200mV:%d\t%d0\n", CONST_U_1200mV, CONST_U_1280mV, adc_v[ADC_PIN_12V], 50*adc_v[ADC_PIN_12V]/1023*3);
	//fprintf(stdout, "countOn: %d\t countOff:%d\n", countOn/4, countOff/4);
	SetTimerTask(TaskUART,1000);
	//SetTask(TaskLedBlink);
}*/
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
	adc_v[ADC_PIN_RED] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_RED);
	adc_v[7] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, 0x0E);
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
	SetTimerTask(TaskAlarm, 1000);
	if (ALARM_IN_GUARD)//fprintf(stdout, "Alarm is On");
	{
		i2c_lcd_backlight_off();
		error1 = ER_OK;
		SetBit(action1,AT_PING);
		SetTask(TaskLedBlink);
		
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
				//i2c_lcd_gotoxy(1, 4);
				//fprintf_P(stderr, PSTR("Call"));
				callTime = 0;
			}
		}
		//fprintf(stdout, "STATUS is High\t");
	} else 
	{
		i2c_lcd_backlight_on();
		error1 = ER_OK;
		SetBit(action1,AT_PING);
		if (time_no_answer > 10)
		{
			SetTimerTask(TaskLedBlink, 100);
		}
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
	//SetTask(TaskAlarmByBattery);
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
				time_no_answer++;
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
	char str[64];	
	fscanf(stdin,"%50s", str);
	//i2c_lcd_gotoxy(0, 0);
	//fprintf(stderr, str);
	SetBit(action1, AT_PHONE_IS_ON);	
	
	 if (strstr(str, "2375997") || strstr(str, "6088689"))
	 {
		 SetTask(TaskBinarOn);
		 //i2c_lcd_gotoxy(1, 9);
		 //fprintf_P(stderr, PSTR("ATA "));
		 fprintf_P(stdout, PSTR("ATA\r\n"));
		 unsigned int c = 0;
		 do
		 {
			 c = uart_getc();
		 } while (c != (UART_NO_DATA&c));
	 } else	if (strstr(str, "RING"))
	{
		//SetTask(TaskLedBlink);
		fprintf_P(stdout, PSTR("AT+CLCC\r\n"));
		//i2c_lcd_gotoxy(1, 9);
		//fprintf_P(stderr, PSTR("CLCC"));
	} else if (strstr(str, "NO"))
	{
		ClearBit(action1, AT_CALL);
		error1 = ER_NO_DIALTONE;
	} else if (strstr(str, "BUSY"))
	{
		ClearBit(action1, AT_CALL);
		error1 = ER_BUSY;						
	} else if (strstr(str, "ERROR"))
	{
		ClearBit(action1, AT_CALL);
		error1 = ER_ERROR;		
	} else if (strstr(str, "OK"))
	{
		time_no_answer = 0;
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
		SetTimerTask(TaskParseUart, 750);
	}
}

void initFromEE(){
	my_eeprom_array[EE_OCR] = read_eeprom_word(&eeprom_array[EE_OCR]);
	my_eeprom_array[EE_C_ON_OFF] = read_eeprom_word(&eeprom_array[EE_C_ON_OFF]) == PWM_ON ? PWM_ON : PWM_OFF;
}
//==============================================================================
int main(void)
{
	SetBit(action1, AT_PING);
	initFromEE();
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

    // Запуск фоновых задач.
	SetTimerTask(TaskADC3,10);
	SetTimerTask(TaskAlarm,1000);
	SetTimerTask(TaskPWM,5000);
	SetTimerTask(TaskTurnOnPhone, 1000);
	SetTimerTask(TaskSideLights,25);
	SetTimerTask(TaskADCButton, 50);
	//SetTimerTask(TaskAlarmByBattery, 250);
	//SetTimerTask(TaskUART, 50);
	SetTimerTask(TaskLcd, 50);
	//SetTimerTask(TaskBinarOn, 5000);
	//fprintf(stdout,"### START ###\r\n");
	fprintf(stdout,"START\r\n");	
	
	
	wdt_enable(WDTO_1S);
    while(1) 		// Главный цикл диспетчера
    {
        wdt_reset();	// Сброс собачьего таймера
        TaskManager();	// Вызов диспетчера
    }
    return 0;
}
