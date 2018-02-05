//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Profiler.h"
#include "ProfSampEdit.h"
#include "ProfileDetail.h"
#include "symbolstore.h"
#include "z80.h"

static const int START = 0;
static const int END = 1;
static const int MIN = 2;
static const int MAX = 3;
static const int COUNT = 4;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfiler *Profiler;
//---------------------------------------------------------------------------
__fastcall TProfiler::TProfiler(TComponent* Owner)
        : TForm(Owner)
{
        _pse = new TProfileSampleEdit(this);
}
//---------------------------------------------------------------------------
void __fastcall TProfiler::EnableButtons(bool enabled)
{
        ButtonNew->Enabled = enabled;
        ButtonDelete->Enabled = enabled;
        ButtonEdit->Enabled = enabled;
        ButtonReset->Enabled = enabled;
}
//---------------------------------------------------------------------------
void __fastcall TProfiler::UpdateItem(TListItem* item, AnsiString tag, ProfileDetail& pd)
{
        item->Caption = tag;
        item->SubItems->Strings[START] = symbolstore::addressToSymbolOrHex(pd._start);
        item->SubItems->Strings[END] = symbolstore::addressToSymbolOrHex(pd._end);
        item->SubItems->Strings[MIN] = pd.Min() == INT_MAX ? "--" : IntToStr(pd.Min()).c_str();
        item->SubItems->Strings[MAX] = pd.Max() == INT_MIN ? "--" : IntToStr(pd.Max()).c_str();
        item->SubItems->Strings[COUNT] = pd.SampleCount();
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonNewClick(TObject *Sender)
{
        EnableButtons(false);

        _newPD = new ProfileDetail(0, 0);
        _pse->EditValues("", _newPD, &SampleEditComplete);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonEditClick(TObject *Sender)
{
        TListItem* editItem = ListViewProfileSamples->Selected;
        if (!editItem) return;

        EnableButtons(false);

        _pse->EditValues(editItem->Caption, &_profileDetails[editItem->Index], &SampleEditComplete);
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

void __fastcall TProfiler::SampleEditCompleteImpl(bool valid, AnsiString tag)
{
        EnableButtons(true);

        if (!valid) {
                delete(_newPD);
                _newPD = NULL;
                return;
        }

        if (_newPD)
        {
                _profileDetails.push_back(*_newPD);

                TListItem* newItem = ListViewProfileSamples->Items->Add();
                newItem->Caption = tag;
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");

                _newPD = NULL;
        }

        Refresh();
}

void TProfiler::SampleEditComplete(bool valid, AnsiString tag)
{
        Profiler->SampleEditCompleteImpl(valid, tag);
}

//---------------------------------------------------------------------------

void __fastcall TProfiler::Refresh()
{
        for (size_t i = 0; i < _profileDetails.size(); ++i) {
                TListItem* item = ListViewProfileSamples->Items->Item[i];
                UpdateItem(item, item->Caption, _profileDetails[i]);
        }
}


void __fastcall TProfiler::ButtonRefreshClick(TObject *Sender)
{
        Refresh();
}
//---------------------------------------------------------------------------

