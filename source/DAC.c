#include "DAC.h"

void InitializeGPDMA(uint32_t* Source,GPDMA_LLI_Type* DMA_Struct,GPDMA_Channel_CFG_Type* GPDMA_Cfg, int NumSamples)
{
  DMA_Struct->SrcAddr = (uint32_t)Source; //The address where the list of values are stored
  DMA_Struct->DstAddr = (uint32_t)&(LPC_DAC->DACR); //What memory address to send the data to
  DMA_Struct->NextLLI = (uint32_t)DMA_Struct; //Once the current list is finished, which DMA_Struct to output afterwards
  DMA_Struct->Control = NumSamples|(2<<18)|(2<<21)|(1<<26);//Bit pattern to send to the DMA controller to specify the size of the addresses + to increment the source ptr each time
  GPDMA_Init();
  GPDMA_Cfg->ChannelNum = 1;
  GPDMA_Cfg->SrcMemAddr = (uint32_t)Source;
  GPDMA_Cfg->DstMemAddr = 0;
  GPDMA_Cfg->TransferSize = NumSamples;
  GPDMA_Cfg->TransferWidth = 0;
  GPDMA_Cfg->TransferType = GPDMA_TRANSFERTYPE_M2P;
  GPDMA_Cfg->SrcConn = 0;
  GPDMA_Cfg->DstConn = GPDMA_CONN_DAC; //Point give data to the DAC converter memory address
  GPDMA_Cfg->DMALLI = (uint32_t)DMA_Struct;//Maybe try changing this back
  GPDMA_Setup(GPDMA_Cfg);
}

void InitializeDAC()
{
  PINSEL_CFG_Type PINCFG;//DAC
  PINCFG.Funcnum = 2;
  PINCFG.OpenDrain =0;
  PINCFG.Pinmode = 0;
  PINCFG.Pinnum = 26;
  PINCFG.Portnum = 0;
  PINSEL_ConfigPin(&PINCFG);

  DAC_Init(LPC_DAC);

}

void DAC_StartSend(uint32_t Frequency,uint32_t NumSamples)
{
  DAC_CONVERTER_CFG_Type DAC_Conv;
  DAC_Conv.CNT_ENA = SET;
  DAC_Conv.DMA_ENA = SET;
  DAC_SetDMATimeOut(LPC_DAC,25000000/(Frequency*NumSamples));//Set the time between connections
  DAC_ConfigDAConverterControl(LPC_DAC,&DAC_Conv);
  GPDMA_ChannelCmd(1,ENABLE);//Enable GPDMA on channel specified
}

void InitTimer(uint32_t Frequency)
{
  uint8_t mseconds = (uint8_t)((1/(float)Frequency) * 1000000);
  TIM_TIMERCFG_Type TIM_ConfigStruct;
  TIM_MATCHCFG_Type TIM_MatchConfigStruct;
  TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct.PrescaleValue  = 1;
  TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);

  TIM_MatchConfigStruct.MatchChannel = 0;
  TIM_MatchConfigStruct.MatchValue   = mseconds;//match every time
  TIM_MatchConfigStruct.IntOnMatch   = TRUE;
  TIM_MatchConfigStruct.ResetOnMatch = TRUE;
  TIM_MatchConfigStruct.StopOnMatch  = FALSE;
  TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;//Nothing as only for the interrupt
  TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);

  NVIC_SetPriority(TIMER1_IRQn, ((0x01<<3)|0x02));
  NVIC_EnableIRQ(TIMER1_IRQn);
  TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&TIM_ConfigStruct);
}

void InitializeADC()
{
  //Open pins:

  PINSEL_CFG_Type PINCFG;//GPIO Number
  PINCFG.Funcnum = 1;
  PINCFG.OpenDrain =0;
  PINCFG.Pinmode = 0;
  PINCFG.Pinnum = 25;
  PINCFG.Portnum = 0;
  PINSEL_ConfigPin(&PINCFG);
  //Run Default Functions

  ADC_Init(LPC_ADC,200000);
  //ADC_IntConfig(LPC_ADC,ADC_ADINTEN2,DISABLE);//Enable ADC Interrupts
  ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE);//Enable channel to use
  //ADC_EdgeStartConfig(LPC_ADC,ADC_START_ON_FALLING);//Specify the edge to start inputting on
}
