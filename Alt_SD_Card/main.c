#include "main.h"

void FatRead() {
    WriteText("-");

    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */

    WriteText("FatFs Init\n");

    SSP_Cmd(LPC_SSP0,ENABLE); // AHHHHHHHHH
    WriteText("SSP0_Enable\n");
    /* Register work area to the default drive */
    fr = f_mount(&FatFs, "", 1);
    WriteText("fr and f_mount\n");
    if (fr)
    {
      sprintf(line, "Not Mounted WIth Code: %d\n\r",fr);
      WriteText(line);
      return (int)fr;
    }
    /* Open a text file */
    WriteText("B4 2nd fr\n");
    fr = f_open(&fil, "a.wav", FA_READ);
    WriteText("After 2nd fr\n");
    if (fr)
    {
      sprintf(line, "Exited with Error Code: %d\n\r",fr);
      WriteText(line);
      return (int)fr;
    }
    WriteText("No Errors!\n");
    /* Read every line and display it */
    uint y;
    int n;
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
}

int main() {
    InitSerial();
    FatRead();

    return 0;
}
