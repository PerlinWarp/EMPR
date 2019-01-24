#include "spi.h"
#include "SerialIO.h"
#include "Delay.h"
#include "lpc17xx_uart.h"

int main(void) {
    InitSerial();
    WriteText("Initialising SSP\n\r");
    Init_SSP0();
    Init_SSP1();
    uint8_t Analog_Path = 0x816;
    WriteText("Writing on SSP0");
    SSP0_Write(&Analog_Path, 1);
    uint8_t Input;
    WriteText("Reading on SSP0");
    SSP1_Read(&Input, 1);
    WriteText((char)Input);
    return 0;
}