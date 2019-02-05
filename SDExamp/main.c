#include <string.h>
#include "rtc176x.h"
#include "uart176x.h"
#include "ff.h"
#include "diskio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <serial.h>

void main(void)
{
    serial_init();
    write_usb_serial_blocking("///////", 7);

    FATFS FatFs;
    DIR dir;        
    FRESULT fr;

    f_mount(&FatFs, "", 0);
    


    fr = f_opendir(&dir, "/");
    write_usb_serial_blocking("breaks here", 11);
    int i;
    FILINFO fi;
    for (i=0; i < 10; i++){
        fr = f_readdir(&dir, &fi);
        if (fi.fattrib == AM_DIR){
            write_usb_serial_blocking("FOLDER ", 7);
            write_usb_serial_blocking(fi.fname, 13);
        } else {
            write_usb_serial_blocking("FILE ", 5);
            write_usb_serial_blocking(fi.fname, 13);
            write_usb_serial_blocking(",", 1);
            write_usb_serial_blocking("1234", 4);
            //write_usb_serial_blocking(fi.fsize, 13);
        }
        write_usb_serial_blocking("\r\n", 2);
    }
    
    return 0;
}