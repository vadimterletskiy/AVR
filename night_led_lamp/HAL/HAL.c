#include "HAL.h"
FILE uart_str = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);
volatile u16 adc_v[8];
volatile u08 tmp;
#ifdef LCD_SUPPORT
FILE lcd_str = FDEV_SETUP_STREAM(i2c_lcd_putch, NULL, _FDEV_SETUP_RW);
#endif

inline void InitAll(void)
	{
	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	//uartSetRxHandler(uartHendler);
	//stdout = &uart_str;
	//stdin = &uart_str;	

	//настраиваем вывод со светодиодом на выход

	//PM_DirPin(UART_TXD, _OUT);
	//PM_DirPin(UART_RXD, _IN);

	PM_SetPin(LED_GREEN);
	PM_SetPin(LED_RED);

	PM_DirPin(LED_GREEN, _OUT);
	PM_DirPin(LED_RED, _OUT);
	PM_DirPin(LED_RED_DOT, _OUT);
	
	PM_DirPin(PWM_FAN, _OUT);
	
	PM_DirPin(BUTTON_DPLUS, _IN);
	PM_DirPin(BUTTON_DMINUS, _IN);
	
	PM_DirPin(BUTTON_PLUS, _IN);
	PM_DirPin(BUTTON_MINUS, _IN);
	PM_DirPin(BUTTON_ON_OFF, _IN);
		
	TCCR1A = 0; // Отключаем PWM пока будем конфигурировать
    ICR1 = ICR_MAX; // Частота всегда 50 Гц
	// Включаем Fast PWM mode via ICR1 на Таймере 1
    TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1A1) | (1<<COM1B1);;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10); // CLK/8
	
	
	//DDRB &= ~(4<<0); // configure PB3 as an input
	//PORTB |= (4<<0); // enable the pull-up on PB3
	//DDRB &= ~(3<<0); // configure PB3 as an input
	//PORTB |= (3<<0); // enable the pull-up on PB3
	
	//DDRD &= ~(1<<0); // configure PB3 as an input
	//PORTD |= (1<<0); // enable the pull-up on PB3
	
	//PM_DirPin(PWM_FAN, _OUT);
		
	//POWER_PIN
	//DDRB |= _BV(PORTB3);

	//todo define for adc3
	//adc_init(ADC_INT_DIS, 200, ADC_PIN_RES_PWM /*| ADC_PIN_12V*/);
}



