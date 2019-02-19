#include "TLV320.h"

/*
We will be sending data to the TLV320 via I2C. To do this, we will need bring the
Mode pin low. After this, SDIN and SCLK pins will open I2C.
SDIN ->> Pin 9
SCLK ->> Pin 10
MODE ->> Ground/low

if CS = low,  address = 0x1A
if CS = high, address = 0x1B

Each Instruction Takes 2 parts, an instruction 7 bits and a data 9 bits. Important instructions are as follows:
NOTE: This means that the lsb of every instruction will be a data bit, and that the address needs to be
right shifted by 1/doubled.

//Nifty things:
Sidetone -> an attentuator for the input
Mic vs Line input circuits:
  The TLV320 has 2 circuits you can choose between for alternate functions.
*/
void TLV320_ChangeInputVolume(uint8_t newVolume){//Input Volume must be between 0 and 31.
  uint8_t Data[2] = {0x01,0x00};//0x01 = update left and right simultaneously
  Data[1] = (newVolume<<3);
  TLV320_SendData(Data,2);}
void TLV320_ChangeOutputVolume(uint8_t newVolume){//Output Volume must be between 0 and 128 BUT everything below 48 is MUTED
  uint8_t Data[2] = {0x05,0x00};//update left headphone = 0x04 + data bit 0x01
  Data[1] = (newVolume<<1)|(1<<7);//Enable Zero Cross Detection -> reduces clicks on volume change, but may result in latency of change.
  TLV320_SendData(Data,2);}
void TLV320_EnablePassThrough(){
  uint8_t Data[4] = {0x0C,0x00,0x08,0x08};//Turn Everything on, Disable DAC and sidetone for bypass, then
  TLV320_SendData(Data,4);}
void TLV320_DisablePassThrough(){
  uint8_t Data[6] = {0x08,0x08,0x08,0x08,0x08,0x10};//Renable DAC etc.
  TLV320_SendData(Data,6);}
void TLV320_Setup(){
  uint8_t Data[2] = {0x00,0xC0};
  TLV320_SendData(Data,2);}
void TLV320_EnableDAC()
{ uint8_t Data[8] = {0x0C,0x00,0x0E,0x02,0x10,0x20,0x12,0x01};//Turn on all, enable i2s mode, change sample rate, activate interface
  TLV320_SendData(Data,8);}
void TLV320_EnableOutput()
{ uint8_t Data[16] = {0x1E,0x00,0x0A,0x00,0x05,0xF9,0x08,0x10,0x0C,0x07,0x0E,0x02,0x10,0x00,0x12,0x01};//Enable DAC,Turn on Audio, enable i2s mode, change sample rate, activate interface
  TLV320_SendData(Data,16);}
void TLV320_Start_I2S_Polling_Passthrough(){
  uint8_t Data[22] = {0x1E,0x00,0x12,0x01,0x10,0x00,0x0E,0x02,0x0C,0x00,0x0A,0x01,0x08,0x10,0x00,0x17,0x02,0x17,0x05,0xFF,0x07,0xFF};//Reset, then
                      //0x1E,0x00,0x01,0x17,0x10,0x00,0x0E,0x02,0x12,0x01
  TLV320_SendData(Data,22);}
void TLV320_SendData(uint8_t* Data,int length)
{
  I2C_M_SETUP_Type trData;//void SetupVals(I2C_M_SETUP_Type* trData,int Address,uint8_t* Data, int TRLength);

  int i =0;
  for(i=0;i<length-1;i=i+2){
  	SetupVals(&trData,TLV320_ADDR,&Data[i],2);
  	I2C_MasterTransferData(I2CBUS,&trData,I2C_TRANSFER_POLLING);
  }

}
