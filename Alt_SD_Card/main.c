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

void Mount_Test(void) {

}

int main (void) {
  InitSerial();
  WriteText("Start\n");

  char i;
	FRESULT result;
	UINT s1; 
	//RTCTime rtc;
	DSTATUS status;
	FATFS drive;		/* File system object for each logical drive */
	FIL file;			// File objects
  char buf[40]="SD card experiment\r\n";

  WriteText("Begin Disk Init\n");
  status = disk_initialize(0); //Prepare the card
  WriteText("Disk Initialised\n");
	result = f_mount(&drive, "", 0); //Open card
  WriteText("Disk Mounted\n");
	result = f_open(&file, "test.txt", FA_CREATE_NEW);//The new file is formed.
	result = f_close(&file); //Close the file created
	result = f_open(&file, "test.txt", FA_WRITE);// Open the file for writing
	for(i=0;i<5;i++) result = f_write(&file, buf,strlen(buf), &s1);//file type.	
	result = f_close(&file); //Close file
	result = f_open(&file, "test.txt", FA_WRITE);//Open the file again.
	result = f_lseek(&file, file.fptr);//Go to the end of the file
	result = f_write(&file, "Add to the end of the file",27, &s1);//Write to end the file.
	result = f_close(&file);// Close the file.

  WriteText("Test Complete\n");
  return 0;
}
