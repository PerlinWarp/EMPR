#include "SPI.h"
#include "SerialIO.h"
#include "Delay.h"

int main(void){
    Delay_Init();
    InitSerial();
    WriteText("Start \n");
    Init_SSP0();
    WriteText("SSP0 Write \n");
	Write_SSP0(0x5A);

    Init_SSP1();
    char data = 0xFF;
    WriteText("SSP1 Read \n");
    data = Read_SSP1();
    WriteText(&data);
    Delay(10);
    WriteText("Completed \n");
    return 0;
}
