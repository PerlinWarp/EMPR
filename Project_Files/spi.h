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
void CLR_Flags()
{
  uint8_t temp = LPC_SPI->SPSR;
  uint8_t temp = LPC_SPI->SPDR;
}
void InitSPI_Default()
{
  InitSPI(16,spi_GetPclk()/4000000,0);
}
void InitSPI(uint8_t BitsPerTransfer,uint8_t ClockFreq,uint8_t IntENA)
{
  BitsPerTransfer %= 16;
  //BitsPerTransfer = ((BitsPerTransfer)<8?8:BitsPerTransfer);
/*
  Direct GPIO for spi
*/
  GPIO_PinFunction(SCK_PIN,SPI_FUNCTION);
  GPIO_PinFunction(MOSI_PIN,SPI_FUNCTION);
  GPIO_PinFunction(MISO_PIN,SPI_FUNCTION);
  GPIO_PinFunction(SSEL_PIN,PINSEL_FUNC_0);

  GPIO_PinDirection(SCK_PIN,OUTPUT);        /* Configure SCK,MOSI,SSEl as Output and MISO as Input */
  GPIO_PinDirection(MOSI_PIN,OUTPUT);
  GPIO_PinDirection(MISO_PIN,INPUT);
  GPIO_PinDirection(SSEL_PIN,OUTPUT);

  SPI_DisableChipSelect();
  LPC_SC->PCONP |= (1 << 8);//turn on power for SPI
  LPC_SPI->SPCCR = ClockFreq;
  //     Reserved|BitsPerTransfer|MasterSel|IntE
  LPC_SPI->SPCR = (0)|(BitsPerTransfer<<7)|(3<<4)|(IntENA<<3);
  CLR_Flags();
}

uint8_t SPI_Write(uint8_t Data)
{
  LPC_SPI->SPDR = Data;
  while(util_GetBitStatus(LPC_SPI->SPSR,SBIT_SPIF)==0);
  CLR_Flags();
}

uint8_t SPI_Read()
{
  LPC_SPI->SPDR = 0xff;
  while(util_GetBitStatus(LPC_SPI->SPSR,SBIT_SPIF)==0);
  LPC_SPI->SPSR;
  return (uint8_t)LPC_SPI->SPDR;
}



#endif
