#ifndef LEAF_H_
#define LEAF_H_

#ifndef __LPC17xx_H__
typedef unsigned long uint32_t;
typedef unsigned char uint8_t;
#endif

#define __ctl(val) *(unsigned long*)(val)
typedef uint32_t BUFLENGTH;

#include <math.h>
#include <stdlib.h>

/***********************************************************************//**
 * @file		LossyEncoderAudioFormat.h
 * @brief		Contains all macro definitions and function prototypes
 * 				to encode and decode .leaf files
 * @version		0.1
 * @date		21. February. 2019
 * @author		Idris Al-Ghabra
 **************************************************************************/

/*Leaf header is at the start of any .leaf file is variable in length, but ends with*/
typedef struct
{
  char* Leaf; //Name of Leaf file format. Should be "LEAF\0"
  uint32_t FileSize;//filesize determined from file data
  uint32_t FrameSize;//Frame size in bytes likely 512 or 1024
  uint32_t FrameNum;
  uint32_t BitRate;
  uint32_t SampleRate;
  char* fName;//File names can be up to 100 characters in length, and end with a null terminator to avoid wasted space
  
  uint8_t* FirstFrameIndex;//Not stored in .leaf but useful for other users
}LEAF_HEADER;

typedef struct 
{
    uint32_t* data;
}LEAF_FRAME;


BUFLENGTH Wrap_Header(uint32_t* buf,LEAF_HEADER header);
LEAF_HEADER UnWrap_Header(uint32_t* buf);
void Wrap_Frame(uint32_t* buf,LEAF_FRAME frame);
LEAF_FRAME UnWrap_Frame(uint32_t* buf,uint32_t buf_Length);

void Decode_Frame(uint32_t* buf,uint32_t buf_Length);
void Encode_Frame(uint32_t* buf,uint32_t buf_Length);

#endif