#include "HAL.h"
FILE uart_str = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);
volatile u16 adc_v[8];
#ifdef LCD_SUPPORT
FILE lcd_str = FDEV_SETUP_STREAM(i2c_lcd_putch, NULL, _FDEV_SETUP_RW);
#endif

inline void InitAll(void)
{		 
	#ifdef LCD_SUPPORT
	i2c_init();
	i2c_lcd_init();
	_delay_ms(100);
	i2c_lcd_clr();
	_delay_ms(100);
	i2c_lcd_clr();
	i2c_lcd_backlight_on();
	i2c_lcd_gotoxy(1,1);
	i2c_lcd_puts("CAR ALARM");
	stderr = &lcd_str; 
	#endif
	
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uartSetRxHandler(uartHendler);
	stdout = &uart_str;
	stdin = &uart_str;	

	//����������� ����� �� ����������� �� �����
	//PM_DirPin(LED_RED, _OUT);
	//PM_OffPin(LED_RED);
	
	PM_DirPin(LED_NEW, _OUT);
	PM_OffPin(LED_NEW);	
	
	
	PM_DirPin(UART_TXD, _OUT);
	PM_DirPin(UART_RXD, _IN);

	PM_DirPin(ADC_12V, _IN);
	PM_DirPin(ADC_RED, _IN);
	//PM_DirPin(ADC_GREEN, _IN);
	
	PM_DirPin(ADC_RES_PWM, _IN);
	PM_DirPin(ADC_RED, _IN);
	
	PM_DirPin(ALARM_STATUS, _IN);
	PM_DirPin(ALARM_PAGER, _IN);	

	PM_DirPin(SIDE_LIGHTS, _OUT);
	
	TCCR1A = 0; // ��������� PWM ���� ����� ���������������
    ICR1 = ICR_MAX; // ������� ������ 50 ��
	// �������� Fast PWM mode via ICR1 �� ������� 1
    TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1A1) | (1<<COM1B1);;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10);// CLK/8
	 
	//PM_DirPin(PWM_FAN, _OUT);
	//DDRB |= _BV(PORTB2);
	
	//POWER_PIN
	DDRB |= _BV(PORTB3);

	//binar start
	//DDRB |= _BV(PORTB5);
	PM_DirPin(BINAR, _OUT);
	
	//todo define for adc3
	//adc_init(ADC_INT_DIS, 200, ADC_PIN_RES_PWM /*| ADC_PIN_12V*/);
}



