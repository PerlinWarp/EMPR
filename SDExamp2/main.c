#include <string.h>
#include "rtc176x.h"
#include "uart176x.h"
#include "xprintf.h"
#include "ff.h"
#include "diskio.h"
#include "sound.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// #include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

/* Read a text file and display it */

FATFS FatFs;   /* Work area (filesystem object) for logical drive */

void InitSerial(void)
{
    UART_CFG_Type UartConfiguration;
    UART_FIFO_CFG_Type FifoConfiguration;
    PINSEL_CFG_Type PinConfig;

    PinConfig.Funcnum = 1;
    PinConfig.OpenDrain = 0;
    PinConfig.Pinmode = 0;
    PinConfig.Portnum = 0;
    PinConfig.Pinnum = 2;
    PINSEL_ConfigPin(&PinConfig);
    PinConfig.Pinnum = 3;
    PINSEL_ConfigPin(&PinConfig);

    UART_ConfigStructInit(&UartConfiguration);
    UART_FIFOConfigStructInit(&FifoConfiguration);

    UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UartConfiguration);
    UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &FifoConfiguration);
    UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
}

int WriteText(char* TexttoWrite)
{
    return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t*)TexttoWrite,strlen(TexttoWrite)+1, BLOCKING));
}


int main (void)
{
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    InitSerial();

    /* Register work area to the default drive */
    f_mount(&FatFs, "", 0);

    /* Open a text file */
    fr = f_open(&fil, "message.txt", FA_READ);
    if (fr) return (int)fr;


    /* Read every line and display it */
    while (f_gets(line, sizeof line, &fil)) {
     WriteText(line);
    }

    /* Close the file */
    f_close(&fil);

    return 0;
}