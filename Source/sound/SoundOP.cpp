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
 * SoundOP.cpp
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

void TSoundOutput::UpdateImage(short *data, int channels, int framesize)
{
        long x;
        int f;
        static int skip=0;

        if (++skip <3) return;
        skip=0;
        if (!SoundOutput->Visible) return;

        Img->Pen->Color = clWhite;
        Img->FillRect(rect);

        Img->Pen->Color = clRed;
        Img->MoveTo(0,Image1->Height/2);
        Img->LineTo(Image1->Width,Image1->Height/2);

        Img->Pen->Color = clBlack;
        for (x=0,f=0; x<Image1->Width; x++,f+=framesize/Image1->Width)
        {
                //Img->MoveTo(x,64);
                int currval=0;
                for (int i=0;i<channels;i++)
                        currval+=(int)data[channels*f+i];

                if (x==0)
                        Img->MoveTo(0, ((currval/channels/256)+128)/2);
                else
                        Img->LineTo(x, ((currval/channels/256)+128)/2);
        }
}
//---------------------------------------------------------------------------
__fastcall TSoundOutput::TSoundOutput(TComponent* Owner)
        : TForm(Owner)
{
        Img=Image1->Canvas;
        rect.Top=0; rect.Left=0;
        rect.Right=Image1->Width; rect.Bottom=Image1->Height;
        //ClearImage();

        TIniFile *ini;
        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TSoundOutput::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->SoundOutput1->Checked=false;
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

