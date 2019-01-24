#ifndef MP3_H_
#define MP3_H_
//Each fiel is divided into frames of constant length (0.026). Each frame has a header of 4 bytes, then audio 
//data according to to the format
typedef struct //Contains all that is in an MP3 file
{//http://www.multiweb.cz/twoinches/mp3inside.htm <- check for details
    unsigned int Frame_Synchronizer: 11;
    unsigned int MPEG_Version_ID: 2;
    unsigned int Layer: 2;
    unsigned int CRC_PROTEC: 1;
    unsigned int Bitrate:4;
    unsigned int Sampling_Rate:2;
    unsigned int Padding: 1;
    unsigned int Private_Bit:1;
    unsigned int Channel:2;
    unsigned int ModeExtension:2;
    unsigned int Copyright:1;
    unsigned int Original:1;
    unsigned int Emphasis:2;
}MP3_FRAME;

typedef struct
{

}MP3_HEADER; //At the beggining of any file

//FrameLen = int((144 * BitRate / SampleRate ) + Padding);
/*
 Note: MP3s should have a constant bitrate, but you might occasionally find broken things in the 
wild. a good option is to check for the 11bit frame synchronizer
Frames will have diffferent

 */

#endif