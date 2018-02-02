//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Profiler.h"
#include "ProfSampEdit.h"
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
        item->SubItems->Add(pd.Start());
        item->SubItems->Add(pd.End());
        item->SubItems->Add(pd.Max());
        item->SubItems->Add(pd.SampleCount());
}

void __fastcall TProfiler::ButtonNewClick(TObject *Sender)
{
        TProfileSampleEdit* pse = new TProfileSampleEdit(this);

        ProfileDetail pd(0, 0);
        if (pse->EditValues(pd)) {
                _profileDetails.push_back(pd);

                TListItem* newItem = ListViewProfileSamples->Items->Add();
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
        if (pse->EditValues(pd)) {
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

void __fastcall TProfiler::UpdateProfileDetails(int addr, int tstates)
{
}

