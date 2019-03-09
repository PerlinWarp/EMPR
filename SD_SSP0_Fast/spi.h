#ifndef SPI_H
#define SPI_H


#define SPI_MMC_SPEED 0x10
void spiInit (void);
short  SPIWrite(unsigned char *, short );
short  SPIRead(unsigned char *, short );
void spiEthChipSelect(char select);
void spiMMCChipSelect (char select);
void spiSendByte (unsigned char c);
unsigned char spiReceiveByte (void);
void spiReceivePtr (unsigned char *c);
void spiResetEthernet(void);



#endif
