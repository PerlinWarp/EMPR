#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_pinsel.h"

#include "LPC17xx.h"
#include "SerialIO.h"
#include "TLV320.h"


void I2S_IRQHandler(void);
void Buffer_Init(void);
Bool Buffer_Verify(void);
void print_menu(void);
int I2S_Demo (void);