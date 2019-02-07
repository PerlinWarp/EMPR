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


#include <math.h>
#include <stdlib.h>

#include "LPC17xx.h"

#include "source/SerialIO.h"
#include "source/LCD.h"
#include "source/I2C.h"
#include "source/KeyAccess.h"
#include "source/Delay.h"
#include "source/TLV320.h"
#include "source/i2s.h"
#include "source/i2s_polling.h"
/*
#include "SD/sd.h"
#include "ADC/dac.h"
#include "Project_Files/spi.h"
#include "Project_Files/i2s.h"
*/
volatile int buttonpress;
volatile char prevKey,key;
volatile char* Audio_buf;
 uint32_t* BufferOut;
 uint32_t* BufferIn;
int SelMenuItem;
char* MenuText[MENUTEXTNUM] = {"A1.Rec Audio  ","A2.Play Audio  ",
                     "A3.Save to SD  ", "A4.Browse SD   ",
                     "U2.IPod Mode   ","N1.PassThrough "," "};

void Menu();
void DrawMenu();
void DrawArrows();
void IRQInit();


void PlayLoop();
void PassThroughLoop();
void RecordLoop();
void I2S_PassThroughLoop();
void temp();

void (*menuFuncs[])(void) = {&PassThroughLoop,&PlayLoop,&I2S_PassThroughLoop,&PassThroughLoop,&PassThroughLoop,&PassThroughLoop,&PassThroughLoop,&PassThroughLoop,&PassThroughLoop,&temp};
/*
TO DO:
NAVIGATION:
    View File List + Display, Navigate
    View Information about the file e.g. size
PC ACCESS:
    Communicate to PC
    Data Visualizer
    External File Manager
PLAYBACK:
    Everything
FILE SYSTEM:
    Everything
*/

#endif
