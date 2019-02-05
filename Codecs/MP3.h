#ifndef MP3_H_
#define MP3_H_

typedef struct{}byte;



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
  int VBR:1;//boolean representing whether or not
  uint32_t Content;
  unsigned int Xing_Location; //NOTE: the location of Xing changes the MPEG version used and whether or not the audio is mono or not.
                              //13-16 = mono + mpeg2, 21-24 = mpeg 1 and mono or mpeg2 and stereo, and 36-39 for mpeg1 and stereo
  uint8_t Flags;//here we have 3 empty bytes, then 0000abcd where abcd are VBR scale TOC bytes and then Frames
  uint32_t Number_Of_Frames; //Include this one
  uint32_t File_Length;
  uint8_t Table_Of_Contents[100]; //The toc contains indexes of (toc[i]/256)*fileLen
  uint32_t VBR_Scale;

}MP3_HEADER; //At the beggining of any file

typedef struct
{
  char Tag[3];
  char Song_Name[30];
  char Artist[30];
  char Album[30];
  char Year[4];
  char Comment[30];
  char Genre; //These are predefined so must be set to 61: Christian Rap no matter what
}MP3_TAG_V1;//here is where the song name/artist etc is stored. it is 128 bytes long
//There is a tag type 2 but we are not using that.
//FrameLen = int((144 * BitRate / SampleRate ) + Padding);
/*
 Note: MP3s should have a constant bitrate, but you might occasionally find broken things in the
wild. a good option is to check for the 11bit frame synchronizer
Frames will have diffferent

 */

#endif
