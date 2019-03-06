#ifndef NMALLOC_H_
#define NMALLOC_H_

#include "LPC17xx.h"
#include <string.h>

#define MALLOC_BASE (void*)LPC_AHBRAM1_BASE
#define MALLOC_END  (void*)LPC_GPIO_BASE
#define END 100

typedef struct{
  void* Address;
  void* EndAddress;
}UsedMemory;


void* NewMalloc(unsigned long size);
void NewFree(void* toRemove);
void initMalloc();
void insertionSort();
void* assignAddress(void* address,unsigned long size);

#endif
