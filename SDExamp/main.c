#include <string.h>
#include "rtc176x.h"
#include "uart176x.h"
#include "ff.h"
#include "diskio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <serial.h>

/* Read a text file and display it */

FATFS FatFs;   /* Work area (filesystem object) for logical drive */

int main (void)
{

    serial_init();
    write_usb_serial_blocking("aaa",3);
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */

    /* Register work area to the default drive */
    f_mount(&FatFs, "", 0);
    write_usb_serial_blocking("bbb",3);

    /* Open a text file */
    fr = f_open(&fil, "/message.txt", FA_READ);
    if (fr) return (int)fr;
    

    /* Read every line and display it */
    while (f_gets(line, sizeof line, &fil)) {
     write_usb_serial_blocking(line,10);
    }

    /* Close the file */
    f_close(&fil);

    return 0;
}