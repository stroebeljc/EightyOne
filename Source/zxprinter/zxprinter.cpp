/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * zxprinter.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "zxprinter.h"
#include "main_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPrinter *Printer;
//---------------------------------------------------------------------------
void TPrinter::ClearBitmap(Graphics::TBitmap *Bitmap)
{
        int y;

        Bitmap->Canvas->Pen->Color = Bg;
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
        if (XPos<1 || XPos>(PrinterOutput->Width)) return;
        PrinterOutput->Canvas->Pixels[XPos-1][YPos]= StylusActive?Fg:Bg;
}

void TPrinter::OutputLine(void)
{
        YPos++;
        if (YPos>=PrinterOutput->Height-1) ScrollOutput();
        DrawImage();
}

void TPrinter::ScrollOutput()
{
        int y;
        Graphics::TBitmap *Bitmap;

        Bitmap=PrinterOutput;

        PrinterOutput->Height += 8;
        ScrollBar->Max = PrinterOutput->Height;
        ScrollBar->Position += 8;

        Bitmap->Canvas->Pen->Color = Bg;
        for(y=Bitmap->Height-8; y < Bitmap->Height; y++)
        {
                Bitmap->Canvas->MoveTo(0,y);
                Bitmap->Canvas->LineTo(Bitmap->Width,y);
        }
}

//---------------------------------------------------------------------------
__fastcall TPrinter::TPrinter(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        Fg=clBlack;
        Bg=clWhite;

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

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TPrinter::DrawImage()
{
        TRect source,dest;
        int pos;

        pos=ScrollBar->Position;

        if (pos<DisplayImage->Height) pos=DisplayImage->Height;

        source.Left=0;
        source.Top=pos - DisplayImage->Height;
        source.Right=PrinterOutput->Width;
        source.Bottom=pos;

        if (source.Top < 0) source.Top=0;

        dest.Left=0;
        dest.Top=0;
        dest.Right = DisplayImage->Width;
        dest.Bottom = DisplayImage->Height;

        DisplayImage->Canvas->CopyRect(dest, PrinterOutput->Canvas, source);
}
//---------------------------------------------------------------------------
void TPrinter::ClockTick(int ts)
{
        if (!MotorOn) return;
        Counter1-=ts;
        if (Counter1>0) return;
        Counter1 += 1016 - ((100-BitSpeed) * 10);

        if (Counter2)
        {
                OnPaper=StylusActive;
                EncoderWheel=false;
                Counter2--;
        }
        else
        {
                if (XPos==0) OnPaper=true;
                EncoderWheel=true;
                OutputBit();
                XPos++;

                if (XPos==(zx81.machine==MACHINESPEC48 ? 257:258))
                {
                        OutputLine();
                        OnPaper=StylusActive;
                        EncoderWheel=false;
                        XPos=0;
                        Counter2=1+LineSpeed*10;
                }
        }
}
void TPrinter::WritePort(unsigned char Data)
{
        if (Data&4) ResetPrinter();
        else MotorOn=true;

        if (Data&128) StylusActive=true;
        else StylusActive=false;

        OnPaper=StylusActive;
        EncoderWheel=false;
}

unsigned char TPrinter::ReadPort(BYTE idleDataBus)
{
        unsigned char Data=idleDataBus;

        Data &= 0xBF;

        if (!OnPaper) Data &= 0x7F;

        if (!EncoderWheel) Data &= 0xFE;

        return(Data);
}

//---------------------------------------------------------------------------
void ZXPrinterClockTick(int ts)
{
        Printer->ClockTick(ts);
}
void ZXPrinterWritePort(unsigned char Data)
{
        Printer->WritePort(Data);
}

unsigned char ZXPrinterReadPort(BYTE idleDataBus)
{
        return(Printer->ReadPort(idleDataBus));
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

        //MotorOn=StylusActive=OnPaper=false;
        XPos=0;
        YPos=0;
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
        Form1->ViewPrinter->Checked=false;
}
//---------------------------------------------------------------------------


void __fastcall TPrinter::OptionsBtnClick(TObject *Sender)
{
        if (Panel1->Visible)
        {
                Panel1->Visible=false;
                ClientHeight=Panel1->Top;
        }
        else
        {
                ClientHeight=Panel1->Top+Panel1->Height+4;
                Panel1->Visible=true;
        }
}
//---------------------------------------------------------------------------

void TPrinter::LoadSettings(TIniFile *ini)
{
        OptionsBtnClick(NULL);

        Top = ini->ReadInteger("PRINTER","Top",Top);
        Left = ini->ReadInteger("PRINTER","Left",Left);

        BitSpeed = ini->ReadInteger("PRINTER","BitSpeed",BitSpeed );
        LineSpeed = ini->ReadInteger("PRINTER","LineSpeed",LineSpeed );
        BitRate->Position = ini->ReadInteger("PRINTER","SBitSpeed",BitRate->Position );
        LineRate->Position = ini->ReadInteger("PRINTER","SLineSpeed",LineRate->Position );
        Style->ItemIndex = ini->ReadInteger("Printer","Style",Style->ItemIndex );
        Panel1->Visible = ini->ReadBool("Printer","Options",Panel1->Visible );

        BitRateChange(NULL);
        LineRateChange(NULL);
        StyleChange(NULL);

        ClearImageClick(NULL);
        OptionsBtnClick(NULL);
        OptionsBtnClick(NULL);

        if (Form1->ViewPrinter->Checked) Show();
}

void TPrinter::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("PRINTER","Top",Top);
        ini->WriteInteger("PRINTER","Left",Left);

        ini->WriteInteger("PRINTER","BitSpeed",BitSpeed );
        ini->WriteInteger("PRINTER","LineSpeed",LineSpeed );
        ini->WriteInteger("PRINTER","SBitSpeed",BitRate->Position );
        ini->WriteInteger("PRINTER","SLineSpeed",LineRate->Position );
        ini->WriteInteger("Printer","Style",Style->ItemIndex );
        ini->WriteBool("Printer","Options",Panel1->Visible );

}
void __fastcall TPrinter::BitRateChange(TObject *Sender)
{
        BitRateTxt->Caption = ((int) BitRate->Position);
        BitRateTxt->Caption = "Carriage Speed: "+BitRateTxt->Caption;
        BitSpeed = 99 - BitRate->Position;

}
//---------------------------------------------------------------------------

void __fastcall TPrinter::LineRateChange(TObject *Sender)
{
        LineRateTxt->Caption = ((int) LineRate->Position);
        LineRateTxt->Caption = "Paper Feed Speed: "+LineRateTxt->Caption;
        LineSpeed = 100 - LineRate->Position;

}
//---------------------------------------------------------------------------

void __fastcall TPrinter::StyleChange(TObject *Sender)
{
        switch(Style->ItemIndex)
        {
        case 0:
                Fg=clBlack;
                Bg=clLtGray;
                BitRate->Position=0;
                LineRate->Position=75;
                break;
        case 1:
                Fg=clBlue;
                Bg=clWhite;
                BitRate->Position=30;
                LineRate->Position=100;
                break;
        case 2:
                Fg=clBlack;
                Bg=clWhite;
                BitRate->Position=65;
                LineRate->Position=100;
                break;
        default:
                Style->ItemIndex=0;
                StyleChange(NULL);
                break;
        }
        ClearImageClick(NULL);

}
//---------------------------------------------------------------------------

