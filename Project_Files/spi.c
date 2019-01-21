#include "spi.h"


void SSP0_IRQHandler()//If all's well, this should trigger whenever SSP calls an interrupt
{//https://github.com/luka1995/LPC17xx-Libraries/blob/master/Libraries/SSP.c <-- See for details
  //To do:
    //initialize SSP interrupt
    //Whenever
}
/*void CLR_Flags()
{
  uint8_t temp = LPC_SPI->SPSR;
  temp = LPC_SPI->SPDR;
}*/
void InitSPI_Default()
{
  InitSPI((uint8_t)16,(uint32_t)(1000000),(uint8_t)0);
}
void InitSPI(uint8_t BitsPerTransfer,uint32_t ClockFreq,uint8_t IntENA)
{

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
  To set a bit, or the relevant Pinsel with (PINSEL_FUNC)<<(PIN*2-(PINSEL*16))

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
  SystemInit();
  LPC_PINCON->PINSEL0 |= 0x80000000;//Set 15 to Func 3 - -> PIN SWITCHED
  LPC_PINCON->PINSEL1 |= 0x2A; //Set 17,18,16 to Func 3-> 0010 1010 2A
  /* Configure SCK,MOSI,SSEl as Output and MISO as Input */
  LPC_GPIO0->FIODIR |= 0x0B<<15;//1011 - Pins 15,16,18 are set high (output), and 17 low (input)
  //An additional note: p23 in this configuration is Select/Enable Slave - allowing for multiple slave devices.
  //Considering the fact that SSP1 is used to connect to the SD card, perhaps this should be used, keeping SSP0 free
  //LPC_GPIO0->FIODIR |= 1<<23;
  LPC_GPIO0->FIOSET = 1<<16;//Set pin 16 to high
  //PCONP enables power to devices present on the MBED
  LPC_SC->PCONP |= (1 << 21);//legacy SPI on = bit 8. SSP0 on = bit 21

  /*--- LEGACY SPI CODE
  LPC_SPI->SPCCR = ClockFreq;
  //     Reserved|BitsPerTransfer|MasterSel|IntE
  LPC_SPI->SPCR = (0)|(BitsPerTransfer<<7)|(3<<4)|(IntENA<<3);*/
  LPC_SSP0->CPSR |= 8;//Divide the peripheral clock by 8. Minimum value is 2.
  LPC_SSP0->CR0 |= (BitsPerTransfer)|~(3<<4)|(3<<6) //This is the control register for SSP0, it is used to control the phase, the bit size, the mode (we are using SPI here)
  NVIC_EnableIRQ(SSP0_IRQn);
  LPC_SSP0->CR1 &= ~(1<<2);//As a precaution, SSP is set to master just in case.
  LPC_SSP0->CR1 |=1<<1;//This is the second control register, used to control Maslter/Slave, enable/disable
  //CLR_Flags();

}
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
