//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include "LiveMemoryWindow_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------
__fastcall TLiveMemoryWindow::TLiveMemoryWindow(TComponent* Owner)
        : TForm(Owner)
{
        BITMAPINFOHEADER bmphdr = {sizeof(bmphdr), 256, -256, 1, 32, BI_RGB,
                0,0,0,0,0};

        _hdib = CreateDIBSection (Canvas->Handle, (BITMAPINFO*)&bmphdr,
                DIB_RGB_COLORS, (VOID**)&_pbits, NULL, 0);

        Reset();
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Reset()
{
        memset((void*)_pbits, 0, sizeof(RGBQUAD) * 65536);
        memset(_reads, 0, 65536);
        memset(_writes, 0, 65536);
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Write(int address)
{
        if (!Visible) return;
        if (!Writes1->Checked) return;

        _writes[address]=255;
        Update();
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Read(int address)
{
        if (!Visible) return;
        if (!Reads1->Checked) return;

        _reads[address]=255;
        Update();
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Update(void)
{
        int touchCol = Touches1->Checked ? 128 : 0;

        // should really be time based, not access count based
        ++_count;
        if (_count < 1000) return;

        _count = 0;

        for(int i = 0; i < 65536; ++i)
        {
                if (_writes[i])
                {
                        --_writes[i];
                        _pbits[i].rgbBlue = _writes[i];
                        _pbits[i].rgbGreen = touchCol;
                }
                if (_reads[i])
                {
                        --_reads[i];
                        _pbits[i].rgbRed = _reads[i];
                        _pbits[i].rgbGreen = touchCol;
                }
        }

        Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::WMEraseBkgnd(TWMEraseBkgnd &Message)
{
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::FormPaint(TObject *Sender)
{
        HDC hdcMem = CreateCompatibleDC (Canvas->Handle);
        HBITMAP hbmpOld = (HBITMAP)SelectObject (hdcMem, _hdib);
        BitBlt (Canvas->Handle, 0, 0, 256, 256, hdcMem, 0, 0, SRCCOPY);
        SelectObject (hdcMem, hbmpOld);
        DeleteDC (hdcMem);
}

void __fastcall TLiveMemoryWindow::Reset1Click(TObject *Sender)
{
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Reads1Click(TObject *Sender)
{
        Reads1->Checked = !Reads1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Writes1Click(TObject *Sender)
{
        Writes1->Checked = !Writes1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Touches1Click(TObject *Sender)
{
        Touches1->Checked = !Touches1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        int n = X + 256 * Y;
        if (n > 0xffff) n = 0xffff;

        StatusBar1->Panels->Items[0]->Text = Format("$%0.4x", ARRAYOFCONST((n)));
}
//---------------------------------------------------------------------------

