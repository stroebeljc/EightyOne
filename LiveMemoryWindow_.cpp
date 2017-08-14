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
        int maxCount = (Reads1->Checked ? 500 : 0) +
                        (Writes1->Checked ? 500 : 0);

        ++_count;
        if (_count < maxCount) return;

        _count = 0;
        for(int i = 0; i < 65536; ++i)
        {
                if (_writes[i]!=0)
                {
                        --_writes[i];
                        _pbits[i].rgbBlue = _writes[i];
                        _pbits[i].rgbGreen = 128;
                }
                if (_reads[i]!=0)
                {
                        --_reads[i];
                        _pbits[i].rgbRed = _reads[i];
                        _pbits[i].rgbGreen = 128;
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

