#include "I2S.h"

#define BUFFER_SIZE        0x400
#define I2S_BUFFER_SRC     LPC_AHBRAM1_BASE //0x20080000
#define I2S_BUFFER_DST     (I2S_BUFFER_SRC+0x1000UL) //0x20081000 
#define RXFIFO_EMPTY       0
#define TXFIFO_FULL        8

volatile uint8_t  I2STXDone = 0;
volatile uint8_t  I2SRXDone = 0;

volatile uint32_t *I2STXBuffer = (uint32_t*)(I2S_BUFFER_SRC);
volatile uint32_t *I2SRXBuffer = (uint32_t *)(I2S_BUFFER_DST);

volatile uint32_t I2SReadLength = 0;
volatile uint32_t I2SWriteLength = 0;

uint8_t tx_depth_irq = 0;
uint8_t rx_depth_irq = 0;
uint8_t dummy=0;


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************/
void I2S_IRQHandler()
{
        uint32_t RXLevel = 0;

        //Check RX interrupt
        if(I2S_GetIRQStatus(LPC_I2S, I2S_RX_MODE))
        {
                RXLevel = I2S_GetLevel(LPC_I2S, I2S_RX_MODE);
                if ( (RXLevel != RXFIFO_EMPTY) && !I2SRXDone )
                {
                        while ( RXLevel > 0 )
                        {
                                if ( I2SReadLength == BUFFER_SIZE )
                                {
                                        //Stop RX
                                        I2S_Stop(LPC_I2S, I2S_RX_MODE);
                                        // Disable RX
                                        I2S_IRQCmd(LPC_I2S, I2S_RX_MODE, DISABLE);
                                        I2SRXDone = 1;
                                        break;
                                }
                                else
                                {
                                        I2SRXBuffer[I2SReadLength++] = LPC_I2S->I2SRXFIFO;
                                }
                                RXLevel--;
                        }
                }
        }
        return;
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************/
void Buffer_Init(void) {
        uint32_t i;
        for ( i = 0; i < BUFFER_SIZE; i++ )     /* clear buffer */
        {
        I2STXBuffer[i] = i;
        I2SRXBuffer[i] = 0;
        }

}

/*********************************************************************/
Bool Buffer_Verify(void) {
        uint32_t i;
        uint32_t *pTX = (uint32_t *) &I2STXBuffer[0];
        uint32_t *pRX = (uint32_t *) &I2SRXBuffer[1];
        /* Because first element of RX is dummy 0 value, so we just verify
         * from 2nd element
         */
        for (i = 1; i < BUFFER_SIZE; i++) {
                if (*pTX++ != *pRX++)  {
                        return FALSE;
                }
        }
        return TRUE;
}
/*********************************************************************/


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************/
int I2S_Demo (void) {                       /* Main Program */
        I2S_MODEConf_Type I2S_ClkConfig;
        I2S_CFG_Type I2S_ConfigStruct;
        PINSEL_CFG_Type PinCfg;

        uint32_t i;

/* Initialize I2S peripheral ------------------------------------*/
        /* Pin configuration:
         * Assign:              - P0.23 as I2SRX_CLK
         *                      - P0.24 as I2SRX_WS
         *                      - P0.25 as I2SRX_SDA
         *                      - P0.7 as I2STX_CLK
         *                      - P0.8 as I2STX_WS
         *                      - P0.9 as I2STX_SDA
         */
        PinCfg.Funcnum = 2;
        PinCfg.OpenDrain = 0;
        PinCfg.Pinmode = 0;
        PinCfg.Pinnum = 23;
        PinCfg.Portnum = 0;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 24;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 25;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Funcnum = 1;
        PinCfg.Pinnum = 7;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Funcnum = 1;
        PinCfg.Pinnum = 8;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Funcnum = 1;
        PinCfg.Pinnum = 9;
        PINSEL_ConfigPin(&PinCfg);

        Buffer_Init();

        I2S_Init(LPC_I2S);

        /* setup:
         *              - wordwidth: 16 bits
         *              - stereo mode
         *              - master mode for I2S_TX and slave for I2S_RX
         *              - ws_halfperiod is 31
         *              - not use mute mode
         *              - use reset and stop mode
         *              - select the fractional rate divider clock output as the source,
         *              - disable 4-pin mode
         *              - MCLK ouput is disable
         *              - Frequency = 44.1 kHz
         * Because we use mode I2STXMODE[3:0]= 0000, I2SDAO[5]=0 and
         * I2SRX[3:0]=0000, I2SDAI[5] = 1. So we have I2SRX_CLK = I2STX_CLK
         * --> I2SRXBITRATE = 1 (not divide TXCLK to produce RXCLK)
         */

        /* Audio Config*/
        I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
        I2S_ConfigStruct.mono = I2S_STEREO;
        I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
        I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
        I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
        I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
        I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);

        I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
        I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);

        /* Clock Mode Config*/
        I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
        I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
        I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
        I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
        I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);

        I2S_FreqConfig(LPC_I2S, 44100, I2S_TX_MODE);
        I2S_SetBitRate(LPC_I2S, 0, I2S_RX_MODE);

        I2S_Stop(LPC_I2S, I2S_TX_MODE);
        I2S_Stop(LPC_I2S, I2S_RX_MODE);

        NVIC_EnableIRQ(I2S_IRQn);

        /* RX FIFO depth is 1, TX FIFO depth is 8. */
        I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,8);
        I2S_IRQConfig(LPC_I2S,I2S_RX_MODE,1);
        I2S_IRQCmd(LPC_I2S,I2S_RX_MODE,ENABLE);
        I2S_Start(LPC_I2S);

/* I2S transmit ---------------------------------------------------*/
        while ( I2SWriteLength < BUFFER_SIZE )
        {
                while(I2S_GetLevel(LPC_I2S, I2S_TX_MODE)==TXFIFO_FULL);
                I2S_Send(LPC_I2S, I2STXBuffer[I2SWriteLength++]);
        }

        I2STXDone = 1;

        /* Wait for transmit/receive complete */
        while ( !I2SRXDone || !I2STXDone );
        return 0;
}

