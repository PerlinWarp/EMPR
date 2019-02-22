#include "main.h"

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<10);
  key = GetKeyInput();
  if(key != prevKey && key != ' ')
  {
    buttonpress  = 1;
    prevKey = key;
    if(i2s_Interrupt_Mode)NVIC_DisableIRQ(I2S_IRQn);//Frightful fudge to fix a flooey
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
  NVIC_SetPriority(EINT3_IRQn, 0x00);
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
        sprintf(inputBuf,"%s  \n%s%s",MenuText[SelMenuItem],MenuText[SelMenuItem+1],lfill2);
        break;
      case MENUTEXTNUM-2:
        sprintf(inputBuf,"%s%s\n                ",MenuText[SelMenuItem],lfill1);
        break;
      default:
        sprintf(inputBuf,"%s%s\n%s%s",MenuText[SelMenuItem],lfill1,MenuText[SelMenuItem+1],lfill2);
    }

    LCDGoHome();
    LCDPrint(inputBuf);
}
void Menu()
{

    SelMenuItem =  0;
    int optionSelected = 0,changed =1;
    DrawMenu();
    while(!optionSelected)
    {
        if(buttonpress == 1)
        {
            switch(key)
            {
                case BUTTON_UP:
                    if(!SelMenuItem)changed =0;
                    --SelMenuItem;
                    SelMenuItem = max(SelMenuItem,0);
                    break;
                case BUTTON_DOWN:
                    ++SelMenuItem;
                    if(SelMenuItem ==MENUTEXTNUM-1)changed =0;
                    SelMenuItem = min(SelMenuItem,MENUTEXTNUM-2);
                    break;
                case BUTTON_SEL:
                    buttonpress = 0;
                    menuFuncs[SelMenuItem]();//Jump to the appropriate function
                    break;
                case BUTTON_SEL_2:
                    buttonpress = 0;
                    if(SelMenuItem < MENUTEXTNUM){
                      menuFuncs[SelMenuItem+1]();
                    }

                    break;
                case 'C':
                {
                    changed = 0;
                    break;}
                case 'D':
                {
                    changed = 0;
                    break;}
                default:
                  {
                    buttonpress = 0;
                    menuFuncs[(int)(key - '0')]();
                  }
            }
            if(changed)DrawMenu();
            changed =1;
            buttonpress = 0;
        }
    }

}

/*void RecordLoop()
{
  //Enable I2S
  //Init_I2S(uint32_t* BufferOut,uint32_t BufferOutWidth,uint32_t* BufferIn,uint32_t BufferInWidth);
  //Check for interrn
}*/

// void PlayLoop()
// {
//   int i =0;
//   BufferOut = (uint32_t*)malloc(sizeof(uint32_t*)*BUFO_LENGTH);
//   BufferIn = (uint32_t*)malloc(sizeof(uint32_t*)*BUFI_LENGTH);
//   for(i=0;i<BUFO_LENGTH;i++)BufferOut[i] = (i%30)*60;
//   Init_I2S(BufferOut,BUFO_LENGTH,BufferIn,BUFI_LENGTH);
//   TLV320_EnableOutput();
//   EnableOutput();
//   EnableInput();
//   LCDClear();
//   LCDPrint("Audio Mode\nPress Any Key");
//   while(!buttonpress);//Loop until new input
//   char outp[40];
//   sprintf(outp,"%lu\n\r",Channel0_TC);
//   WriteText(outp);
//   DisableOutput();
//   I2S_DeInit(LPC_I2S);
//   free(BufferOut);
//   free(BufferIn);
//   buttonpress = 0;
// }
// void I2S_PassThroughLoop()
// {
//   //int i =0;
//   BufferOut = (uint32_t*)(I2S_SRC);
//   //for(i=0;i<0x400;i++)BufferOut[i] = (i%30)*60;
//   LCDClear();
//   LCDPrint("I2S Passthrough\n......Mode......");
//   TLV320_Start_I2S_Polling_Passthrough();
//   I2S_Polling_Init(48000,I2S_MODE_POLLING);
//   while(!buttonpress)
//   {
//     I2S_Polling_Read(BufferOut,1);
//     I2S_Polling_Write(BufferOut,1);
//   }
//   I2S_DeInit(LPC_I2S);
//   //free(BufferOut);
//   buttonpress = 0;
// }
// void I2S_PassThroughInterrupt()
// {
//   BufferOut = (uint32_t*)(I2S_SRC);
//   buffer = BufferOut;
//   LCDClear();
//   LCDPrint("I2S Passthrough\n.Interrupt Mode.");
//   TLV320_Start_I2S_Polling_Passthrough();
//   i2s_Interrupt_Mode =1;
//   I2S_Polling_Init(48000,I2S_MODE_INTERRUPT);
//   while(!buttonpress);
//   i2s_Interrupt_Mode =0;
//   WriteText("Finis");
//   I2S_DeInit(LPC_I2S);
//   //free(BufferOut);
//   buttonpress = 0;
// }
// void MASSIVE_TEST()
// {
//   LCDClear();
//   LCDGoHome();
//   char outidy[33];
//   int big = 4;
//   sprintf(outidy,"%s\n%d%d","woahtherehorsey",EGG_ON_TOAST(big),big);
//   LCDPrint(outidy);
//   while(!buttonpress);
//   buttonpress =0;
// }
/*void I2S_DmaPassThrough(){
  Init_I2S(BufferOut,BUFO_LENGTH,BufferOut,BUFO_LENGTH);
  TLV320_Start_I2S_Polling_Passthrough();
  EnableOutput();
  LCDClear();
  LCDPrint("Audio Mode\nPress Any Key");
  while(!buttonpress);//Loop until new input
  free(BufferOut);
  free(BufferIn);
  buttonpress =0;
}*/
void PassThroughLoop()
{
  LCDClear();
  LCDPrint("Please Press any\nkey to continue");
  TLV320_EnablePassThrough();
  while(!buttonpress);//Loop as nothing needs to be done
  TLV320_DisablePassThrough();
  buttonpress = 0;
}
void UART_Mode()
{//Note: pyserial likely sends utf 16, which is being split into h0e0l0l0o0
  //Here, the audio is set to bypass, and the LCD is configured to display any input from UART
  TLV320_EnablePassThrough();
  InitSerInterrupts();
  LCDGoHome();
  LCDPrint("...UART..MODE...\n________________");
  char data[17],oldData[17];
  oldData[17] = '\0';
  while(!buttonpress)
  {
    if(CHECK_BUFFER(rbuf.rx_head)!=rbuf.rx_tail)
    {
      /*LCDGoHome();
      LCDNewLine();*/
      uint32_t len = ReadText(data, 16);
      data[len] = '\0';
      WriteText(data);
      /*if(strcmp(data,oldData))//If old and new values differ
      {
        WriteText(data);
        strcpy(oldData,data);
      }*/
    }
  }
  TLV320_DisablePassThrough();
  buttonpress =0;

}


void FatRead()
{
    LCDGoHome();
    LCDPrint("FatFS Audio Read\nMode            ");
    WriteText("-");

    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */


    /* Register work area to the default drive */
    f_mount(&FatFs, "", 0);

    /* Open a text file */
    fr = f_open(&fil, "a.wav", FA_READ);
    if (fr) return (int)fr;

    /* Read every line and display it */
    uint y;
    int n;
    char buffer [0x2000];

    while (!fr){
        fr = f_read(&fil,buffer,0x2000, &y);
        //n = sprintf(buffer,"%s\n\r", line);

        write_usb_serial_blocking(buffer,y);
    }
    WriteText("Closing the file \n");
    
    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0);
}
/*
  
*/
void Read2Audio()
{
    LCDGoHome();
    LCDPrint("FatFS Audio Play\nMode            ");
    WriteText("-");
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */


    /* Register work area to the default drive */
    f_mount(&FatFs, "", 0);

    /* Open a text file */
    fr = f_open(&fil, "a.wav", FA_READ);
    if (fr) return (int)fr;
    TLV320_Start_I2S_Polling_Passthrough();
    /* Read every line and display it */
    fre = f_lseek(&fil, 0x20636);
    WriteText("Enabling Interrupts");
    I2S_A_Polling_Init(48000,I2S_MODE_INTERRUPT);
    
    WriteText("MEME");
    while(fre){}
    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0);
}


int main()
{//CURRENTLY PIN 28 IS BEING USED FOR EINT3
    InitSerial();
    SystemInit();
    DelayInit();
    I2CInit();
    IRQInit();
    LCDInit();
    LCDClear();
    WriteText("Start");
    Menu();


    return 0;
}
void temp(){buttonpress = 0;}//Delete at your earliest convienience
