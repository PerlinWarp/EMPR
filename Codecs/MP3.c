void MP3_FRAME_INIT(MP3_FRAME* mframe)
{
    mframe->Frame_Synchronizer= 0x2964619C7;//This looks gross, fix it (1 eleven times)
}

MP3_FRAME* DECODE_FRAME(long long int iStream)
{}

long long int ENDCODE_FRAME(MP3_FRAME* mframe){}