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
void TaskLed1On (void);
void TaskLed2On (void);
void TaskLed3On (void);
void TaskLed4On (void);
void TaskLed5On (void);
void TaskLed6On (void);
void TaskLedOff (void);

//============================================================================
// Область задач
//============================================================================

void TaskUART (void)
{
	fprintf(stdout, "123456 %d", adc_v[ADC_PIN_RES_PWM]);
	//fprintf(stdout, "pwm:%d\t 12v:%d\tOCR:%d\r\n", adc_v[ADC_PIN_RES_PWM], adc_v[ADC_PIN_12V],OCR1B);
    //fprintf(stdout, "U_1200mV:%d, U_1320mV:%d, U_1200mV:%d\t%d0\n", CONST_U_1200mV, CONST_U_1280mV, adc_v[ADC_PIN_12V], 50*adc_v[ADC_PIN_12V]/1023*3);
	//fprintf(stdout, "countOn: %d\t countOff:%d\n", countOn/4, countOff/4);
	SetTimerTask(TaskUART,1000);
}

void TaskLed1On (void) {PM_OnPin(LED_1);SetTimerTask(TaskLed2On,TIME_LED);}
void TaskLed2On (void) {PM_OnPin(LED_2);SetTimerTask(TaskLed3On,TIME_LED);}
void TaskLed3On (void) {PM_OnPin(LED_3);SetTimerTask(TaskLed4On,TIME_LED);}
void TaskLed4On (void) {PM_OnPin(LED_4);SetTimerTask(TaskLed5On,TIME_LED);}
void TaskLed5On (void) {PM_OnPin(LED_5);SetTimerTask(TaskLed6On,TIME_LED);}
void TaskLed6On (void) {PM_OnPin(LED_6);SetTimerTask(TaskLedOff,TIME_LED);}
		
void TaskLedOff (void)
{
	SetTimerTask(TaskLed1On,TIME_LED);
	PM_OffPin(LED_1);
	PM_OffPin(LED_2);
	PM_OffPin(LED_3);
	PM_OffPin(LED_4);
	PM_OffPin(LED_5);
	PM_OffPin(LED_6);
}

//==============================================================================
int main(void)
{
	InitAll();			// Инициализируем периферию
    InitRTOS();			// Инициализируем ядро
    RunRTOS();			// Старт ядра. 

    // Запуск фоновых задач.
	SetTask(TaskLedOff);
	//fprintf(stdout,"start\r\n");
	wdt_enable(WDTO_1S);
    while(1) 		// Главный цикл диспетчера
    {
        wdt_reset();	// Сброс собачьего таймера
        TaskManager();	// Вызов диспетчера
    }
    return 0;
}
