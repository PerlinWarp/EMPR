#ifndef I2S_POL_H_
#define I2S_POL_H_


#define PINS7_9TX 0x54000
#define PINS023_025RX 0xA8000

#include "LPC17xx.h"
#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg_default.h"
#include "i2s.h"

void I2S_Polling_Init(uint32_t Freq);
void I2S_Polling_Read(volatile uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);
void I2S_Polling_Write(volatile uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length);

#endif
