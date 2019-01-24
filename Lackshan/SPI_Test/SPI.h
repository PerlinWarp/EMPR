#include "LPC17xx.h"

//Initialises P0.15 - P0.18 with SSP0 as Master
void Init_SSP0(void){
    LPC_SC->PCONP |= 1<<21; //re-enable POWER to SSP0-SPI if required

    LPC_SC->PCLKSEL1 |= 1<<10; 	//pclk = cclk

    LPC_SSP0->CPSR |= 8; //internal divider

    LPC_PINCON->PINSEL0 |= 0x80000000; 	//Pin P0.15 allocated to function 3 SCLK
    LPC_PINCON->PINSEL1 |= 0x02<<0; 	//Pin P0.16 allocated to function 3 NSS/SSEL
    LPC_PINCON->PINSEL1 |= 0x02<<2; 	//Pin P0.17 allocated to function 3 MISO
    LPC_PINCON->PINSEL1 |= 0x02<<4; 	//Pin P0.18 allocated to function 3 MOSI

    LPC_SSP0->CR0 &= ~(3<<4); //spi mode
    LPC_SSP0->CR0 |= 7<<0; // 8 bits       
    LPC_SSP0->CR0 |= 10000000; //CPOL=0 and CPHA=1

    LPC_SSP0->CR1 &= ~(1<<2); //master mode
    LPC_SSP0->CR1 |= 1<<1; //enable SSP - must be LAST

}

void Write_SSP0(char data){
    LPC_SSP0->DR = data;
	while (!(LPC_SSP0->SR & (1<<2)));
}

//Initialises P0.6 - P0.9 with SSP1 as Slave
void Init_SSP1(void){
    LPC_SC->PCONP |= 1<<10; //re-enable POWER to SSP1-SPI if required

    LPC_SC->PCLKSEL0 |= 1<<20; 	//pclk = cclk

    LPC_SSP0->CPSR |= 8; //CHECK DOCS - COULD BE WRONG

    LPC_PINCON->PINSEL0 |= 0000000010000000; 	//Pin P0.7 allocated to function 3 SCLK
    LPC_PINCON->PINSEL0 |= 0000000001000000; 	//Pin P0.6 allocated to function 3 NSS/SSEL
    LPC_PINCON->PINSEL0 |= 0000000100000000; 	//Pin P0.8 allocated to function 3 MISO
    LPC_PINCON->PINSEL0 |= 0000001000000000; 	//Pin P0.9 allocated to function 3 MOSI

    LPC_SSP0->CR0 &= ~(3<<4); //spi mode
    LPC_SSP0->CR0 |= 7<<0; // 8 bits       
    LPC_SSP0->CR0 |= 10000000; //CPOL=0 and CPHA=1

    LPC_SSP0->CR1 &= (1<<2); //Slave mode - Need a 0 here so can be WRONG - ASK IDRIS
    LPC_SSP0->CR1 |= 1<<1; //enable SSP - must be LAST
}

char Read_SSP1(void){
    return(LPC_SSP1->DR);
    //Might need a while statement here - idk why
}

// Char may not be suitable for 8 bit value - change it
// Add SerialIO stuff to this directory
// Test