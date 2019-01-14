//---------------------------------------------------------------------------

#include <vcl.h>
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
        TIniFile* ini = new TIniFile(zx81.inipath);
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

bool TLoadBasicListingOptionsForm::GetDiscardRedundantSpaces()
{
        return cboDiscardRedundantSpaces->Checked;
}

void TLoadBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "TokeniseRemContents", cboTokeniseRemContents->Checked);
        ini->WriteBool("BASICLISTING", "TokeniseStringContents", cboTokeniseStringContents->Checked);
        ini->WriteBool("BASICLISTING", "DiscardRedundantSpaces", cboDiscardRedundantSpaces->Checked);
}

void TLoadBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboTokeniseRemContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseRemContents", false);
        cboTokeniseStringContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseStringContents", 0);
        cboDiscardRedundantSpaces->Checked = ini->ReadBool("BASICLISTING", "DiscardRedundantSpaces", 0);
}

