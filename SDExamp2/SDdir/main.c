#include <string.h>
#include "diskio.h"
#include "ff.h"
#include <serial.h>
#include <stdio.h>
#include "SerialIO.h"
#include "SD.h"

int main (void)
{
    InitSerial();

    char** filenames = MallocFilenames();
    int filenamesAmount = SDGetFiles(filenames);

    char buff[64];
    int j;

    sprintf(buff, "Found %d files\n\r", filenamesAmount);
    WriteText(buff);

    for (j = 0; j < filenamesAmount; j++ ) {
        sprintf(buff, "Filename #%d: %s\n\r", j, filenames[j]);
        WriteText(buff);
    }
    
    FreeFilenames(filenames);

    return 0;
}