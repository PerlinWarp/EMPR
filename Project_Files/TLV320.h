/***********************************************************************//**
 * @file		TLV320.h
 * @brief		Contains Code to communicate with the tlv320aic23b audio interface
  module.
 * @version		0.1
 * @date		21. January. 2019
 * @author		Idris Al-Ghabra
 **************************************************************************/
#ifndef TLV_H_
#define TLV_H_
#include "LPC17xx.h"
#include "spi.h"
#include "i2s.h"
/*SPI - CS goes low while the command is active, and high when not used
  SCLK - Pulse for each bit input
  SDIN - Pass instruction here -> address bloc , then data block

  TODO:
  Power Down Control: 0000110
    Turns on and off various parts of the chip
  Format -> 0000111
    Enables slave and format, default should be 0000000 for slave, high phase, 16 bit, right aligned.
  Sample Rate Control -> 0001000

*/

void TLV320_Setup();

#endif
