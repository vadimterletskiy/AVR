#ifndef HAL_H
#define HAL_H

//Clock Config
//#define F_CPU 11059200L project
#define UART_BAUD_RATE 115200
#define UART_BAUD UART_BAUD_RATE 
#define LCD_SUPPORT

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

extern volatile u16 adc_v[8];

enum {
	NEED_TURN_ON = 1,
	NEED_TURN_OFF = 2,
	IS_ON = 4
};

enum actions{
	AT_PING = 1<<1,
	AT_CALL = 1<<2,
	AT_PHONE_IS_ON = 1<<3,
};
enum error{
	ER_OK = 1,
	ER_NO_DIALTONE = 0,
	ER_BUSY = 2,
	ER_ERROR = 3
};

// macro for easier usage
#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define update_eeprom_word(address,value) eeprom_update_word ((uint16_t*)address,(uint16_t)value)


#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)


//eeprom config
#define EE_OCR 0
#define EE_C_ON_OFF 1
#define PWM_ON 1
#define PWM_OFF 0

//System Timer Config
#define Prescaler	  		64
#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS

#define ICR_MAX F_CPU/1/20000  // ICR1(TOP) = fclk/N/f ; N-Делитель; f-Частота;  8000000/8/50 = 20000
	
//описываем выводы
#define LED_NEW D, 3, _LOW

//#define LED_RED C, 0, _LOW	//adc
//#define LED_GREAN C, 0, _LOW	//adc

#define ADC_PIN_RED 0
#define ADC_RED C, ADC_PIN_RED, _HI

#define ADC_PIN_12V 2
#define ADC_12V C, ADC_PIN_12V, _HI

#define ADC_PIN_RES_PWM 1
#define ADC_RES_PWM C, ADC_PIN_RES_PWM, _HI
//pwm
//#define ADC_PIN_GREEN 1
//#define ADC_GREEN C, ADC_PIN_GREEN, _HI

#define UART_TXD D, 1, _HI
#define UART_RXD D, 0, _HI

#define ALARM_STATUS B, 0, _HI
#define ALARM_PAGER B, 1, _HI

#define ALARM_IN_GUARD PM_PinIsClear(ALARM_STATUS) && PM_PinIsClear(ALARM_PAGER)
#define ALARM_IN_CALLING PM_PinIsClear(ALARM_STATUS) && PM_PinIsSet(ALARM_PAGER)

#define POWER_PIN B, 3, _HI
#define POWER_PUSH_DOWN PM_OnPin(POWER_PIN)
#define POWER_PUSH_UP	PM_OffPin(POWER_PIN)

//#define BINAR C, 0, _LOW //for test
#define BINAR B, 5, _HI


#define PWM_PIN_FAN 2
#define PWM_FAN B, PWM_PIN_FAN, _HI

#define SIDE_LIGHTS B, 4, _HI //for test use LED_RED

//#define R1_DIV 200
//#define R2_DIV 100
//#define U_OP 500
//#define CONST_U_1100mV 1024/U_OP*1100/(R1_DIV + R2_DIV)
//#define CONST_U_1300mV 1024/U_OP*1300/(R1_DIV + R2_DIV)
//#define CONST_U_1320mV 1024/U_OP*1320/(R1_DIV + R2_DIV)
//#define CONST_U_1000mV 1024/U_OP*1000/(R1_DIV + R2_DIV)
//#define CONST_U_mV_div10(mV) 1024/U_OP*mV/(R1_DIV + R2_DIV)

//#define CONST_U_mV_div10(mV) 1024/U_OP*mV/(R1_DIV + R2_DIV)
#define R1 200
#define R2 100
#define K_R (R1 + R2)/R2
#define CONST_U_mV(mV, ADC_bg) (mV/100*ADC_bg)/(13*K_R)

//#define DIV_100 10*(R1_DIV + R2_DIV)
//#define ADC_TO_VOLT(adc)    ((int32_t)adc)*50*DIV_100/1024
#define ADC_TO_VOLT2(ADCbg, ADCizm)		((int32_t)1300*ADCizm*K_R/ADCbg)
#define ADC_TO_VOLT_I(ADCbg, ADCizm)		(2500*12 - (int32_t)1300*12*ADCizm/ADCbg)




#define NeedTurnOn(status) BitIsSet(status,NEED_TURN_ON)
#define NeedTurnOff(status) BitIsSet(status,NEED_TURN_OFF)

#define NeedTurnOff_false(status) ClearBit(status, NEED_TURN_OFF)
#define NeedTurnOff_true(status)  SetBit(status, NEED_TURN_OFF) ;
#define NeedTurnOn_false(status) ClearBit(status, NEED_TURN_ON)
#define NeedTurnOn_true(status) SetBit(status, NEED_TURN_ON)

#define LightIsOn(status) BitIsSet(status, IS_ON)
#define LightIsOff(status) BitIsClear(status, IS_ON)

#define TurnOnLight(status)  PM_OnPin(SIDE_LIGHTS); SetBit(status, IS_ON)
#define TurnOffLight(status) PM_OffPin(SIDE_LIGHTS); ClearBit(status, IS_ON)

//#define TIME_STEP 200
//#define TIME_LIMIT_1S 1000/TIME_STEP
//#define TIME_ON_COUNTER	  1
//#define TIME_OFF_COUNTER -1

#endif
