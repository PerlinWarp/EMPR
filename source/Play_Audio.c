#include "Play_Audio.h"

volatile uint8_t SampleDone,InputDone,bufferIndex;
uint32_t Freq;
uint32_t writeIndex;
uint16_t* buffers[2],i;
uint32_t buf[PRECISION];
UINT count = 0;
static void (*TIMER1_Handler_Funcs[])(void) = {&timer_Done,&onboard,&onboard_record};
uint8_t TIMER1_IRQ_Index;
FIL* fil;
void TIMER1_IRQHandler(void)
{
  TIMER1_Handler_Funcs[TIMER1_IRQ_Index]();
}
char output[60];
void onboard(){}
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

void timer_Done(){
  timerDone = 1;
  TIM_Cmd(LPC_TIM1,DISABLE);//disable timer while write finishes
  NVIC_DisableIRQ(TIMER1_IRQn);
}
uint32_t j;
void PWM1_IRQHandler()
{
 if(InputDone == 0 && buttonpress == 0)
 {
  DAC_UpdateValue(LPC_DAC,++j);
 }
  //DAC_UpdateValue(LPC_DAC,(*(buf+writeIndex)>>24));
  //uint32_t i;
  //while(1) {
  //  for(i = 0xF; i++; i < 0xFFFF){
  //    DAC_UpdateValue(LPC_DAC, i);
  //  }
  //}
  // writeIndex++;
  // if(writeIndex == PRECISION*4)
  // {
  //   f_read(fil,buf,PRECISION*4,&count);
  //   if(count != PRECISION*4)InputDone =1;
  //   writeIndex = 0;
  // }{
//  else
//  {
//     TIM_Cmd(LPC_TIM1,DISABLE);//disable timer while write finishes
//     NVIC_DisableIRQ(TIMER1_IRQn);
//  }
}
/*
Raw files on SD are:
little endian

*/
//Onboard is unsigned, mono, so data needs to be converted
void init_onboard_audio_no_DMA(FIL* file,uint32_t Frequency)
{//NOTE: Pin Is PIN 18 on the header board --CHECK PINOUTS!!!
  
  Freq = Frequency;
  fil = file;
  //(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);

  InitializeDAC();

  LCDGoHome();
  LCDPrint(" Playing buffer \n****************");

  f_lseek(fil,0);//Seek to start of raw data
  f_read(fil,buf,PRECISION*4,&count);

  buttonpress = 0;
  // TIMER1_IRQ_Index = 1;
  InputDone = 0;
  // InitTimer(Freq);
  // TIM_Cmd(LPC_TIM1,ENABLE);
  NVIC_EnableIRQ(PWM1_IRQn);
  PWM_Pin_Type P26 = {&LPC_PINCON->PINSEL4, (1<<0), &LPC_PWM1->MR1}; 
  initPWM(P26);
  initTimer(LPC_TIM0);
  while(InputDone == 0 && buttonpress == 0);
}

uint32_t record_onboard_audio_no_DMA(FIL* fil, uint32_t Frequency)
{
  UINT count = 0;
  Freq = Frequency;
  uint32_t counter;
  uint16_t buf1[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
  uint16_t buf2[PRECISION];//(uint16_t*)malloc(sizeof(uint16_t)*PRECISION);
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
