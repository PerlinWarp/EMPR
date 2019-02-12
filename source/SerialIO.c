#include "SerialIO.h"

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

/*Reads the entire buffer, and then checks through it for data - assuming nothing else but command data is passed through*/
void ProcessBuffer()
{//Figure something out which isn't trash PLEASE.
	char data[UART_RING_BUFSIZE];
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, DISABLE);
	uint32_t length = ReadText(data,UART_RING_BUFSIZE);
	int i;
	for(i =0;i<length;i++)
	{
		if(data[i] == 'C' && data[i+1] == 'M' && data[i+2] == 'D' && data[i+3] == ':')
		{
			switch(data[i+4])
			{
				case 'B':
					// buttonpress = 1;
					// key = 'B';
					break;
				case 'F':
					// buttonpress = 1;
					// key = '*';
					break;
				case 'P':
					// buttonpress = 1;
					// key = '#';
					break;
			}
		}
	}
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);
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
void InitSerInterrupts(void)
{
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
