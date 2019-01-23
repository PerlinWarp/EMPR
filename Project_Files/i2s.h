/***********************************************************************//**
 * @file		I2S.h
 * @brief		Contains all macro definitions and function prototypes
 * 				support for I2S with DMA on LPC1768
 * @version		0.1
 * @date		23. January. 2019
 * @author		Idris Al-Ghabra
 **************************************************************************/
#ifndef I2S_H_
#define I2S_H_

#include "lpc17xx.h"
#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_pinsel.h"


    __IO uint32_t Channel0_TC;
    __IO uint32_t Channel0_Err;
    __IO uint32_t Channel1_TC;
    __IO uint32_t Channel1_Err;

    void DMA_IRQHandler();
    void Init_I2S(uint32_t* BufferOut,uint32_t BufferOutWidth,uint32_t* BufferIn,uint32_t BufferInWidth);
    void ConfInit(I2S_CFG_Type* I2S_Config_Struct,uint8_t wordwidth,uint8_t mono,uint8_t stop,uint8_t reset,uint8_t mute);
    void ClockInit(I2S_MODEConf_Type* I2S_ClkConfig,uint8_t clksource,uint8_t mode4pin,uint8_t mclkout);
    void InitializeGPDMA(uint32_t* DataOut,uint32_t OutWidth,uint32_t* DataIn,uint32_t InWidth,GPDMA_Channel_CFG_Type* GPDMA_Cfg);
    void EnableInput();
    void EnableOutput();
    void DisableInput();
    void DisableOutput();



#endif
