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
  GPDMA_Cfg->DMALLI = (uint32_t)DMA_Struct;

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

void DAC_StartSend(int Frequency,int NumSamples)
{
  DAC_CONVERTER_CFG_Type DAC_Conv;

  DAC_Conv.CNT_ENA = SET;
  DAC_Conv.DMA_ENA = SET;

  DAC_SetDMATimeOut(LPC_DAC,25000000/(Frequency*NumSamples));//Set the time between connections
  DAC_ConfigDAConverterControl(LPC_DAC,&DAC_Conv);

  GPDMA_ChannelCmd(1,ENABLE);//Enable GPDMA on channel specified
}
