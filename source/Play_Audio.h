#ifndef GSINE_H_
#define GSINE_H_
//The number of samples within the sine wave
#define PRECISION 32
#define MAXVOLTS 2.5
#define PI 3.1415926535897932384626433832

#include <math.h>
#include <stdlib.h>

#include "DAC.h"
#include "NewMalloc.h"
#include "SD.h"
#include "Wave.h"
#include "LCD.h"
volatile int buttonpress;


void init_onboard_audio_no_DMA(FIL* fil,uint32_t Frequency);
void init_onboard_audio(FIL* fil,uint32_t Frequency);
void record_onboard_audio_no_DMA(FIL* fil, uint32_t Frequency);

void onboard_record();
void onboard_dma_irq();
void onboard();

#endif
