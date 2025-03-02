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

#ifndef BasicLoaderOptions_H
#define BasicLoaderOptions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iniFiles.hpp"

//---------------------------------------------------------------------------
class TLoadBasicListingOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *cboTokeniseRemContents;
        TCheckBox *cboTokeniseStringContents;
        TButton *btnOK;
        TLabel *Label1;
        TCheckBox *cboDiscardSurplusSpaces;
        TCheckBox *cboAcceptAlternateKeywordSpelling;
        TComboBox *cboLineNumberIncrement;
        TLabel *Label6;
        TCheckBox *cboAcceptZxTokenFormatExtensions;
private:	// User declarations
public:		// User declarations
        __fastcall TLoadBasicListingOptionsForm(TComponent* Owner);
        bool GetTokeniseRemContents();
        bool GetTokeniseStringContents();
        bool GetDiscardSurplusSpaces();
        bool GetAcceptAlternateKeywordSpelling();
        int GetAutomaticLineNumberIncrement();
        bool GetSupportZxTokenFormatExtensions();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoadBasicListingOptionsForm *LoadBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif
