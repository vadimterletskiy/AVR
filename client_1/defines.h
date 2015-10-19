/*
 * defines.h
 *
 * Created: 10/16/2015 4:47:10 PM
 *  Author: vaterlet
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#define F_CPU 11059200L
#define UART_BAUD_RATE 115200

#include <avr/io.h>
#include "EERTOS.h"
#include <avr/wdt.h>
#include "tasks.h"
#include "init.h"
#include "UART/uart.h"
void initAll(void);

#endif /* DEFINES_H_ */