/*
 * def.h
 *
 * Created: 10/16/2015 4:32:10 PM
 *  Author: vaterlet
 */ 


#ifndef DEF_H_
#define DEF_H_

#define F_CPU 11059200L
//#define UART_BAUD_RATE 115200
//#define UART_BAUD UART_BAUD_RATE
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/common.h>


#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)

//System Timer Config
#define Prescaler	  		64
#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS


#endif /* DEF_H_ */