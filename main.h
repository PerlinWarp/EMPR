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

#define MENUTEXTNUM 7
#define BUTTON_DOWN '#'
#define BUTTON_UP '*'
#define BUTTON_SEL 'A'
#define BUTTON_SEL_2 'B'
//Buffer length in bytes
#define BUFO_LENGTH 150
#define BUFI_LENGTH 150
#define PI 3.1415827
#define I2S_SRC LPC_AHBRAM1_BASE
#define I2S_DST (I2S_SRC + 0x1000UL)

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
#include "source/interrupt_handlers.h"
#include "source/Wave.h"
#include "source/SD.h"
#include "source/FatFS/diskio.h"
#include "source/FatFS/ff.h"
//#include "source/FatFS/mmc_176x_ssp.c"
volatile int buttonpress;
volatile char prevKey,key;
volatile char* Audio_buf;
 uint32_t* BufferOut;
 uint32_t* BufferIn;
int SelMenuItem;

uint8_t int_Handler_Enable=0,int_Handler_Index =0;


char* MenuText[MENUTEXTNUM] = {"A1.Rec Audio  ","A2.Play Audio  ",
                     "A3.Save to SD  ", "A4.Browse SD   ",
                     "U2.IPod Mode   ","N1.PassThrough "," "};
FATFS FatFs;

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
void FatRead();
void PC_Mode();
void (*menuFuncs[])(void) = {&PassThroughLoop,&Play_Audio,&I2S_PassThroughLoop,&I2S_PassThroughInterrupt,&UART_Mode,&PC_Mode,&FatRead,&PassThroughLoop,&PassThroughLoop,&temp};

void (*int_Handler_Funcs[])(void) = {&I2S_PassThroughInt_Handler};
/*
Menu Organisation:



*/

#endif
