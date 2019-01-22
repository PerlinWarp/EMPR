#include <LPC17xx.h>
#include "lpc17xx_spi.h"
// #include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

// Pin Defenitions
// PORT number that /CS pin assigned on
#define CS_PORT_NUM             0
// PIN number that  /CS pin assigned on
// #define CS_PIN_NUM           16
#define CS_PIN_NUM              49

#define SD_DETECT_PORTNUM       4
#define SD_DETECT_PINNUM        29

//To get SDInit to work 
// SPI Configuration structure variable
SPI_CFG_Type SPI_ConfigStruct;
sd_error SD_WaitR1(uint8_t *buffer, uint32_t length, uint32_t timeout);
sd_error SD_WaitDeviceIdle(uint32_t num_char);
sd_error SD_Init(uint8_t retries);
sd_error SD_GetCID(void);

typedef enum _sd_error
{
        SD_OK,                  //0
        SD_NG,                  //1
        SD_CMD_BAD_PARAMETER,   //2
        SD_ERROR_TOKEN,         //3
        SD_ERROR_TIMEOUT,       //4
        SD_ERROR_BUS_NOT_IDLE,  //5
        SD_ERROR_CMD0,          //6
        SD_ERROR_CMD55,         //7
        SD_ERROR_ACMD41,        //8
        SD_ERROR_CMD59,         //9
}sd_error;

void CS_Init(void)
{
        GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
        GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
}

sd_error SD_Init(uint8_t retries)
{
        uint8_t rxdata,errors;
        uint8_t SD_arg[4]={0,0,0,0};
        uint16_t i;

        // initialize SPI configuration structure to default
        SPI_ConfigStructInit(&SPI_ConfigStruct);
        // Initialize SPI peripheral with parameter given in structure above
        SPI_Init(LPC_SPI, &SPI_ConfigStruct);
        // Initialize /CS pin to GPIO function
        CS_Init();

        // check for SD card insertion
        _DBG("\n\rPlease plug-in SD card!");
        //while(SD_GetCardConnectStatus()==SD_DISCONNECTED);
        _DBG("...Connected!\n\r");
        // Wait for bus idle
        if(SD_WaitDeviceIdle(160) != SD_OK) return SD_ERROR_BUS_NOT_IDLE;
        _DBG("Initialize SD card in SPI mode...");

        errors = 0;
        /* Send the CMD0_GO_IDLE_STATE while CS is asserted */
        /* This signals the SD card to fall back to SPI mode */
        while(errors < retries)
        {
                SD_SendCommand(CMD0_GO_IDLE_STATE, SD_arg);
                if(SD_WaitR1(&rxdata,0,1000)!= SD_OK)
                {
                        errors++;
                        continue;
                }
                if(rxdata != R1_IDLE)
                {
                        errors++;
                        continue;
                }
                else break;
        }
        if(errors >= retries)return SD_ERROR_CMD0;

        /* Check if the card is not MMC */
        /* Start its internal initialization process */
        while(1)
        {
                SD_SendCommand(CMD55_APP_CMD, SD_arg);
                if(SD_WaitR1(&rxdata,0,1000)!= SD_OK) return SD_ERROR_CMD55;

                SD_SendCommand(ACMD41_SEND_OP_COND, SD_arg);
                SD_WaitR1(&rxdata,0,1000);
                if (rxdata & R1_IDLE) //in_idle_state = 1
                        for (i = 0; i < 1000; i++); /* wait for a while */
                else break; //in_idle_state=0 --> ready
        }
        /* Enable CRC */
        SD_arg[3] = 0x01;
        SD_SendCommand(CMD59_CRC_ON_OFF, SD_arg);
        if(SD_WaitR1(&rxdata,0,1000)!= SD_OK) return SD_ERROR_CMD59;
        if(rxdata != R1_NOERROR) return SD_ERROR_CMD59;

        return SD_OK;
}


int main(){

    PINSEL_CFG_Type PinCfg;
/*
     * Initialize SPI pin connect
     * 7  P0.15 - SCK;
     * 11 P0.16 - SSEL - used as GPIO
     * 8  P0.17 - MISO
     * 9  P0.18 - MOSI
     */
    PinCfg.Funcnum = 3;
    //SCK
    //PinCfg.Funcnum = 2;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = 0;
    // PinCfg.Pinnum = 15;
    PinCfg.Pinnum = 78;
    PINSEL_ConfigPin(&PinCfg);


    // SDCS
    PinCfg.Pinnum = 49;
    PinCfg.Funcnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    // // SSEL
    // PinCfg.Funcnum = 0;
    // PinCfg.Pinnum = 16;
    // //PinCfg.Pinnum = 8;
    // PINSEL_ConfigPin(&PinCfg);

    // MOSI
    PinCfg.Funcnum = 3;
    // PinCfg.Pinnum = 18;
    PinCfg.Pinnum = 76;
    PINSEL_ConfigPin(&PinCfg);

    //MISO
    // PinCfg.Pinnum = 17;
    PinCfg.Pinnum = 77;
    PINSEL_ConfigPin(&PinCfg);

    //Initialize SD card detection pin P4.29
    //PinCfg.Portnum = 0;
    //PinCfg.Pinnum = 16;
    PinCfg.Portnum = 4; //What it was
    PinCfg.Pinnum = 29; // ""
    PinCfg.Funcnum = 0;//GPIO function
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(SD_DETECT_PORTNUM, (1<<SD_DETECT_PINNUM), 0);//input

    //Initialising the SD card and error check
    sd_error sd_status;
    sd_status = SD_Init(10);


    _DBG(sd_status);

    return 0;

}