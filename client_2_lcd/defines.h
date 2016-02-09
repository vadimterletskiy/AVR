#ifndef defines_h
#define defines_h

//Clock Config
#define F_CPU 12000000L

/* UART baud rate */
#define UART_BAUD 57600L

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrlibtypes.h"
#include "uart/uart.h"
#include "util/delay.h"
#include "atmega-adc.h"
//#include "lcd/lcd.h"

#include "avrlibdefs.h"
#include "avr/pgmspace.h"
#include <avr/wdt.h>



#define   SetBit(reg, bit)          reg |= (1<<bit)
#define   ClearBit(reg, bit)       reg &= (~(1<<bit))
#define   InvBit(reg, bit)          reg ^= (1<<bit)
#define   BitIsSet(reg, bit)       ((reg & (1<<bit)) != 0)
#define   BitIsClear(reg, bit)    ((reg & (1<<bit)) == 0)





#define MAX_BUFFER 64

//System Timer Config
#define Prescaler	  		64
#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS
	

#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)


//PORT Defines
#define ADC_U_CHARGE 2
#define ADC_I_CHARGE 0
#define ADC_I_DISCHARGE 6 //todo
#define ADC_RESISTOR		2 //usbadc slow jumper


#define LED_RED 		0

/*#define	LED3		7*/

#define LED_PORT 	PORTC
#define LED_DDR		DDRC   

#define VD_BLINK InvBit(LED_PORT,LED_RED);_delay_ms(50);InvBit(LED_PORT,LED_RED)

extern void InitAll(void); 

#endif
