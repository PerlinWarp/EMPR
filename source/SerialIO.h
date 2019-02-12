#ifndef SER_H_
#define SER_H_

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg_default.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include "main.h"//send key data to main for control
#define UART_RING_BUFSIZE 256

__IO FlagStatus TxIntStat;

typedef struct
{
  __IO uint32_t tx_head;
  __IO uint32_t tx_tail;
  __IO uint32_t rx_head;
  __IO uint32_t rx_tail;
  __IO uint8_t  tx[UART_RING_BUFSIZE];
  __IO uint8_t  rx[UART_RING_BUFSIZE];
}UART_Ring_Buffer;

UART_Ring_Buffer rbuf;

void ReceiveText(void);
void TransmitText(void);

void InitSerial(void);
int WriteText(char*);

void InitSerInterrupts(void);
void EnableSerInterrupts(void);
void EnableSerInterrupts(void);
void ProcessBuffer(void);

#endif
