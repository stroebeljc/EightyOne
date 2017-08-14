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

        _writes[address]=255;
        ++_count;
        if (_count == 1000)
        {
                _count = 0;
                Update();
                Invalidate();
        }
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Read(int address)
{
        if (!Visible) return;

        _reads[address]=255;
        ++_count;
        if (_count == 1000)
        {
                _count = 0;
                Update();
                Invalidate();
        }
}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Update(void)
{
        for(int i = 0; i < 65536; ++i)
        {
                if (_writes[i]!=0)
                {
                        --_writes[i];
                        _pbits[i].rgbRed = _writes[i];
                        _pbits[i].rgbGreen = 128;
                }
                if (_reads[i]!=0)
                {
                        --_reads[i];
                        _pbits[i].rgbBlue = _reads[i];
                }
        }
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

