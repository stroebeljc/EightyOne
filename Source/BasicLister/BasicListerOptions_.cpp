//---------------------------------------------------------------------------

#include <vcl.h>
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
        TIniFile* ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------

bool TSaveBasicListingOptionsForm::GetOutputTokensAsCharacterCodes()
{
        return cboOutputAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputNonAsciiAsCharacterCodes()
{
        return cboOutputNonAsciiAsCharacterCodes->Checked;
}

void TSaveBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", cboOutputAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", cboOutputNonAsciiAsCharacterCodes->Checked);
}

void TSaveBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboOutputAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", false);
        cboOutputNonAsciiAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", 0);
}


