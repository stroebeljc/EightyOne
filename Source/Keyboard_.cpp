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
 * keyboard_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Keyboard_.h"
#include "KeyboardFunctions_.h"
#include "main_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKeyboard *Keyboard;

//---------------------------------------------------------------------------
__fastcall TKeyboard::TKeyboard(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TKeyboard::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->KeyboardMap1->Checked=false;
        KeyboardFunctions->Visible = false;
        KeyboardFunctions->EnableTimer(false);
}
//---------------------------------------------------------------------------

void TKeyboard::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("KBMAP","Top",Top);
        Left = ini->ReadInteger("KBMAP","Left",Left);

        if (Form1->KeyboardMap1->Checked) Show();
}

void TKeyboard::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("KBMAP","Top",Top);
        ini->WriteInteger("KBMAP","Left",Left);
}

void __fastcall TKeyboard::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        Form1->FormKeyDown(Sender, Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TKeyboard::FormKeyPress(TObject *Sender, char &Key)
{
        Form1->FormKeyPress(Sender, Key);
}
//---------------------------------------------------------------------------

void __fastcall TKeyboard::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        Form1->FormKeyUp(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void TKeyboard::KbChange(void)
{
        Keyboard->zx80kb->Visible=false;
        Keyboard->zx81kb->Visible=false;
        Keyboard->zx80zxpandkb->Visible=false;
        Keyboard->zx81zxpandkb->Visible=false;
        Keyboard->ts1000zxpandkb->Visible=false;
        Keyboard->acekb->Visible=false;
        Keyboard->ts1500kb->Visible=false;
        Keyboard->ts1000kb->Visible=false;
        Keyboard->lambdakb->Visible=false;
        Keyboard->tk85kb->Visible=false;
        Keyboard->zx97kb->Visible=false;
        Keyboard->r470kb->Visible=false;
        Keyboard->spec16kb->Visible=false;
        Keyboard->spec48kb->Visible=false;
        Keyboard->spec128kb->Visible=false;
        Keyboard->spec128kbSpanish->Visible=false;
        Keyboard->ts2048kb->Visible=false;
        Keyboard->ts2068kb->Visible=false;
        Keyboard->specPlus2kb->Visible=false;
        Keyboard->specPlus3kb->Visible=false;

        KeyboardFunctions->Visible=false;
        KeyboardFunctions->EnableTimer(false);

        switch(emulator.romcrc)
        {
        case CRCACE:
                Keyboard->acekb->Visible=true;
                break;

        case CRCTS1500:
                Keyboard->ts1500kb->Visible=true;
                break;

        case CRCZX80:
                if (zx81.zxpand) Keyboard->zx80zxpandkb->Visible=true;
                else Keyboard->zx80kb->Visible=true;
                KeyboardFunctions->Visible = Keyboard->Visible;
                KeyboardFunctions->EnableTimer(KeyboardFunctions->Visible);
                break;

        case CRCLAMBDA:
                Keyboard->lambdakb->Visible=true;
                break;

        case CRCTK85:
                Keyboard->tk85kb->Visible=true;
                break;

        case CRCZX97LE:
                Keyboard->zx97kb->Visible=true;
                break;

        case CRCR470:
                Keyboard->r470kb->Visible=true;
                break;

        case CRCZX81_ED1:
        case CRCZX81_ED2_3:
                if (machine.NTSC && zx81.zxpand) Keyboard->ts1000zxpandkb->Visible=true;
                else if (machine.NTSC) Keyboard->ts1000kb->Visible=true;
                else if (zx81.zxpand) Keyboard->zx81zxpandkb->Visible=true;
                else Keyboard->zx81kb->Visible=true;
                break;

        case CRCSP48:
        case CRCSP81:
                if (spectrum.model == SPECCY16)
                        Keyboard->spec16kb->Visible=true;
                else if (emulator.machine == MACHINESPECTRUM && spectrum.model == SPECCYPLUS)
                        Keyboard->spec128kb->Visible=true;
                else
                        Keyboard->spec48kb->Visible=true;
                break;
        case CRCH4TH:
        case CRCSG81:
        case CRCTREE4TH:
        case CRC8300:
        case CRCASZMIC:
                if (zx81.zxpand) Keyboard->zx81zxpandkb->Visible=true;
                else Keyboard->zx81kb->Visible=true;
                break;
        case CRCSPANISH128:
                Keyboard->spec128kbSpanish->Visible=true;
                break;
        default:
                switch(emulator.machine)
                {
                case MACHINEZX80:
                        if (zx81.zxpand) Keyboard->zx80zxpandkb->Visible=true;
                        else Keyboard->zx80kb->Visible=true;
                        KeyboardFunctions->Visible = Keyboard->Visible;
                        KeyboardFunctions->EnableTimer(KeyboardFunctions->Visible);
                        break;
                case MACHINEZX81:
                        if (machine.NTSC && zx81.zxpand) Keyboard->ts1000zxpandkb->Visible=true;
                        else if (machine.NTSC) Keyboard->ts1000kb->Visible=true;
                        else if (zx81.zxpand) Keyboard->zx81zxpandkb->Visible=true;
                        else Keyboard->zx81kb->Visible=true;
                        break;
                case MACHINEACE:
                        Keyboard->acekb->Visible=true;
                        break;
                case MACHINETS1000:
                        Keyboard->ts1000kb->Visible=true;
                        break;
                case MACHINETS1500:
                        Keyboard->ts1500kb->Visible=true;
                        break;
                case MACHINER470:
                        Keyboard->r470kb->Visible=true;
                        break;
                case MACHINETK85:
                        Keyboard->tk85kb->Visible=true;
                        break;
                case MACHINELAMBDA:
                        Keyboard->lambdakb->Visible=true;
                        break;
                case MACHINESPECTRUM:
                        switch (spectrum.model)
                        {
                        case SPECCYPLUS:
                        case SPECCY128:
                                Keyboard->spec128kb->Visible=true;
                                break;
                        case SPECCYTC2048:
                                Keyboard->ts2048kb->Visible=true;
                                break;
                        case SPECCYTS2068:
                                Keyboard->ts2068kb->Visible=true;
                                break;
                        case SPECCY16:
                                Keyboard->spec16kb->Visible=true;
                                break;
                        case SPECCYPLUS2:
                                Keyboard->specPlus2kb->Visible=true;
                                break;

                        case SPECCYPLUS2A:
                        case SPECCYPLUS3:
                                Keyboard->specPlus3kb->Visible=true;
                                break;

                        default:
                                Keyboard->spec48kb->Visible=true;
                                break;
                        }
                        break;
                }
        }

        KeyboardFunctions->Left = Keyboard->Left + Keyboard->Width + Keyboard->FunctionsOffset;
        KeyboardFunctions->Top = Keyboard->Top;
}

void SetKeyboardSize(TImage* image, bool large)
{
        if (large)
        {
                image->Width = image->Width * 2;
                image->Height = image->Height * 2;
        }
        else
        {
                image->Width = image->Width / 2;
                image->Height = image->Height / 2;
        }
}

void __fastcall TKeyboard::KeyboardDblClick(TObject *Sender)
{
        const int normalWidth = 505;
        bool large = (zx81kb->Width == normalWidth);

        SetKeyboardSize(r470kb, large);
        SetKeyboardSize(zx80zxpandkb, large);
        SetKeyboardSize(zx81zxpandkb, large);
        SetKeyboardSize(ts1000zxpandkb, large);
        SetKeyboardSize(zx80kb, large);
        SetKeyboardSize(zx81kb, large);
        SetKeyboardSize(acekb, large);
        SetKeyboardSize(ts1500kb, large);
        SetKeyboardSize(lambdakb, large);
        SetKeyboardSize(tk85kb, large);
        SetKeyboardSize(zx97kb, large);
        SetKeyboardSize(ts1000kb, large);
        SetKeyboardSize(spec48kb, large);
        SetKeyboardSize(spec128kb, large);
        SetKeyboardSize(spec128kbSpanish, large);
        SetKeyboardSize(ts2068kb, large);
        SetKeyboardSize(ts2048kb, large);
        SetKeyboardSize(spec16kb, large);
        SetKeyboardSize(specPlus2kb, large);
        SetKeyboardSize(specPlus3kb, large);

        SetKeyboardSize(KeyboardFunctions->zx80IntegralFunctions, large);
}

