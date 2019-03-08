#include "Play_Audio.h"

volatile uint8_t SampleDone,InputDone,bufferIndex;
volatile uint32_t Freq;
volatile uint32_t writeIndex;
uint16_t* buffers[2];
GPDMA_Channel_CFG_Type CCFG_Struct[2];

static void (*TIMER1_Handler_Funcs[])(void) = {&onboard_dma_irq,&onboard,&onboard_record};
uint8_t TIMER1_IRQ_Index;

void TIMER1_IRQHandler(void)
{
  TIMER1_Handler_Funcs[TIMER1_IRQ_Index]();
}
char output[60];
void onboard_record()
{

   if(writeIndex < PRECISION)
 {
    ADC_StartCmd(LPC_ADC,ADC_START_NOW);
    while(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,1)==RESET);
    buffers[bufferIndex][writeIndex] = (uint16_t)ADC_GetData(ADC_CHANNEL_2);
    writeIndex++;
 }
 else if(InputDone == 1)
 {
      InputDone = writeIndex = 0;
      bufferIndex = 1 - bufferIndex;
 }
 else{
   TIM_Cmd(LPC_TIM1,DISABLE);//disable timer while write finishes
   NVIC_DisableIRQ(TIMER1_IRQn);
 }
}

void onboard()
{
 if(writeIndex < PRECISION)
 {
  sprintf(output,"%08x\n\r",(uint32_t)buffers[bufferIndex][writeIndex]);
  WriteText(output);
  DAC_UpdateValue(LPC_DAC,(uint32_t)buffers[bufferIndex][writeIndex]);
  writeIndex++;
 }
 else if(InputDone == 1)
 {
      InputDone = writeIndex = 0;
      bufferIndex = 1 - bufferIndex;
 }
 else{
   TIM_Cmd(LPC_TIM1,DISABLE);//disable timer while write finishes
   NVIC_DisableIRQ(TIMER1_IRQn);
 }
}

void onboard_dma_irq()
{
  TIM_Cmd(LPC_TIM1,DISABLE);
  TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
  TIM_ResetCounter(LPC_TIM1);
  if(SampleDone && InputDone)
  {
    SampleDone = InputDone = 0;
    bufferIndex = 1 - bufferIndex;
    GPDMA_Setup(&CCFG_Struct[bufferIndex]);
    GPDMA_ChannelCmd(1, ENABLE);
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

      GPDMA_ChannelCmd(1, DISABLE);
      GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 1);
      //DAC_UpdateValue(LPC_DAC,buffers[bufferIndex][PRECISION-1]);
      NVIC_DisableIRQ(DMA_IRQn);//This runs when the buffer has emptied
    }
  }
}
/*
Raw files on SD are:
little endian

*/
//Onboard is unsigned, mono, so data needs to be converted
void init_onboard_audio_no_DMA(FIL* fil,uint32_t Frequency)

{//NOTE: Pin Is PIN 18 on the header board --CHECK PINOUTS!!!
  int count = 0;
  Freq = Frequency;

  uint16_t* buf1[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
  uint16_t* buf2[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
  buffers[0] = buf1;
  buffers[1] = buf2;


  bufferIndex =0;

  InitializeDAC();//Initialize onboard Audio    sprintf(output,"ID:%d,wI:%d,d:%d\n\r",InputDone,writeIndex,buffers[bufferIndex][writeIndex]);

  LCDGoHome();
  LCDPrint(" Playing  audio \n****************");

  f_lseek(fil,SPLIFF_SIZE);//Seek to start of raw data

  buttonpress = 0;
  TIMER1_IRQ_Index=1;
  breakout = 0;
  InitTimer(Freq);
  f_read(fil,buffers[bufferIndex],PRECISION*2,&count);
  while(buttonpress == 0)
  {
    InputDone = 1;
    TIM_Cmd(LPC_TIM1,ENABLE);
    NVIC_EnableIRQ(TIMER1_IRQn);
    while(InputDone == 1 && breakout == 0);
    f_read(fil,buffers[1-bufferIndex],PRECISION*2,&count);
    if(count != PRECISION*2)break;
  }

  TIM_Cmd(LPC_TIM1,DISABLE);
  NVIC_DisableIRQ(TIMER1_IRQn);
}

uint32_t record_onboard_audio_no_DMA(FIL* fil, uint32_t Frequency)
{
  int count = 0;
  Freq = Frequency;
  uint32_t counter;
  uint16_t* buf1[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
  uint16_t* buf2[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
  buffers[0] = buf1;
  buffers[1] = buf2;

  SampleDone = InputDone = 0;
  bufferIndex =0;
  breakout =0;
  InitializeADC();//Initialize onboard Audio

  LCDGoHome();
  LCDPrint("Recording audio \n****************");

  f_lseek(fil,SPLIFF_SIZE);//Seek to start of raw data

  buttonpress = 0;
  TIMER1_IRQ_Index=2;
  InitTimer(Freq);
  while(buttonpress == 0)
  {
    InputDone = 1;
    TIM_Cmd(LPC_TIM1,ENABLE);
    NVIC_EnableIRQ(TIMER1_IRQn);
    while(InputDone == 1 && breakout == 0);
    f_write(fil,buffers[1-bufferIndex],PRECISION*2,&count);
    counter +=PRECISION*2;
    if(count != PRECISION*2)break;

  }
  TIM_Cmd(LPC_TIM1,DISABLE);
  NVIC_DisableIRQ(TIMER1_IRQn);

  /*free(buf1);
  free(buf2);*/
  return counter;
}
