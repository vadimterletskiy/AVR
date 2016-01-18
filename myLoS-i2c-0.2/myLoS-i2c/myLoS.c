/*
 * myLoS.c (I2C version)
 *
 * Copyright 2012 Markus Dolze <bsdfan@nurfuerspam.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "uart.h"
#include "rotary.h"

#define UART_BAUD_RATE      9600

extern volatile uint8_t key_press;

/* Read states */
enum states {
    ST_DATA,
    ST_CMD,
    ST_BL,
    ST_ESC
};

static void
pwm_set(uint8_t pwm)
{
    OCR1A = pwm;
}

int main(void)
{
    int8_t enc = 0;
    unsigned int c;
    uint8_t state = ST_DATA;

    /* start-up */
    lcd_init();
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    encode_init();
    pwm_set(0xFF);

    sei();

    lcd_clrscr();
    lcd_puts_P("I2C demo 0.1");

    /* loop forever */
    for (;;) {
        c = uart_getc();

        if (!(c & 0xFF00)) {
            switch (state) {
                case ST_DATA:
                    if (c == 0x00FE)
                        state = ST_CMD;
                    else if (c == 0x00FD)
                        state = ST_BL;
                    else if (c == 0x00FC)
                        state = ST_ESC;
                    else
                        lcd_data(c);
                    break;
                case ST_CMD:
                    if (c == 0x01)
                        lcd_clrscr();
                    else
                        lcd_command(c);
                    state = ST_DATA;
                    break;
                case ST_BL:
                    pwm_set(c);
                    state = ST_DATA;
                    break;
                default:
                    lcd_data(c);
                    state = ST_DATA;
            }
        }

        enc = encode_read2();   /* Adopt this to your encoder type */
        if (enc > 0) {
            uart_putc(0xFE);
            uart_putc(0x01);
        }
        else if (enc < 0) {
            uart_putc(0xFE);
            uart_putc(0x02);
        }

        if (key_press > 0) {
            uart_putc(0xFE);
            uart_putc(0x04);
            key_press = 0;
        }
    } /* end for() */
}
