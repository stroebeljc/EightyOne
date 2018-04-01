//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>

#pragma hdrstop

#include "ProfilePlot_.h"
#include "ProfileDetail.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfilePlot *ProfilePlot;
//---------------------------------------------------------------------------
__fastcall TProfilePlot::TProfilePlot(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::FormPaint(TObject *Sender)
{
        if (_pd == NULL || _pd->SampleCount() == 0) return;

        int min = _pd->Min() - 100;
        int max = _pd->Max() + 100;
        if (min < 0) min = 0;
        int range = max - min;

        double scale = (double)ClientHeight / (double)range;

        Canvas->Pen->Color = clBlue;
        Canvas->MoveTo(0, ClientHeight - (int)((double)_pd->Sample(0) * scale));

        int sbBase = ScrollBarHorizontal->Enabled ? ScrollBarHorizontal->Position : 0;
        int mini = ClientWidth;
        if (_pd->SampleCount() < mini) mini = _pd->SampleCount();

        for (int i = 0; i < mini; ++i) {
                double sam = _pd->Sample(i + sbBase) - _pd->Min();
                Canvas->LineTo(i, ClientHeight - (int)(sam * scale));
        }
}
//---------------------------------------------------------------------------

void TProfilePlot::InitScrollbar()
{
        if (_pd == NULL) return;

        ScrollBarHorizontal->Min = 0;
        bool enabled = _pd->SampleCount() > ClientWidth;
        ScrollBarHorizontal->Enabled = enabled;
        ScrollBarHorizontal->Max = _pd->SampleCount() - ClientWidth;
        ScrollBarHorizontal->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::PlotTGraph(ProfileDetail* pd)
{
        _pd = pd;
        InitScrollbar();

        AnsiString minmax = "Min: ---- Max: ----";
        if (_pd) {
                minmax = "Min: ";
                minmax += _pd->Min();
                minmax += "Max: ";
                minmax += _pd->Max();
        }
        StatusBar1->Panels->Items[0]->Text = minmax;

        Show();
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::FormResize(TObject *Sender)
{
        InitScrollbar();
        Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::ScrollBarHorizontalChange(TObject *Sender)
{
        Refresh();
}


void __fastcall TProfilePlot::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        TPoint coords;
        coords.x = X;
        coords.y = Y;
        ScreenToClient(coords);
        int sample = coords.x;
        if (ScrollBarHorizontal->Enabled) {
                sample += ScrollBarHorizontal->Position;
        }

        AnsiString cur = "Current: ----";
        AnsiString sam = "Sample: ----";

        if (_pd) {
                cur = "Current: ";
                cur += _pd->Sample(sample);
                sam = "Sample: ";
                sam += sample;
        }
        StatusBar1->Panels->Items[1]->Text = sam;
        StatusBar1->Panels->Items[2]->Text = cur;
}
//---------------------------------------------------------------------------

