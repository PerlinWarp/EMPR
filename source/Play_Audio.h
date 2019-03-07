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
#include "Wave.h"

volatile int buttonpress;

void init_onboard_audio(FIL* fil,uint32_t Frequency)
#endif
