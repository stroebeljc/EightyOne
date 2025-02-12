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
 * zx97Config.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "zx97Config.h"
#include "zx81config.h"
#include "main_.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TZX97Dialog *ZX97Dialog;
unsigned char zx97bankmem[16*16384];

//---------------------------------------------------------------------------
__fastcall TZX97Dialog::TZX97Dialog(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void TZX97Dialog::UpdateMachine(ZX97& settings)
{
        zx97.saveram=settings.saveram;
        zx97.protectab=settings.protectab;
        zx97.protect08=settings.protect08;
        zx97.protectb0=settings.protectb0;
        zx97.protectb115=settings.protectb115;
        zx97.bankswitch=settings.bankswitch;
}

void TZX97Dialog::SaveSettings(TIniFile *ini)
{
        FILE *f;
        char filename[256];

        if (SaveRAM->Checked)
        {
                strcpy(filename, emulator.cwd);
                strcat(filename, nvMemoryFolder);
                strcat(filename, "zx97.nv");

                f=fopen(filename,"wb");
                if (f)
                {
                        fwrite(zx97bankmem, 16384, 16, f);
                        fclose(f);
                }
        }

        AccessIniFile(ini, Write);
}

void TZX97Dialog::LoadSettings(TIniFile *ini)
{
        FILE *f;
        char filename[256];

        strcpy(filename, emulator.cwd);
        strcat(filename, nvMemoryFolder);
        strcat(filename, "zx97.nv");

        f=fopen(filename,"rb");
        if (f)
        {
                fread(zx97bankmem, 16384, 16, f);
                fclose(f);
        }

        AccessIniFile(ini, Read);
}

void TZX97Dialog::AccessIniFile(TIniFile* ini, IniFileAccessType accessType)
{
        AccessIniFileInteger(ini, accessType, "ZX97", "Top",   Top);
        AccessIniFileInteger(ini, accessType, "ZX97", "Left",  Left);
}

//---------------------------------------------------------------------------

void TZX97Dialog::RetrieveFormSettings(ZX97& settings)
{
        settings.saveram=Zx97form.saveram;
        settings.protectab=Zx97form.protectab;
        settings.protect08=Zx97form.protect08;
        settings.protectb0=Zx97form.protectb0;
        settings.protectb115=Zx97form.protectb115;
        settings.bankswitch=Zx97form.bankswitch;
}

void TZX97Dialog::SaveToInternalSettings()
{
        Zx97form.saveram=SaveRAM->Checked;
        Zx97form.protectab=ProtectAB->Checked;
        Zx97form.protect08=Protect08->Checked;
        Zx97form.protectb0=ProtectB0->Checked;
        Zx97form.protectb115=ProtectB115->Checked;
        Zx97form.bankswitch=(SwapRAMROM->Checked!=0);
}

void TZX97Dialog::UpdateFormSettings(ZX97& settings)
{
        Zx97form.saveram=settings.saveram;
        Zx97form.protectab=settings.protectab;
        Zx97form.protect08=settings.protect08;
        Zx97form.protectb0=settings.protectb0;
        Zx97form.protectb115=settings.protectb115;
        Zx97form.bankswitch=settings.bankswitch;
        LoadFromInternalSettings();
}

void TZX97Dialog::LoadFromInternalSettings()
{
        SaveRAM->Checked=Zx97form.saveram;
        ProtectAB->Checked=Zx97form.protectab;
        Protect08->Checked=Zx97form.protect08;
        ProtectB0->Checked=Zx97form.protectb0;
        ProtectB115->Checked=Zx97form.protectb115;
        SwapRAMROM->Checked=Zx97form.bankswitch;
}

void __fastcall TZX97Dialog::OKClick(TObject *Sender)
{
        SaveToInternalSettings();
}
//---------------------------------------------------------------------------

void __fastcall TZX97Dialog::CancelClick(TObject *Sender)
{
        LoadFromInternalSettings();
}
//---------------------------------------------------------------------------

