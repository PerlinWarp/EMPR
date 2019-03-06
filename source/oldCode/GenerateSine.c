#include "GenerateSine.h"

void init_onboard_audio()
{//NOTE: Pin Is PIN 18 on the header board
  InitSerial();
  Delay_Init();
  uint32_t* buf1 = (uint32_t*)malloc(sizeof(uint32_t)*PRECISION);
  uint32_t* buf2 = (uint32_t*)malloc(sizeof(uint32_t)*PRECISION);
  uint32_t** buffers[2];
  buffer
  int Frequency = 10,i;//in Hz
  double MaxAmplitude = 3,oldMax=0;//in Volts

  GPDMA_LLI_Type LLI_Struct;
  GPDMA_Channel_CFG_Type CCFG_Struct;
  for(i = 0;i<PRECISION;i++)
  {
    SineWaveTable[i] = (uint32_t)(sin(2*i*PI/PRECISION)* MaxAmplitude * 9680+(9680*MaxAmplitude));
    //Calculate Sine and multiply Each Wavetable entry by Amplitude and convert to UINT32
  }
  InitializeDAC();
  InitializeGPDMA(SineWaveTable,&LLI_Struct,&CCFG_Struct,PRECISION);
  DAC_StartSend(Frequency,PRECISION);
  while(1)
  {//Every Ten Seconds, Change the Frequency and Amplitude
    count = SD_READ(fileptr,buffer,PRECISION);

    DAC_StartSend(rand()%100,PRECISION);

  }//Loop forever or do what you want

  free(SineWaveTable);
  return 0;
}
/*
while(1){
  count = SD_READ(fileptr,buffer,BUFFER_SIZE);//read the buffer full
  f_lseek(fileptr,44);
  if(count == BUFFER_SIZE)NVIC_EnableIRQ(I2S_IRQn);
  else {break;}
}*/
