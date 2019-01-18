#include "main.h"

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<5);
  key = GetKeyInput();
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
  NVIC_EnableIRQ(EINT3_IRQn);
  __enable_irq();
}

void DrawArrows()
{
    LCDGoHome();
    LCDCursorShift(15-strlen(MenuText[SelMenuItem]));
    if(SelMenuItem == 0)
    {
        
        LCDNewLine();
        LCDPrint("#\x30");
    }
    else if(SelMenuItem == MENUTEXTNUM - 2)
    {
        LCDPrint("*\x02");
    }
    else()
    {
        LCDPrint("*\x02");
        LCDNewLine();
        LCDPrint("#\x30");
    }  
}
void DrawMenu()
{
    char[80] inputBuf;
    sprintf(inputBuf,"%s\n%s",MenuText[SelMenuItem],MenuText[SelMenuItem]);
    LCDClear(); 
    LCDPrint(inputBuf);
    DrawArrows();
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
            switch(key)
            {
                case: BUTTON_UP
                    --SelMenuItem;
                    SelMenuItem = max(SelMenuItem,0);
                    break;
                case: BUTTON_DOWN
                    ++SelMenuItem;
                    SelMenuItem = min(SelMenuItem,MENUTEXTNUM-2;
                    break;
                case: BUTTON_SEL
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
    SystemInit();
    DelayInit();
    I2CInit();
    IRQInit();
    LCDInit();
    Menu();


    return 0;
}