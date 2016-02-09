/*
 * adc.h
 * Created: 20.06.2013 22:56:15
 *  Author: wadim
 */ 

#ifndef ADC_H_1
#define ADC_H_1

#define ADC_VREF_AREF 	0
#define ADC_VREF_AVCC 	64
#define ADC_VREF_MISC1 	128
#define ADC_VREF_MISC2 	192

#define ADC_VREF_TYPE ADC_VREF_AVCC

//Чтение канала АЦП:
uint16_t adcRead1(uint8_t ch) {
    ADMUX = _BV(REFS0) | (ch & 0x1F);    // set channel (VREF = VCC)
	_delay_us(10);
    ADCSRA &= ~_BV(ADIF);            // clear hardware "conversion complete" flag
    //ADCSRA |= _BV(ADSC);            // start conversion
	ADCSRA = _BV(ADSC) | 7;
 
    while(ADCSRA & _BV(ADSC));        // wait until conversion complete
 
    return ADC;                // read ADC (full 10 bits);
}

#endif /* ADC_H_1 */