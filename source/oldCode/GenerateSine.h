#ifndef GSINE_H_
#define GSINE_H_
//The number of samples within the sine wave
#define PRECISION 256
#define MAXVOLTS 2.5
#define PI 3.1415926535897932384626433832

#include <math.h>
#include <stdlib.h>

#include "DAC.h"
#include "Delay.h"
#include "NewMalloc.h"
#include "SD.h"

void init_onboard_audio();
#endif
