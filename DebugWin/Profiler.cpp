//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Profiler.h"
#include "ProfSampEdit.h"
#include "ProfileDetail.h"
#include "symbolstore.h"
#include "z80.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfiler *Profiler;
//---------------------------------------------------------------------------
__fastcall TProfiler::TProfiler(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::UpdateItem(TListItem* item, AnsiString tag, ProfileDetail& pd)
{
        item->Caption = tag;
        item->SubItems->Strings[0] = symbolstore::addressToSymbolOrHex(pd._start);
        item->SubItems->Strings[1] = symbolstore::addressToSymbolOrHex(pd._end);
        item->SubItems->Strings[2] = pd.Max();
        item->SubItems->Strings[3] = pd.SampleCount();
}

void __fastcall TProfiler::ButtonNewClick(TObject *Sender)
{
        TProfileSampleEdit* pse = new TProfileSampleEdit(this);

        ProfileDetail pd(0, 0);
        if (pse->EditValues("", pd)) {
                _profileDetails.push_back(pd);

                TListItem* newItem = ListViewProfileSamples->Items->Add();
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                UpdateItem(newItem, pse->EditTag->Text, pd);
        }

        delete(pse);
}

//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonEditClick(TObject *Sender)
{
        TListItem* editItem = ListViewProfileSamples->Selected;
        if (!editItem) return;

        TProfileSampleEdit* pse = new TProfileSampleEdit(this);
        ProfileDetail pd = _profileDetails[editItem->Index];
        if (pse->EditValues(editItem->Caption, pd)) {
                _profileDetails[editItem->Index] = pd;
                UpdateItem(editItem, pse->EditTag->Text, pd);
        }

        delete pse;
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonResetClick(TObject *Sender)
{
        TListItem* selected = ListViewProfileSamples->Selected;
        if (!selected) return;

        int idx = selected->Index;
        ProfileDetail& detail = _profileDetails[idx];
        detail.Reset();
        UpdateItem(selected, selected->Caption, detail);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonDeleteClick(TObject *Sender)
{
        TListItem* selected = ListViewProfileSamples->Selected;
        if (!selected) return;

        int idx = selected->Index;
        ListViewProfileSamples->Items->Delete(idx);
        _profileDetails.erase(_profileDetails.begin() + idx);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::DebugTick(processor* z80)
{
        for (size_t i = 0; i < _profileDetails.size(); ++i) {
                _profileDetails[i].Update(z80->pc.w, z80->tstates);
        }
}

//---------------------------------------------------------------------------

void __fastcall TProfiler::Refresh()
{
        for (size_t i = 0; i < _profileDetails.size(); ++i) {
                TListItem* item = ListViewProfileSamples->Items->Item[i];
                UpdateItem(item, item->Caption, _profileDetails[i]);
        }
}

