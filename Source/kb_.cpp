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

#include "kb_.h"
#include "kbstatus.h"
#include "zx81config.h"
#include "main_.h"
#include "joystick.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKb *Kb;
//---------------------------------------------------------------------------
__fastcall TKb::TKb(TComponent* Owner)
        : TForm(Owner)
{
        if (CursorMode->ItemIndex==-1) CursorMode->ItemIndex=1;

        TIniFile *ini;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TKb::OKClick(TObject *Sender)
{
        switch(CursorMode->ItemIndex)
        {
        case 0:
                if (emulator.machine==MACHINEACE) PCKeySetCursor('5', '7', '6', '8', 0);
                else PCKeySetCursor('5', '6', '7', '8', 0);
                break;
        case 1:
                if (emulator.machine==MACHINEACE) PCKeySetCursor('5', '7', '6', '8', 1);
                else PCKeySetCursor('5', '6', '7', '8', 1);
                break;
        case 2:
                PCKeySetCursor('B', 'V', 'C', 'N', 0);
                break;
        case 3:
                PCKeySetCursor('B', 'V', 'C', 'N', 1);
                break;
        case 4:
                PCKeySetCursor((char)toupper(CustomLeft->Text[1]),
                               (char)toupper(CustomDown->Text[1]),
                               (char)toupper(CustomUp->Text[1]),
                               (char)toupper(CustomRight->Text[1]),
                               (char)0);
        }

        if (RadioButton1->Checked) PCKeySetCTRL(0);
        if (RadioButton2->Checked) PCKeySetCTRL('0');

        if (UseRightShiftCheckBox->Checked && (emulator.machine==MACHINESPECTRUM
                                    || emulator.machine==MACHINEACE)) emulator.UseRShift=true;
        else emulator.UseRShift=false;

        emulator.UseNumericPadForJoystick1 = (CFGBYTE)((UseNumericPadForJoystickCheckBox->Checked && UseNumericPadForJoystick1->Checked) ? 1 : 0);
        emulator.UseNumericPadForJoystick2 = (CFGBYTE)((UseNumericPadForJoystickCheckBox->Checked && UseNumericPadForJoystick2->Checked) ? 1 : 0);

        Form1->Keyboard1->Checked=false;
        
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TKb::CursorModeChange(TObject *Sender)
{
        if (CursorMode->ItemIndex==4)
        {
                CustomLeft->Enabled=true;
                CustomRight->Enabled=true;
                CustomUp->Enabled=true;
                CustomDown->Enabled=true;
        }
        else
        {
                CustomLeft->Enabled=false;
                CustomRight->Enabled=false;
                CustomUp->Enabled=false;
                CustomDown->Enabled=false;
        }
}
//---------------------------------------------------------------------------
void TKb::LoadSettings(TIniFile *ini)
{
        Top  = ini->ReadInteger("KB", "Top",  Top);
        Left = ini->ReadInteger("KB", "Left", Left);

        CustomLeft->Text  = ini->ReadString("KB", "CustomLeft",  CustomLeft->Text);
        CustomDown->Text  = ini->ReadString("KB", "CustomDown",  CustomDown->Text);
        CustomUp->Text    = ini->ReadString("KB", "CustomUp",    CustomUp->Text);
        CustomRight->Text = ini->ReadString("KB", "CustomRight", CustomRight->Text);

        RadioButton1->Checked          = ini->ReadBool(   "KB", "CTRLFunc",   RadioButton1->Checked);
        RadioButton2->Checked          = ini->ReadBool(   "KB", "CTRL0",      RadioButton2->Checked);
        UseRightShiftCheckBox->Checked = ini->ReadBool(   "KB", "RIGHTSHIFT", UseRightShiftCheckBox->Checked);
        CursorMode->ItemIndex          = ini->ReadInteger("KB", "CursorMode", CursorMode->ItemIndex);

        UseRightShiftCheckBox->Checked            = ini->ReadBool("KB", "UseRShift",                UseRightShiftCheckBox->Checked);
        UseNumericPadForJoystickCheckBox->Checked = ini->ReadBool("KB", "UseNumericPadForJoystick", UseNumericPadForJoystickCheckBox->Checked);
        UseNumericPadForJoystick1->Checked        = ini->ReadBool("KB", "UseNumericPadForJoystick1", UseNumericPadForJoystick1->Checked);
        UseNumericPadForJoystick2->Checked        = ini->ReadBool("KB", "UseNumericPadForJoystick2", UseNumericPadForJoystick2->Checked);

        UpdateJoystickSettings();
        CursorModeChange(NULL);
        OKClick(NULL);
}

void TKb::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("KB", "Top",  Top);
        ini->WriteInteger("KB", "Left", Left);

        ini->WriteInteger("KB", "CursorMode",  CursorMode->ItemIndex);
        ini->WriteString("KB",  "CustomLeft",  CustomLeft->Text);
        ini->WriteString("KB",  "CustomDown",  CustomDown->Text);
        ini->WriteString("KB",  "CustomUp",    CustomUp->Text);
        ini->WriteString("KB",  "CustomRight", CustomRight->Text);

        ini->WriteBool("KB", "CTRLFunc",   RadioButton1->Checked);
        ini->WriteBool("KB", "CTRL0",      RadioButton2->Checked);
        ini->WriteBool("KB", "RIGHTSHIFT", UseRightShiftCheckBox->Checked);

        ini->WriteBool("KB", "UseRShift",                UseRightShiftCheckBox->Checked);
        ini->WriteBool("KB", "UseNumericPadForJoystick", UseNumericPadForJoystickCheckBox->Checked);
        ini->WriteBool("KB", "UseNumericPadForJoystick1", UseNumericPadForJoystick1->Checked);
        ini->WriteBool("KB", "UseNumericPadForJoystick2", UseNumericPadForJoystick2->Checked);
}

void TKb::UpdateCursors()
{
        if (CursorMode->ItemIndex!=4)
        {
                if (emulator.machine==MACHINELAMBDA) CursorMode->ItemIndex=3;
                else CursorMode->ItemIndex=1;
        }

        CursorModeChange(NULL);
        OKClick(NULL);
}

void __fastcall TKb::FormShow(TObject *Sender)
{
        if (emulator.machine==MACHINESPECTRUM || emulator.machine==MACHINEACE)
        {
                CtrlKeyMapsToLabel->Visible=false;
                RadioButton1->Visible=false;
                RadioButton2->Visible=false;
                UseRightShiftCheckBox->Visible=true;
        }
        else
        {
                CtrlKeyMapsToLabel->Visible=true;
                RadioButton1->Visible=true;
                RadioButton2->Visible=true;
                UseRightShiftCheckBox->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TKb::UseNumericPadForJoystickCheckBoxClick(TObject *Sender)
{
        UpdateJoystickSettings();
}              
//---------------------------------------------------------------------------

void __fastcall TKb::UseNumericPadForJoystick1Click(TObject *Sender)
{
        UpdateJoystickSettings();
}
//---------------------------------------------------------------------------

void __fastcall TKb::UseNumericPadForJoystick2Click(TObject *Sender)
{
        UpdateJoystickSettings();
}
//---------------------------------------------------------------------------

void TKb::UpdateJoystickSettings()
{
        emulator.UseNumericPadForJoystick1 = (CFGBYTE)((UseNumericPadForJoystickCheckBox->Checked && UseNumericPadForJoystick1->Checked) ? 1 : 0);
        emulator.UseNumericPadForJoystick2 = (CFGBYTE)((UseNumericPadForJoystickCheckBox->Checked && UseNumericPadForJoystick2->Checked) ? 1 : 0);

        UseNumericPadForJoystick1->Enabled = UseNumericPadForJoystickCheckBox->Checked;
        UseNumericPadForJoystick2->Enabled = UseNumericPadForJoystickCheckBox->Checked;

        bool joystickInterfaceSelected     = (machine.joystickInterfaceType != JOYSTICK_NONE);
        bool twinJoystickInterfaceSelected = (machine.joystickInterfaceType == JOYSTICK_INTERFACE2 || machine.joystickInterfaceType == JOYSTICK_TIMEX);

        if (emulator.UseNumericPadForJoystick1 == 0)
        {
                if (machine.joystick1Controller == -1)
                {
                        machine.joystick1Connected = 0;
                        machine.joystick1AutoFireEnabled = 0;
                }
        }
        else
        {
                machine.joystick1Connected = joystickInterfaceSelected;
        }

        if (emulator.UseNumericPadForJoystick2 == 0)
        {
                if (machine.joystick2Controller == -1)
                {
                        machine.joystick2Connected = 0;
                        machine.joystick2AutoFireEnabled = 0;
                }
        }
        else
        {
                machine.joystick2Connected = twinJoystickInterfaceSelected;
        }

        Form1->BuildMenuJoystickSelection();
}
//---------------------------------------------------------------------------

