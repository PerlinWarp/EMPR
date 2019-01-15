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

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
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
  temp = LPC_SPI->SPDR;
}
void InitSPI_Default()
{
  InitSPI((uint8_t)16,(uint8_t)(1000000),(uint8_t)0);
}
void InitSPI(uint8_t BitsPerTransfer,uint8_t ClockFreq,uint8_t IntENA)
{
  BitsPerTransfer %= 16;
  //BitsPerTransfer = ((BitsPerTransfer)<8?8:BitsPerTransfer);
/*
  Direct GPIO for spi
*///unfuck this 
  GPIO_PinFunction(PINSEL_PIN_15,PINSEL_FUNC_3);
  GPIO_PinFunction(PINSEL_PIN_18,PINSEL_FUNC_3);
  GPIO_PinFunction(PINSEL_PIN_17,PINSEL_FUNC_3);
  GPIO_PinFunction(PINSEL_PIN_16,PINSEL_FUNC_0);

  GPIO_PinDirection(PINSEL_PIN_15,1);        /* Configure SCK,MOSI,SSEl as Output and MISO as Input */
  GPIO_PinDirection(PINSEL_PIN_18,1);
  GPIO_PinDirection(PINSEL_PIN_17,0);
  GPIO_PinDirection(PINSEL_PIN_16,1);

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
  while(util_GetBitStatus(LPC_SPI->SPSR,7)==0);
  CLR_Flags();
}

uint8_t SPI_Read()
{
  LPC_SPI->SPDR = 0xff;
  while(util_GetBitStatus(LPC_SPI->SPSR,7)==0);
  LPC_SPI->SPSR;
  return (uint8_t)LPC_SPI->SPDR;
}



#endif
