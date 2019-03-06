#ifndef NMALLOC_H_
#define NMALLOC_H_

#include "LPC17xx.h"
#include <string.h>

#define malloc_base LPC_AHBRAM1_BASE
#define malloc_end  LPC_GPIO_BASE
#define END 100

typedef struct{
  void* Address;
  void* EndAddress;
}UsedMemory;


void* NewMalloc(unsigned long data);
void NewFree(void* toRemove);
void initMalloc();


#endif
