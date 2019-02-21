#ifndef MULE_H_
#define MULE_H_

#include <math.h>
#include <stdlib.h>
//steps:

/*
http://www.hpl.hp.com/techreports/1999/HPL-1999-144.pdf
Create a symbol table containing keys for
//Compress buffer
each buffer creates a frame of a set size

For each buffer, to reduce required data needed to be stored, we need to do intra followed by inter channel decorrelation
here audi

After this, entropy encoding is used on the buffer. Either RLE or huffman can be used.
https://w3.ual.es/~vruiz/Docencia/Apuntes/Coding/Text/03-symbol_encoding/09-Golomb_coding/index.html

https://github.com/anirudhvr/golomb-coding/blob/master/encode.c
^ Rice Encoder, much more efficient than

Each frame will have the decorrelation predictor value stored at its beginning, followed by data.
*/

typedef struct
{//The mule header is 24 bytes long followed by padding to make one frame and preceeds any data;
  uint32_t FileSize=0;//filesize determined from file data
  uint32_t FrameSize=0;//Frame size in bytes likely 576 or 1152
  uint32_t FrameNum = 0;
  uint32_t Bit_Rate=0;
  uint32_t Sample_Rate=0;
  char Mule[4];
}mule_Header;
mule_Header ReadHeader(char* buf,unsigned int bufLength);
void CompressBuffer(char* buf,unsigned int bufLength );
void DecompressBuffer(char* buf,unsigned int bufLength);
#endif
