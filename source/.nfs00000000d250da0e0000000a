#ifndef SD_H_
#define SD_H_

#include <string.h>
#include "FatFS/diskio.h"
#include "FatFS/ff.h"
#include <stdio.h>
#include "SerialIO.h"

#define SD_DEBUG 1 // enables status printing, assumes SerialInit() was ran

FATFS fs;
void SDPrintFresult(FRESULT fr);
// void SDInit(void);
uint8_t SDGetFiles(char* path, char** result);

char** SDMallocFilenames();
void SDFreeFilenames(char** filenames);



#endif