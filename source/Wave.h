#ifndef WAVE_H_
#define WAVE_H_

#include <stdlib.h>
#include <string.h>
#include "LPC17xx.h"
#include "FatFS/ff.h"
#include "FatFS/diskio.h"

char* head_buffer;

#define bswap_16(val) (val>>8)|(val<<8)
#define bswap_32(val) ((val>>24)&0xff) |((val<<8)&0xff0000) | ((val>>8)&0xff00) |((val<<24)&0xff000000);

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN -1

typedef struct {
  FIL* file;
  char* ChunkID;
  char* ChunkSize;
  char* Format;
  char* Subchunk1ID;
  char* Subchunk1Size;
  char* AudioFormat;
  char* NumChannels;
  char* SampleRate;
  char* ByteRate;
  char* BlockAlign;
  char* BitsPerSample;
  char* Subchunk2ID;
  char* Subchunk2Size;
  char* DataPtr;
  char Endian;
}WAVE_HEADER;

WAVE_HEADER Wav_Init(FIL* file );
void Wav_Read_Buffer(char* buffer, uint32_t buf_Size);


#endif
