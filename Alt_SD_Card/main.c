#include "main.h"

DWORD get_fattime (void)
{
	return 0;
}

int SD_Test (void) {
  FIL fil;        /* File object */
  char line[100]; /* Line buffer */
  FRESULT fr;     /* FatFs return code */
  char default_path = 0;

  WriteText("Mount Check Start\n");
  fr = f_mount(&FatFs, &default_path, 1);
  WriteText("Mount Check Done\n");

  if (fr)
  {
    sprintf(line, "Not Mounted With Code: %d\n\r",fr);
    WriteText(line);
    return (int)fr;
  }

  /* Open a text file */
  WriteText("File Read Start\n");
  fr = f_open(&fil, "a.wav", FA_READ);
  WriteText("File Read End\n");

  if (fr)
  {
    sprintf(line, "Exited with Error Code: %d\n\r",fr);
    WriteText(line);
    return (int)fr;
  }
  WriteText("No Errors!\n");

  /* Read every line and display it */
  uint y;
  char buffer [0x20];
  WriteText("Buffer Initialised\n");

  while (!fr){
      fr = f_read(&fil,buffer,0x20, &y);
      //n = sprintf(buffer,"%s\n\r", line);
      write_usb_serial_blocking(buffer,y);
  }

  /* Close the file */
  f_close(&fil);

  //Unmount the file system
  f_mount(0, "", 0);

  return 0;
}

int main (void) {
  InitSerial();
  WriteText("Start\n");
  SD_Test();
  WriteText("Test Complete\n");
  return 0;
}
