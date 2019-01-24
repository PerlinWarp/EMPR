#include "SPI.h"

int main(void) {
    Init_SSP0();
    while(1) {
	    Write_SSP0(0x5A);
	}
    return 0;
}
