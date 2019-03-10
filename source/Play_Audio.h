#ifndef GSINE_H_
#define GSINE_H_
//The number of samples within the sine wave
#define PRECISION 256
#define MAXVOLTS 2.5
#define PI 3.1415926535897932384626433832

#include <math.h>
#include <stdlib.h>

#include "DAC.h"
#include "NewMalloc.h"
#include "SD.h"
#include "Delay.h"
#include "Wave.h"
#include "lpc17xx_adc.h"
#include "LCD.h"
volatile int buttonpress;
volatile int breakout;
uint8_t timerDone;
void init_onboard_audio_no_DMA(FIL* fil,uint32_t Frequency);
uint32_t record_onboard_audio_no_DMA(FIL* fil, uint32_t Frequency);

void onboard_record();
void onboard_dma_irq();
void timer_Done();
void onboard();

#endif
