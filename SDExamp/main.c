#include <string.h>
#include "diskio.h"
#include "ff.h"
#include <serial.h>
#include <stdio.h>


/* Read a text file and display it */

FATFS FatFs;   /* Work area (filesystem object) for logical drive */

int main (void)
{
    serial_init();
    write_usb_serial_blocking("-", 1);

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
        write_usb_serial_blocking(buffer, y);
    }
    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0); 
    return 0;

}
