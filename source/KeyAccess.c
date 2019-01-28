#include "KeyAccess.h"

char GetKeyInput()
{
	char keys[4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'}

	};
	tuple coords = DecodeInput();
	if(coords.i == TP_FAILURE)
	{
		return ' ';
	}
	else
	{
		return keys[coords.i][coords.j];
	}
}

tuple DecodeInput()
{
	tuple coords,fail;
	fail.i = TP_FAILURE;
	int rowMask = 1,columnMask =16;
	unsigned char Data = GetKeyData(0x0F);//Mask out Columns
	if(Data==0)return fail;
	for(coords.i = 3;coords.i>=0;coords.i--)
		{
			if((Data&rowMask)>0)
			{
				for(coords.j = 3;coords.j>=0;coords.j--)//Cycle through columns
				{
					Data = GetKeyData(columnMask);//Check rows

					if((Data&columnMask)>0)
					{
						return coords;
					}
					columnMask<<=1;
				}
			}
			rowMask<<=1;
		}
	return fail;
}

unsigned char GetKeyData(char BitMask)
{
	uint8_t recieve,addr = (uint8_t)BitMask;
	unsigned char Data;
	I2C_M_SETUP_Type trData;

	trData.sl_addr7bit = 0x21;//Address for Keypad
	trData.retransmissions_max = 2;

	trData.tx_data = &addr;
	trData.tx_length = 1;
	trData.tx_count = 0;

	trData.rx_data = NULL;
	trData.rx_length = 0;
	trData.rx_count = 0;

	I2C_MasterTransferData(I2CBUS,&trData,I2C_TRANSFER_POLLING);

	trData.tx_data = NULL;
	trData.tx_length = 0;
	trData.tx_count = 0;

	trData.rx_data = &recieve;
	trData.rx_length = 1;
	trData.rx_count = 0;

	I2C_MasterTransferData(I2CBUS,&trData,I2C_TRANSFER_POLLING);

	Data = (unsigned char)recieve;
	Data = ~Data;
	return Data;
}
