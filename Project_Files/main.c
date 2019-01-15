#include "main.h"
int main()
{
  InitSPI_Default();
  InitSerial();
  Delay_Init();
  char buff[50];
  uint8_t Din,Dout=0;
  while(1)
  {
    Dout++;
    if(Dout == 255)Dout = 0;
    SPI_Write(Dout);
    Delay(100);
    Din = SPI_Read();
    sprintf(buff, "Output: %d Input: %d \n",Dout,Din);
    WriteText(buff);
  }
  return 0;
}
