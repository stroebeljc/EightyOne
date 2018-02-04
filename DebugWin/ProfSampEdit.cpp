//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ProfSampEdit.h"
#include "z80.h"
#include "SymbolStore.h"
#include "AddressResolver.h"
#include "ProfileDetail.h"
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
        Close();
}
//---------------------------------------------------------------------------

bool __fastcall TProfileSampleEdit::EditValues(AnsiString tag, ProfileDetail& pd)
{
        _cancelled = true;

        EditTag->Text = tag;
        EditStart->Text = symbolstore::addressToSymbolOrHex(pd._start);
        EditEnd->Text = symbolstore::addressToSymbolOrHex(pd._end);

        ShowModal();

        if (_cancelled) return false;

        int start, end;
        if (!AddressResolver::Validate(EditStart->Text, start) ||
                !AddressResolver::Validate(EditEnd->Text, end)) return false;

        pd._start = start;
        pd._end = end;
        return true;
}
//---------------------------------------------------------------------------

