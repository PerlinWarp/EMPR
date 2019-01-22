#include "SPI.h"

int main(void) {
    Init_SSP0();
    u_int16_t Analog_Path = 0x816;
    SSP0_Write(&Analog_Path, 1);

    return 0;
}