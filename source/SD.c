#include "SD.h"



unsigned int SD_READ(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  CS_LOW();//Enable chip select
  f_read(fil,buf,bufSize, &count);
  CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}

unsigned int SD_WRITE(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  CS_LOW();//Enable chip select
  f_write(fil,buf,bufSize, &count);
  CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}
