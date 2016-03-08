//---------------------------------------------------------------------------

#ifndef WavFileH
#define WavFileH

struct RIFFHeader
{
        char Head[4];
        unsigned int TotalLen;
        char Type[4];
};

struct RIFFFormat
{
        char Head[4];
        unsigned int BlkLen;
        short fill;
        short NoChannels;
        unsigned int SampleRate;
        unsigned int BytesSec;
        short BytesSample;
        short BitsSample;
};

struct RIFFData
{
        char Head[4];
        unsigned int DataLen;
        unsigned char *Data;
};



class TWavFile
{
private:
        AnsiString FileName;
        struct RIFFHeader Head;
        struct RIFFFormat Format;
        struct RIFFData Data;
        bool Signed;
public:
        int SampleRate, NoSamples;
        bool Stereo;

        bool LoadFile(AnsiString Filename);
        bool SaveFile(AnsiString Filename);
        unsigned char Sample(int Pos, int Channel);

        TWavFile();
        ~TWavFile();
};
#endif
