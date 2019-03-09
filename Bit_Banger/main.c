#include "main.h"

FATFS FatFs;
uint16_t audioBuffer[2];
FIL* fil;
uint8_t breakout;
volatile int buttonpress;
volatile char prevKey,key;
char SELECTED_FILE[40];

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<10);
  key = GetKeyInput();
  if(key != prevKey && key != ' ')
  {
    buttonpress  = 1;
    prevKey = key;
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
void FileSelection() {
  char path[32] = "/", header[16];
  char **filenames = SDMallocFilenames();
  uint8_t fileCount, chosenIndex;
  while(1) {
    fileCount = SDGetFiles(path, filenames);

    sprintf(header, "%s:\n", path);

    chosenIndex = SelectOne(filenames, header, fileCount);

    if (chosenIndex == 100) { // move up a directory
      int lastIdx = strlen(path) - 1;
      while (path[lastIdx] != '/') {
        path[lastIdx] = 0x00;
        lastIdx -= 1;
      }
      if(lastIdx > 0) {
        path[lastIdx] = 0x00; // elmchan does not like trailing slash
        continue;
      } else {
        break;
      }
    }

   if(filenames[chosenIndex][0] == 'd') {
      int curlen = strlen(path);
      sprintf(path + curlen, "/%s", filenames[chosenIndex] + 2);
      SDCleanPath(path);
   } else {
    SELECTED_FILE[0] = '/';
    sprintf(SELECTED_FILE, "/%s/%s", path, filenames[chosenIndex] + 2);
    SDCleanPath(SELECTED_FILE);

    break;
   }
  }

  if (chosenIndex == 100) {
    SELECTED_FILE[0] = '\0';
    // WriteText("selection cancelled");
  }

  SDFreeFilenames(filenames);
}

/*Test and see if additional complexity could be added to get wave files to play using a byteswap*/
void TIMER1_IRQHandler(void)
{
  //Clear interrupts
  unsigned int isrMask,x;
  isrMask = LPC_TIM1->IR;
  LPC_TIM1->IR = isrMask;
  //Send the left channel
  LPC_GPIO0->FIOCLR = LRCIN;
  DO_16_TIMES(SEND_BIT,audioBuffer[0]);
  //Send the right channel
  LPC_GPIO0->FIOSET = LRCIN;
  DO_16_TIMES(SEND_BIT,audioBuffer[1]);
  
  f_read(fil,audioBuffer,4,&isrMask);//read full the audioBuffer
  if(isrMask != 4)NVIC_DisableIRQ(TIMER1_IRQn);
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
  LPC_PINCON->PINSEL0 &=~((3<<(BCLK*2))|(3<<(DIN*2))|(3<<(LCRIN*2)));//setup GPIO Pins 
  LPC_GPIO0->FIODIR |= (1<<BCLK)|(1<<DIN)|(1<<LRCIN);//0 = input, 1 = output

  FileSelection();

  fil = (FIL*)malloc(sizeof(FIL));
  f_mount(&FatFs,"",0);
  f_open(fil,SELECTED_FILE,FA_READ);
  f_lseek(fil,0);

  LCDGoHome();
  LCDPrint("**PLAYING FILE**\ndooddodoodoododo");

  buttonpress =  breakout = 0;
  TLV320_PlayWav();

  InitTimer(SONG_FREQUENCY);
  TIM_Cmd(LPC_TIM1,ENABLE);
  NVIC_EnableIRQ(TIMER1_IRQn);

  while(buttonpress == 0 && breakout == 0);

  TIM_Cmd(LPC_TIM1,DISABLE);
  buttonpress =  breakout = 0;
  f_close(fil);
  f_mount(0, "", 0);
  free(fil);
}

int main()
{
    SystemInit();
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