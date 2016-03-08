//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "wavload_.h"
#include "main_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWavLoad *WavLoad;
TWavFile Wav;

void TWavLoad::ClearImage()
{
        TCanvas *Img;
        int w,h,x,y;


        Img=Image1->Canvas;
        w= Image1->Width;
        h=Image1->Height;

        Img->Pen->Color = clWhite;

        for(y=0;y<h;y++)
        {
                Img->MoveTo(0,y);
                Img->LineTo(w,y);
        }

        Img->Pen->Color = clRed;
        Img->MoveTo(0,h/2);
        Img->LineTo(w,h/2);
        Img->MoveTo(w/2,0);
        Img->LineTo(w/2,256);
        Img->Pen->Color = clGreen;
        Img->MoveTo(0,64-(Threshold/2));
        Img->LineTo(w,64-(Threshold/2));
        DoCaption("");
}

void TWavLoad::UpdateImage()
{
        TCanvas *Img;
        long w,h,x,y;
        int Pos,Sample;

        Img=Image1->Canvas;
        Pos=ScrollBar->Position;

        w= Image1->Width;
        h=Image1->Height;

        Img->Pen->Color = clWhite;

        for (x=0; x<w; x++)
        {
                Img->MoveTo(x,0);
                Img->LineTo(x,128);
        }

        Img->Pen->Color = clRed;
        Img->MoveTo(0,h/2);
        Img->LineTo(w,h/2);
        Img->MoveTo(w/2,0);
        Img->LineTo(w/2,128);
        Img->Pen->Color = clGreen;
        Img->MoveTo(0,64-(Threshold/2));
        Img->LineTo(w,64-(Threshold/2));

        Img->Pen->Color = clBlack;
        Img->MoveTo(0, Wav.Sample(Pos,Channel)/2);
        for (x=0; x<w; x++)
        {
                //Img->LineTo(x,255-Wav.Sample(Pos+x));
                //Img->MoveTo(x,64);
                Sample=Wav.Sample(Pos+x,Channel);


                Img->LineTo(x, Sample/2);

                if  (    (((128-Sample) > Threshold) && Threshold>=0)
                      || (((128-Sample) < Threshold) && Threshold<0)  )
                {
                        Img->MoveTo(x,64-(Threshold/2));
                        Img->LineTo(x, Wav.Sample(Pos+x,Channel)/2);
                }
        }
        DoCaption("");
}
//---------------------------------------------------------------------------
__fastcall TWavLoad::TWavLoad(TComponent* Owner)
        : TForm(Owner)
{
        TStatesSample=0;
        TapePos=0;
        CurrentTStates=0;
        ScreenCounter=0;
        Playing=0;
        Threshold=12;
        Channel=0;
        StatusText="Stopped";
        StereoTxt1->Enabled = false;
        StereoTxt2->Enabled = false;
        LeftBtn->Enabled=false;
        RightBtn->Enabled=false;
        ClearImage();
}
//---------------------------------------------------------------------------


void __fastcall TWavLoad::LoadClick(TObject *Sender)
{
        if (!LoadWavDialog->Execute()) return;

        DoCaption("Loading...");
        Wav.LoadFile(LoadWavDialog->FileName);

        TStatesSample=((3250000 / Wav.SampleRate));//*7)/8;
        TapePos=0;
        CurrentTStates=0;
        Playing=0;
        Channel=0;
        ScrollBar->Max = Wav.NoSamples;
        ScrollBar->Position = 0;

        StereoTxt1->Enabled = false;
        StereoTxt2->Enabled = false;
        LeftBtn->Enabled=false;
        RightBtn->Enabled=false;
        if (Wav.Stereo)
        {
                StereoTxt1->Enabled = true;
                StereoTxt2->Enabled = true;
                LeftBtn->Enabled=true;
                RightBtn->Enabled=true;
        }
        UpdateImage();
        DoCaption("Stopped");
}
//---------------------------------------------------------------------------
void __fastcall TWavLoad::ScrollBarChange(TObject *Sender)
{
        TapePos=ScrollBar->Position;
        UpdateImage();
}
//---------------------------------------------------------------------------
void __fastcall TWavLoad::PlayBtnClick(TObject *Sender)
{
        if (Wav.NoSamples==0) return;

        Playing=!Playing;

        if (Playing)
        {
                PlayBtn->Caption="Stop";
                DoCaption("Playing...");
        }
        else
        {
                PlayBtn->Caption="Play";
                DoCaption("Stopped");
        }
}
//---------------------------------------------------------------------------

bool TWavLoad::GetEarState()
{
        int i;

        if (!Playing) return (0);
        
        i=128-Wav.Sample(TapePos+200,Channel);

        //if (i<0) i=-i;
        if (    (Threshold>=0 && i>=Threshold)
             || (Threshold<0 && i<Threshold) ) return(1);
        else return(0);
}

void TWavLoad::ClockTick(int TStates)
{
        if (!Playing) return;

        ScreenCounter += TStates;
        CurrentTStates += TStates;

        if (CurrentTStates<TStatesSample) return;

        CurrentTStates -= TStatesSample;
        TapePos++;
        if (TapePos==Wav.NoSamples-200)
        {
                Stop();
                return;
        }

        if (ScreenCounter<1083333) return;

        ScreenCounter=0;
        ScrollBar->Position = TapePos;
        UpdateImage();
        DoCaption("Playing...");
}

void TWavLoad::Stop()
{
        if (!AutoBtn->Checked) return;

        Playing=0;
        ScrollBar->Position = TapePos;

        DoCaption("Stopped");
        PlayBtn->Caption="Play";
        UpdateImage();
}

void TWavLoad::Start()
{
        if (!AutoBtn->Checked) return;

        Playing=1;

        DoCaption("Playing");
        PlayBtn->Caption="Stop";
}


void TWavLoad::DoCaption(AnsiString Message)
{
        if (Message=="") Message=StatusText;
        else StatusText=Message;

        if (Wav.SampleRate>0)
        {
                Message += " ";
                Message += TapePos / Wav.SampleRate;
                Message += "s";
        }

        Status->Caption = Message;
}

void __fastcall TWavLoad::Image1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        if (Shift.Contains(ssLeft))
        {
                Threshold = (64-Y)*2;
                UpdateImage();
        }

}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::OKBtnClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->WavLoadBtn->Checked=false;
}
//---------------------------------------------------------------------------


void __fastcall TWavLoad::LeftBtnClick(TObject *Sender)
{
        Channel=0;        
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::RightBtnClick(TObject *Sender)
{
        Channel=1;
}
//---------------------------------------------------------------------------

