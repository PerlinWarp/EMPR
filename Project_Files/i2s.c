#include "i2s.h"
//https://vle.york.ac.uk/bbcswebdav/pid-2848390-dt-content-rid-7066727_2/courses/Y2018-006400/2015-16/CMSIS/drivers/html/lpc17xx__i2s_8c_source.html

void Init_I2S(uint8_t wordwidth,uint8_t)
{
  GPDMA_Channel_CFG_Type GPDMACfg;
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  I2S_DMAConf_Type I2SDMA_Struct;
  //Assign
  LPC_PINCON->PINSEL0|=0x15540;//Set pins 5-9 as func 1
  I2S_Init(LPC_I2S);
  LPC_I2S->I2SDAI =;wordwidth|(mono<<2)|(stop<<3)|(reset<<4)|(1<<5)|((8*(wordwidth+1))<<6);
  LPC_I2S->I2SDAO = ;
}
