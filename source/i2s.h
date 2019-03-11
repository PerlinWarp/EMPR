#ifndef I2S_POL_H_
#define I2S_POL_H_


#define PINS7_9TX 0x54000
#define PINS023_025RX 0xA8000

#include "LPC17xx.h"
#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg_default.h"
#include "SerialIO.h"
#include "TLV320.h"
#include "Wave.h"
#include "SD.h"

#include <math.h>
#define PI 3.1415926535897932384626433832

#define I2S_MODE_POLLING 0
#define I2S_MODE_INTERRUPT 1
#define BUFFER_SIZE 256
#define BASE_FREQUENCY 48000
#define WAVE_BUFFER_LEN 2

//#define __bit_rev(val) ((val * 0x0802LU & 0x22110LU) | (val * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16
FIL* fileptr;
uint32_t ReadInd,WriteInd;//Pointer to a value
uint32_t* buffer;//Pointer to a list
uint16_t* buffer16;
int16_t* sineBuffer;

void I2S_Polling_Init(uint32_t Freq, int i2smode);
void I2S_Polling_Read(uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);
void I2S_Polling_Write(uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);

void ConfInit(I2S_CFG_Type* I2S_Config_Struct,uint8_t wordwidth,uint8_t mono,uint8_t stop,uint8_t reset,uint8_t mute);
void ClockInit(I2S_MODEConf_Type* I2S_ClkConfig,uint8_t clksource,uint8_t mode4pin,uint8_t mclkout);


void Init_I2S_Wav(uint16_t NumChannels,uint32_t SampleRate,uint16_t BitsPerSample,FIL* fil);
void I2S_BaseInit();
void I2S_Create_Sine(uint32_t frequency);
void i2s_int_Passthrough();
void i2s_wav_play_16_bit();
void i2s_playSound();


#endif
