#ifndef MAIN_H_
#define MAIN_H_


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define MENUTEXTNUM 8
#define BUTTON_DOWN '#'
#define BUTTON_UP '*'
#define BUTTON_SEL 'A'
#define BUTTON_SEL_2 'B'
#define BUTTON_CONFIRM '0'
#define BUTTON_CANCEL '8'


#define EGG_ON_TOAST(bufd) (bufd+10)

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "LPC17xx.h"
#include "lpc17xx_ssp.h"
#include "source/SerialIO.h"
#include "source/LCD.h"
#include "source/I2C.h"
#include "source/KeyAccess.h"
#include "source/Delay.h"
#include "source/TLV320.h"
#include "source/i2s.h"
#include "source/SineWave.h"
#include "source/SD.h"
#include "source/interrupt_handlers.h"
#include "source/Wave.h"
#include "source/NewMalloc.h"
#include "source/FatFS/diskio.h"
#include "source/FatFS/ff.h"
#include "source/Play_Audio.h"
//#include "source/FatFS/mmc_176x_ssp.c"
extern volatile int buttonpress;
volatile char prevKey,key;
volatile char* Audio_buf;
 uint32_t* BufferOut;
 uint32_t* BufferIn;
int SelMenuItem;

uint8_t int_Handler_Enable=0,int_Handler_Index =0;


char* MenuText[MENUTEXTNUM] = {"0-A1.Rec Sample","1-A2.Play Sine ",
                     "2-A3.Rec to SD ", "3-A4.Play SD   ",
                     "4-U4.View Info ","5-N1.PassThru  ",
                     "6-D1 PC-Mode   "," "};
FATFS FatFs;

int settings[20];

void Menu();
void DrawMenu();
void DrawArrows();
void IRQInit();

void Play_Audio();
void PassThroughLoop();
void RecordLoop();
void I2S_PassThroughLoop();
void I2S_PassThroughInterrupt();
void UART_Mode();
void temp();
uint32_t GetWaveInfo(char* fpath,uint32_t fSize);
void FatRead();
void PC_Mode();
void Play(char* directory);
uint8_t ShowFileSelection(char** filenames, char* header, uint8_t fileCount);
void FileSelection();
void FileInfo();
void PC_Play(char* fpath);
void PC_Record(char* fpath);
char SELECTED_FILE[40];

extern int f_copy(char* filepath);
extern void f_delete(char* filepath);
extern void Reverse_Wav(char* src);

void Record_OnBoard_Audio();
void Play_OnBoard_Audio();


void A1();
void A2();
void A3();
void A4();

void (*menuFuncs[])(void) = {
	&A1,
	&A2,
	&A3,
	&A4,
  &FileInfo,
	&I2S_PassThroughInterrupt,
	&PC_Mode,
	&FatRead,
	&PassThroughLoop,

	&UART_Mode};

void (*int_Handler_Funcs[])(void) = {&I2S_PassThroughInt_Handler};


/*
Menu Organisation:



*/

#endif
