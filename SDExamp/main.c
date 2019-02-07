#include <string.h>
#include "diskio.h"
#include "ff.h"
#include <serial.h>
#include <stdio.h>


void main(void)
{
    serial_init();
    write_usb_serial_blocking("///////", 7);

    FATFS FatFs;    // Work area (filesystem object) for logical drive
    DIR dir;        // Directory object
    FRESULT fr;

    f_mount(&FatFs, "/", 0);

    fr = f_opendir(&dir, "/");
    
    int i;
    FILINFO fi;
    for (i=0; i < 10; i++){
        fr = f_readdir(&dir, &fi);
        if (fi.fattrib == AM_DIR){
            write_usb_serial_blocking("FOLDER ", 7);
            write_usb_serial_blocking(fi.fname, 13);
        } else {
            write_usb_serial_blocking("FILE ", 5);
            char buff[14];
            sprintf(buff, "/%s\n", fi.fname);
            write_usb_serial_blocking(buff, 13);
            write_usb_serial_blocking(",", 1);
            write_usb_serial_blocking("1234", 4);
            //write_usb_serial_blocking(fi.fsize, 13);
        }
        write_usb_serial_blocking("\r\n", 2);
    }
    
    return 0;
}