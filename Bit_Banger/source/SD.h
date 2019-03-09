#ifndef SD_H_
#define SD_H_

#include <string.h>
#include "FatFS/diskio.h"
#include "FatFS/ff.h"
#include <stdio.h>
#include "LPC17xx.h"

#define SD_DEBUG 0 // enables status printing, assumes SerialInit() was ran

FATFS fs;

void sd_init(void);

void sd_deinit();

extern void SDPrintFresult(FRESULT fr);

uint8_t SDGetFiles(char* path, char** result);
uint8_t SDGetDirectories(char *path, char** result);
uint32_t SDGetFileSize(char *path);
uint8_t SDReadBytes(char* path, BYTE* result, uint8_t n);
void SDCleanPath(char *path);
uint8_t SDGetAllFiles(char** result);
uint8_t SDGetAllFilesandDirs(char** result,char**allDirs);
char** SDMallocFilenames();
void SDFreeFilenames(char** filenames);

#endif
