#include <string.h>
#include "diskio.h"
#include "ff.h"
#include <SerialIO.h>
#include <stdio.h>

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
                int n;
                char buffer [250];                                      /* It is a file. */
                n = sprintf(buffer,"%s/%s\n", path, fno.fname);
                WriteText(buffer);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

/* Read a text file and display it */

FATFS FatFs;   /* Work area (filesystem object) for logical drive */

int main (void)
{
    InitSerial();
    WriteText("-");

    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */


    /* Register work area to the default drive */
    f_mount(&FatFs, "", 1);
    WriteText("_\n\r");
    /* Open a text file */
    fr = f_open(&fil, "FILE.txt", FA_READ);
    WriteText("Opened\n\r");
    if (fr) {
        char err[50];
        sprintf(err, "Err No: %i", fr);
        return (int)fr;
    }

    /* Read every line and display it */
    while (f_gets(line, sizeof line, &fil)) {
        WriteText("reading line\r\n");
         int n;
        char buffer [250];                                      /* It is a file. */
        n = sprintf(buffer,"%s\r\n", line);
        WriteText(buffer);
    }
    WriteText("done reading\r\n");
    /* Close the file */
    f_close(&fil);

    //Unmount the file system
    f_mount(0, "", 0); 
    return 0;
}

