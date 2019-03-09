#include "main.h"

FATFS FatFs;




void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<10);
  key = GetKeyInput();
  if(key != prevKey && key != ' ')
  {
    buttonpress  = 1;
    prevKey = key;
    timerDone = 1;
    breakout = 1;
  }
  else if (key == ' ')
  {
    prevKey = ' ';
  }
}

void IRQInit()
{
  LPC_PINCON->PINSEL4|=0x4000000;
  LPC_PINCON->PINSEL0&=~(3<<20);
  LPC_SC->EXTMODE = 1<<3;
  LPC_SC->EXTPOLAR = 1<<3;
  LPC_GPIO0->FIODIR &= ~(1<<10);
  LPC_GPIOINT->IO0IntClr = (1<<10);
  LPC_GPIOINT->IO0IntEnF |= (1<<10);
  LPC_SC->EXTINT = 1<<3; 
  key = ' ';
  buttonpress = 0;
  NVIC_SetPriority(EINT3_IRQn, 0x00);
  NVIC_EnableIRQ(EINT3_IRQn);
  __enable_irq();
}


uint16_t audioBuffer[2];
void TIMER1_IRQHandler(void)
{//Send the left channel
  DO_16_TIMES(SEND_BIT,audioBuffer[0]);
  //Send the right channel
  DO_16_TIMES(SEND_BIT,audioBuffer[1]);
  
}
void InitTimer(uint32_t Frequency)
{
  uint8_t mseconds = (uint8_t)((1/(float)Frequency) * 1000000);
  TIM_TIMERCFG_Type TIM_ConfigStruct;
  TIM_MATCHCFG_Type TIM_MatchConfigStruct;
  TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct.PrescaleValue  = mseconds;
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


void Play_File()
{
  LPC->PINSEL0 |= 1;//setup GPIO Pins for 
  InitTimer(48000);
  TIM_Cmd(LPC_TIM1,ENABLE);
  NVIC_EnableIRQ(TIMER1_IRQn);
}

int main()
{
    SystemInit();
    InitSerial();
    I2CInit();
    IRQInit();
    LCDInit();
    LCDClear();

    Play_File();

    return 0;
}

/*
TLV 320 Information:

BCLK: GPIO Timer 0

*/