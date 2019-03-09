#include "I2C.h"

void I2CInit(void)
{
	SetupPINS();

	I2C_Init(I2CBUS,100000);
	I2C_Cmd(I2CBUS,ENABLE);

}

void SetupVals(I2C_M_SETUP_Type* trData,int Address,uint8_t* Data, int TRLength)
{
	trData->sl_addr7bit = Address;

	trData->tx_data = Data;
	trData->tx_length = TRLength;
	trData->tx_count = 0;

	trData->rx_data = NULL;
	trData->rx_length = 0;
	trData->rx_count = 0;

	trData->retransmissions_max = 2;
}

void SetupPINS(void)
{
	PINSEL_CFG_Type PinCFG;
	PinCFG.Funcnum = 3;
	PinCFG.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCFG.Pinmode = PINSEL_PINMODE_PULLUP;

	PinCFG.Portnum =0;
	PinCFG.Pinnum = 0;
	PINSEL_ConfigPin(&PinCFG);
	PinCFG.Pinnum = 1;
	PINSEL_ConfigPin(&PinCFG);
}

void Send_Data(uint8_t* Data,int length)
{
  I2C_M_SETUP_Type trData;

  int i =0;
  for(i=0;i<length-1;i=i+2){
  	SetupVals(&trData,TLV320_ADDR,&Data[i],2);
  	I2C_MasterTransferData(I2CBUS,&trData,I2C_TRANSFER_POLLING);
  }

}

void TLV320_PlayWav(){
    uint8_t Data[8] = {0x1E,0x00,0x12,0x01,0x08,0x10,0x0A,0x00};
    TLV320_SendData(Data,8);}