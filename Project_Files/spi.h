/***********************************************************************//**
 * @file		SPI.h
 * @brief		Contains all macro definitions and function prototypes
 * 				support for SPI on LPC1768
 * @version		0.1
 * @date		14. January. 2019
 * @author		Idris Al-Ghabra
 **************************************************************************/
#ifndef SPI_H_
#define SPI_H_

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "SerialIO.h"
#include "Delay.h"
/*
What is it?:
  SPI is a serial interface used to communicate between master and slave devices
Pins:
  0.15 = SCK
  0.16 = SSEL
  0.17 = MISO
  0.18 = MOSI
Registers (these are all 32bits):
  SPCR - For Configurations
  SPSR - Status Register
  SPDR - Data Register for I/O
  SPCCR - Clock Counter register- controls the master Clock Frequency
*/

extern void SSP0_IRQHandler();//Note: extern references the declared instance of irqhandler in lpc17xx
extern void SSP1_IRQHandler();
extern void Init_SSP0();
extern void Init_SSP1();
void SSP1_Write(uint16_t *buf, uint32_t Length);
void SSP0_Write(uint16_t *buf, uint32_t Length);
void SSP1_Read(uint16_t *buf, uint32_t Length);
void SSP0_Read(uint16_t *buf, uint32_t Length);
#endif


//BitsPerTransfer %= 16; // -> only relevant to legacy SPI, where 16 bit = 0000

//BitsPerTransfer = ((BitsPerTransfer)<8?8:BitsPerTransfer);
/*
https://sites.google.com/site/johnkneenmicrocontrollers/spi/ssp-spi-_lpc1768  <<<< LOOK AT THIS
https://github.com/luka1995/LPC17xx-Libraries/blob/master/Libraries/SSP.c
The LPC1768 contains 1 SPI bus, which is referenced here, and 2 SSP controllers. 1 is accessed through pins 5678, and the
other is connected to the SD Card. This SSP bus is capable of SPI, SSI or Microwire.

Pins 15,16,17 and 18 (11,12,13,14)  in function 3 are configured to form SSP0 SPI
Pins 9-6 (MBED Pins 5-8) in function 3 are configured to form SSP1 SPI

http://nanohome.be/nxp/LPC1768_Pins.php - A table of pins
https://github.com/ExploreEmbedded/Explore-Cortex-M3-LPC1768-DVB-14001/tree/master/Code/KeilExamples/00-libfiles
https://sites.google.com/site/johnkneenmicrocontrollers/spi/ssp-spi-_lpc1768
Direct GPIO for spi:
How this works:
Each Pinsel is a 32 bit register, containing the pinsel_func for each of the 16 pins it references.
E.g. 00-00-00-00-00-00-11-00-00-00-00-00-00-00-00-00
Pins:15-14-13-12-11-10-09-08-07-06-05-04-03-02-01-00
as the bits for 9 are set to 11, then pinselfunc3 is configured for 9.
Pinsel0 handles pins 0-15
Pinsel1 handles pins 16-31
and so on ...
To set a b#define FIFOSIZE		8
it, or the relevant Pinsel with (PINSEL_FUNC)<<(PIN*2-(PINSEL*16))

FIODIR a 32bit register storing  also works as follows.
There are  4 GPIO registers for each of the different pins sections e.g. 0.14 = gpio0,1.12 = gpio2, 2.4 = gpio3
Where the register stores the direction for the relevant pin. It can be set by ORing it with: DIR<<PIN

For this example,
SCK = 15, SSEL = 16, MISO = 17, MOSI = 18

NOTE: For some reason trying to access PINSEL 0 results in a crash every time -> Need to fix

The Following is a list of all pins that can be used for SSP:
    MOSI --- MISO --- SCK --- SSEL
SSP0 0.18      0.17     0.15    0.16 -> FUNC3 11,12,13,14 MBED Pins
   1.24      1.23     1.20    1.21 -> FUNC4 38,37,34,35
SSP1 0.9       0.8      0.7     0.6  -> FUNC3 5,6,7,8 MBED Pins


About the SD Card:
The SD card is connected to SSP1 across pins 2375
*/

/*
SENDING:
LPC_SSP0->DR is used for receiving and sending data each frame. This is a 32 bit register, but only the first 16 bits are accessible.
When you can read and write depends on the content of the status register, LPC_SSP0->SR.
When TNF = 1, the Buffer is not full and data can be sent for transmission. bit 1
Whenever RNE = 1Data Can be read, as data has been transferred. bit 2

The best way to do this is on interrupt for TNF/RNE, write to the register.
*/

/*
LPC_SSP0->DR = Data;
while (!(LPC_SSP0->SR&2));

*/
/*
void SPI_Write(uint16_t Data)
{ //LPC_SSP0->DR = Data;
  LPC_SPI->SPDR = Data;
  while((LPC_SPI->SPSR&0x2)==0); //Well, of course this is looping infinitely, it being not connected to anything
  CLR_Flags();
}

uint16_t SPI_Read()
{
  LPC_SPI->SPDR = 0xff;
  while((LPC_SPI->SPSR&0x40)==0);
  LPC_SPI->SPSR;
  return (uint8_t)LPC_SPI->SPDR;
}
*/
/*void CLR_Flags()
{
  uint8_t temp = LPC_SPI->SPSR;
  temp = LPC_SPI->SPDR;
}*/
