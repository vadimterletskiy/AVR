#include "HAL.h"
#include "EERTOS.h"
#include <string.h>
//unsigned int EEMEM  eeprom_array[5];
//unsigned int my_eeprom_array[5];
volatile char ADCbutton = '_';


// RTOS Interrupt
ISR(RTOS_ISR) { TimerService();}
// Прототипы задач ============================================================
void TaskUART (void);
void TaskSaveDataEE (void);
void TaskADC (void);
void TaskADC_12V (void);
void TaskPWM (void);
void TaskADCButton (void);
void TaskLCD (void);
void TaskMaxPWM (void);
void TaskCapCalc (void);

void TaskParseUart (void);

volatile uint16_t ADCButtonCount = 0;
volatile short prevPWM = 0;
volatile uint16_t count = 0;
volatile int32_t capacity = 0;
int16_t tmp16;

//============================================================================
// Область задач
//============================================================================


#define CAPTURE_INTERVAL 60*1000 //60sec, 1min
#define TASK_INTERVAL 1000 // 1sec
#define MINUTE 60000/TASK_INTERVAL
#define MIN_COUNT_5 5
#define MIN_COUNT_1 1
#define MIN_COUNT_10 10
#define INTERVAL_5_MIN MINUTE*MIN_COUNT_5
#define INTERVAL_1_MIN MINUTE*MIN_COUNT_1
#define INTERVAL_10_MIN MINUTE*MIN_COUNT_10

void TaskSaveDataEE (void){	
	static uint16_t timeCount = INTERVAL_10_MIN - 1;
	if (BitIsClear(ZU_STATUS, CAPTURE)){
		timeCount = INTERVAL_10_MIN - 1;
		return;
	}
	SetTimerTask(TaskSaveDataEE, TASK_INTERVAL);
	++timeCount;	
	if (timeCount == INTERVAL_10_MIN){
		timeCount = 0;
		EELogger_writeData(adc_v[ADC_PIN_12V], adc_v[ADC_PIN_I]);		//save
	}	
}
void TaskCapCalc (void)
{
	SetTimerTask(TaskCapCalc, TASK_INTERVAL);
	static uint8_t timeCount = 0;
	timeCount++;
	if (timeCount == INTERVAL_1_MIN){
		timeCount = 0;
		if (adc_v[ADC_PIN_I] > 512)
		{
			tmp16 = adc_v[ADC_PIN_I] - 512;
			capacity += MIN_COUNT_1 * tmp16;
		} else {
			tmp16 = 512 - adc_v[ADC_PIN_I];
			capacity -= MIN_COUNT_1 * tmp16;
		}
	}
}

void TaskLCD (void)
{
	#define  IADCmin_to_IAh(cap) cap*30/6/512
	
	SetTimerTask(TaskLCD,500);
	++count;
	i2c_lcd_gotoxy(0,0);
	fprintf(stderr, "%2ld.%02ldV %c  %2ld.%02ldA   ", 
			VOLT_FROM_ADC(adc_v[ADC_PIN_12V])/1000, 	
			VOLT_FROM_ADC(adc_v[ADC_PIN_12V])/10%100,
			ADCbutton,			
			CURRENT_FROM_ADC(adc_v[ADC_PIN_I]-512)/1000, //todo -0.5A
			CURRENT_FROM_ADC((abs(adc_v[ADC_PIN_I]-512)))/10%100);
			
	i2c_lcd_gotoxy(0,1);		
	if (count % 2){		
		fprintf(stderr, "%3d %2d.%1dAh %02d:%02d",
			(offset/2-1),
			(int16_t)(IADCmin_to_IAh(capacity)/10),
			abs((int16_t)(IADCmin_to_IAh(capacity)%10)),
			count/2/60/60,
			count/2/60%60);
	} else
	{		
		fprintf(stderr, "%3d %2d.%1dAh %02d %02d",
			(offset/2-1),
			(int16_t)(IADCmin_to_IAh(capacity)/10),
			abs((int16_t)(IADCmin_to_IAh(capacity)%10)),
			count/2/60/60,
			count/2/60%60);
	}	
	if (IADCmin_to_IAh(capacity) < 0 && IADCmin_to_IAh(capacity)/10 == 0)
	{
		i2c_lcd_gotoxy(4,1);
		i2c_lcd_putch('-');
	}
	if (adc_v[ADC_PIN_I]-512<0 && CURRENT_FROM_ADC(adc_v[ADC_PIN_I]-512)/1000 == 0)
	{
		i2c_lcd_gotoxy(11,0);
		i2c_lcd_putch('-');
	}
}

void TaskUART (void)
{
	fprintf(stdout, "123456 %d", adc_v[ADC_PIN_RES_PWM]);
	//fprintf(stdout, "pwm:%d\t 12v:%d\tOCR:%d\r\n", adc_v[ADC_PIN_RES_PWM], adc_v[ADC_PIN_12V],OCR1B);
    //fprintf(stdout, "U_1200mV:%d, U_1320mV:%d, U_1200mV:%d\t%d0\n", CONST_U_1200mV, CONST_U_1280mV, adc_v[ADC_PIN_12V], 50*adc_v[ADC_PIN_12V]/1023*3);
	//fprintf(stdout, "countOn: %d\t countOff:%d\n", countOn/4, countOff/4);
	SetTimerTask(TaskUART,1000);
}
void TaskMaxPWM (void)
{	
	if (OCR1A++ <= ICR_MAX){
		SetTimerTask(TaskMaxPWM, 50);
	}
	//OCR1B = OCR1A;
}

#define BUTTON1 0b010
#define BUTTON2 0b100
#define BUTTON_DOWN 0b1
#define IS_BUTTON_DOWN(button) button&BUTTON_DOWN
#define ANY_BUTTON_UP 0
#define BUTTON1_DOWN BUTTON1|BUTTON_DOWN
#define BUTTON2_DOWN BUTTON2|BUTTON_DOWN
#define IS_B1(button) button&BUTTON1
#define IS_B2(button) button&BUTTON2

#define IS_TIME_1500mS(timeStep50mS) timeStep50mS == 1500/25
#define IS_TIME_100mS(timeStep50mS) timeStep50mS == 100/25
#define IS_TIME_5S(timeStep50mS) timeStep50mS == 5000/25

void ButtonHandler(uint8_t timeStep50mS, uint8_t pressed)
{
	static uint8_t lastButton = 0;
	if (IS_BUTTON_DOWN(pressed)) {
		pressed &= 0xFE;
		lastButton = pressed;
		if (IS_TIME_1500mS(timeStep50mS)) // after 1500 mSec
		{
			ADCbutton = IS_B1(pressed)? 'X' : 'Y';
			i2c_lcd_gotoxy(7,0);
			i2c_lcd_putch(ADCbutton);
		} else if (IS_TIME_100mS(timeStep50mS))
		{
			ADCbutton = IS_B1(pressed)? 'x' : 'y';
			i2c_lcd_gotoxy(7,0);
			i2c_lcd_putch(ADCbutton);
		} else if (IS_TIME_5S(timeStep50mS))
		{
			ADCbutton = 'E';
			i2c_lcd_gotoxy(7,0);
			i2c_lcd_putch(ADCbutton);
		}
	} else
	{
		pressed = lastButton;
		if ((timeStep50mS >= 100/25) && (timeStep50mS <= 1500/25)) //short pressed
		{
			InvBit(ZU_STATUS, CHARGE);	//trigger Stasur Reg: charge/discharge
			PM_InvPin(SWITCH_PIN);		//trigger Relay:	  charge/discharge
			
		} else if (timeStep50mS >= 1500/25 && timeStep50mS < 5000/25) // long pressed:  1.5 - 5 sec 
		{
			InvBit(ZU_STATUS, CAPTURE);
			if (BitIsSet(ZU_STATUS, CAPTURE))
			{				
				SetTimerTask(TaskSaveDataEE, 1000);
				capacity = 0;
				count = 0;
			}
		} if (timeStep50mS >= 5000/25)      //long pressed:	  more than 5 sec 
		{		
			EraseEEPROM();
			count = 0;
		}
		ADCbutton = (BitIsSet(ZU_STATUS, CAPTURE) ? 'C' : '_'),  // C:capture, x:shortPress, X:longPress
		i2c_lcd_gotoxy(7,0);
		i2c_lcd_putch(ADCbutton);		
		lastButton = 0;
	}
}

void TaskADCButton (void)
{
	SetTimerTask(TaskADCButton, 25);
	if (adc_v[ADC_PIN_RES_PWM] < 16)
	{
		ButtonHandler(ADCButtonCount++, BUTTON1_DOWN);
	} else if (adc_v[ADC_PIN_RES_PWM] < 32) 
	{
		ButtonHandler(ADCButtonCount++, BUTTON2_DOWN);
	} else
	{
		if (ADCButtonCount > 0)
		{
			ButtonHandler(ADCButtonCount, ANY_BUTTON_UP);
			ADCButtonCount = 0;
		}		
	}
}

void TaskPWM (void)
{
	#define ADC_MAX 1023
	#define ADC_MIN 66
	
	SetTimerTask(TaskPWM,50);
	
	if (adc_v[ADC_PIN_RES_PWM] < 3){		
		return;
	} else if (adc_v[ADC_PIN_RES_PWM] <= ADC_MIN + 3)
	{
		OCR1B = 0;
		OCR1A = OCR1B;
		return;
	} else 
	{
		prevPWM = OCR1A;
	}
	if (BitIsClear(ZU_STATUS ,PWM_ENABLE)){
		OCR1B = 0;
		OCR1A = OCR1B;
		return;	
	}		
	tmp16 = (adc_v[ADC_PIN_RES_PWM]-ADC_MIN)*ICR_MAX/(ADC_MAX-ADC_MIN);
	if (prevPWM < tmp16)
	{
		OCR1A++;
	} else {
		OCR1A--;
	}
	OCR1B = OCR1A;
	TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
}

void TaskADC (void)
{
	SetTimerTask(TaskADC,50);
	adc_v[ADC_PIN_RES_PWM] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_RES_PWM);
	adc_v[ADC_PIN_12V] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_12V);
	adc_v[ADC_PIN_I] = adc_read(ADC_PRESCALER_64, ADC_VREF_AVCC, ADC_PIN_I);
}

void TaskParseUart(void)
{	
	char str[64];	
	fscanf(stdin,"%s", str);
	if (strstr(str, "RING"))
	{
		//fprintf_P(stdout, PSTR("AT+CLCC\r\n"));
	} else if (strstr(str, "2375997") || strstr(str, "6088689"))
	{
		//fprintf_P(stdout, PSTR("ATA\r\n"));
		unsigned int c = 0;
		do 
		{
			c = uart_getc();
		} while (c != (UART_NO_DATA&c));
		
	} else if (strstr(str, "NO"))
	{
	} else if (strstr(str, "BUSY"))
	{
	} else	if (strstr(str, "ERROR"))
	{
		
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
	//my_eeprom_array[EE_OCR] = read_eeprom_word(&eeprom_array[EE_OCR]);
	//my_eeprom_array[EE_C_ON_OFF] = read_eeprom_word(&eeprom_array[EE_C_ON_OFF]) == PWM_OFF ? PWM_OFF : PWM_ON;
}
//==============================================================================
int main(void)
{
	InitAll();			// Инициализируем периферию
    InitRTOS();			// Инициализируем ядро
    RunRTOS();			// Старт ядра. 
	initFromEE();
	
    // Запуск фоновых задач.
	SetTimerTask(TaskADC,10);
	SetTimerTask(TaskLCD,10);
	SetTimerTask(TaskPWM,25);
	//SetTimerTask(TaskUART,25);
	SetTimerTask(TaskADCButton, 50);	
	SetTimerTask(TaskCapCalc, 1000);
	fprintf(stdout,"start\r\n");
	wdt_enable(WDTO_1S);
    while(1) 		// Главный цикл диспетчера
    {
        wdt_reset();	// Сброс собачьего таймера
        TaskManager();	// Вызов диспетчера
    }
    return 0;
}
