#include "TLV320.h"


void TLV320_Setup()
{
  Init_SSP0();
  SSP0_Write( *buf,Length);
}
