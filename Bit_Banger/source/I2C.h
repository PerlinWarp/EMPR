#ifndef I2C_H_
#define I2C_H_

#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"

#define TLV320_ADDR 0x1A

#define I2CBUS LPC_I2C1

void I2CInit(void);
void SetupVals(I2C_M_SETUP_Type* trData,int Address,uint8_t* Data, int TRLength);
void SetupPINS(void);
void Send_Data(uint8_t* Data,int length);
void TLV320_PlayWav();




#endif
