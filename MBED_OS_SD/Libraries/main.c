#include "SerialIO.h"
#include "ff.h"

int SD_Test(void) {
  FIL fil;        /* File object */
  char line[100]; /* Line buffer */
  FRESULT fr;     /* FatFs return code */
  FATFS FatFs;

  WriteText("Mount Check Start\n");
  fr = f_mount(1, &FatFs);
  WriteText("Mount Check Done\n");

  if (fr)
  {
    sprintf(line, "Not Mounted With Code: %d\n\r",fr);
    WriteText(line);
    return (int)fr;
  }

  /* Close the file */
  f_close(&fil);

  //Unmount the file system
  f_mount(0, 0);

  return 0;
}

int main(void) {
  InitSerial();
  WriteText("Serial On");
  SD_Test();
  WriteText("SD Test End");
  
  return 0;
}
