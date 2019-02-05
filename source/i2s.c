#include "i2s.h"
//https://vle.york.ac.uk/bbcswebdav/pid-2848390-dt-content-rid-7066727_2/courses/Y2018-006400/2015-16/CMSIS/drivers/html/lpc17xx__i2s_8c_source.html
int ENAINP,ENAOUT;

void DMA_IRQHandler()
{
  if(GPDMA_IntGetStatus(GPDMA_STAT_INT, 0))//Check for output interrupts
  {
    if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0))
    {
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 0);
      Channel0_TC++;
    }
    if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0))
    {
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 0);
      Channel0_Err++;
    }
  }
  if(GPDMA_IntGetStatus(GPDMA_STAT_INT, 1))//Check for input interrupts
  {
    if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 1))
    {
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 1);
      Channel1_TC++;
    }
    if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 1))
    {
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 1);
      Channel1_Err++;
    }
  }
}

void Init_I2S(volatile uint32_t* BufferOut,uint32_t BufferOutWidth,volatile uint32_t* BufferIn,uint32_t BufferInWidth)
{
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  LPC_PINCON->PINSEL0|=0x54000;//Set pins 0.7-0.9 as func 2 (i2s Tx)
  LPC_PINCON->PINSEL1|=0xA8000;//Set Pins 0.23-0.25 as func 3 (i2s Rx)
  I2S_Init(LPC_I2S);
  ConfInit(&I2S_Config_Struct, I2S_WORDWIDTH_16,I2S_STEREO,I2S_STOP_ENABLE,I2S_RESET_ENABLE,I2S_MUTE_DISABLE);
  ClockInit(&Clock_Config,I2S_CLKSEL_FRDCLK,I2S_4PIN_DISABLE,I2S_MCLK_DISABLE);
  I2S_FreqConfig(LPC_I2S, 48000, I2S_TX_MODE);//Set frequency for output
  I2S_SetBitRate(LPC_I2S, 0, I2S_RX_MODE);//Set bit rate for input
  GPDMA_Channel_CFG_Type CCFG_Struct;

  InitializeGPDMA(BufferOut,BufferOutWidth,BufferIn,BufferInWidth,&CCFG_Struct);
  I2S_Start(LPC_I2S);
}
void ConfInit(I2S_CFG_Type* I2S_Config_Struct,uint8_t wordwidth,uint8_t mono,uint8_t stop,uint8_t reset,uint8_t mute)
{
  I2S_Config_Struct->wordwidth = wordwidth;
  I2S_Config_Struct->mono = mono;
  I2S_Config_Struct->stop = stop;
  I2S_Config_Struct->reset = reset;
  I2S_Config_Struct->ws_sel = I2S_MASTER_MODE;
  I2S_Config_Struct->mute = mute;
  I2S_Config(LPC_I2S,I2S_TX_MODE,I2S_Config_Struct);
  I2S_Config_Struct->ws_sel = I2S_SLAVE_MODE;
  I2S_Config(LPC_I2S,I2S_RX_MODE,I2S_Config_Struct);
}
void ClockInit(I2S_MODEConf_Type* I2S_ClkConfig,uint8_t clksource,uint8_t mode4pin,uint8_t mclkout)
{
  I2S_ClkConfig->clksel = clksource;
  I2S_ClkConfig->fpin = mode4pin;
  I2S_ClkConfig->mcena = mclkout;
  I2S_ModeConfig(LPC_I2S,I2S_ClkConfig,I2S_TX_MODE);
  I2S_ModeConfig(LPC_I2S,I2S_ClkConfig,I2S_RX_MODE);
}

void InitializeGPDMA(volatile uint32_t* DataOut,uint32_t OutWidth,volatile uint32_t* DataIn,uint32_t InWidth,GPDMA_Channel_CFG_Type* GPDMA_Cfg)
{
  GPDMA_LLI_Type GPDMA_LLI_Struct;
  GPDMA_LLI_Struct.SrcAddr = (uint32_t)DataOut;
  GPDMA_LLI_Struct.DstAddr = (uint32_t)&(LPC_I2S->I2SDMA1);
  GPDMA_LLI_Struct.NextLLI = (uint32_t)&GPDMA_LLI_Struct;
  GPDMA_LLI_Struct.Control = OutWidth|(2<<18)|(2<<21)|(1<<26);
  GPDMA_Init();
  LPC_GPDMA->DMACConfig = 0x01;
  NVIC_DisableIRQ (DMA_IRQn);
  NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));
  GPDMA_Cfg->ChannelNum = 0;
  GPDMA_Cfg->SrcMemAddr = (uint32_t)DataOut;
  GPDMA_Cfg->DstMemAddr = 0;
  GPDMA_Cfg->TransferSize = OutWidth;
  GPDMA_Cfg->TransferWidth = 0;
  GPDMA_Cfg->TransferType = GPDMA_TRANSFERTYPE_M2P;
  GPDMA_Cfg->SrcConn = 0;
  GPDMA_Cfg->DstConn = GPDMA_CONN_I2S_Channel_0;
  GPDMA_Cfg->DMALLI = (uint32_t)&GPDMA_LLI_Struct;

  GPDMA_Setup(GPDMA_Cfg);

  GPDMA_Cfg->ChannelNum = 1;
  GPDMA_Cfg->SrcMemAddr = 0;
  GPDMA_Cfg->DstMemAddr = (uint32_t)DataIn;
  GPDMA_Cfg->TransferSize = InWidth+1;
  GPDMA_Cfg->TransferWidth = 0;
  GPDMA_Cfg->TransferType = GPDMA_TRANSFERTYPE_P2M;
  GPDMA_Cfg->SrcConn = GPDMA_CONN_I2S_Channel_1;
  GPDMA_Cfg->DstConn = 0;
  GPDMA_Cfg->DMALLI = 0;

  GPDMA_Setup(GPDMA_Cfg);

  Channel0_TC = 0;
  Channel0_Err = 0;
  Channel1_TC = 0;
  Channel1_Err = 0;


  I2S_DMAConf_Type I2S_DMAStruct;

  I2S_DMAStruct.DMAIndex = I2S_DMA_2;
  I2S_DMAStruct.depth = 8;
  I2S_DMAConfig(LPC_I2S, &I2S_DMAStruct, I2S_RX_MODE);
  I2S_DMAStruct.DMAIndex = I2S_DMA_1;
  I2S_DMAStruct.depth = 1;
  I2S_DMAConfig(LPC_I2S, &I2S_DMAStruct, I2S_TX_MODE);
}

void EnableInput()
{
  ENAINP = 1;
  GPDMA_ChannelCmd(1, ENABLE);
  if(!ENAOUT)NVIC_EnableIRQ (DMA_IRQn);
  I2S_DMACmd(LPC_I2S, I2S_DMA_2, I2S_RX_MODE, ENABLE);
}

void EnableOutput()
{
  ENAOUT = 1;
  GPDMA_ChannelCmd(0, ENABLE);
  if(!ENAINP)NVIC_EnableIRQ (DMA_IRQn);
  I2S_DMACmd(LPC_I2S, I2S_DMA_1, I2S_TX_MODE, ENABLE);
}
void DisableInput()
{
  ENAINP = 1;
  GPDMA_ChannelCmd(1, DISABLE);
  if(!ENAOUT)NVIC_DisableIRQ (DMA_IRQn);
  I2S_DMACmd(LPC_I2S, I2S_DMA_2, I2S_RX_MODE, DISABLE);
}
void DisableOutput()
{
  ENAOUT = 1;
  GPDMA_ChannelCmd(0, DISABLE);
  if(!ENAINP)NVIC_DisableIRQ (DMA_IRQn);
  I2S_DMACmd(LPC_I2S, I2S_DMA_1, I2S_TX_MODE, DISABLE);
}
