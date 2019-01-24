#include "spi.h"

volatile uint32_t SSP0_Int_Timeout = 0;
volatile uint32_t SSP0_Int_Readable = 0;
volatile uint32_t SSP0_Int_OverRun = 0;
volatile uint32_t SSP1_Int_Timeout = 0;
volatile uint32_t SSP1_Int_Readable = 0;
volatile uint32_t SSP1_Int_OverRun = 0;

void SSP0_IRQHandler(){
  uint32_t reg;
  reg = LPC_SSP0->MIS;
  if(reg & 1){
    SSP0_Int_OverRun++;
    LPC_SSP0->ICR = 1;
  }
  if(reg & (1<<1)){
    SSP0_Int_Timeout++;
    LPC_SSP0->ICR = 2;
  }
  if(reg & (1<<2))SSP0_Int_Readable++;
}

void SSP1_IRQHandler()//If all's well, this should trigger whenever SSP calls an interrupt
{//https://github.com/luka1995/LPC17xx-Libraries/blob/master/Libraries/SSP.c <-- See for details
  //To do:
    //initialize SSP interrupt
    //Whenever
  uint32_t reg;
  reg = LPC_SSP1->MIS;
  if(reg & 1){
    SSP1_Int_OverRun++;
    LPC_SSP1->ICR = 1;
  }
  if(reg & (1<<1)){
    SSP1_Int_Timeout++;
    LPC_SSP1->ICR = 2;
  }
  if(reg & (1<<2))SSP1_Int_Readable++;
}



/*For the SD Card*/
void Init_SSP1()
{
  uint8_t temp,i;
  SystemInit();
  LPC_PINCON->PINSEL0 |= 0x000AA000;//(2<<12)|(2<<14)|(2<<16)|(2<<18);// 9,8,7,6 -> Func3
  //LPC_GPIO0->FIODIR |= 0x0B<<15; //The SSP automatically configures these accordingly
  //LPC_GPIO0->FIOSET = 1<<16;
  LPC_SC->PCONP |= (1 << 21);
  LPC_SC->PCLKSEL1|= 1<<10;//Return to syst clock freq
  LPC_SSP1->CPSR |= 8;//Divide clock by 8
  for(i=0;i<8;i++)temp = LPC_SSP1->DR; // Clear the read
  LPC_SSP1->CR0 |= (16)|~(3<<4)|(3<<6);
  NVIC_EnableIRQ(SSP1_IRQn);
  LPC_SSP1->CR1 &= ~(1<<2);
  LPC_SSP1->CR1 |=1<<1;
}
void Init_SSP0()
{
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
  LPC_SC->PCLKSEL1|= 1<<10;//Return to syst clock freq
  /*--- LEGACY SPI CODE
  LPC_SPI->SPCCR = ClockFreq;
  //     Reserved|BitsPerTransfer|MasterSel|IntE
  LPC_SPI->SPCR = (0)|(BitsPerTransfer<<7)|(3<<4)|(IntENA<<3);*/
  LPC_SSP0->CPSR |= 8;//Divide the peripheral clock by 8. Minimum value is 2.
  LPC_SSP0->CR0 |= (16)|~(3<<4)|(3<<6); //This is the control register for SSP0, it is used to control the phase, the bit size, the mode (we are using SPI here)
  NVIC_EnableIRQ(SSP0_IRQn);
  LPC_SSP0->CR1 &= ~(1<<2);//As a precaution, SSP is set to master just in case.
  LPC_SSP0->CR1 |=1<<1;//This is the second conuint8_t temp,i;trol register, used to control Maslter/Slave, enable/disable
}

void SSP1_Write(uint16_t *buf, uint32_t Length){
  uint8_t temp,i;
  for(i = 0;i<Length;i++)
  {
    while(LPC_SSP1->SR&(1<<1));//Wait for next byte
    LPC_SSP1->DR = buf[i];
  }
  while(LPC_SSP1->SR&(1<<4));
  temp = LPC_SSP1->DR;
}
void SSP0_Write(uint16_t *buf, uint32_t Length){
  uint8_t temp,i;
  for(i = 0;i<Length;i++)
  {
    while(LPC_SSP0->SR&(1<<1));//Wait for next byte
    LPC_SSP0->DR = buf[i];
  }
  while(LPC_SSP0->SR&(1<<4));
  temp = LPC_SSP0->DR;
}
void SSP1_Read(uint16_t *buf, uint32_t Length){
  uint8_t temp,i;
  for(i = 0;i<Length;i++)
  {
    LPC_SSP1->DR = 0xFF;
    while(LPC_SSP1->SR&(1<<2));
    buf[i] = LPC_SSP1->DR;
  }
  while(LPC_SSP1->SR&(1<<4));
  temp = LPC_SSP1->DR;
}
void SSP0_Read(uint16_t *buf, uint32_t Length){
  uint8_t temp,i;
  for(i = 0;i<Length;i++)
  {
    LPC_SSP0->DR = 0xFF;
    while(LPC_SSP0->SR&(1<<2));
    buf[i] = LPC_SSP0->DR;
  }
  while(LPC_SSP0->SR&(1<<4));
  temp = LPC_SSP0->DR;
}
