
#define F_CPU 4800000UL  // Define software reference clock for delay duration

#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdint-gcc.h>
#include "port_macros.h"

#define SDA_release   DDRB &= 0xEF
#define SDA_low     DDRB |= 0x10
#define SCL_release   DDRB &= 0xF7
#define SCL_low     DDRB |= 0x08

#define FREQ_STEPS 10
#define RADIO_REG_CHIPID  0x00
#define RADIO_REG_CTRL    0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO   0x2000
#define RADIO_REG_CTRL_BASS   0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK   0x0100
#define RADIO_REG_CTRL_RDS    0x0008
#define RADIO_REG_CTRL_NEW    0x0004
#define RADIO_REG_CTRL_RESET  0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define RADIO_REG_CHAN    0x03
#define RADIO_REG_CHAN_SPACE     0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND      0x000C
#define RADIO_REG_CHAN_BAND_FM      0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE   0x0010
#define RADIO_REG_CHAN_NR     0x7FC0
#define RADIO_REG_R4    0x04
#define RADIO_REG_R4_EM50   0x0800
#define RADIO_REG_R4_SOFTMUTE   0x0200
#define RADIO_REG_R4_AFC   0x0100
#define RADIO_REG_VOL     0x05
#define RADIO_REG_VOL_VOL   0x000F

#define RADIO_REG_RA      0x0A
#define RADIO_REG_RA_RDS       0x8000
#define RADIO_REG_RA_RDSBLOCK  0x0800
#define RADIO_REG_RA_STEREO    0x0400
#define RADIO_REG_RA_NR        0x03FF

#define RADIO_REG_RB          0x0B
#define RADIO_REG_RB_FMTRUE   0x0100
#define RADIO_REG_RB_FMREADY  0x0080


#define RADIO_REG_RDSA   0x0C
#define RADIO_REG_RDSB   0x0D
#define RADIO_REG_RDSC   0x0E
#define RADIO_REG_RDSD   0x0F

// I2C-Address RDA Chip for sequential  Access
#define I2C_SEQ  0x20

// I2C-Address RDA Chip for Index  Access
#define I2C_INDX  0x22


#define   RADIO_BAND_NONE     0x0
#define   RADIO_BAND_FM       0x1
#define   RADIO_BAND_FMWORLD  0x2

void _saveRegisters();
void setVol();
void setBassBoost();
void setMono();
void setFrequency();
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
void setFrequency();
void _saveRegister(short regNr);
void i2c_start(char data);
void _write16(uint16_t val);
void i2c_stop(void);
void i2c_write(char data);
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);


// ----- implement
uint16_t registers[10]={
	0x5804,
	0x0000,
	0x0001,
	0x0000,
	0x0800,
	0x9089, //  последняя цифра громкость  1 - F
	0x0000,
	0x0000,
	0x0000,
	0x0000
};
uint8_t fm, vol;
uint8_t bits = 0;
#define BTNUP 0
#define BTNDN 1
#define BTNSK 2
#define BASS 4
#define MONO 8

#define set(n) SetBit(bits, n)
#define clear(n) ClearBit(bits, n)
#define IsClear(n) BitIsClear(bits, n)
#define IsSet(n) BitIsSet(bits, n)
#define inv(n) InvBit(bits, n)



//#define BTNUP BitIsSet(bits, 0)
//#define BTNDN BitIsSet(bits, 1)
//#define BTNSK BitIsSet(bits, 2)


//  Частоты радиостанций в вашем регионе
const uint16_t preset[] PROGMEM= {
	9080,
	10100,
	10140,
	10180, // hr1
	10220,
	10290, // Bayern2
	10340, // ???
	10390, // * hr3
	10430,
	10490,
	10670,
	10720
};

int main( void )
{  // всё что до while(1){ выполняется один раз при старте
	//-----setup--------
	DDRB = 0x00;    //PORTB = Inputs
	PORTB = 0x87;   //Set Outputs to Low (when DDRB is set to low the pin associated goes Hi-Z)
	//fm=EEPROM_read(0);
	//vol=EEPROM_read(1);
	//BASS=EEPROM_read(2);
	//MONO=EEPROM_read(3);
	_delay_ms(1000);
	_saveRegisters();
	setVol();
	setBassBoost();
	setMono();
	setFrequency();
	while(1){ // вечный цыкл, тоже самое что и void loop(){
		//----loop--------
		if (PINB & (1<<PINB0)) clear(BTNUP);
		if (PINB & (1<<PINB1)) clear(BTNDN);
		if (PINB & (1<<PINB2)) clear(BTNSK);
		
		if (IsClear(BTNUP) & IsClear(BTNDN) & IsClear(BTNSK))
		{
			_delay_ms(5);
			if (!(PINB & (1<<PINB2)))
			{
				set(BTNSK);
				fm ++;
				if (fm>11) fm=0;  //   11 Количество радиостанций  в таблице
				setFrequency();
				EEPROM_write(0, fm);
			}
			if (!(PINB & (1<<PINB1)))
			{
				set(BTNDN);
				vol --;
				if (vol<1) vol=1;
				setVol();

			}
			if (!(PINB & (1<<PINB0)))
			{
				set(BTNUP);
				vol ++;
				if (vol>15) vol=15;
				setVol();

			}
			} else {
			_delay_ms(5);
			if (!(PINB & (1<<PINB2))) { if (BTNUP) {inv(BASS); setBassBoost();  clear(BTNUP);set(BTNSK);}
			if (BTNDN) {inv(MONO);setMono(); clear(BTNDN);set(BTNSK);}
		}
	}
} // конец вечного цикла
return 0;
}

void setVol(void)
{
	registers[RADIO_REG_VOL] = (0x9080 | vol);
	_saveRegister(RADIO_REG_VOL);
	EEPROM_write(1, vol);
} // setBassBoost()


void setBassBoost()
{
	if (BASS)
	registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_BASS;
	else
	registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_BASS);
	_saveRegister(RADIO_REG_CTRL);
	EEPROM_write(2, BASS);
} // setBassBoost()


// Mono / Stereo
void setMono(void)
{
	registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
	if (!MONO) {
		registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_MONO;
	}
	else {
		registers[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO;
	}
	_saveRegister(RADIO_REG_CTRL);
	EEPROM_write(3, MONO);

} // setMono


void setFrequency(void) {
	uint16_t newChannel;
	uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);

	newChannel = (pgm_read_word_near(preset + fm) - 8700) / 10;
	regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
	regChannel |= newChannel << 6;
	
	// enable output and unmute
	registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_ENABLE; //  | RADIO_REG_CTRL_NEW
	_saveRegister(RADIO_REG_CTRL);

	registers[RADIO_REG_CHAN] = regChannel;
	_saveRegister(RADIO_REG_CHAN);

	// adjust Volume
	//  _saveRegister(RADIO_REG_VOL);
} // setFrequency()


// Save one register back to the chip
void _saveRegister(short regNr)
{
	i2c_start(I2C_INDX);
	i2c_write(regNr);
	_write16(registers[regNr]);
	i2c_stop();
} // _saveRegister

void _saveRegisters()
{
	i2c_start(I2C_SEQ);
	for (int i = 2; i <= 6; i++)
	_write16(registers[i]);
	i2c_stop();
} // _saveRegisters

void _write16(uint16_t val)
{
	i2c_write(val >> 8);
	i2c_write(val & 0xFF);
} // _write16

void i2c_clk(void)
{
	SCL_release;    //Release Clock
	_delay_us(5);
	SCL_low;      //Pull Clock Low
}
/**************************************************************/
void i2c_write(char data)
{
	int i;

	for (i=0;i<=7;i++)            //Clocks out byte MSB first
	{
		if (data & 0x80)  {SDA_release;}  //Release Data
		else        {SDA_low;};   //Pull Data Low
		data = data * 2;
		i2c_clk();
	}

	// _delay_us(5);

	SDA_release;              //Release Data to check for ACK

	if (PINB & 0x08){   } //slave didn't acknowledge (data held low by slave)
	else      {   } //slave did acknowledge (data not held low by slave)
	i2c_clk();
	SDA_low;                //Pull Data Low
}
/**************************************************************/
void i2c_start(char data)
{ SDA_low;    //Pull Data Low
	SCL_low;    //Pull Clock Low
	i2c_write(data);
}
/**************************************************************/
void i2c_stop(void)
{
	SCL_release;  //Release Clock
	SDA_release ; //Release Data
}
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	//while(EECR & (1<<EEWE)); /*Ждать завершения предыдущей записи*/
	while(EECR & (1<<EEPE));
	EEAR = uiAddress; /*Проинициализировать регистры*/
	EEDR = ucData;
	/* Start eeprom read by writing EERE */
	//EECR |= (1<<EEMW); /*Установить флаг EEMWE*/
	EECR |= (1<<EEPE); /*Начать запись в EEPROM*/
}
unsigned char EEPROM_read(unsigned int uiAddress)
{
	while(EECR & (1<<EEPE)); /*Ждать завершения предыдущей записи*/
	EEAR = uiAddress; /* Проинициализировать регистр адреса*/
	EECR |= (1<<EERE); /*Выполнить чтение*/
	return EEDR;
}