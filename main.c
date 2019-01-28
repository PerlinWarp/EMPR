#include "main.h"

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<5);
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
  LPC_GPIO0->FIODIR &= ~(1<<5);
  LPC_GPIOINT->IO0IntClr = (1<<5);
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntEnF |= (1<<5);

  /*LPC_PINCON->PINSEL1 = (1<<7);//Check which pins the pcf8574 uses
  LPC_SC->EXTMODE = 1; //Edge Triggered
  LPC_SC->EXTPOLAR = 1;//Falling vs Rising Edge*/
  key = ' ';
  buttonpress =0;
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
    lfill1[s1-2] = '\x02';
    lfill1[s1-1] = '\0';
    for(i = 0;i<s2;i++)lfill2[i]=' ';
    WriteText("\n\r1");
    lfill2[s2-2] = '\x30';
    lfill2[s2-1] = '\0';
    WriteText("\n\r2");
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
    int optionSelected = 0;
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
                    break;
                case BUTTON_DOWN:
                    ++SelMenuItem;
                    SelMenuItem = min(SelMenuItem,MENUTEXTNUM-2);
                    break;
                case BUTTON_SEL:
                    optionSelected = 1;
                    break;
            }
            DrawMenu();
            buttonpress = 0;
        }
    }

}

int main()
{
    InitSerial();
    SystemInit();
    DelayInit();

    IRQInit();
    I2CInit();
    LCDInit();
    LCDClear();
    Menu();


    return 0;
}
