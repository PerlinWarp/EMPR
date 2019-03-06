#include "i2s.h"

uint8_t I2S_ihf_Index;
static void (*I2S_int_Handler_Funcs[])(void) = {&i2s_int_Passthrough,&i2s_wav_play_16_bit,&i2s_playSound};
uint32_t buff2[BUFFER_SIZE];
void I2S_IRQHandler()
{
  I2S_int_Handler_Funcs[I2S_ihf_Index]();
}

void i2s_int_Passthrough(){
  if(I2S_GetIRQStatus(LPC_I2S,I2S_RX_MODE))
  {
    if(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)>=I2S_GetIRQDepth(LPC_I2S,I2S_RX_MODE))
    {
      if(WriteInd != CHECK_BUFFER(ReadInd))
      {
         buffer[ReadInd] = I2S_Receive (LPC_I2S);
         INC_BUFFER(ReadInd);
      }
    }
  }
  if (I2S_GetIRQStatus(LPC_I2S,I2S_TX_MODE))
  {
    if(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)<=I2S_GetIRQDepth(LPC_I2S,I2S_TX_MODE))
    {
      if(CHECK_BUFFER(WriteInd) != ReadInd)
      {
         I2S_Send(LPC_I2S,buffer[WriteInd]);
         INC_BUFFER(WriteInd);
      }
    }
  }
}
void i2s_playSound()
{
  if (I2S_GetIRQStatus(LPC_I2S,I2S_TX_MODE))
  {
    if(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)<=I2S_GetIRQDepth(LPC_I2S,I2S_TX_MODE))
    {
         I2S_Send(LPC_I2S,buffer[WriteInd]);
         INC_BUFFER(WriteInd);
    }
  }
}
int counter = 0;
uint32_t* bufs[2];
int bufidx = 0;
void i2s_wav_play_16_bit()
{
  if (I2S_GetIRQStatus(LPC_I2S,I2S_TX_MODE))
  {
    // if(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)<=I2S_GetIRQDepth(LPC_I2S,I2S_TX_MODE))
    // {
      if(WriteInd < BUFFER_SIZE)
      {
      buffer[WriteInd] = bswap_32(buffer[WriteInd]);
      //__bit_rev(buffer[WriteInd]);
      I2S_Send(LPC_I2S,bufs[1-bufidx][WriteInd]);
     //for(counter = 0;counter<200;counter++);
      ++WriteInd;
      // if(counter<48)
      //   {++WriteInd;counter++;}
      // else
      //   {counter = 0;


      //   }
    }
    else
      {
        NVIC_DisableIRQ(I2S_IRQn);
        //Read Buffer full and reset write and readInd
        //char output[29];
        //sprintf(output,"%lu ",fileptr->fptr);
        //WriteText(output);
        // SD_READ(fileptr,buffer,BUFFER_SIZE);
        WriteInd = 0;
      }
    }
  // }
}
/*
To do this, we are going to create a small read write buffer of
since i2s must be disabled while we read, it is pointless to have a large buffer, as we have to wait for refills each time.


*/
void Init_I2S_Wav(uint16_t NumChannels,uint32_t SampleRate,uint16_t BitsPerSample,FIL* fil)
{
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  LPC_PINCON->PINSEL0|=PINS7_9TX;//Set pins 0.7-0.9 as func 2 (i2s Tx)
  LPC_PINCON->PINSEL1|=PINS023_025RX;//Set Pins 0.23-0.25 as func 3 (i2s Rx)
  I2S_Init(LPC_I2S);
  ConfInit(&I2S_Config_Struct, I2S_WORDWIDTH_16,I2S_STEREO,I2S_STOP_ENABLE,I2S_RESET_ENABLE,I2S_MUTE_DISABLE);
  ClockInit(&Clock_Config,I2S_CLKSEL_FRDCLK,I2S_4PIN_DISABLE,I2S_MCLK_DISABLE);
  I2S_FreqConfig(LPC_I2S, 16000, I2S_TX_MODE);//Set frequency for output
  I2S_FreqConfig(LPC_I2S, 16000, I2S_RX_MODE);
  WriteText("test");
  WriteInd = 0;
  LPC_I2S->I2STXRATE = 0x00;
  LPC_I2S->I2STXBITRATE = 0x00;
  I2S_SetBitRate(LPC_I2S,0,I2S_TX_MODE);
  I2S_Start(LPC_I2S);
  I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,4);
  I2S_IRQCmd(LPC_I2S,I2S_TX_MODE,ENABLE);
  NVIC_SetPriority(I2S_IRQn, 0x03);
  fileptr = fil;
  TLV320_Start_I2S_WavPlay();
  I2S_ihf_Index = 1;
   bufs[0] = buffer;
   bufs[1] = buff2;
  //Read a buffer of audio into the data
  f_lseek(fileptr,44);
  SD_READ(fileptr,bufs[bufidx],BUFFER_SIZE);
  bufidx = (1 - bufidx);
  NVIC_EnableIRQ(I2S_IRQn);
  
  while(1){
    SD_READ(fileptr,bufs[bufidx],BUFFER_SIZE);
    bufidx = (1 - bufidx);//read the buffer full
    NVIC_EnableIRQ(I2S_IRQn);
    }
  WriteText("end");
}

void I2S_Polling_Init(uint32_t freq,int i2smode)
{
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  LPC_PINCON->PINSEL0|=PINS7_9TX;//Set pins 0.7-0.9 as func 2 (i2s Tx)
  LPC_PINCON->PINSEL1|=PINS023_025RX;//Set Pins 0.23-0.25 as func 3 (i2s Rx)
  I2S_Init(LPC_I2S);
  ConfInit(&I2S_Config_Struct, I2S_WORDWIDTH_16,I2S_STEREO,I2S_STOP_ENABLE,I2S_RESET_ENABLE,I2S_MUTE_DISABLE);
  ClockInit(&Clock_Config,I2S_CLKSEL_FRDCLK,I2S_4PIN_DISABLE,I2S_MCLK_DISABLE);

  I2S_FreqConfig(LPC_I2S, 48000, I2S_TX_MODE);//Set frequency for output
  I2S_FreqConfig(LPC_I2S, 48000, I2S_RX_MODE);
  if(i2smode){
    WriteInd = ReadInd =0;
    LPC_I2S->I2STXRATE = 0x00;
    LPC_I2S->I2STXBITRATE = 0x00;
    I2S_SetBitRate(LPC_I2S,0,I2S_TX_MODE);
    I2S_Start(LPC_I2S);
    I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,4);
    I2S_IRQCmd(LPC_I2S,I2S_TX_MODE,ENABLE);
    I2S_IRQConfig(LPC_I2S,I2S_RX_MODE,4);
    I2S_IRQCmd(LPC_I2S,I2S_RX_MODE,ENABLE);
    NVIC_SetPriority(I2S_IRQn, 0x03);
    /*fill out buffer here to avoid clicks*/
    while(WriteInd != CHECK_BUFFER(ReadInd))
    {
         buffer[ReadInd] = I2S_Receive (LPC_I2S);
         INC_BUFFER(ReadInd);
    }
    I2S_ihf_Index =0;
    NVIC_EnableIRQ(I2S_IRQn);
  }
  else {I2S_Start(LPC_I2S);}
}

void I2S_Create_Sine(uint32_t frequency)
{
  int16_t value;
  int16_t* b = (int16_t*)buffer;
  I2S_BaseInit();
  I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,4);
  I2S_IRQCmd(LPC_I2S,I2S_TX_MODE,ENABLE);
  NVIC_SetPriority(I2S_IRQn, 0x03);
  while(CHECK_BUFFER(WriteInd))
  {//Max frequency = 24000, min frequency = 180
  /*
  Note about I2S:
  Each I2S data chunk is separated into 16 bits left channel and 16 bits right channel signed integers with most significant bit first (big endian)
  */
       value =  (int16_t)(sin((2*frequency*WriteInd*PI)/48000)* 200)+200;
       b[(2*WriteInd)] =value;//this doesn't take aliasing into account, so sounds really bad at times
       b[(2*WriteInd)+1] = value;
       INC_BUFFER(WriteInd);
  }
  I2S_ihf_Index =2;
  NVIC_EnableIRQ(I2S_IRQn);
}
void I2S_BaseInit()
{
  I2S_MODEConf_Type Clock_Config;
  I2S_CFG_Type I2S_Config_Struct;
  LPC_PINCON->PINSEL0|=PINS7_9TX;//Set pins 0.7-0.9 as func 2 (i2s Tx)
  LPC_PINCON->PINSEL1|=PINS023_025RX;//Set Pins 0.23-0.25 as func 3 (i2s Rx)
  I2S_Init(LPC_I2S);
  ConfInit(&I2S_Config_Struct, I2S_WORDWIDTH_16,I2S_STEREO,I2S_STOP_ENABLE,I2S_RESET_ENABLE,I2S_MUTE_DISABLE);
  ClockInit(&Clock_Config,I2S_CLKSEL_FRDCLK,I2S_4PIN_DISABLE,I2S_MCLK_DISABLE);
  ReadInd =WriteInd=0;
  LPC_I2S->I2STXRATE = 0x00;
  LPC_I2S->I2STXBITRATE = 0x00;
  I2S_FreqConfig(LPC_I2S, 48000, I2S_TX_MODE);//Set frequency for output
  I2S_FreqConfig(LPC_I2S, 48000, I2S_RX_MODE);//Set frequency for output
  I2S_SetBitRate(LPC_I2S,0,I2S_TX_MODE);
  I2S_Start(LPC_I2S);
}

void I2S_Polling_Read( uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length)
{
  unsigned int counter=0;
  uint32_t Dummy;
  while(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)==0x00);
  Dummy = I2S_Receive(LPC_I2S);//Dummy receive as first value is often trash
  while(counter<I2S_Pol_Length){
    while(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)==0x00);
    I2S_Pol_Buffer[counter] = I2S_Receive(LPC_I2S);
    ++counter;
  }
}

void I2S_Polling_Write( uint32_t* I2S_Pol_Buffer,uint32_t I2S_Pol_Length)
{
  unsigned int counter=0;
  while(counter<I2S_Pol_Length){
    while(I2S_GetLevel(LPC_I2S,I2S_TX_MODE)!=0x00);
    I2S_Send(LPC_I2S,I2S_Pol_Buffer[counter]);
    ++counter;
  }
}
void ConfInit(I2S_CFG_Type* I2S_Config_Struct,uint8_t wordwidth,uint8_t mono,uint8_t stop,uint8_t reset,uint8_t mute)
{
  I2S_Config_Struct->wordwidth = wordwidth;
  I2S_Config_Struct->mono = mono;
  I2S_Config_Struct->stop = stop;
  I2S_Config_Struct->reset = reset;
  I2S_Config_Struct->ws_sel = I2S_MASTER_MODE;
  I2S_Config_Struct->mute = mute;
  I2S_Config(LPC_I2S,I2S_TX_MODE,I2S_Config_Struct);
  I2S_Config_Struct->ws_sel = I2S_MASTER_MODE;
  I2S_Config(LPC_I2S,I2S_RX_MODE,I2S_Config_Struct);
}
void ClockInit(I2S_MODEConf_Type* I2S_ClkConfig,uint8_t clksource,uint8_t mode4pin,uint8_t mclkout)
{
  I2S_ClkConfig->clksel = clksource;
  I2S_ClkConfig->fpin = mode4pin;
  I2S_ClkConfig->mcena = mclkout;
  I2S_ModeConfig(LPC_I2S,I2S_ClkConfig,I2S_TX_MODE);
  I2S_ClkConfig->fpin = I2S_4PIN_ENABLE;
  I2S_ModeConfig(LPC_I2S,I2S_ClkConfig,I2S_RX_MODE);
}
