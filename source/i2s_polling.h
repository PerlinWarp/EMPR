#ifndef I2S_POL_H_
#define I2S_POL_H_


#define PINS7_9TX 0x54000
#define PINS023_025RX 0xA8000

#include "LPC17xx.h"
#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg_default.h"
#include "i2s.h"
#include "SerialIO.h"

#include "FatFS/diskio.h"
#include "FatFS/ff.h"

#define I2S_RING_BUFSIZE 256
#define I2S_BIT_MASK (I2S_RING_BUFSIZE - 1)
#define I2S_INC_BUFFER(bufd) (bufd = (bufd+1)&I2S_BIT_MASK)
#define I2S_CHECK_BUFFER(bufd) ((bufd+1)&I2S_BIT_MASK)

#define I2S_MODE_POLLING 0
#define I2S_MODE_INTERRUPT 1
uint32_t ReadInd,WriteInd;//Pointer to a value
uint32_t* buffer;//Pointer to a list

FIL fil;        /* File object */
uint y;
uint i;

FRESULT fre;     /* FatFs return code */


void I2S_Polling_Init(uint32_t Freq, int i2smode);
void I2S_A_Polling_Init(uint32_t freq,int i2smode);
void I2S_Polling_Read(uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);
void I2S_Polling_Write(uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);
#endif
