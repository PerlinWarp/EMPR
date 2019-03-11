#include "SineWave.h"


#define PRECISION 147
#define MAXVOLTS 2.5
#define PI 3.1415926535897932384626433832

void SineWave(uint32_t Frequency)
{
  uint32_t SineWaveTable[PRECISION];
  WriteText("ere\n\r");
  int i;
  double MaxAmplitude = 3;//in Volts
  WriteText("fear\n\r");
  GPDMA_LLI_Type LLI_Struct;
  GPDMA_Channel_CFG_Type CCFG_Struct;
  for(i = 0;i<PRECISION;i++)
  {
    SineWaveTable[i] = (uint32_t)(sin(2*i*PI/PRECISION)* MaxAmplitude * 9680+(9680*MaxAmplitude));
    //Calculate Sine and multiply Each Wavetable entry by Amplitude and convert to UINT32
  }
  WriteText("is\n\r");
  InitializeGPDMA(SineWaveTable,&LLI_Struct,&CCFG_Struct,PRECISION);
  DAC_StartSend(Frequency,PRECISION);
  leave = 0;
  WriteText("building\n\r");
  while(leave == 0);
  WriteText("fuckg this shite\n\r");
}