#include "main.h"



uint8_t SelectOne(char** items, char* header, uint8_t fileCount);
uint8_t TextEntry(char* result, char* header);

void EINT3_IRQHandler(void)
{
  LPC_SC->EXTINT = 1<<3;
  LPC_GPIOINT->IO0IntClr = (1<<10);
  key = GetKeyInput();
  if(key != prevKey && key != ' ')
  {
    buttonpress  = 1;
    prevKey = key;
    if(int_Handler_Enable)int_Handler_Funcs[int_Handler_Index]();
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
    free(lfill1);
    free(lfill2);
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
void I2S_PassThroughLoop()
{
  //int i =0;
  BufferOut = (uint32_t*)(I2S_SRC);
  //for(i=0;i<0x400;i++)BufferOut[i] = (i%30)*60;
  LCDClear();
  LCDPrint("I2S Passthrough\n......Mode......");
  TLV320_Start_I2S_Polling_Passthrough();
  I2S_Polling_Init(48000,I2S_MODE_POLLING);
  while(!buttonpress)
  {
    I2S_Polling_Read(BufferOut,1);
    I2S_Polling_Write(BufferOut,1);
  }
  I2S_DeInit(LPC_I2S);
  //free(BufferOut);
  buttonpress = 0;
}
void I2S_PassThroughInterrupt()
{
  BufferOut = (uint32_t*)(I2S_SRC);
  buffer = BufferOut;
  LCDClear();
  LCDPrint("I2S Passthrough\n.Interrupt Mode.");
  TLV320_Start_I2S_Polling_Passthrough();
  int_Handler_Enable =1;
  I2S_Polling_Init(48000,I2S_MODE_INTERRUPT);
  while(!buttonpress);
  int_Handler_Enable =0;
  WriteText("Finis");
  I2S_DeInit(LPC_I2S);
  //free(BufferOut);
  buttonpress = 0;
}

uint8_t NewFileSelection(char* newpath) {
	char** directoryNames = SDMallocFilenames();
    int directoryCount = SDGetDirectories("/", directoryNames);
    // SDFreeFilenames(directoryNames);
    int dir = SelectOne(directoryNames, "Location:\n", directoryCount);
    WriteText("oi\n\r");

    if (dir == 100) {
    	
    } else {
    	WriteText("Selected Directory: ");
    	WriteText(directoryNames[dir]);
    	WriteText("\n\r");
    	char newfilename[16];
    	TextEntry(newfilename, "Name? (# ends)\n");
    	return sprintf(newpath, "%s/%s", directoryNames[dir], newfilename);
    }
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
   } else {

    // DO SOMETHING WITH filenames[chosenIndex] HERE

    WriteText("chosen file: ");
    WriteText(filenames[chosenIndex]);
    WriteText("\n\r");
    break;
   }
  }

  if (chosenIndex == 100) {
    WriteText("selection cancelled");
  }

  SDFreeFilenames(filenames);
}

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

void Play_Audio()
{
  FIL fil;        /* File object */
  FRESULT fr;     /* FatFs return code */
  char fpath[100];// = browse_Files();
  buffer = (uint32_t*)(I2S_SRC);
  fr = f_mount(&FatFs, "", 0);
  if(fr)return;
  WriteText("mounted\r\n");
  fr = f_open(&fil, "meme.wav", FA_READ);
  if(fr)return;
  WriteText("opened\r\n");
  CS_HIGH();//Disable chip select until next use
  WAVE_HEADER w = Wav_Init(&fil);
  int_Handler_Enable =1;
  WriteText("opened wav\n\r");
  Init_I2S_Wav(w.NumChannels,w.SampleRate,w.BitsPerSample,&fil);
  while(!buttonpress);//loop until a buttonpress is received - TODO: set serial to change this value for pc play/pause
  int_Handler_Enable =0;
  I2S_DeInit(LPC_I2S);
  CS_LOW();
  f_close(&fil);
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
      uint32_t len = ReadText(data, 16);
      data[len] = '\0';
      WriteText(data);
    }
  }
  TLV320_DisablePassThrough();
  buttonpress =0;

}
/*
SSP:
Deselect sets the SSP pin high
Select sets the SSP pin low

*/
void FatRead()
{
    LCDGoHome();
    LCDPrint("FatFS Test      \nMode            ");
    WriteText("-");

    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */

    SSP_Cmd(LPC_SSP0,ENABLE);
    /* Register work area to the default drive */
    fr = f_mount(&FatFs, "", 0);
    if (fr)
    {
      sprintf(line, "Not Mounted WIth Code: %d\n\r",fr);
      WriteText(line);
      return;
    }
    /* Open a text file */
    fr = f_open(&fil, "a.wav", FA_READ);
    if (fr)
    {
      sprintf(line, "Exited with Error Code: %d\n\r",fr);
      WriteText(line);
      return;
    }

    /* Read every line and display it */
    uint y;
    char buffer [0x2000];

    while (!fr){
        fr = f_read(&fil,buffer,0x2000, &y);
        //n = sprintf(buffer,"%s\n\r", line);
        write_usb_serial_blocking(buffer,y);
    }
    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0);
}

void PC_Mode()
{
  LCDGoHome();
  LCDPrint("****PC**MODE****\n****************");
  InitSerInterrupts();
  WriteText("CONNECT|");
  while(!Connected);//Wait until response from PC is recorded
  Connected =0;
  LCDGoHome();
  LCDPrint("****PC**MODE****\n***CONNECTED.***");
  while(!Connected);//Wait for next input
  /*
  Possible inputs:
  Play - load the song specified, send the song length back to the embed, send a start flag when song starts so embed can keep track of position in song
        |Pause -stop the song, sending a timestamp to the pc interface
                |Play - restart the song, sending a timestamp to the pc interface
        |Next - Play the next song (open up directory and select next song before play
        |Back - Restart the song (f_seekl(fil,44))

  */
  //enable decoding from device
  //wait until instruction is recieved
  //do each task
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
    Menu();


    return 0;
}
void temp(){buttonpress = 0;}//Delete at your earliest convienience


// reusable UI functions

// they are here because they need global `buttonpress`
// interactive menu to choose one of the `items` displayed on line 2 of LCD
// line 1 is always `header`, fileCount is len(items)
// # - down, * - up, 0 - select , 8 - cancel
// cancel returns 100, select returns item index
uint8_t SelectOne(char** items, char* header, uint8_t fileCount) {
  LCDClear();
  LCDGoHome();

  uint8_t offset = 0;
  char line[16];
  char patline2[7] = "> %13s";

  while(1) {
    LCDGoHome();
    LCDPrint(header);
    if (fileCount == 0) {
      LCDPrint("<EMPTY>");
    } else {
      sprintf(line, patline2, items[offset]);
      LCDPrint(line);
    }
    
    while(!buttonpress);
    buttonpress = 0;

    switch (key) {
        case BUTTON_DOWN:
          if (offset + 1 < fileCount) {
            offset += 1;

          } else {
            offset = 0;
          }
          break;
        case BUTTON_UP:
          if (offset - 1 >= 0) {
            offset -= 1;
          } else {
            offset = fileCount - 1;
          }
          break;
        case BUTTON_CONFIRM:
          return offset;
        case BUTTON_CANCEL:
          return 100;
    }
  }
}

// BLOCKING, enter single string from keboard,
// stores in result, returns length
uint8_t TextEntry(char* result, char* header) {
	LCDClear();
	LCDGoHome();

	uint8_t i = 0;
	result[0] = 0x00;

	while (1) {
		LCDPrint(header);
		LCDPrint(result);
		LCDGoHome();
		while(!buttonpress);
		buttonpress = 0;
		if (key == '#') {
			break;
		} else {
			result[i] = key;
			result[i + 1] = 0x00; // make sure we dont show random garbis on lcd
			i++;
		}
	}

	return i;
}
