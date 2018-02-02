//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ProfSampEdit.h"
#include "z80.h"
#include "SymbolStore.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfileSampleEdit *ProfileSampleEdit;
__fastcall TProfileSampleEdit::TProfileSampleEdit(TComponent* Owner) :
        TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::ButtonOKClick(TObject *Sender)
{
        _cancelled = false;
}
//---------------------------------------------------------------------------

bool __fastcall TProfileSampleEdit::EditValues(ProfileDetail& pd)
{
        _cancelled = true;

        ShowModal();

        return _cancelled;
}
//---------------------------------------------------------------------------

