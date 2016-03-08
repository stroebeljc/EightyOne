//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "WavFile.h"

//---------------------------------------------------------------------------

TWavFile::TWavFile()
{
        FileName="";
        SampleRate=0;
        NoSamples=0;
        Data.Data=NULL;
}

TWavFile::~TWavFile()
{
        FileName="";
        SampleRate=0;
        NoSamples=0;
        Stereo=false;

        if (Data.Data) delete Data.Data;

}

bool TWavFile::LoadFile(AnsiString FName)
{
        FILE *f;
        int i, total;
        unsigned char *buffer;

        f=fopen(FName.c_str(), "rb");

        fread(&Head,sizeof(struct RIFFHeader),1,f);
        fread(&Format,sizeof(struct RIFFFormat),1,f);
        fread(&Data.Head, 4,1,f);
        fread(&Data.DataLen, 4,1,f);

        if (strncmp(Head.Head, "RIFF",4)) return false;
        if (strncmp(Format.Head,"fmt",3)) return false;
        if (strncmp(Data.Head,"data",4)) return false;

        SampleRate = Format.SampleRate;
        NoSamples = Data.DataLen /  Format.BytesSample;
        Signed=false;

        if (Format.NoChannels == 1) Stereo=false;
        else Stereo=true;

        if (Data.Data!=NULL) delete Data.Data;
        Data.Data=new char[Data.DataLen];

        //buffer=(signed char *) malloc(Data.DataLen);

        fread(Data.Data, Data.DataLen, 1, f);
        fclose(f);

        //for(i=0; i<NoSamples; i++)
        //{
        //        Application->ProcessMessages();
        //        Data.Data[i]=buffer[i];
        //}

        //free(buffer);

        FileName=FName;

        //total=0;
        //for(i=0;i<256;i++) total += Sample(i,0);
        //total /= 256;
        //if (total<64 || total>192) Signed=true;
        if (Format.BitsSample>8) Signed=true;

        return(true);
}

bool TWavFile::SaveFile(AnsiString FName)
{

        return(true);
}

unsigned char TWavFile::Sample(int SampleNo, int Channel)
{
        int Pos, SampleSize;
        unsigned char data;
        if (SampleNo>=NoSamples || SampleNo<0) return(128);

        SampleSize = Format.BytesSample / Format.NoChannels;

        Pos = SampleNo * Format.BytesSample + Channel * SampleSize;


        switch( SampleSize )
        {
        case 1:
                data = Data.Data[Pos];
                break;

        case 2:
                data = (* (( unsigned short *) (Data.Data + Pos))) / 256;
                break;

        default:
                data=128;
        }

        if (Signed==true) data = 128 + ((signed char) data);
        return(data);
}

