#include "main.h"

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<10);
  key = GetKeyInput();
  WriteText("IRQ Polled");
  if(key != prevKey && key != ' ')
  {
    buttonpress  = 1;
    prevKey = key;
    WriteText("button Pressed");
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
  LPC_SC->EXTINT = 1<<3; //Clear Pending Interrupts
  key = ' ';
  buttonpress = 0;
  NVIC_EnableIRQ(EINT3_IRQn);
  __enable_irq();
}

void DrawMenu()
{
    char inputBuf[80];
    char* lfill1,*lfill2;
    int i;
    int s1= max(17-strlen(MenuText[SelMenuItem]),2),s2 = max(17-strlen(MenuText[SelMenuItem+1]),2);
    lfill1 = (char*)malloc(s1);
    lfill2 = (char*)malloc(s2);
    for(i = 0;i<s1;i++)lfill1[i]=' ';
    lfill1[s1-2] = '\x12';
    lfill1[s1-1] = '\0';
    for(i = 0;i<s2;i++)lfill2[i]=' ';
    lfill2[s2-2] = '\x30';
    lfill2[s2-1] = '\0';
    switch(SelMenuItem)
    {
      case 0:
        sprintf(inputBuf,"%s\n%s%s",MenuText[SelMenuItem],MenuText[SelMenuItem+1],lfill2);
        break;
      case MENUTEXTNUM-2:
        sprintf(inputBuf,"%s%s\n%s",MenuText[SelMenuItem],lfill1,MenuText[SelMenuItem+1]);
        break;
      default:
        sprintf(inputBuf,"%s%s\n%s%s",MenuText[SelMenuItem],lfill1,MenuText[SelMenuItem+1],lfill2);
    }

    LCDClear();
    LCDPrint(inputBuf);
}
void Menu()
{
    SelMenuItem = 0;
    int optionSelected = 0,changed =1;
    DrawMenu();
    while(!optionSelected)
    {
        if(buttonpress == 1)
        {
            WriteText("ButtonFound\n\r");
            switch(key)
            {
                case BUTTON_UP:
                    --SelMenuItem;
                    SelMenuItem = max(SelMenuItem,0);
                    if(SelMenuItem ==0)changed =0;
                    break;
                case BUTTON_DOWN:
                    ++SelMenuItem;
                    if(SelMenuItem ==MENUTEXTNUM-1)changed =0;
                    SelMenuItem = min(SelMenuItem,MENUTEXTNUM-2);
                    break;
                case BUTTON_SEL:
                    optionSelected = 1;
                    break;
                default:
                    Audio_buf = (char*)malloc(sizeof(char)*BUF_LENGTH);
                    changed =0;
            }
            if(changed)DrawMenu();
            changed =1;
            buttonpress = 0;
        }
    }

}

void RecordLoop()
{
  //Enable I2S
  Init_I2S(uint32_t* BufferOut,uint32_t BufferOutWidth,uint32_t* BufferIn,uint32_t BufferInWidth);
  //Check for interrn
}

void PlayLoop()
{}

int main()
{//CURRENTLY PIN 28 IS BEING USED FOR EINT3
    InitSerial();
    SystemInit();
    DelayInit();
    I2CInit();
    IRQInit();
    LCDInit();
    LCDClear();
    Menu();


    return 0;
}
