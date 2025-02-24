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

#include <vcl4.h>
#pragma hdrstop

#include "zx81config.h"
#include "BasicLoaderOptions_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoadBasicListingOptionsForm *LoadBasicListingOptionsForm;
//---------------------------------------------------------------------------
__fastcall TLoadBasicListingOptionsForm::TLoadBasicListingOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
bool TLoadBasicListingOptionsForm::GetTokeniseRemContents()
{
        return cboTokeniseRemContents->Checked;
}

bool TLoadBasicListingOptionsForm::GetTokeniseStringContents()
{
        return cboTokeniseStringContents->Checked;
}

bool TLoadBasicListingOptionsForm::GetDiscardSurplusSpaces()
{
        return cboDiscardSurplusSpaces->Checked;
}

bool TLoadBasicListingOptionsForm::GetAcceptAlternateKeywordSpelling()
{
        return cboAcceptAlternateKeywordSpelling->Checked;
}

int TLoadBasicListingOptionsForm::GetAutomaticLineNumberIncrement()
{
        return StrToInt(cboLineNumberIncrement->Text);
}

bool TLoadBasicListingOptionsForm::GetSupportZxTokenFormatExtensions()
{
        return cboAcceptZxTokenFormatExtensions->Checked;
}

void TLoadBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "TokeniseRemContents", cboTokeniseRemContents->Checked);
        ini->WriteBool("BASICLISTING", "TokeniseStringContents", cboTokeniseStringContents->Checked);
        ini->WriteBool("BASICLISTING", "DiscardSurplusSpaces", cboDiscardSurplusSpaces->Checked);
        ini->WriteBool("BASICLISTING", "AcceptAlternateKeywordSpelling", cboAcceptAlternateKeywordSpelling->Checked);
        ini->WriteInteger("BASICLISTING", "AutomaticLineNumberIncrementIndex", cboLineNumberIncrement->ItemIndex);
        ini->WriteBool("BASICLISTING", "SupportZxTokenFormatExtensions", cboAcceptZxTokenFormatExtensions->Checked);
}

void TLoadBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboTokeniseRemContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseRemContents", false);
        cboTokeniseStringContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseStringContents", false);
        cboDiscardSurplusSpaces->Checked = ini->ReadBool("BASICLISTING", "DiscardSurplusSpaces", false);
        cboAcceptAlternateKeywordSpelling->Checked = ini->ReadBool("BASICLISTING", "AcceptAlternateKeywordSpelling", false);
        cboLineNumberIncrement->ItemIndex = ini->ReadInteger("BASICLISTING", "AutomaticLineNumberIncrementIndex", 0);
        cboAcceptZxTokenFormatExtensions->Checked = ini->ReadBool("BASICLISTING", "SupportZxTokenFormatExtensions", false);
}


