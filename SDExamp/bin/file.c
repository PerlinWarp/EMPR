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
    fr = f_open(&fil, "message.txt", FA_READ);
    if (fr) return (int)fr;

    /* Read every line and display it */
    while (f_gets(line, sizeof line, &fil)) {
        int n;
        char buffer [250];                                      /* It is a file. */
        n = sprintf(buffer,"%s\n\r", line);
        write_usb_serial_blocking(buffer, n);
    }

    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0); 
    return 0;
}