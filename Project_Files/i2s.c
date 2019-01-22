#include "i2s.h"


void Init_I2S()
{
  GPDMA_Channel_CFG_Type GPDMACfg;
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  I2S_DMAConf_Type I2SDMA_Struct;
  //Assign
  LPC_PINCON->PINSEL0|=0x15540;//Set pins 5-9 as func 1
  I2S_Init(LPC_I2S);
  LPC_
}
