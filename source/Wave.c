#include "Wave.h"


WAVE_HEADER Wav_Init(FIL* file )
{
  WAVE_HEADER w;
  head_buffer = (char*)NewMalloc(43);
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
  w.ChunkSize     = __ctl(&head_buffer[4]);//little
  w.Format        = &head_buffer[8];
  w.Subchunk1ID   = &head_buffer[12];
  if(strncmp(w.Subchunk1ID,"fmt",3))return w;
  w.Subchunk1Size = __ctl(&head_buffer[16]);//little
  w.AudioFormat   = head_buffer[20];//little
  //if(AudioFormat[1] !=1)return w;//Other forms of compression not supported
  w.NumChannels   = head_buffer[22];//little
  w.SampleRate    = __ctl(&head_buffer[24]);//little
  w.ByteRate      = __ctl(&head_buffer[28]);//little
  w.BlockAlign    = head_buffer[32];//little
  w.BitsPerSample = head_buffer[34];//little

  w.Subchunk2ID   = &head_buffer[36];
  if(strncmp(w.Subchunk2ID,"data",4))return w;
  w.Subchunk2Size = __ctl(&head_buffer[40]);

  #if WAVE_DEBUG==1
  char fileInfo[100];
  sprintf(fileInfo,"Audio Format: %u\n\rNum Channels: %u\n\rSample Rate: %u\n\r",w.AudioFormat,w.NumChannels,w.SampleRate);
  WriteText(fileInfo);
  sprintf(fileInfo,"Bits Per Sample: %u\n\rByte Rate: %u\n\rChunk Size: %u\n\r",w.BitsPerSample,w.ByteRate,w.ChunkSize);
  WriteText(fileInfo);
  sprintf(fileInfo,"Subchunk 1 Size: %u\n\rBlock Align: %u\n\rSubchunk 2 Size: %u\n\r",w.Subchunk1Size,w.BlockAlign,w.Subchunk2Size);
  WriteText(fileInfo);
  WriteText("loaded Wave Successfully\n\r");
  #endif

  NewFree(head_buffer);
  return w;
}


/*
"What is spliff?":
    Spliff is a 32 bit raw file with simplified wave header (everythings big endian), so big endian other than the

*/

SPLIFF_HEADER CREATE_SPLIFF_HEADER(uint32_t Sample_Rate,uint32_t Data_Size)
{
  SPLIFF_HEADER s;
  s.Format        = "SPLIFF";
  s.Header_ID     = 'H';
  s.Header_Size   = 32;
  s.Sample_Rate   = Sample_Rate;
  s.Byte_Rate     = Sample_Rate*32;
  s.Audio_Format  = 0;
  s.Data_Size     = Data_Size;
  s.Data_Start    = 32;
  return s;
}


void SPLIFF_WRITE(FIL* file, SPLIFF_HEADER* s)
{
  UINT size;
  f_write(file,&s->Format,5,&size);
  f_write(file,&s->Header_ID,1,&size);
  f_write(file,&s->Header_Size,4,&size);
  f_write(file,&s->Sample_Rate,4,&size);
  f_write(file,&s->Byte_Rate,4,&size);
  f_write(file,&s->Audio_Format,2,&size);
  f_write(file,&s->Data_Size,4,&size);
  f_write(file,&s->Data_Start,2,&size);
}

SPLIFF_HEADER SPLIFF_DECODE(FIL* file)
{
  SPLIFF_HEADER s;
  head_buffer = (char*)NewMalloc(SPLIFF_SIZE);

  UINT size;
  f_read(file,head_buffer,SPLIFF_SIZE, &size);
  if(size < SPLIFF_SIZE)return s;
  s.Format        = &head_buffer[0];
  if(!strncmp(s.Format,"SPLIFF",5))return s;
  s.Header_ID     = *&head_buffer[5];
  s.Header_Size   = *&head_buffer[6];
  s.Sample_Rate   = *&head_buffer[10];
  s.Byte_Rate     = *&head_buffer[14];
  s.Audio_Format  = *&head_buffer[18];
  s.Data_Size     = *&head_buffer[20];
  s.Data_Start    = *&head_buffer[24];
  NewFree(head_buffer);
  return s;
}
