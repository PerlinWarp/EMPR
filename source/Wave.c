#include "Wave.h"


WAVE_HEADER Wav_Init(FIL* file )
{
  WAVE_HEADER w;
  head_buffer = (char*)malloc(43);
  UINT size;
  f_read(file,head_buffer,43, &size);
  if(size < 43)return w;
  w.file          = file;
  w.ChunkID       = &head_buffer[0];
  if(strncmp(w.ChunkID,"RIFF",4))
  {
    if(strncmp(w.ChunkID,"RIFX",4))return w;
    else{w.Endian = BIG_ENDIAN;}
  }
  else{w.Endian = LITTLE_ENDIAN;}
  w.ChunkSize     = head_buffer[4];//little
  w.Format        = &head_buffer[8];
  w.Subchunk1ID   = &head_buffer[12];
  if(strncmp(w.Subchunk1ID,"fmt",3))return w;
  w.Subchunk1Size = head_buffer[16];//little
  w.AudioFormat   = head_buffer[20];//little
  //if(AudioFormat[1] !=1)return w;//Other forms of compression not supported
  w.NumChannels   = head_buffer[22];//little
  w.SampleRate    = head_buffer[24];//little
  w.ByteRate      = head_buffer[28];//little
  w.BlockAlign    = head_buffer[32];//little
  w.BitsPerSample = head_buffer[34];//little
  w.Subchunk2ID   = &head_buffer[36];
  if(strncmp(w.Subchunk2ID,"data",4))return w;
  w.Subchunk2Size = head_buffer[40];
  char fileInfo[100];
  sprintf(fileInfo,"Audio Format: %u\n\rNum Channels:%u\n\rSample Rate:0x%08x\n\r",w.AudioFormat,w.NumChannels,w.SampleRate);
  WriteText(fileInfo);
  WriteText("loaded Wave Successfully\n\r");
  return w;
}

void Wav_Read_head_buffer(char* head_buffer, uint32_t buf_Size)
{

}
