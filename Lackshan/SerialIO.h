#ifndef SER_H_
#define SER_H_

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void InitSerial(void);
int WriteText(char*);

#endif