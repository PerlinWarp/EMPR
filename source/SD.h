#ifndef SD_H_
#define SD_H_

#include <string.h>
#include "FatFS/diskio.h"
#include "FatFS/ff.h"
#include <stdio.h>
#include "SerialIO.h"
#include "LPC17xx.h"
#include "lpc17xx_i2s.h"

#define SD_DEBUG 1 // enables status printing, assumes SerialInit() was ran

FATFS fs;

FATFS sd_init(void);

void sd_deinit();

void SDPrintFresult(FRESULT fr);
// void SDInit(void);
uint8_t SDGetFiles(char* path, char** result);
uint8_t SDGetDirectories(char *path, char** result);

uint8_t SDReadBytes(char* path, BYTE* result, uint8_t n);

char** SDMallocFilenames();
void SDFreeFilenames(char** filenames);

#define	_BV(bit) (1<<(bit))
#define	FIO0CLR2	(*(volatile uint8_t*)0x2009C01E)
#define	FIO0SET2	(*(volatile uint8_t*)0x2009C01A)
#define	CS_LOW()	{FIO0CLR2 = _BV(0);}	/* Set P0.16 low */
#define	CS_HIGH()	{FIO0SET2 = _BV(0);}	/* Set P0.16 high */

unsigned int SD_READ(FIL* fil,uint32_t* buf,uint32_t bufSize);
unsigned int SD_WRITE(FIL* fil,uint32_t* buf,uint32_t bufSize);

#endif
