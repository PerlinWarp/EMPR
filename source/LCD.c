#include "LCD.h"

void SendData(uint8_t* Data,int length)
{
	I2C_M_SETUP_Type trData;
	SetupVals(&trData,LCD_ADDR,Data,length);
	I2C_MasterTransferData(I2CBUS,&trData,I2C_TRANSFER_POLLING);
}

void LCDEnable(){
	uint8_t Data[2] = {0x00,0x0C};
	SendData(Data,2);}
void LCDDisable(){
	uint8_t Data[2] = {0x00,0x08};
	SendData(Data,2);}
void LCDGoHome(){
	uint8_t Data[2] = {0x00,0x02};
	SendData(Data,2);}
void LCDNewLine(){
	uint8_t Data[2] = {0x00,0xC0};
	SendData(Data,2);}
void LCDCursorShift(uint8_t value){//Note the possibility of using this to create a shift register animation clear
	uint8_t Data[value],i;
	for(i=1;i<value;i++)Data[i] = 0x14;
	Data[0] = 0x00;
	SendData(Data,value);}
void LCDInit(){
	uint8_t Data[11] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02};
	SendData(Data,11);}

void LCDClear()//NOTE: With character set R, clear command doesn't work. Instead, blank space (0xA0 or 0x91) must be written to every space
{
	uint8_t Data[4] = {0x00,0x34,0x0E,0x06};
	uint8_t clrLine[17] = {0x40,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0};
	LCDDisable();
	LCDGoHome();
	SendData(Data,4);
	SendData(clrLine,17);
	LCDNewLine();
	SendData(clrLine,17);
	LCDGoHome();
	LCDEnable();
	Delay(10);
}


void LCDPrint(char* input)
{
	int i,datacount=1,inputlen = strlen(input);
	uint8_t* Data = (uint8_t*)malloc(sizeof(uint8_t)*(inputlen+1));
	Data[0] = 0x40;
	for(i = 0; i<inputlen;i++)
	{
		if(input[i]==10){
			SendData(Data,datacount);
			LCDNewLine();
			Data[0] = 0x40;
			datacount = 0;
		}
		else if	(input[i]<0x60){
			Data[datacount]=input[i]|0x80;
		}
		else{
	 		Data[datacount] = input[i];
	 	}
	 	datacount++;
	}

	SendData(Data,datacount);
	free(Data);
}

void main_LDCPrintHelloWorld(void)
{
	SysTick_Config(SystemCoreClock/10-1);

	SetupPINS();
	I2C_Init(I2CBUS,100000);
	I2C_Cmd(I2CBUS,ENABLE);
	LCDInit();
	LCDClear();
	LCDPrint("What the\n heggins?");
}
