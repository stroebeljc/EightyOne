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

void TSaveBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", cboOutputRemTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", cboOutputStringTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", cboOutputNonAsciiAsCharacterCodes->Checked);
}

void TSaveBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboOutputRemTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", false);
        cboOutputStringTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", false);
        cboOutputNonAsciiAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", 0);
}




