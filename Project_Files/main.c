#include "main.h"
int main()
{
  InitSerial();

  WriteText("Start\n\r");
  Delay_Init();
  Init_SSP0();
  char buff[50];
  uint16_t Din,Dout=0;

  while(1)
  {
    Dout++;
    if(Dout == 255)Dout = 0;
    SSP0_Write(&Dout,1);
    SSP0_Read(&Din,1);
    sprintf(buff, "Output: %d Input: %d ?? \n\r",Dout,Din);
    WriteText(buff);
  }
  return 0;
}
