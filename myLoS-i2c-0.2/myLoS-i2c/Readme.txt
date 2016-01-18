                  I2C LCD Demo Firmware For Use With LCDproc

-- INTRODUCTION --------------------------------------------------------------

This firmware package demonstrates how to drive a LCD connected to I2C (aka
TWI) bus using a PCF8574 remote 8-bit I/O expander.

It uses the following libraries and code:

* Peter Fleury's UART and LCD libraries
  (http://jump.to/fleury)
* Peter Dannegger's rotary encoder solution
  (http://www.mikrocontroller.net/articles/Drehgeber) (German only)
* Atmel appnote AVR155 for the TWI master driver.

-- HARDWARE ------------------------------------------------------------------

The software is designed to work with an ATmega8 used on the myAVR Board light
(shop.myavr.com) running at 3.6864 MHz.

Running the ATmega8 at a different speed should work as well. Edit the
Makefile and adjust -DF_CPU=3686400UL to match your crystal's speed.

Using an ATmega48/88/168/328 should work, too, as they are pin and feature
compatible. Adjust the Makefile accordingly. Running any other AVR will
likely require editing of the timer code in rotary.c.

The display should be connected like this:

PCF8574 | HD44780
     P0 = D4
     P1 = D5
     P2 = D6
     P3 = D7
     P4 = RS
     P5 = RW
     P6 = EN
     P7 = Backlight switch (using BC327)

This wiring is used e.g. by the Pollin LCD-I2C Modul (order no. 810 145)
available from http://www.pollin.de.

The rotary encoder code is designed to work with an Panasonic EVEQ encoder
(available from Pollin, too). Connect the rotary encoder like this (the
common pins of the encoder are to be connected to ground):

Key     = PC0
Phase A = PC1
Phase B = PC2

You may change the ports and pins used by editing rotary.c. Depending on the
encoder used one of the other 'encode_readX' functions may have to be used.

-- COMPILING -----------------------------------------------------------------

Just change into the default/ sub-directory and run 'make' or 'gmake' (GNU
make is required).

-- USE WITH LCDPROC ----------------------------------------------------------

A sample configuration file (extras/LCDd.myLoS.conf) is provided that shows
the key configurations necessary. Edit this file to match your serial device!

A patch for LCDproc (extras/patch-menu-rotary.diff) is required to add a
'back' entry to each menu. Otherwise you can enter any menu, but never leave
it!

Applying the second patch (extras/patch-hd44780-serial.diff) adds escape
character suppport to the hd44780/serial connection type.

-- LICENSE -------------------------------------------------------------------

This software uses existing code with several different licenses:

myLoS.c       GPLv2 or later
uart.c        GPLv2 or later
lcd.c         Unknown
rotary.c      CC-BY-SA
twi_master.c  Unkown

# EOF #
