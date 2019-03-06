#include "Play_Audio.h"


void DMA_IRQHandler()
{
  if(GPDMA_IntGetStatus(GPDMA_STAT_INT, 1))//We are using channel 1
  {
    if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 1))
    {
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 1);
      if(tmp == 1) GPDMA_ChannelCmd(1,DISABLE);
    }
  }
}


void init_onboard_audio(FIL* fil,uint32_t Frequency)
{//NOTE: Pin Is PIN 18 on the header board --CHECK PINOUTS!!!
  int count = 0,bufferIndex = 0;
  uint32_t* buf1 = (uint32_t*)NewMalloc(sizeof(uint32_t)*PRECISION);//Ping pong buffers: while one is DMA'd, the other is filling
  uint32_t* buf2 = (uint32_t*)NewMalloc(sizeof(uint32_t)*PRECISION);
  uint32_t** buffers[2];
  buffers[0] = buf1;
  buffers[1] = buf2;

  GPDMA_LLI_Type LLI_Struct;
  GPDMA_Channel_CFG_Type CCFG_Struct[2];

  InitializeDAC();//Initialize onboard Audio
  InitializeGPDMA(buf1,&LLI_Struct,&CCFG_Struct[0],PRECISION);
  InitializeGPDMA(buf2,&LLI_Struct,&CCFG_Struct[1],PRECISION);

  f_lseek(fileptr,0);//Seek to start of raw data
  
  while(buttonpress == 0)
  {//Every Ten Seconds, Change the Frequency and Amplitude
    count = SD_READ(fil,buffers[i],PRECISION);
    //If is a wave file, decode here.
    if(count == PRECISION)
    {
      bufferIndex = 1 - bufferIndex;
      GPDMA_Setup(CCFG_Struct[bufferIndex]);
      DAC_StartSend(Frequency,PRECISION);
    }
    else
    {
      break;
    }
  }
  NewFree(buf1);
  NewFree(buf2);
}
