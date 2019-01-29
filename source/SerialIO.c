#include "SerialIO.h"

void UART0_IRQHandler(void)
{
	uint32_t tmp,error,intsrc;
	tmp = intsrc & UART_IIR_INTID_MASK;
	intsrc = UART_GetIntID(LPC_UART0);
	switch(tmp)
	{
		/*case UART_IIR_INTID_RLS:
			error = UART_GetLineStatus(LPC_UART0);
			error &= (UART_LSE_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
			if(error)UART_IntErr(error);
			break;*/
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
	uint32_t rLen;
	while(1)
	{
		rLen = UART_Receive((LPC_UART_TypeDef *)LPC_UART0, &tmp, 1, NONE_BLOCKING);
		if(!rLen)return;
		if((rbuf.rx_tail)&(UART_RING_BUFSIZE-1)!=(rbuf.rx_head+1)&(UART_RING_BUFSIZE-1))//if the buffer is full
		{
			rbuf.rx[rbuf.rx_head] = tmp;
			rbuf.rx_head = (rbuf.rx_head + 1)&(UART_RING_BUFSIZE-1);
		}
	}
	ProcessBuffer();
}

void ProcessBuffer()
{
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, DISABLE);
	while(!empty)
	{
		char	 cmd[5];
		memcpy(cmd,&rbuf.rx[rbuf.rx_tail],4);
		cmd[4] = '\0';
		if(strcmp("CMD:",cmd)==0)
		{
			switch(rbuf.rx[(rbuf.rx_tail+4) & (UART_RING_BUFSIZE-4)])
			{
				case 'B':

					break;
				case 'F':
					break;
				case 'P':
					break;
			}
		}
		rbuf.rx_tail = (rbuf.rx_tail+4) & (UART_RING_BUFSIZE-4);
	}
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);
}

void TransmitText(void)
{
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_THRE, DISABLE);
	while (UART_CheckBusy((LPC_UART_TypeDef *)LPC_UART0) == SET);
	while((rbuf.tx_tail)&(UART_RING_BUFSIZE-1)!=(rbuf.tx_head+1)&(UART_RING_BUFSIZE-1))
	{
		if(!(UART_Send((LPC_UART_TypeDef *)LPC_UART0, (uint8_t *)&rbuf.tx[rbuf.tx_tail], 1, NONE_BLOCKING)))break;
		rbuf.tx_tail = (rbuf.tx_tail + 1)&UART_RING_BUFSIZE-1;
	}
	//If there is no more data to send disable transmit interrupt
	if((rbuf.tx_tail)&(UART_RING_BUFSIZE-1)!=(rbuf.tx_head+1)&(UART_RING_BUFSIZE-1))
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
	NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
	NVIC_EnableIRQ(UART0_IRQn);
}
void EnableSerInterrupts(void)NVIC_EnableIRQ(UART0_IRQn);
void DisableSerInterrupts(void)NVIC_DisableIRQ(UART0_IRQn);
