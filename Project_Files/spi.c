#include "spi.h"



void CLR_Flags()
{
  uint8_t temp = LPC_SPI->SPSR;
  temp = LPC_SPI->SPDR;
}
void InitSPI_Default()
{
  WriteText("Crashpt1\n\r");
  Delay(10);
  InitSPI((uint8_t)16,(uint32_t)(1000000),(uint8_t)0);
}
void InitSPI(uint8_t BitsPerTransfer,uint32_t ClockFreq,uint8_t IntENA)
{

  BitsPerTransfer %= 16;

  //BitsPerTransfer = ((BitsPerTransfer)<8?8:BitsPerTransfer);
/*

  http://nanohome.be/nxp/LPC1768_Pins.php - A table of pins

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
*/

  LPC_PINCON->PINSEL0 = 3<<30;//Set 15 to Func 3
  LPC_PINCON->PINSEL1 = 0x3C; //Set 17,18 to Func 3, 16 to Func 0
  /* Configure SCK,MOSI,SSEl as Output and MISO as Input */
  WriteText("Crashpt2\n\r");
  Delay(10);
  LPC_GPIO0->FIODIR = 0x0B<<15;//1011 - for pins 15 - 18
  LPC_GPIO0->FIOSET = 1<<16;//Set pin 16 to high
  LPC_SC->PCONP |= (1 << 8);//turn on power for SPI
  LPC_SPI->SPCCR = ClockFreq;
  //     Reserved|BitsPerTransfer|MasterSel|IntE
  LPC_SPI->SPCR = (0)|(BitsPerTransfer<<7)|(3<<4)|(IntENA<<3);

  CLR_Flags();

}

void SPI_Write(uint8_t Data)
{
  LPC_SPI->SPDR = Data;
  while((LPC_SPI->SPSR&0x40)==0);
  CLR_Flags();
}

uint8_t SPI_Read()
{
  LPC_SPI->SPDR = 0xff;
  while((LPC_SPI->SPSR&0x40)==0);
  LPC_SPI->SPSR;
  return (uint8_t)LPC_SPI->SPDR;
}
