#include "MadeUpLosslessEncoder.h"


mule_Header* ReadHeader(char* buf,unsigned int bufLength)
{
  mule_Header* header;
  int i;
  for(i =0;i<4;i++)
  {
    header->Mule[i] = buf[i];
    header->FrameSize |= buf[i+4]<<(i*8);
    header->Bit_Rate |= buf[i+8]<<(i*8);
    header->Sample_Rate |= buf[i+12]<<(i*8);
    header->FrameNum |= buf[i+16]<<(i*8);
  }
  if(strncmp(header.Mule,"Mule",4) !=0)return NULL;
  header->FileSize = header->Bit_Rate*header->Sample_Rate*header->FrameSize*header->FrameNum;
  return header;
}

void DecompressBuffer(char* buf,unsigned int bufLength)
{
  //whenever the buffer is empty it needs to be refilled. Ideally, each frame is a buffer
  getBuffer(buf,bufLength);
  mule_Header* header = ReadHeader(buf,bufLength);
  if (header == NULL)return;//Not a Mule File
  uint32_t FrameCounter = 1;
  while(FrameCounter!= header->FrameNum)
  {
    getBuffer(buf,bufLength);

    ++FrameCounter;
  }
}
char* DecodeFrame(char* buf,unsigned int bufLength,uint8_t bitDepth)
{
  //Do decorrelation - do lossy algorithm and then work out difference.
  //do rle on difference
  //do rice on rle- converts low values to low binary values.
  char* outbuf = calloc(bufLength,sizeof(char*));
  double probability = GetProbability(buf,bufLength,bitDepth);
  int val = (int)ceil(-(0.69314718055994531/log(probability)));
  div_t divres;
  while()
  {
    divres = div(buf[i],b);
    ++divres.quot;
    int a = 0;
    while(!__builtin_popcount(divres.quot)){
      a+=pow(2,__builtin_ctz(divres.quot));

      //get leading zeros
    }

  }
}

unsigned double GetProbability(char* buf, unsigned int bufLength,uint8_t bitDepth)
{
  unsigned int i,totalonbitsinBuffer=0;
  for (i =0; i<bufLength;i++)totalonbitsinBuffer += __builtin_popcount(buf[i]);
  return = (double)totalonbitsinBuffer/ (double)(bufLength*bitDepth);
}
