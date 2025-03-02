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
#include "BasicListerOptions_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSaveBasicListingOptionsForm *SaveBasicListingOptionsForm;

//---------------------------------------------------------------------------
__fastcall TSaveBasicListingOptionsForm::TSaveBasicListingOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------

bool TSaveBasicListingOptionsForm::GetOutputRemTokensAsCharacterCodes()
{
        return cboOutputRemTokensAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputStringTokensAsCharacterCodes()
{
        return cboOutputStringTokensAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputNonAsciiAsCharacterCodes()
{
        return cboOutputNonAsciiAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputVariableNamesInLowercase()
{
        return cboOutputVariableNamesInLowercase->Checked;
}

bool TSaveBasicListingOptionsForm::GetLimitLineLengths()
{
        return cboLimitLineLengths->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputFullWidthLineNumbers()
{
        return cboOutputFullWidthLineNumbers->Checked;
}

int TSaveBasicListingOptionsForm::GetScalingFator()
{
        return (cboTextScaling->Text == "100%") ? 1 : 2;
}

void TSaveBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", cboOutputRemTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", cboOutputStringTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", cboOutputNonAsciiAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputVariableNamesInLowercase", cboOutputVariableNamesInLowercase->Checked);
        ini->WriteBool("BASICLISTING", "LimitLineLengths", cboLimitLineLengths->Checked);
        ini->WriteBool("BASICLISTING", "OutputFullSidthLineNumbers", cboOutputFullWidthLineNumbers->Checked);
        ini->WriteString("BASICLISTING", "TextScaling", cboTextScaling->Text);
}

void TSaveBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboOutputRemTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", false);
        cboOutputStringTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", false);
        cboOutputNonAsciiAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", false);
        cboOutputVariableNamesInLowercase->Checked = ini->ReadBool("BASICLISTING", "OutputVariableNamesInLowercase", false);
        cboLimitLineLengths->Checked = ini->ReadBool("BASICLISTING", "LimitLineLengths", false);
        cboOutputFullWidthLineNumbers->Checked = ini->ReadBool("BASICLISTING", "OutputFullWidthLineNumbers", false);
        cboTextScaling->Text = ini->ReadString("BASICLISTING", "TextScaling", "100%");
}

