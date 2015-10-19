/*
 * config.c
 *
 * Created: 16.10.2015 21:08:27
 *  Author: wadim
 */ 

#include "config.h"
#include "avr/io.h"
#include "../libs/UART/uart.h"

void initAll(void)
{
	uart_init(1);	
	return;
}