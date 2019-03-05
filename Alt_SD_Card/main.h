#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "LPC17xx.h"

#include "Libraries/SerialIO.h"

#include "Libraries/FatFS/LPC176x.h"
#include "Libraries/FatFS/diskio.h"
#include "Libraries/FatFS/ff.h"

FATFS FatFs;

int SD_Test();
