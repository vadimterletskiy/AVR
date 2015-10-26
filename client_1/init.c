/*
 * init.c
 *
 * Created: 16.10.2015 21:30:06
 *  Author: wadim
 */ 
#include "defines.h"
#include "UART/uart.h"
#include "init.h"

<<<<<<< HEAD
volatile int16_t LightsCount = 0;

void initAll(){
	
	//uart 
	LightsCounter = 0;
	status = 0;
	PM_DirPin(UART_TXD, _OUT);
	PM_DirPin(UART_RXD, _IN);
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	
	//
	PM_DirPin(LED_RED, _OUT);
	PM_OffPin(LED_RED);
	
	PM_DirPin(LED_GREEN, _OUT);
	PM_OffPin(LED_GREEN);
	
	PM_DirPin(PWM_FAN, _OUT);
	
	//pwm
	TCCR1A = 0; // ????????? PWM ???? ????? ???????????????
	ICR1 = ICR_MAX; // ??????? ?????? 50 ??
	// ???????? Fast PWM mode via ICR1 ?? ??????? 1
	TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1A1) | (1<<COM1B1);;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10);// CLK/8
	
=======
void initAll(){
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
>>>>>>> 51ade5dbc4972b61d92b53420a53a74ae90e0bd0
	return;
}