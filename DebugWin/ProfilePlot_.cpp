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
        if (_pd == NULL) return;

        double scale = (double)ClientHeight / (double)_pd->Max();

        Canvas->Pen->Color = clBlue;
        Canvas->MoveTo(0, ClientHeight - (int)((double)_pd->Sample(0) * scale));

        for (int i = 0; i < min(ClientWidth, _pd->SampleCount()); ++i) {
                Canvas->LineTo(i, ClientHeight - (int)((double)_pd->Sample(i) * scale));
        }
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::PlotTGraph(ProfileDetail* pd)
{
        _pd = pd;
        Show();
}

