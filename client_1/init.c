/*
 * init.c
 *
 * Created: 16.10.2015 21:30:06
 *  Author: wadim
 */ 
#include "defines.h"
#include "UART/uart.h"
#include "init.h"

void initAll(){
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	return;
}