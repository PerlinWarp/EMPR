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

void CLR_Flags();
void InitSPI_Default();
void InitSPI(uint8_t BitsPerTransfer,uint32_t ClockFreq,uint8_t IntENA);
void SPI_Write(uint8_t Data);
uint8_t SPI_Read();
#endif
