#ifndef DAC_H_
#define DAC_H_

#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"//Do Direct memory access Deely
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_pinsel.h"

void InitializeDAC();

void DAC_StartSend(int Frequency,int NumSamples);

void InitializeGPDMA(uint32_t* Source,GPDMA_LLI_Type* DMA_Struct,GPDMA_Channel_CFG_Type* GPDMA_Cfg, int NumSamples);


#endif
