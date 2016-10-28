#ifndef HAL_H
#define HAL_H

//Clock Config
//#define F_CPU 11059200L project
#define UART_BAUD_RATE 115200
#define UART_BAUD UART_BAUD_RATE 
//#define LCD_SUPPORT

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/eeprom.h>
//#include <string.h>

#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "port_macros.h"
#include "TaskLamp.h"

//#include "lcd/lcd.h"
#include "../UART/uart.h"
#include "../adc/atmega-adc.h"
//#include "adc/adc.h"


#ifdef LCD_SUPPORT
#include "../lcd/i2c_master.h"
#include "../lcd/i2c_lcd.h"
#endif

extern void InitAll(void);
extern void uartHendler(unsigned char c);


#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)


//System Timer Config
#define Prescaler	  		64
#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS

#define ICR_MAX F_CPU/1/20000  // ICR1(TOP) = fclk/N/f ; N-Делитель; f-Частота;  8000000/8/50 = 20000


//#define UART_TXD D, 1, _HI
//#define UART_RXD D, 0, _HI

#define PWM_PIN_FAN 2
#define PWM_FAN B, PWM_PIN_FAN, _LOW

#define PIN_GREEN 1
#define LED_GREEN C, PIN_GREEN, _LOW

#define PIN_RED 0
#define LED_RED C, PIN_RED, _LOW

#define PIN_RED_DOT 1
#define LED_RED_DOT D, PIN_RED_DOT, _HI

#define BUTTON_PIN_PLUS 5
#define BUTTON_PLUS B, BUTTON_PIN_PLUS, _LOW

#define BUTTON_PIN_MINUS 4
#define BUTTON_MINUS B, BUTTON_PIN_MINUS, _LOW

#define BUTTON_PIN_ON_OFF 0
#define BUTTON_ON_OFF D, BUTTON_PIN_ON_OFF, _LOW

#define BUTTON_PIN_DPLUS 2
#define BUTTON_DPLUS D, BUTTON_PIN_DPLUS, _LOW

#define BUTTON_PIN_DMINUS 0
#define BUTTON_DMINUS B, BUTTON_PIN_DMINUS, _LOW

#endif
