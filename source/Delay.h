#ifndef DEL_H_
#define DEL_H_

#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_systick.h"

volatile unsigned long Counter;
void DelayInit();
void Delay(unsigned long tick);
void SysTick_Handler(void);

#endif
