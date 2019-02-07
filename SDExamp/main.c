#include <string.h>
#include "rtc176x.h"
#include "uart176x.h"
#include "ff.h"
#include <serial.h>
#include <stdio.h>



int main(void)
{
    serial_init();
    write_usb_serial_blocking("///////", 7);

    FATFS fs;
    FRESULT res;
    char path[256];


    res = f_mount(&fs, "/", 0);
    strcpy(path, "/");

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
                sprintf(&path[i], "/%s", fno.fname);               /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                
               /* It is a file. */
                char buff[256];
                sprintf(buff,"%s/%s\n\r",path,fno.fname);
                write_usb_serial_blocking(buff, 11);
                //write_usb_serial_blocking("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return 0;
}
