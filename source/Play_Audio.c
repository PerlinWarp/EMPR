#include "Play_Audio.h"

volatile uint8_t SampleDone,InputDone,bufferIndex;
volatile uint32_t Freq;
GPDMA_Channel_CFG_Type CCFG_Struct[2];

void TIMER1_IRQHandler(void)
{
  TIM_Cmd(LPC_TIM1,DISABLE);
  TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
  TIM_ResetCounter(LPC_TIM1);
  if(SampleDone && InputDone)
  {
    WriteText("next Sample");
    SampleDone = InputDone = 0;
    bufferIndex = 1 - bufferIndex;
    GPDMA_Setup(&CCFG_Struct[bufferIndex]);
    NVIC_EnableIRQ(DMA_IRQn);
    DAC_StartSend(Freq,PRECISION);
  }


  TIM_Cmd(LPC_TIM1,ENABLE);
}


void DMA_IRQHandler()
{
  if(GPDMA_IntGetStatus(GPDMA_STAT_INT, 1))//We are using channel 1
  {
    if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 1))
    {
      SampleDone =1;
      WriteText("int triggered");
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 1);
      NVIC_DisableIRQ(DMA_IRQn);//This runs when the buffer has emptied
    }
  }
}
/*
Raw files on SD are:
little endian

*/
//Onboard is unsigned, mono, so data needs to be converted
void init_onboard_audio(FIL* fil,uint32_t Frequency)
{//NOTE: Pin Is PIN 18 on the header board --CHECK PINOUTS!!!
  int count = 0;
  Freq = Frequency;
  uint32_t* buf1 = (uint32_t*)NewMalloc(sizeof(uint32_t)*PRECISION);//Ping pong buffers: while one is DMA'd, the other is filling
  uint32_t* buf2 = (uint32_t*)NewMalloc(sizeof(uint32_t)*PRECISION);
  char* inbuf1 = (char*)NewMalloc(sizeof(char)*PRECISION);
  char* inbuf2 = (char*)NewMalloc(sizeof(char)*PRECISION);
  uint32_t* buffers[2];
  char* inbuffers[2];
  buffers[0] = buf1;
  buffers[1] = buf2;
  inbuffers[0] = inbuf1;
  inbuffers[1] = inbuf2;
  SampleDone = InputDone = 0;
  GPDMA_LLI_Type LLI_Struct;
  bufferIndex =0;
  InitializeDAC();//Initialize onboard Audio
  InitializeGPDMA(buf1,&LLI_Struct,&CCFG_Struct[0],PRECISION);
  InitializeGPDMA(buf2,&LLI_Struct,&CCFG_Struct[1],PRECISION);
  LCDGoHome();
  LCDPrint(" Playing  audio \n****************");
  f_lseek(fil,0);//Seek to start of raw data
  buttonpress = 0;
  int i;
  SampleDone = 1;
  InitTimer(48000);
  TIM_Cmd(LPC_TIM1,ENABLE);
  while(buttonpress == 0)
  {
    count = SD_READ(fil,(uint32_t*)inbuffers[bufferIndex],PRECISION);
    if(count != PRECISION)break;
    for(i = 0;i<PRECISION;i++)
    {
      buffers[bufferIndex][i] = (uint32_t)inbuffers[bufferIndex][i]<<8;
    }
    InputDone = 1;
    while(InputDone == 1);
  }
  TIM_Cmd(LPC_TIM1,DISABLE);
  NVIC_DisableIRQ(DMA_IRQn);
  NVIC_DisableIRQ(TIMER1_IRQn);
  NewFree(buf1);
  NewFree(buf2);
  NewFree(inbuf1);
  NewFree(inbuf2);

}
