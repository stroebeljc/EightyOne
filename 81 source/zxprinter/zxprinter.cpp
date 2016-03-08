//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "zxprinter.h"
#include "PrintOptions_.h"
#include "main_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPrinter *Printer;
//---------------------------------------------------------------------------
void TPrinter::ClearBitmap(Graphics::TBitmap *Bitmap)
{
        int x,y;

        Bitmap->Canvas->Pen->Color = clWhite;
        for(y=0; y < Bitmap->Height; y++)
        {
                Bitmap->Canvas->MoveTo(0,y);
                Bitmap->Canvas->LineTo(Bitmap->Width,y);
        }
}
//---------------------------------------------------------------------------
void TPrinter::ResetPrinter(void)
{
        MotorOn=false;
        if (XPos) OutputLine();
        XPos=0;
        Counter2=16;
        StylusActive=false;
}
void TPrinter::OutputBit(void)
{
        if (XPos<2 || XPos>(PrinterOutput->Width+1)) return;
        PrinterOutput->Canvas->Pixels[XPos-2][YPos]= StylusActive?clBlack:clWhite;
}

void TPrinter::OutputLine(void)
{
        YPos++;
        if (YPos>=PrinterOutput->Height-1) ScrollOutput();
        DrawImage();
}

void TPrinter::ScrollOutput()
{
        bool AtEnd;

        //AtEnd = (ScrollBar->Max - ScrollBar->Position) < (ScrollBar->Max/10);

        PrinterOutput->Height += 8;
        ScrollBar->Max = PrinterOutput->Height;
        ScrollBar->Position += 8;
        //if (AtEnd) ScrollBar->Position = ScrollBar->Max;
//
//        int x,y;
//
//        for(y=PrinterOutput->Height-8; y<PrinterOutput->Height; y++)
//                for(x=0; x<PrinterOutput->Width; x++)
//                        PrinterOutput->Canvas->Pixels[x][y]=clWhite;
        //YPos -=8;
}

//---------------------------------------------------------------------------
__fastcall TPrinter::TPrinter(TComponent* Owner)
        : TForm(Owner)
{
        //ClientWidth=256;
        //StaticWidth=Width;

        PrinterOutput= new Graphics::TBitmap;
        PrinterOutput->PixelFormat=pf4bit;
        PrinterOutput->Width = 256;
        PrinterOutput->Height = 8;

        ClearBitmap(PrinterOutput);
        DrawImage();

        MotorOn=StylusActive=OnPaper=false;
        XPos=0;
        YPos=0;//PrinterOutput->Height-8;
        Counter1=25;
        Counter2=16;

        ScrollBar->Position = PrinterOutput->Height;
        ScrollBar->Min=0;
        ScrollBar->Max=PrinterOutput->Height;
}
//---------------------------------------------------------------------------
void __fastcall TPrinter::DrawImage()
{
        TRect source,dest;
        int pos;

        pos=ScrollBar->Position;

        if (pos<DisplayImage->Height) pos=DisplayImage->Height;

        source.Left=0;
        source.Top=pos - DisplayImage->Height;//ScrollBar->Position - DisplayImage->Height;
        source.Right=PrinterOutput->Width;
        source.Bottom=pos; //ScrollBar->Position; //PrinterOutput->Height;

        if (source.Top < 0) source.Top=0;

        dest.Left=0;
        dest.Top=0;
        dest.Right = DisplayImage->Width;
        dest.Bottom = DisplayImage->Height;

        DisplayImage->Canvas->CopyRect(dest, PrinterOutput->Canvas, source);
}
//---------------------------------------------------------------------------
void TPrinter::ClockTick(void)
{
        if (!MotorOn) return;
        if (Counter1--) return;
        Counter1=PrintOptions->BitSpeed;

        if (Counter2)
        {
                OnPaper=false;
                EncoderWheel=false;
                Counter2--;
        }
        else
        {
                OnPaper=true;
                EncoderWheel=true;
                OutputBit();
                XPos++;

                if (XPos==259)
                {
                        OutputLine();
                        OnPaper=false;
                        EncoderWheel=false;
                        XPos=0;
                        Counter2=PrintOptions->LineSpeed;
                }
        }
}
void TPrinter::WritePort(unsigned char Data)
{
        if (Data&4) ResetPrinter();
        else MotorOn=true;

        if (Data&128) StylusActive=true;
        else StylusActive=false;
        OnPaper=false;
        EncoderWheel=false;
}

unsigned char TPrinter::ReadPort(void)
{
        unsigned char Data=0;

        if (OnPaper) Data=128;
        //else Data=0;

        if (EncoderWheel) Data |= 1;

        return(Data);

}


//---------------------------------------------------------------------------
extern "C" void ZXPrinterClockTick(void)
{
        Printer->ClockTick();
}
extern "C" void ZXPrinterWritePort(unsigned char Data)
{
        Printer->WritePort(Data);
}

extern "C" unsigned char ZXPrinterReadPort(void)
{
        return(Printer->ReadPort());
}


void __fastcall TPrinter::ScrollBarChange(TObject *Sender)
{
        DrawImage();
}
//---------------------------------------------------------------------------

void __fastcall TPrinter::ClearImageClick(TObject *Sender)
{
        PrinterOutput->Width = 256;
        PrinterOutput->Height = 8;

        ClearBitmap(DisplayImage->Picture->Bitmap);
        ClearBitmap(PrinterOutput);
        DrawImage();

        MotorOn=StylusActive=OnPaper=false;
        XPos=0;
        YPos=0;//PrinterOutput->Height-8;
        Counter2=16;

        ScrollBar->Min=1;
        ScrollBar->Max=PrinterOutput->Height;
        ScrollBar->Position = PrinterOutput->Height;
}
//---------------------------------------------------------------------------

void __fastcall TPrinter::SaveImageClick(TObject *Sender)
{
        if (SaveDialog->Execute())
                PrinterOutput->SaveToFile(SaveDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TPrinter::FormClose(TObject *Sender, TCloseAction &Action)
{
        PrintOptions->Close();
        Form1->ViewPrinter->Checked=false;
}
//---------------------------------------------------------------------------


void __fastcall TPrinter::OptionsBtnClick(TObject *Sender)
{
        PrintOptions->Show();
}
//---------------------------------------------------------------------------

