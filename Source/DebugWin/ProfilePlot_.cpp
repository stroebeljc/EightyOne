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

        double scale = (double)ClientHeight / (double)(_pd->Max() - _pd->Min());

        Canvas->Pen->Color = clBlue;
        Canvas->MoveTo(0, ClientHeight - (int)((double)_pd->Sample(0) * scale));

        int sbBase = ScrollBarHorizontal->Position;

        for (int i = 0; i < min(ClientWidth, _pd->SampleCount()); ++i) {
                double sam = _pd->Sample(i + sbBase) - _pd->Min();
                Canvas->LineTo(i, ClientHeight - (int)(sam * scale));
        }

        AnsiString min = "MIN T = ";
        AnsiString max = "MAX T = ";
        if (_pd->SampleCount()) {
                min += _pd->Min();
                max += _pd->Max();
        }
        else {
                min += "---";
                max += "---";
        }

        Canvas->TextOutA(8,  8, max);
        Canvas->TextOutA(8, 24, min);

        if(_mouseX + ScrollBarHorizontal->Position < _pd->SampleCount()) {
                Canvas->TextOutA(8, 40, _pd->Sample(_mouseX + ScrollBarHorizontal->Position));
        }
}
//---------------------------------------------------------------------------

void TProfilePlot::InitScrollbar()
{
        if (_pd == NULL) return;

        ScrollBarHorizontal->Min = 0;
        bool visible = _pd->SampleCount() > ClientWidth;
        ScrollBarHorizontal->Visible = visible;
        if (visible) {
                ScrollBarHorizontal->Max = _pd->SampleCount() - ClientWidth;
        }
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::PlotTGraph(ProfileDetail* pd)
{
        _pd = pd;
        InitScrollbar();
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
        _mouseX = coords.x + ScrollBarHorizontal->Position;
        TRect invreg;
        invreg.left = 8;
        invreg.top = 40;
        invreg.right = 108;
        invreg.bottom = 56;
        int ax = _mouseX + ScrollBarHorizontal->Position;
        if(ax > 0 && ax < _pd->SampleCount()) {
                InvalidateRect(this, &invreg, false);
                Refresh();
        }
}
//---------------------------------------------------------------------------

