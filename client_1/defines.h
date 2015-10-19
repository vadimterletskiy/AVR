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
#include "port_macros.h"
#include "EERTOS.h"
#include <avr/wdt.h>
#include "tasks.h"
#include "init.h"
#include "UART/uart.h"
#include "adc/atmega-adc.h"

volatile uint16_t adc_v[8];
volatile int16_t LightsCounter;
volatile int16_t callTime;
volatile int8_t status;

enum ST{
	ST_ALARM = 1<<1,
	ST_ALARM1 = 1<<2,
	ST_ALARM2 = 1<<3
};

#define   SetBit(reg, bit)          reg |= (1<<bit)
#define   ClearBit(reg, bit)       reg &= (~(1<<bit))
#define   InvBit(reg, bit)          reg ^= (1<<bit)
#define   BitIsSet(reg, bit)       ((reg & (1<<bit)) != 0)
#define   BitIsClear(reg, bit)    ((reg & (1<<bit)) == 0)

#define ICR_MAX F_CPU/1/20000  // ICR1(TOP) = fclk/N/f ; N-????????; f-???????;  8000000/8/50 = 20000

#define ADC_PIN_12V 2
#define ADC_12V C, ADC_PIN_12V, _HI

#define ADC_PIN_RES_PWM 1
#define ADC_RES_PWM C, ADC_PIN_RES_PWM, _HI

#define UART_TXD D, 1, _HI
#define UART_RXD D, 0, _HI

#define LED_RED C, 0, _HI
#define LED_GREEN C, 1, _HI

#define PWM_FAN B, 2, _LOW
#define POWER_PIN B, 3, _LOW

#define ALARM_STATUS B, 0, _HI
#define ALARM_PAGER B, 1, _HI

#define R1_DIV 2
#define R2_DIV 1
#define U_OP 500
#define U_mV(mV) 1024/U_OP*mV/(R1_DIV + R2_DIV)

#define ON 1
#define OFF 0

#define SIDE_LIGHTS B, 4, _HI //for test use LED_RED

#define SwithLightsOn PM_OnPin(SIDE_LIGHTS)
#define SwithLightsOff PM_OffPin(SIDE_LIGHTS)
#define SwithLights(mode) if (mode == ON) SwithLightsOn ; else SwithLightsOff

void initAll(void);

#endif /* DEFINES_H_ */