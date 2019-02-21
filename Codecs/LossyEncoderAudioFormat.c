#include "LossyEncoderAudioFormat.h"

BUFLENGTH Wrap_Header(uint8_t* buf,LEAF_HEADER* header)
{//returns the bufferlength
    buf = (uint8_t*)header;
    return header->FirstFrameIndex-1;
}


LEAF_HEADER UnWrap_Header(uint8_t* buf)
{
    LEAF_HEADER header;
    header.Leaf = &buf[0];
    if(strncmp(header.Leaf,"LEAF",4))return header;//not a leaf file 
    header.FileSize = __ctl(buf[4]);
    header.FrameSize = __ctl(buf[8]);
    header.FrameNum = __ctl(buf[12]);
    header.BitRate = __ctl(buf[16]);
    header.SampleRate = __ctl(buf[20]);
    uint8_t c = 20;
    while(1)
    {
        if(buf[++c]=='\0')break;
        header.fName = buf[c];
    }
    header.FirstFrameIndex = ++c;
    return header;
}