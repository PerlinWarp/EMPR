#ifndef LCDPHW_H_
#define LCDPHW_H_

#include "I2C.h"
#include "Delay.h"
#include <string.h>
#include <stdlib.h>
#include "SerialIO.h"
#define LCD_ADDR 0x3B


void SendData(uint8_t * data, int length);
void LCDEnable();
void LCDDisable();
void LCDGoHome();
void LCDNewLine();
void LCDInit();
void LCDClear();
void LCDPrint(char* input);
void LCDCursorShift(uint8_t value);

void main_LDCPrintHelloWorld(void);

#endif
