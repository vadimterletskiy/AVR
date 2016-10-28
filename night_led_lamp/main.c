#include "HAL.h"
#include "EERTOS.h"
#include <string.h>
volatile u08 pressedTime = 0;
// RTOS Interrupt
ISR(RTOS_ISR) { TimerService();}
// Прототипы задач ============================================================


//============================================================================
// Область задач
//============================================================================
void TaskScanButtons (void);
void TaskLedBlink(void);
void TaskWaitButtonOff (void);
void TaskGreanLedOff (void);
void TaskRedLedOff (void);
void TaskSwitchLamp (void);
void ChangePwd  (void);


//	OCR1A = adc_v[ADC_PIN_RES_PWM]*ICR_MAX/1024;
//	OCR1B = OCR1A;	

void TaskSwitchLamp(void){
	//PM_InvPin(LED_GREEN);
	//PM_InvPin(LED_RED);
	if (TCCR1A == 0)
	{
		TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
	} else
	{
		TCCR1A = 0;		
	}
}

void TaskGreanLedOff(void){
	PM_OffPin(LED_GREEN); 
}
void TaskRedLedOff(void){
	PM_OffPin(LED_RED);
}

void ChangePwd (void){	
	if(PM_PinIsClear(BUTTON_PLUS))
	{		
		if (OCR1A >= 0){
			TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
		}
		if (OCR1A < ICR_MAX){
			OCR1A++;
			OCR1B = OCR1A;			
		} 
		if (OCR1A == ICR_MAX)
		{
			PM_OnPin(LED_GREEN); //push pressed		
		}
	} 
	if(PM_PinIsClear(BUTTON_MINUS))
	{
		if (OCR1A != 0){
			OCR1A--;
			OCR1B = OCR1A;
			PM_OffPin(LED_GREEN);
		} 
		if (OCR1A == 0){
			TCCR1A = 0;
		}				
	}
}

void TaskWaitButtonOff (void){
	if(PM_PinIsClear(BUTTON_PLUS) ||
	   PM_PinIsClear(BUTTON_MINUS) || 
	   PM_PinIsClear(BUTTON_ON_OFF))
	{
		SetTimerTask(TaskWaitButtonOff,5);		
		if (++pressedTime > 45)		
		{
			pressedTime = 45;
			ChangePwd();
		}		
		return;
	}
	pressedTime = 0;
	SetTimerTask(TaskScanButtons,10);
}


void TaskScanButtons (void){	
	if (PM_PinIsClear(BUTTON_PLUS)){
		PM_OnPin(LED_GREEN); //push pressed
		pressedTime = 0;
		ChangePwd();
		SetTimerTask(TaskWaitButtonOff,10);		
		SetTimerTask(TaskGreanLedOff,100);		
		return;
	}
	if (PM_PinIsClear(BUTTON_MINUS)){
		PM_OnPin(LED_RED); //push pressed
		pressedTime = 0;
		ChangePwd();
		SetTimerTask(TaskWaitButtonOff,10);		
		SetTimerTask(TaskRedLedOff,100);
		return;
	}
	
	if (PM_PinIsClear(BUTTON_ON_OFF)){
		SetTimerTask(TaskWaitButtonOff,10);
		SetTimerTask(TaskSwitchLamp,10);
		return;
	}
	SetTimerTask(TaskScanButtons, 10);
	return;	
}

//==============================================================================
int main(void)
{
	InitAll();			// Инициализируем периферию
	
	/*for (short i = 0 ; i < 10 ; i++)
	{
		PM_OnPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
		_delay_ms(100);
		PM_OffPin(LED_RED);//зажигаем светодиод //LED_PORT  ^=1<<LED1;
		_delay_ms(400);
	}*/
	pressedTime = 0;
    InitRTOS();			// Инициализируем ядро
    RunRTOS();			// Старт ядра. 
	SetTask(TaskScanButtons);	
	
    wdt_enable(WDTO_1S);
    while(1) 		// Главный цикл диспетчера
    {
        wdt_reset();	// Сброс собачьего таймера
        TaskManager();	// Вызов диспетчера
    }
    return 0;
}
