void MP3_FRAME_INIT(MP3_FRAME* mframe)
{
    mframe->Frame_Synchronizer= 0x2964619C7;//This looks gross, fix it (1 eleven times)
}

MP3_FRAME* DECODE_FRAME(long long int iStream)
{}

long long int ENDCODE_FRAME(MP3_FRAME* mframe){}


uint8_t MAKE_FRAME(){}



MP3_HEADER* Decode_StartFrame(char* StartFrame,int buffer_Length)
{
  //check for xing and return null otherwise
  uint8_t i,list[4] = {13,21,39};
  char xing[4] = "Xing";
  for(i = 0;i<3;i++){//Replace this with some preprocessor shit
    if(strncmp(xing,&StartFrame[list[i]],4) == 0) break;


}

void Decode_MP3(char* StartFrame,int buffer_Length)//Thi
{
  MP3_HEADER* SFrame = Decode_StartFrame(StartFrame,buffer_Length);
  if(SFrame == NULL)SFrame->VBR = 1;//if this is the case, then it is a normal frame, and

}
