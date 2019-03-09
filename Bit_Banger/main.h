#ifndef MAIN_H_
#define MAIN_H_
/*
#include <stdlib.h>

#include "LPC17xx.h"

#include "source/SerialIO.h"
#include "source/LCD.h"
#include "source/I2C.h"
#include "source/KeyAccess.h"
#include "source/Delay.h"
#include "source/i2s.h"
#include "source/SD.h"
#include "source/Wave.h"

#include "source/FatFS/diskio.h"
#include "source/FatFS/ff.h"
*/
volatile int buttonpress;
volatile char prevKey,key;


#define LCRIN 0
#define DIN 0
#define BCLK 0

#define DO_16_TIMES(x,value) = {x(value,15);x(value,14);x(value,13);x(value,12);x(value,11);x(value,10);x(value,9);x(value,8);x(value,7);x(value,6);x(value,5);x(value,4);x(value,3);x(value,2);x(value,2);x(value,1);}

#define SEND_BIT(value,bit) x = ((value>>bit)&1); \
                            LPC_GPIO0->FIOCLR = BCLK;\
                            LPC_GPIO0->FIOSET = x<<DIN;\
                            LPC_GPIO0->FIOCLR = ~(x)<<DIN;\
                            LPC_GPIO0->FIOSET = BLCK;\
void IRQInit();
void Play_File();


#endif
