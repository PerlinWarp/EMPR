#include "main.h"
int main()
{
  InitSerial();

  WriteText("Start\n\r");
  Delay_Init();
  InitSPI_Default();
  char buff[50];
  uint8_t Din,Dout=0;

  while(1)
  {
    Dout++;
    if(Dout == 255)Dout = 0;
    SPI_Write(Dout);
    Din = SPI_Read();
    sprintf(buff, "Output: %d Input: %d ?? \n\r",Dout,Din);
    WriteText(buff);
  }
  return 0;
}
