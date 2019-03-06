#ifndef SER_H_
#define SER_H_

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg_default.h"
#include "LCD.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include "main.h"//send key data to main for control
//Note, this only works while the buffer is a power of 2, so UART_BIT_MASK is all ones.
#define UART_RING_BUFSIZE 256
#define UART_BIT_MASK (UART_RING_BUFSIZE - 1)
#define INSTR_MAX_LEN 15
#define INC_BUFFER(bufd) (bufd = (bufd+1)&UART_BIT_MASK)
#define DEC_BUFFER(bufd) (bufd = ((bufd==0)?UART_BIT_MASK:(bufd-1)))
#define CHECK_BUFFER(bufd) ((bufd+1)&UART_BIT_MASK)
#define CHECK_DEC_BUFFER(bufd) ((bufd==0) ? (UART_BIT_MASK):(bufd-1) )

#define SERIAL_BUFFER_MAXSIZE 4
#define SERIAL_BUFFER_MASK SERIAL_BUFFER_MAXSIZE - 1
#define READ_SERIAL (serialCommandBuffer[serialCommandIndex])
#define POP_SERIAL (serialCommandIndex = ((serialCommandIndex==0)?0:serialCommandIndex-1))
#define PUSH_SERIAL (serialCommandIndex = ((serialCommandIndex==SERIAL_BUFFER_MASK)?SERIAL_BUFFER_MASK:serialCommandIndex+1))

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

char** serialCommandBuffer;//commands in memory
uint8_t serialCommandIndex;

void ReceiveText(void);
void TransmitText(void);

uint32_t ReadText(char* data, uint32_t length);
uint32_t AddTextToBuffer(char* data, uint32_t length);


void InitSerial(void);
int WriteText(char*);
int write_usb_serial_blocking(char *buf,int length);

void InitSerInterrupts(void);
void EnableSerInterrupts(void);
void EnableSerInterrupts(void);
void ProcessBuffer(void);

#endif
