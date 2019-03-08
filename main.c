#include "main.h"


DWORD get_fattime (void)
{
  return(0);
}

uint8_t SelectOne(char** items, char* header, uint8_t fileCount);
uint8_t TextEntry(char* result, char* header);
uint8_t serialInitialized;
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
  LPC_PINCON->PINSEL4|=0x4000000;//
  LPC_PINCON->PINSEL0&=~(3<<20);//
  LPC_SC->EXTMODE = 1<<3;
  LPC_SC->EXTPOLAR = 1<<3;
  LPC_GPIO0->FIODIR &= ~(1<<10);//
  LPC_GPIOINT->IO0IntClr = (1<<10);
  LPC_GPIOINT->IO0IntEnF |= (1<<10);//
  LPC_SC->EXTINT = 1<<3; //Clear Pending Interrupts
  key = ' ';
  buttonpress = 0;
  NVIC_SetPriority(EINT3_IRQn, 0x00);
  NVIC_EnableIRQ(EINT3_IRQn);//
  __enable_irq();
}

void DrawMenu()
{
    char inputBuf[40];
    switch(SelMenuItem)
    {
      case 0:
        sprintf(inputBuf, "%-16s\n%-15s%c",
          MenuText[SelMenuItem],
          MenuText[SelMenuItem+1],'\xFF');
        break;
      case MENUTEXTNUM-2:
        sprintf(inputBuf, "%-15s\n%-16s",
          MenuText[SelMenuItem], "");
        break;
      default:
        sprintf(inputBuf, "%-15s%c\n%-15s%c",
          MenuText[SelMenuItem], '\x12',
          MenuText[SelMenuItem+1], '\xFF');
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
void I2S_PassThroughLoop()
{
  //int i =0;
  buffer = (uint32_t*)NewMalloc(sizeof(uint32_t)*BUFFER_SIZE);
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
  NewFree(buffer);
  buttonpress = 0;
}
void I2S_PassThroughInterrupt()
{
  buffer = (uint32_t*)NewMalloc(sizeof(uint32_t)*BUFFER_SIZE);
  LCDClear();
  LCDPrint("I2S Passthrough\n.Interrupt Mode.");
  TLV320_Start_I2S_Polling_Passthrough();
  int_Handler_Enable =1;
  I2S_Polling_Init(48000,I2S_MODE_INTERRUPT);
  while(!buttonpress);
  int_Handler_Enable =0;
  WriteText("Finis");
  I2S_DeInit(LPC_I2S);
  NewFree(buffer);
  buttonpress = 0;
}

void Passthrough()
{
  buffer = (uint32_t*)NewMalloc(sizeof(uint32_t)*BUFFER_SIZE);
  LCDClear();
  LCDPrint("I2S Passthrough\n.Interrupt Mode.");
  TLV320_Start_I2S_Polling_Passthrough();
  int_Handler_Enable =1;
  I2S_Polling_Init(48000,I2S_MODE_INTERRUPT);
  while(!buttonpress);
  int_Handler_Enable =0;
  WriteText("Finis");
  I2S_DeInit(LPC_I2S);
  NewFree(buffer);
  buttonpress = 0;
}

void FileInfo() {
  FileSelection();
  if(SELECTED_FILE[0] == '\0') {
    return;
  }
  WriteText(SELECTED_FILE);

  FIL fil;        /* File object */
  WriteText(" Filesize fresult: ");
  uint32_t fileSize = SDGetFileSize(SELECTED_FILE);

  sd_init();
  SDPrintFresult(f_open(&fil, SELECTED_FILE, FA_READ));
  WAVE_HEADER w = Wav_Init(&fil);
  sd_deinit(fs);

  char *showItems[4];
  char fileSizeItem[16], playingTimeItem[16], byteRateItem[16], sampleSizeItem[16];
  showItems[0] = fileSizeItem;
  showItems[1] = playingTimeItem;
  showItems[2] = byteRateItem;
  showItems[3] = sampleSizeItem;

  char header[16] = "File Details:\n";
  sprintf(fileSizeItem, "size: %db", (int)fileSize);
  sprintf(playingTimeItem, "time: %.2fs", fileSize / ((float)w.ByteRate));
  sprintf(byteRateItem, "rate: %dHz", (int)w.ByteRate);
  sprintf(sampleSizeItem, "ssize: %dbits", w.BitsPerSample);
  SelectOne(showItems, header, 4);
}

// Stores full path to new file in SELECTED_FILE
uint8_t NewFileSelection() {
	char** directoryNames = SDMallocFilenames();
    int directoryCount = SDGetDirectories("/", directoryNames);
    // SDFreeFilenames(directoryNames);
    int dir = SelectOne(directoryNames, "Location:\n", directoryCount);
    WriteText("oi\n\r");
    uint8_t tmp;
    if (dir == 100) {
      SDFreeFilenames(directoryNames);
      return 0;
    } else {
    	WriteText("Selected Directory: ");
    	WriteText(directoryNames[dir]);
    	WriteText("\n\r");
    	char newfilename[16];
    	TextEntry(newfilename, "Name? (# ends)\n");
    	tmp = sprintf(SELECTED_FILE, "%s/%s", directoryNames[dir], newfilename);
      SDFreeFilenames(directoryNames);
      SDCleanPath(SELECTED_FILE);
      return tmp;

    }

}
// Stores full path to selected file in SELECTED_FILE
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
  char fpath[100] = "/MEME2.WAV";// = browse_Files();
  Play(fpath);
  int_Handler_Enable =1;
  while(!buttonpress);//loop until a buttonpress is received - TODO: set serial to change this value for pc play/pause
  int_Handler_Enable =0;
  I2S_DeInit(LPC_I2S);
}

void Play_OnBoard_Audio()
{
  char fpath[100] = "/DUMMY/NGGYU32k.RAW";
  FIL fil;
  f_mount(&FatFs,"",0);
  f_open(&fil,fpath,FA_READ);
  init_onboard_audio_no_DMA(&fil,48000);
  f_close(&fil);
  f_mount(0, "", 0);
}

void Record_OnBoard_Audio()
{
  char fpath[100] = "/DUMMY/NGGYU32k.RAW";

  FIL fil;
  f_mount(&FatFs,"",0);
  FRESULT fr = f_open(&fil,fpath,FA_WRITE|FA_OPEN_EXISTING);
  if(fr != 0)
  {
    LCDClear();
    sprintf(fpath,"%s\nalready exists.",fpath);
    LCDPrint(fpath);
    Delay(30);
    LCDGoHome();
    LCDPrint("Will you:       \n># Stop >* Write");

    while(buttonpress == 0);
    if(key != '*')
    {
      f_mount(0, "", 0);
      return;
    }
    buttonpress = 0;

    f_open(&fil,fpath,FA_WRITE|FA_OPEN_ALWAYS);
  }


  TextEntry(fpath, "Pick a Frequency\n");
  uint32_t frequency = atoi(fpath);
/*
Add spliff header decoding here.

*/

  record_onboard_audio_no_DMA(&fil,frequency);
  f_close(&fil);
  f_mount(0, "", 0);
}

void Play(char* directory)
{
  uint32_t MEME[BUFFER_SIZE];
  FIL fil;        /* File object */
  FRESULT fr;     /* FatFs return code */
  buffer = MEME;
  fr = f_mount(&FatFs, "", 0);
  SDPrintFresult(fr);
  fr = f_open(&fil, directory, FA_READ);
  SDPrintFresult(fr);
  WAVE_HEADER w = Wav_Init(&fil);
  Init_I2S_Wav(w.NumChannels,w.SampleRate,w.BitsPerSample,&fil);
//
//   WriteText("disabling i2s\n\r");
//   WriteText("major OOF\n\r");
//   FileSelection();

//   FIL fil;        /* File object */
//   FRESULT fr;     /* FatFs return code */
//   buffer = (uint32_t*)(I2S_SRC);
//   fr = f_mount(&FatFs, "", 1);
//   SDPrintFresult(fr);
//   fr = f_open(&fil, SELECTED_FILE, FA_READ);
//   SDPrintFresult(fr);
//   // FATfs should handle it's own chip selects, I'm commenting these out
//   // CS_HIGH();//Disable chip select until next use
//   WAVE_HEADER w = Wav_Init(&fil);
//   int_Handler_Enable = 1;
//   Init_I2S_Wav(w.NumChannels,w.SampleRate,w.BitsPerSample,&fil);
//   while(!buttonpress);//loop until a buttonpress is received - TODO: set serial to change this value for pc play/pause
//   int_Handler_Enable =0;
//   I2S_DeInit(LPC_I2S);
//   // CS_LOW();
//
  NewFree(buffer);
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
  if(serialInitialized == 0)
  {
    InitSerInterrupts();
    serialInitialized = 1;
  }

  WriteText("CONNECT|");
  buttonpress = 0;
  while(READ_SERIAL[0] != 'A')if(buttonpress == 1)return;//Wait until response from PC is recorded
  POP_SERIAL;
  LCDGoHome();
  LCDPrint("****PC**MODE****\n***CONNECTED.***");
  uint8_t finished =0,playing=0;
  while(!finished)//Wait for next input
  {

    if(serialCommandIndex>0){//If there are instructions to process
      char output[50];
      sprintf(output,"COMMAND RECEIVED\n%s",READ_SERIAL);
      LCDGoHome();
      LCDPrint(output);
      switch (READ_SERIAL[0])//Note: I2S Interrupts are disabled here so this can process, and so must be restarted beforehand
      {
        case 'P'://play
          playing =1;
          Play(&READ_SERIAL[2]);
          break;
        case 'W'://pause
          playing = 0;
          NVIC_DisableIRQ(I2S_IRQn);
        case 'R'://resume
          playing = 1;
          NVIC_EnableIRQ(I2S_IRQn);
        case 'S':;//Send back settings data in the form S:settings array index,value|
          char* b = strchr(READ_SERIAL,'.');
          b = '\0';
          char* a  = strchr(READ_SERIAL,',');
          a  = '\0';
          settings[atoi(&READ_SERIAL[2])] = atoi(&READ_SERIAL[4]);
          break;
        case 'E'://Exit and return to main menu [TICK]
          finished = 1;
          break;
        case 'M':// Recording - The M is for Microphone
          A1();
          break;

        case 'F':; //Files - D3 for copying and deleting files.
          /*

          For testing the differerent serial functions before we get file management to work
          On the string sent from the PC:
          first byte = F for files
          Second byte is one of: P(lay),C(opy),D(elete),A(djust Volume),R(everse)
          From the second 2 bytes to the null is the file directory.

          E.g. FPa.wav|
          Byte one means File, P means play, then the last of the string is the path to the file.

          The only difference from this is change volume.
          Where I send:
          FA100 For max volume and FA0 for min.

          */

          char argument[100]; // File name or volume
          strcpy(argument,&READ_SERIAL[2]);
          char func = READ_SERIAL[1];

          switch (func)
          {
            case 'P':
              LCDClear();
              LCDPrint(argument);
              //Play the file in fileName.

            break;

            case 'C':
              /*
              Copy the file in fileName.
              1. Open the source
              2. Create the destination
              3. Use a loop to copy the files over
              5. Close both files.
              */
              LCDClear();
              LCDPrint("Starting copying");
              int error;
              char line[100]; /* Line buffer */
              WriteText("Starting Copying");
              error = f_copy(argument);
              sprintf(line, "Exited with Error Code: %d\n\r",error);
              WriteText(line);
              if(error){ //fr/error = 0 = F OK
                LCDClear();
                strcat(argument, "\n was made.");
                LCDPrint(argument);
                WriteText("Copied");
              }
            break;

            case 'D':
              //Delete the file in fileName.
              LCDClear();

              f_delete(argument);
              write_usb_serial_blocking("File deleted",9);

              strcat(argument, "\n was deleted.");
              LCDPrint(argument);
            break;

            case 'A':
              //Adjust the volume
              // Uses a different format than the others
              LCDClear();
              LCDPrint(argument);
            break;

            case 'R':
              // Reversing playback of the audio
              Reverse_Wav(argument);

            break;
            case 'N':
              switch(READ_SERIAL[2])
              {
                case 'd'://new folder

                break;
                case 'f'://new file

                break;
              }
            //New file and folder creation
              LCDClear();
              LCDPrint(argument);
            break;
          }

        case 'A':
            func = READ_SERIAL[1];
            switch (func)
            {
              case '1':
              LCDClear();
              LCDPrint("Starting A1");
              A1();
              //Play the file in fileName.
              break;

              case '2':
              //Copy the file in fileName.
              LCDClear();
              LCDPrint("Starting A2");
              break;

              case '3':
              //Delete the file in fileName.
              LCDClear();
              LCDPrint("Starting A3");
              break;

              case '4':
              //Adjust the volume
              // Uses a different format than the others
              A4();
              break;

              case '5':
              // Reversing playback of the audio
              LCDClear();
              LCDPrint("Testing mode");
              break;
            }
          break;

        case 'T': // Blue Screening
          //LCDClear();
          //LCDPrint("Windows 95 \nhas crashed");
          //WriteText("M");
          break;
        case 'B':;//send all browsing data back to embed
          char output[100];

          char ** fileList = SDMallocFilenames();
          char ** allDirs = SDMallocFilenames();
          int i,len = SDGetAllFilesandDirs(fileList,allDirs);
          for(i=0;i<len;i++)
          {
            sprintf(output,"%sf|",fileList[i]);
            WriteText(output);
          }
          SDFreeFilenames(fileList);

          for(i=0;i<len;i++)
          {
            sprintf(output,"%sd|",allDirs[i]);
            WriteText(output);
          }
          SDFreeFilenames(allDirs);

          WriteText("ed|");
          break;
      }
      POP_SERIAL;
      if(playing)NVIC_EnableIRQ(I2S_IRQn);
    }
  }
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



int main() {//CURRENTLY PIN 28 IS BEING USED FOR EINT3

  InitSerial();
  // serialInitialized = 0;
  SystemInit();
  DelayInit();
  I2CInit();
  IRQInit();
  LCDInit();
  LCDClear();
  // U2();
  initMalloc();
  // I2S_PassThroughLoop();
  Menu();


  return 0;
}
void temp(){buttonpress = 0;}//Delete at your earliest convienience


// reusable UI functions

// replace all occurances of `f` with `r`
void strreplace(char* str, char f, char r) {
  int i = 0;
  while (str[i] != '\0') {
    if (str[i] == f) {
      str[i] = r;
    }
    i ++;
  }
}

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
  char patline2[7] = ">%15s";

  uint8_t i = 0;
  // zeros display as the downarrow thingies
  // so we're replacing them with capital Os
  // don't worry, nobody will ever know
  for (i = 0; i < fileCount; i ++) {
    strreplace(items[i], '0', 'O');
  }

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

/*
    File functions
*/

void f_delete(char* filepath)
{
  FIL fil;        /* File object */
  char line[100]; /* Line buffer */
  FRESULT fr;     /* FatFs return code */
  FATFS *fs;
}

void U2() {
  char newname[32];
  NewFileSelection();
  WriteText(SELECTED_FILE);
  LCDClear();
  LCDPrint("Recording...\n(# ends)");
  uint32_t iobuff[32];
  UINT written = 0;

  TLV320_Start_I2S_Polling_Passthrough();
  I2S_Polling_Init(48000,I2S_MODE_POLLING);

  sd_init();
  FIL fil;
  SDPrintFresult(f_open(&fil, SELECTED_FILE, FA_WRITE | FA_CREATE_ALWAYS));
  while(key != '#')
  {
    I2S_Polling_Read(iobuff,32);
    SDPrintFresult(f_write(&fil, iobuff, 32, &written));
    if (written < 32) {
      WriteText("kekerino");
    }
  }
  WriteText("done writing\n\r");
  f_close(&fil);
  sd_deinit();

  I2S_DeInit(LPC_I2S);
}

void A2()
{
  buffer = (uint32_t*)NewMalloc(sizeof(uint32_t)*BUFFER_SIZE);
  LCDGoHome();
  TLV320_Start_I2S_Polling_Passthrough();
  int_Handler_Enable =1;
  char result[16];
  TextEntry(result, "Pick a Frequency\n");
  uint32_t frequency = atoi(result);
  LCDPrint("**PLAYING SINE**\n******WAVE******");
  I2S_Create_Sine(frequency);
  while(!buttonpress);
  int_Handler_Enable =0;
  I2S_DeInit(LPC_I2S);
  NewFree(buffer);
}

int f_copy(char* filepath){
  WriteText("Starting the copy");
  FIL fsrc, fdst;    /* File objects */
  UINT br, bw;         /* File read/write count */
  FRESULT fe;     /* FatFs return code */

  f_mount(&fs, "", 0);

  /* Open a text file */
  fe = f_open(&fsrc, filepath, FA_READ);
  if (fe) return (int)fe;
/*
  int len = strlen(filepath);
  filepath[len-3] = '\0';
  strcat(filepath,".copy");
*/
  filepath = "garb.txt";
  fe = f_open(&fdst, filepath, FA_WRITE | FA_CREATE_ALWAYS);
  if (fe) return (int)fe;

  /* Copy source to destination */
  for (;;) {
      fe = f_read(&fsrc, buffer, sizeof buffer, &br);  /* Read a chunk of source file */
      if (fe || br == 0) break; /* error or eof */
      fe = f_write(&fdst, buffer, br, &bw);            /* Write it to the destination file */
      if (fe || bw < br) break; /* error or disk full */
  }


  /* Close the file */
  f_close(&fsrc);
  f_close(&fdst);
  //Unmount the file system
  f_mount(0, "", 0);
  WriteText("File wrote");
  return 0;
}

/*
    Tasks
*/

void A1()
{/*Considerations for alaiasing and nyquists theroem should be made in your solution*/
  buffer = (uint32_t*)NewMalloc(sizeof(uint32_t)*BUFFER_SIZE);
  uint32_t BufferOut[1];
  char output[34];
  LCDGoHome();
  LCDPrint("**Display**Rec**\n*****BUFFER*****");
  TLV320_Start_I2S_Polling_Passthrough();
  I2S_Polling_Init(48000,I2S_MODE_POLLING);
  while(key != '#')
  {
    while(!buttonpress);

    I2S_Polling_Read(BufferOut,1);
    WriteText("about to read\n\r");
    sprintf(output,"  Just Read In  \n   0x%04X   ",(signed int)BufferOut[0]);//print the left channel as a signed integer
    WriteText(output);
    LCDGoHome();
    LCDPrint(output);
    buttonpress = 0;
  }
  I2S_DeInit(LPC_I2S);
  NewFree(buffer);
}

void A3()
{
  LCDGoHome();
  LCDPrint("A3 Demo \n Recording Audio");
}

void A4()
{
  LCDClear();
  LCDPrint("A4 Demo \nPlaying from SD");

  FIL fil;        /* File object */
  char line[100]; /* Line buffer */
  FRESULT fr;     /* FatFs return code */
  FATFS *fs;

  fs = malloc(sizeof(FATFS));
  fr = f_mount(fs, "", 0);

  if (fr)
  {
    sprintf(line, "Not Mounted With Code: %d\n\r",fr);
    return (int)fr;
  }

  /* Open a text file */
  fr = f_open(&fil, "a.wav", FA_READ);

  if (fr)
  {
    sprintf(line, "Exited with Error Code: %d\n\r",fr);
    WriteText(line);
    return (int)fr;
  }

  /* Read every line and display it */
  uint y;
  char buffer [0x20];

  while (!fr){
      fr = f_read(&fil,buffer,0x20, &y);
      //n = sprintf(buffer,"%s\n\r", line);
      write_usb_serial_blocking(buffer,y);
  }

  /* Close the file */
  f_close(&fil);

  //Unmount the file system
  f_mount(0, "", 0);
  free(fs);
  write_usb_serial_blocking("EndOfFile",9);
  return 0;
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


// relies on 16bit samplesize (swaps 2-byte chunks around)
#define WAV_H_SIZE 44
void Reverse_Wav(char* src) {
  char dst[32];
  sprintf(dst, "/inv%s", src + 1);

  sd_init();
  FIL fsrc, fdst;
  f_open(&fsrc, src, FA_READ);
  f_open(&fdst, dst, FA_WRITE | FA_CREATE_ALWAYS);

  char fbuff[256], tmp[2];
  uint32_t countread, dummy;
  f_read(&fsrc, fbuff, WAV_H_SIZE, &countread);
  f_write(&fdst, fbuff, WAV_H_SIZE, &dummy);

  char pbuff[32];

  uint32_t fsize = (uint32_t)f_size(&fsrc) - 43, i = 0, j = 0;
  for (i = fsize / 256; i > 0; i--) {
    f_lseek(&fsrc, WAV_H_SIZE + i * 256);
    f_read(&fsrc, fbuff, 256, &countread);
    for (j = 0; j < countread / 2 - 1; j ++) {
      tmp[0] = fbuff[j];
      tmp[1] = fbuff[j + 1];
      fbuff[j] = fbuff[countread - j - 2];
      fbuff[j + 1] = fbuff[countread - j - 1];
      fbuff[countread - j - 2] = tmp[0];
      fbuff[countread - j - 1] = tmp[1];
    };
    f_write(&fdst, fbuff, countread, &dummy);
  }

  f_close(&fsrc);
  f_close(&fdst);

  f_unlink(src);
  f_rename(dst, src);

  sd_deinit();
}
