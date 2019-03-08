#ifndef WAVE_H_
#define WAVE_H_

#include <stdlib.h>
#include <string.h>
#include "LPC17xx.h"
#include "FatFS/ff.h"
#include "FatFS/diskio.h"
#include "SerialIO.h"
#include "NewMalloc.h"
char* head_buffer;

#define __ctl(val) *(uint32_t*)(val)
#define __cti(val) *(uint16_t*)(val)

#define bswap_16(val) (val>>8)|(val<<8)
#define bswap_32(val) ((val>>24)&0xff) |((val<<8)&0xff0000) | ((val>>8)&0xff00) |((val<<24)&0xff000000)

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN -1

#define WAVE_DEBUG 0
#define SPLIFF_SIZE 32

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


typedef struct {
  char* Format;//should be "SPLIFF"
  char Header_ID; //H for header start
  uint32_t Header_Size; //always 32 bytes
  uint32_t Sample_Rate; //sample rate in hz
  uint32_t Byte_Rate; //will be 32*sample rate
  uint16_t Audio_Format;//0 if 32 bit unsigned data, 1 if 16 bit signed stereo.
  uint32_t Data_Size; //Size of data in bytes
  uint16_t Data_Start;//Where data starts -> always 32th bit
}SPLIFF_HEADER;

WAVE_HEADER Wav_Init(FIL* file );
void Wav_Read_Buffer(char* buffer, uint32_t buf_Size);
WAVE_HEADER Wav_Read_Buffered_Header(char* head_buffer);

SPLIFF_HEADER SPLIFF_DECODE(FIL* file);
void SPLIFF_WRITE(FIL* file, SPLIFF_HEADER* s);
SPLIFF_HEADER CREATE_SPLIFF_HEADER(uint32_t Sample_Rate,uint32_t Data_Size);

#endif
