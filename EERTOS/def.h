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

//PORT Defines
#define ADC_U_CHARGE 2
#define ADC_I_CHARGE 0
#define ADC_I_DISCHARGE 6 //todo
#define ADC_RESISTOR		2 //usbadc slow jumper

//#define LED_PORT PORTC
//#define LED_DDR  DDRC
//#define LED_RED  0

/*#define	LED3		7*/

#define VD_BLINK InvBit(LED_PORT,LED_RED);_delay_ms(50);InvBit(LED_PORT,LED_RED)


#endif /* DEF_H_ */