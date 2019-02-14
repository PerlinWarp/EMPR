#ifndef SD_H_
#define SD_H_

#include <string.h>
#include "diskio.h"
#include "ff.h"
#include <stdio.h>
#include "SerialIO.h"

#define DEBUG 1 // enables status printing, assumes SerialInit() was ran


void PrintFresult(FRESULT fr);
// void SDInit(void);
int SDGetFiles(char** result);

char** MallocFilenames();
void FreeFilenames(char** filenames);

#endif