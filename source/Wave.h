#ifndef WAVE_H_
#define WAVE_H_

#include <stdlib.h>
#include <string.h>
#include "LPC17xx.h"
#include "FatFS/ff.h"
#include "FatFS/diskio.h"
#include "SerialIO.h"
char* head_buffer;

#define __ctl(val) *(uint32_t*)(val)
#define __cti(val) *(uint16_t*)(val)

#define bswap_16(val) (val>>8)|(val<<8)
#define bswap_32(val) ((val>>24)&0xff) |((val<<8)&0xff0000) | ((val>>8)&0xff00) |((val<<24)&0xff000000)

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN -1

typedef struct {
  FIL* file;
  char* ChunkID;
  uint32_t ChunkSize;
  char* Format;
  char* Subchunk1ID;
  uint32_t Subchunk1Size;
  uint16_t AudioFormat;
  uint16_t NumChannels;
  uint32_t SampleRate;
  uint32_t ByteRate;
  uint16_t BlockAlign;
  uint16_t BitsPerSample;
  char* Subchunk2ID;
  uint32_t Subchunk2Size;
  uint32_t DataPtr;
  char Endian;
}WAVE_HEADER;

WAVE_HEADER Wav_Init(FIL* file );
void Wav_Read_Buffer(char* buffer, uint32_t buf_Size);
WAVE_HEADER Wav_Read_Buffered_Header(char* head_buffer);


#endif
