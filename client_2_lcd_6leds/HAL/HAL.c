#include "HAL.h"
FILE uart_str = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);
FILE lcd_str = FDEV_SETUP_STREAM(i2c_lcd_putch, NULL, _FDEV_SETUP_RW);
volatile u16 adc_v[8];
volatile u08 ZU_STATUS;

inline void InitAll(void)
{	
	//SetBit(ZU_STATUS, PWM_ENABLE);
	//SetBit(ZU_STATUS, CHARGE);
	//lcd
	//PORTC = 0x3E;
	//PORTC = 0x00;
	//DDRC  = 0x01;
	//i2c_init();
	//i2c_lcd_init();
	//i2c_lcd_clr();
	//i2c_lcd_gotoxy(1,1);
	//i2c_lcd_puts("ZU");
	
	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	
	//stdout = &uart_str;
	//stdin = &uart_str;
	//stderr = &lcd_str; 

	//настраиваем вывод со светодиодом на выход
	PM_DirPin(LED_1, _OUT);
	PM_DirPin(LED_2, _OUT);	
	PM_DirPin(LED_3, _OUT);
	PM_DirPin(LED_4, _OUT);
	PM_DirPin(LED_5, _OUT);
	PM_DirPin(LED_6, _OUT);
	//PM_OffPin(LED_RED);
	
	//PM_DirPin(UART_TXD, _OUT);
	//PM_DirPin(UART_RXD, _IN);
	//DDRD |= _BV(PORTD1);
	//PORTD |= _BV(PORTD1);
	
	//charge/discharge charge:0  discharge:1
	//PM_DirPin(SWITCH_PIN, _OUT);

	//PM_DirPin(ADC_12V, _IN);
	//PM_DirPin(ADC_I, _IN);
	//PM_DirPin(ADC_RES_PWM, _IN);
	
	//TCCR1A = 0; // Отключаем PWM пока будем конфигурировать
    //ICR1 = ICR_MAX; // Частота всегда 50 Гц
	// Включаем Fast PWM mode via ICR1 на Таймере 1
    //TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1A1) | (1<<COM1B1);;
	//TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10);// CLK/8
	
	//PM_DirPin(PWM_FAN, _OUT);
	//DDRB |= _BV(PORTB2);
	
	//POWER_PIN
	//DDRB |= _BV(PORTB3);

	//todo define for adc3
	//adc_init(ADC_INT_DIS, 200, ADC_PIN_RES_PWM /*| ADC_PIN_12V*/);	
}



