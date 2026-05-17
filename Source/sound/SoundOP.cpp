/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
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
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "SoundOP.h"
#include "main_.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSoundOutput *SoundOutput;
//---------------------------------------------------------------------------

void __fastcall TSoundOutput::WMEraseBkgnd(TWMEraseBkgnd &Message)
{
    Message.Result = 1; // Indicate background is handled
}

void TSoundOutput::UpdateImage(short *data, int channels, int framesize)
{
        static int skip=0;

        if (++skip <3) return;
        skip=0;
        
        int oldsize = m_framesize*m_channels;
        m_framesize = framesize;
        m_channels = channels;
        int size = m_framesize*m_channels;
        if (m_dataBuffer==NULL)
                m_dataBuffer = new short[size];
        else if (size!=oldsize)
        {
                delete m_dataBuffer;
                m_dataBuffer = new short[size];
        }
        memcpy(m_dataBuffer,data,size*sizeof(short));
        Invalidate();
}
//---------------------------------------------------------------------------
__fastcall TSoundOutput::TSoundOutput(TComponent* Owner)
        : TForm(Owner)
{
        Img=this->Canvas;
        m_dataBuffer=NULL;

        TIniFile *ini;
        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        FormResize(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TSoundOutput::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->SoundOutput1->Checked=false;
        if (m_dataBuffer) delete m_dataBuffer;
        m_dataBuffer=NULL;
}
//---------------------------------------------------------------------------

void TSoundOutput::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("SOUNDOP","Top",Top);
        Left = ini->ReadInteger("SOUNDOP","Left",Left);
        Height = ini->ReadInteger("SOUNDOP","Height",Height);
        Width = ini->ReadInteger("SOUNDOP","Width",Width);

        if (Form1->SoundOutput1->Checked) Show();
}

void TSoundOutput::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("SOUNDOP","Top",Top);
        ini->WriteInteger("SOUNDOP","Left",Left);
        ini->WriteInteger("SOUNDOP","Height",Height);
        ini->WriteInteger("SOUNDOP","Width",Width);
}

void __fastcall TSoundOutput::FormResize(TObject *Sender)
{
        Image1->Height=this->ClientHeight;
        Image1->Width=this->ClientWidth;
        rect.Top=0; rect.Left=0;
        rect.Right=Image1->Width; rect.Bottom=Image1->Height;
}
//---------------------------------------------------------------------------

void __fastcall TSoundOutput::FormPaint(TObject *Sender)
{
        long x;

        if (!SoundOutput->Visible) return;

        Img->Pen->Color = clWhite;
        Img->FillRect(rect);

        Img->Pen->Color = clRed;
        Img->MoveTo(0,Image1->Height/2);
        Img->LineTo(Image1->Width,Image1->Height/2);

        Img->Pen->Color = clBlack;
        if (m_dataBuffer==NULL) return;
        for (x=0; x<m_framesize; x++)
        {
                //Img->MoveTo(x,64);
                int currval=0;
                for (int i=0;i<m_channels;i++)
                        currval+=(int)m_dataBuffer[m_channels*x+i];

                int position=Image1->Height*((double)currval/m_channels/32768+1)/2;
                if (x==0)
                        Img->MoveTo(0, position);
                else
                        Img->LineTo(x*Image1->Width/m_framesize, position);
        }
}
//---------------------------------------------------------------------------

