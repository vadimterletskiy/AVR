#ifndef EELOGGER_H
volatile uint16_t offset;

#define DIV_100 475

//#define  CURRENT_FROM_ADC(adc) (((int32_t)adc)*2500/512)*300/25
//показание тока больше чем нужно на (нужно умножить на 0,834) 48,8671875

#define  CURRENT_FROM_ADC(adc) ((int32_t)adc)*6255/128

#define  VOLT_FROM_ADC(adc)    ((int32_t)adc)*50*DIV_100/1024


void EELogger_writeData(uint16_t volt, uint16_t current);
void EraseEEPROM();
#endif

