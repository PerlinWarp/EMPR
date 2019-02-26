#include "SerialIO.h"

uint8_t menuIndex;//what instruction to expect
void connect(char* instruction);
static void (*processinstruction[])(char*) = {&connect};

void UART0_IRQHandler(void)
{
	uint32_t tmp,intsrc =0 ;
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;
	switch(tmp)
	{
		case UART_IIR_INTID_RDA:
			ReceiveText();
			break;
		case UART_IIR_INTID_CTI:
			ReceiveText();
			break;
		case UART_IIR_INTID_THRE:
			TransmitText();
	}
}

void ReceiveText(void)
{
	uint8_t tmp;
	uint32_t rLen; // head = front of buffer, where data is read. tail = end, where data is receieved.
	while(rbuf.rx_head!=CHECK_BUFFER(rbuf.rx_tail))
	{
		rLen = UART_Receive((LPC_UART_TypeDef *)LPC_UART0, &tmp, 1, NONE_BLOCKING);
		if(!rLen)break; //return if nothing left to Receive
		if(tmp == '|')ProcessBuffer();
		rbuf.rx[rbuf.rx_tail] = tmp;
		INC_BUFFER(rbuf.rx_tail);

	}//if buffer is full, disable interrupts until a read takes place
	if(rbuf.rx_head==CHECK_BUFFER(rbuf.rx_tail)){UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, DISABLE);}
	else{UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);}
}
/*
Retrieves a length of data from the buffer
Input: the place to put the data, plus how much you want
Output: How much data was actually filled due to buffer emptiness
*/
uint32_t ReadText(char* data, uint32_t length)
{
	uint32_t counter = 0;
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);
	while((CHECK_BUFFER(rbuf.rx_head)!=rbuf.rx_tail)&&counter<length)
	{
		data[counter] = (char)rbuf.rx[rbuf.rx_head];
		INC_BUFFER(rbuf.rx_head);
		counter++;
	}
	return counter;
}

uint32_t AddTextToBuffer(char* data, uint32_t length)
{//Will stop when the buffer is full, or when you have run out of room to add
	TxIntStat = SET;
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_THRE, ENABLE); //reenable interrupts incase disabled when no more data could be retrieved.
	uint32_t counter = 0;
	while((CHECK_BUFFER(rbuf.tx_tail)!=rbuf.tx_head)&&counter<length)
	{
		rbuf.tx[rbuf.tx_tail] = (uint8_t)data[counter];
		INC_BUFFER(rbuf.tx_tail);
	}
	return counter;
}

/*
Reads backwards from command symbol either to next command symbol or to the beginning of the buffer,
processes the input, and then clears the buffer
Note: this only processes the most recent instruction. This should be fine in most cases as the interrupt routine should
call before then
*/
void ProcessBuffer()
{
	char tmp[INSTR_MAX_LEN];
	char* instruction;
	int i = INSTR_MAX_LEN;
	for(rbuf.rx_tail;CHECK_DEC_BUFFER(rbuf.rx_tail)!=rbuf.rx_head;DEC_BUFFER(rbuf.rx_tail))//go thro
	{
		if(rbuf.rx[rbuf.rx_tail]=='|')break;//break if instruction finished
		tmp[--i] = (char)rbuf.rx[rbuf.rx_tail];
	}
	instruction = (char*)malloc(sizeof(char)*(INSTR_MAX_LEN-i+1));//1 added for null character
	instruction[INSTR_MAX_LEN-i] = '\0';
	strncpy(instruction,tmp,(INSTR_MAX_LEN-i));//copy over the number of instructions present
	processinstruction[menuIndex](instruction);
	free(instruction);
}


void connect(char* instruction)
{
	if(!strcmp(instruction,"ACK")){
		Connected =1;
		return;
	}
	switch(instruction[0])
 	{
	 	case 'X':

	 }
}

void TransmitText(void)
{
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_THRE, DISABLE);
	while (UART_CheckBusy((LPC_UART_TypeDef *)LPC_UART0) == SET);
	while(rbuf.tx_tail!=CHECK_BUFFER(rbuf.tx_head))
	{
		if(!(UART_Send((LPC_UART_TypeDef *)LPC_UART0, (uint8_t *)&rbuf.tx[rbuf.tx_head], 1, NONE_BLOCKING)))break;
		INC_BUFFER(rbuf.tx_head);
	}
	//If there is no more data to send disable transmit interrupt
	if(rbuf.tx_tail==CHECK_BUFFER(rbuf.tx_head))
	{
		UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_THRE, DISABLE);
		TxIntStat = RESET;
	}
	else
	{
		TxIntStat = SET;
		UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_THRE, ENABLE);
	}
}

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

	//UartConfiguration.Baud_rate = 921600;

	UART_ConfigStructInit(&UartConfiguration);
	UART_FIFOConfigStructInit(&FifoConfiguration);

	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UartConfiguration);
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &FifoConfiguration);
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
}

int WriteText(char* TexttoWrite)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t*)TexttoWrite,strlen(TexttoWrite), BLOCKING));
}
int write_usb_serial_blocking(char *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
}
void InitSerInterrupts(void)
{
	menuIndex =0;
	Connected = 0;
	TxIntStat = RESET;
	rbuf.rx_head = 0;
	rbuf.rx_tail = 0;
	rbuf.tx_head = 0;
	rbuf.tx_tail = 0;

	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RLS, ENABLE);
	NVIC_SetPriority(UART0_IRQn, 0x02);// Should be higher than i2s, but lower than keypad
	NVIC_EnableIRQ(UART0_IRQn);
}
void EnableSerInterrupts(void){NVIC_EnableIRQ(UART0_IRQn);}
void DisableSerInterrupts(void){NVIC_DisableIRQ(UART0_IRQn);}
