/*
 * EELogger.c
 *
 * Created: 05.01.2016 17:32:07
 *  Author: wadim
 */ 
#include "HAL.h"

void EELogger_writeData(uint16_t volt, uint16_t current)
{
	if (offset == 0)
	{
		offset = eeprom_read_word(0);
		
		if (offset == 0xffff || offset == 0x0000) {
			offset = 2;
		}
	}
	volt = volt - UINT16_MAX/2;
	volt = (volt >> 1) & 0x3FF;
	uint16_t vi = (volt << 8) & 0xFF00;
		
	if (current > 512)
	{
		current = current - 512;
	} else
	{
		current = 512 - current;
	}
	uint8_t c = current;
	vi = vi | c;
	eeprom_write_word(offset,vi);
	offset += 2;
	if (offset >= 512){
		offset = 2;
	}
	eeprom_write_word(0,offset);
}

#define EEPROM_END 512
void EraseEEPROM()
{
	uint8_t sreg, i;
	uint16_t addr;
	uint8_t clear[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t data[8];

	//sreg = SREG;
	cli();

	// Write page by page using the block writing method
	for(addr = 0; addr < EEPROM_END; addr += 8)
	{
		eeprom_read_block((void *)&data[0], (const void *)addr, 8);
		for(i = 0; i < 8; i++)
		if(data[i] != 0xFF)
		{
			eeprom_write_block((void*)&clear[0], (void*)addr, 8);
			break;
		}
	}	
	offset = 2;
	eeprom_write_word(0,offset);
	//SREG = sreg;
}