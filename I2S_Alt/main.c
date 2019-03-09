#include "SerialIO.h"
#include "ff.h"

#include "I2S.h"
//#include "Wave.h"
#include "TLV320.h"

uint32_t* BufferOut;
uint32_t* BufferIn;

DWORD get_fattime (void)
{
	return(0);
}

int SD_Out(void) {
  FIL fil;        /* File object */
  FRESULT fr;     /* FatFs return code */
  FATFS *fs;

  fs = malloc(sizeof(FATFS));
  fr = f_mount(fs, "", 0);
  fr = f_open(&fil, "a.wav", FA_READ);

  /* Read every line and display it */
  uint y;
  char buffer [0x20];

  while (!fr){
      fr = f_read(&fil,buffer,0x20, &y);
      write_usb_serial_blocking(buffer,y);
      // SEND TO I2S FUNCTION!!!
  }

  // Close the file and unmount
  f_close(&fil);
  f_mount(0, "", 0);
  free(fs);
  return 0;
}

void Bypass() {
  uint8_t Data[14] = {0x1E, 0x00, 0x0C, 0x0E, 0x00, 0x2E, 0x04, 0x2E, 0x08, 0xF2, 0x0C, 0xF2, 0x10, 0x08};
  // 0x1E 0x00 - Reset all Registers
  // 0x0C 0x0E - Power On Modules
  TLV320_SendData(Data, 14);
}

//void TLV320_Out() {
//  uint8_t Data[] = {};
//  TLV320_SendData(Data, );
//}

int main(void) {
  InitSerial();
  //I2CInit();
  //TLV320_Out();
  WriteText("Enabling PassThrough...\n");
  SD_Out();
  WriteText("PassThrough Enabled");
  return 0;
}


// Ask someone to help make a circular buffer that can be passed into f_read.
// Work out register vals to enable I2S on TLV320 (32 bit word len, 44.1khz audio)
// Modify example I2S IRQ to write data from circular buffer

// Refer to https://os.mbed.com/users/d_worrall/notebook/tlv320-stereo-audio-codec/