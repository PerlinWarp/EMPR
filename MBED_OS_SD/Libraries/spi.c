/* Jumentum-SOC

  Copyright (C) 2007 by Daniel Marks

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  
  Daniel L. Marks profdc9@gmail.com

*/

/* Support for SSP added by DLM */

/* BITBANG SPI support mostly added as a hack for LPC2378 to access SD/MMC
   card through the MMC/SD port since I don't know how to control it using
   their interface yet, and I would have to write my own driver to interface
   to ELM FAT FS */

#include "LPC17xx.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "stdio2.h"
#include "sys/types.h"
#include "stdlib2.h"
//#include "kernel.h"
//#include "kernel_events.h"

#include "config.h" //Hopefully this fixes things
	
#define CS_PORT_NUM 		0
#define CS_PIN_NUM 			16
#define MOSI_PIN_NUM 		18
#define MISO_PIN_NUM 		17
#define SCK_PIN_NUM 		15
#define LPC_SSP			LPC_SSP0
#include "spi.h"

/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Set and Reset the SPI CS line, using the state value		*/
/*					  			*/
/****************************************************************/
void CS_Set(char state)
{
	if (state){
		GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	} else {
		GPIO_ClearValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	}
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Setup the basic SPI bus on the chip, including the CS line	*/
/*					  			*/
/****************************************************************/
void spiInit(void)
{
	PINSEL_CFG_Type PinCfg;

	SSP_CFG_Type SSP_ConfigStruct;
	
	// Have we got the SDCARD CS code setup??, slit out so we can
	// use the SPI bus for other things
	// if(!kernel_event(KERNEL_SYS_STATE_BIT,SYSTEM_STATE_SDSPI_UP,NULL)){
	if(1){
		PinCfg.Pinnum = CS_PIN_NUM;		// SSEL
		PinCfg.Funcnum = 0;
		PINSEL_ConfigPin(&PinCfg);

		GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
		GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
		//kernel_event(KERNEL_SET_SYS_STATE,SYSTEM_STATE_SDSPI_UP,NULL);
	}
	// Is the SPI bus setup??
	// if(!kernel_event(KERNEL_SYS_STATE_BIT,SYSTEM_STATE_SPI_UP,NULL)){
	if(1){
		/*
		 * Initialize SPI pin connect
		 * P0.7  - SCK
		 * P0.11 - SSEL - used as GPIO
		 * P0.8  - MISO
		 * P0.9  - MOSI
		 */
		PinCfg.Funcnum = 2;
		PinCfg.OpenDrain = 0;
		PinCfg.Pinmode = 0;
		PinCfg.Portnum = 0;

		PinCfg.Pinnum = SCK_PIN_NUM;		// SCLK
		PINSEL_ConfigPin(&PinCfg);

		PinCfg.Pinnum = MISO_PIN_NUM;		// MISO
		PINSEL_ConfigPin(&PinCfg);

		PinCfg.Pinnum = MOSI_PIN_NUM;		// MOSI
		PINSEL_ConfigPin(&PinCfg);

		

		SSP_ConfigStructInit(&SSP_ConfigStruct);	// initialize SSP configuration structure to default
		SSP_ConfigStruct.ClockRate = CONFIG_SPI_CLOCK;	// Set the bus speed to what we want
		SSP_Init(LPC_SSP, &SSP_ConfigStruct);		// Initialize SSP peripheral with parameter given in structure above
	
		SSP_Cmd(LPC_SSP, ENABLE);			// Enable SSP peripheral
		//kernel_event(KERNEL_SET_SYS_STATE,SYSTEM_STATE_SPI_UP,NULL);
	}
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Write a block of data		  			*/
/*					  			*/
/****************************************************************/
short  SPIWrite(unsigned char  * ptrBuffer, short  ui_Len)
{
  short  i,stat;

    for (i=0;i<ui_Len;i++)
    {
		while (!(LPC_SSP->SR & SSP_SR_TNF));
			LPC_SSP->DR = *ptrBuffer++;
		while (!(LPC_SSP->SR & SSP_SR_RNE));
			stat = LPC_SSP->DR;
	}
	return i;
}

/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Read a block of data			  			*/
/*					  			*/
/****************************************************************/
short  SPIRead(unsigned char  * ptrBuffer, short  ui_Len)
{
	short	i;

	for (i=0;i<ui_Len;i++){
		while (!(LPC_SSP->SR & SSP_SR_TNF));
		LPC_SSP->DR = 0xFF;
		while (!(LPC_SSP->SR & SSP_SR_RNE));
	
		*ptrBuffer = LPC_SSP->DR;
		ptrBuffer++;
	}
	return i;
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Chip Select Set			  			*/
/*					  			*/
/****************************************************************/
void spiMMCChipSelect (char select)
{
	if (select) {
		LPC_SSP->CPSR = SPI_MMC_SPEED;	
		CS_Set(0);
	} else {
		CS_Set(1);
		while (!(LPC_SSP->SR & SSP_SR_TNF));
		
		LPC_SSP->DR = 0xff;
		//
		// Wait until TX fifo and TX shift buffer are empty
		//
		
		while (LPC_SSP->SR & SSP_SR_BSY);
		
		while (!(LPC_SSP->SR & SSP_SR_RNE));
		do {
			select = LPC_SSP->DR;
		} while (LPC_SSP->SR & SSP_SR_RNE);
	}
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Setd a byte of data			  			*/
/*					  			*/
/****************************************************************/
void spiSendByte (unsigned char c)
{
	while (!(LPC_SSP->SR & SSP_SR_TNF));
	LPC_SSP->DR = c;
	while (!(LPC_SSP->SR & SSP_SR_RNE));
	c = LPC_SSP->DR;
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* Get a byte of data			  			*/
/*					  			*/
/****************************************************************/
unsigned char spiReceiveByte (void)
{
	while (!(LPC_SSP->SR & SSP_SR_TNF));
	LPC_SSP->DR = 0xFF;
	while (!(LPC_SSP->SR & SSP_SR_RNE));
	return LPC_SSP->DR;
}
/****************************************************************/
/*					  			*/
/* SPI Driver, Base Code					*/
/*					  			*/
/* ??					  			*/
/*					  			*/
/****************************************************************/
void spiReceivePtr (unsigned char *c)
{
	while (!(LPC_SSP->SR & SSP_SR_TNF));
	LPC_SSP->DR = 0xFF;
	while (!(LPC_SSP->SR & SSP_SR_RNE));
	*c = LPC_SSP->DR;
}
