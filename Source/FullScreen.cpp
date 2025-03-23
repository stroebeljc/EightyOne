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
#include <windows.h>
#include <ctype.h>
#pragma hdrstop

#include "FullScreen.h"
#include "main_.h"
#include "AccDraw_.h"
#include "zx81config.h"

#define isnum(x) (((x)>='0')&&((x)<='9'))

struct FullScr FScreen;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFSSettings *FSSettings;
//---------------------------------------------------------------------------
__fastcall TFSSettings::TFSSettings(TComponent* Owner)
        : TForm(Owner)
{
        DEVMODE Mode;
        int i,j,c,added;
        //int r;
        int retval;
        AnsiString Text,OldText;
        int w,h,modes; //,c,r;
        int x1,x2;

        TIniFile *ini;

        nativeSizeW=GetSystemMetrics(SM_CXSCREEN);
        nativeSizeH=GetSystemMetrics(SM_CYSCREEN);
        nativeBpp=GetDeviceCaps(Form1->Canvas->Handle, BITSPIXEL)
                                * GetDeviceCaps(Form1->Canvas->Handle, PLANES);

        i=0;
        memset(&Mode, 0, sizeof(DEVMODE));
        Text.sprintf("Automatic");
        ModeList->Items->Add(Text);
        do
        {
                retval=EnumDisplaySettings(NULL,i, &Mode);
                w=Mode.dmPelsWidth;
                h=Mode.dmPelsHeight;
                c=Mode.dmBitsPerPel;
                //r=Mode.dmDisplayFrequency;
                i++;
                //if (r==1) r=60;
                if (c>8)
                {
                        if (nativeSizeW==w && nativeSizeH==h)
                                Text.sprintf("%d x %d (%d bit)   Current", w,h,c);
                        else
                                Text.sprintf("%d x %d (%d bit)", w,h,c);

                        added=false;
                        for(j=0;j<ModeList->Items->Count;j++)
                        {
                                OldText=ModeList->Items->Strings[j];
                                if (OldText==Text) added=true;
                        }

                        if (!added) ModeList->Items->Add(Text);
                }
        } while(retval);

        modes=ModeList->Items->Count;

        for(i=1;i<modes-1;i++)
                for(j=i;j<modes;j++)
                {
                        x1= atoi(AnsiString(ModeList->Items->Strings[i]).c_str());
                        x2= atoi(AnsiString(ModeList->Items->Strings[j]).c_str());

                        if (x2<x1) ModeList->Items->Exchange(i,j);
                }

        ModeList->ItemIndex=0; // Automatic

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TFSSettings::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->FullScreenSettings1->Checked=false;
}
//---------------------------------------------------------------------------
void __fastcall TFSSettings::ModeListChange(TObject *Sender)
{
        AnsiString Txt;

        Txt = ModeList->Items->Strings[ModeList->ItemIndex];

        if (Txt=="Automatic")
        {
                FScreen.Width=nativeSizeW;
                FScreen.Height=nativeSizeH;
                FScreen.Bpp=nativeBpp;
        }
        else
        {
                FScreen.Width = atoi(Txt.c_str());
                while (isnum(Txt[1])) Txt = Txt.SubString(2,Txt.Length()-1);
                while (!isnum(Txt[1])) Txt = Txt.SubString(2,Txt.Length()-1);
                FScreen.Height = atoi(Txt.c_str());
                while (isnum(Txt[1])) Txt = Txt.SubString(2,Txt.Length()-1);
                while (!isnum(Txt[1])) Txt = Txt.SubString(2,Txt.Length()-1);
                FScreen.Bpp = atoi(Txt.c_str());
        }

        FScreen.Stretch = Stretch->Checked;
        FScreen.WhiteLetterbox = White->Checked;

        White->Enabled=Letterbox->Checked;
}
//---------------------------------------------------------------------------
void TFSSettings::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("FULLSCREEN","Top",Top);
        ini->WriteInteger("FULLSCREEN","Left",Left);
        ini->WriteInteger("FULLSCREEN","Mode",ModeList->ItemIndex);
        ini->WriteBool("FULLSCREEN", "LetterBox", Letterbox->Checked);
        ini->WriteBool("FULLSCREEN", "Stretch", Stretch->Checked);
        ini->WriteBool("FULLSCREEN", "LBColour", White->Checked);
}

void TFSSettings::LoadSettings(TIniFile *ini)
{
        Top=ini->ReadInteger("FULLSCREEN","Top",Top);
        Left=ini->ReadInteger("FULLSCREEN","Left",Left);
        ModeList->ItemIndex=ini->ReadInteger("FULLSCREEN","Mode",ModeList->ItemIndex);
        Letterbox->Checked=ini->ReadBool("FULLSCREEN", "LetterBox", Letterbox->Checked);
        Stretch->Checked=ini->ReadBool("FULLSCREEN", "Stretch", Stretch->Checked);
        White->Checked=ini->ReadBool("FULLSCREEN", "LBColour", White->Checked);

        ModeListChange(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TFSSettings::FormShow(TObject *Sender)
{
        switch(Form1->RenderMode)
        {
        case RENDERGDI:
                GDIBtn->Checked=true;
                break;
                
        case RENDERDDRAW:
        default:
                DDrawBtn->Checked=true;
                break;
        }
}
//---------------------------------------------------------------------------

void __fastcall TFSSettings::GDIBtnClick(TObject *Sender)
{
        int NewRenderMode;

        if (GDIBtn->Checked) NewRenderMode=RENDERGDI;
        else NewRenderMode=RENDERDDRAW;

        if (Form1->RenderMode!=NewRenderMode)
        {
                RenderEnd();
                Form1->RenderMode=NewRenderMode;
                RenderInit();
                AccurateInit(true);
        }
}
//---------------------------------------------------------------------------

void __fastcall TFSSettings::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

