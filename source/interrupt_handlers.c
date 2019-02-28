#include "interrupt_handlers.h"

void I2S_PassThroughInt_Handler(){NVIC_DisableIRQ(I2S_IRQn);}
