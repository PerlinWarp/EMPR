#include "i2s_polling.h"

void I2S_IRQHandler()
{
  if(I2S_GetIRQStatus(LPC_I2S,I2S_RX_MODE))
  {
    if(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)>=I2S_GetIRQDepth(LPC_I2S,I2S_RX_MODE))
    {
         buffer[ReadInd] = I2S_Receive (LPC_I2S);
         ++ReadInd;
         ReadInd %= BUFFER_SIZE;
    }
  }
  if (I2S_GetIRQStatus(LPC_I2S,I2S_TX_MODE))
  {
    if(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)<=I2S_GetIRQDepth(LPC_I2S,I2S_TX_MODE))
    {
      if(WriteInd != ReadInd)
      {
         I2S_Send(LPC_I2S,buffer[WriteInd]);
         ++WriteInd;
         WriteInd %= BUFFER_SIZE;
      }
    }
  }
}


void I2S_Polling_Init(uint32_t freq,int i2smode)
{
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  LPC_PINCON->PINSEL0|=PINS7_9TX;//Set pins 0.7-0.9 as func 2 (i2s Tx)
  LPC_PINCON->PINSEL1|=PINS023_025RX;//Set Pins 0.23-0.25 as func 3 (i2s Rx)
  I2S_Init(LPC_I2S);
  ConfInit(&I2S_Config_Struct, I2S_WORDWIDTH_16,I2S_STEREO,I2S_STOP_ENABLE,I2S_RESET_ENABLE,I2S_MUTE_DISABLE);
  ClockInit(&Clock_Config,I2S_CLKSEL_FRDCLK,I2S_4PIN_DISABLE,I2S_MCLK_DISABLE);

  I2S_FreqConfig(LPC_I2S, freq, I2S_TX_MODE);//Set frequency for output
  I2S_FreqConfig(LPC_I2S, freq, I2S_RX_MODE);
  if(i2smode){
    ReadInd = WriteInd =  0;
    LPC_I2S->I2STXRATE = 0x00;
    LPC_I2S->I2STXBITRATE = 0x00;
    I2S_SetBitRate(LPC_I2S,0,I2S_TX_MODE);
    I2S_Start(LPC_I2S);
    I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,4);
    I2S_IRQCmd(LPC_I2S,I2S_TX_MODE,ENABLE);
    I2S_IRQConfig(LPC_I2S,I2S_RX_MODE,4);
    I2S_IRQCmd(LPC_I2S,I2S_RX_MODE,ENABLE);
    NVIC_SetPriority(I2S_IRQn, (1 << __NVIC_PRIO_BITS) -1);
    /*Depending on sitch, fill out buffer here to avoid clicks*/
    NVIC_EnableIRQ(I2S_IRQn);
  }
  else {I2S_Start(LPC_I2S);}
}

void I2S_Polling_Read( uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length)
{
  unsigned int counter=0;
  uint32_t Dummy;
  while(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)==0x00);
  Dummy = I2S_Receive(LPC_I2S);//Dummy receive as first value is often trash
  while(counter<I2S_Pol_Length){
    while(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)==0x00);
    I2S_Pol_Buffer[counter] = I2S_Receive(LPC_I2S);
    ++counter;
  }
}

void I2S_Polling_Write( uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length)
{
  unsigned int counter=0;
  while(counter<I2S_Pol_Length){
    while(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)!=0x00);
    I2S_Send(LPC_I2S,I2S_Pol_Buffer[counter]);
    ++counter;
  }
}
