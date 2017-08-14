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
//        BITMAPINFO bmi = {};
  //      void* dibBits;

    //    HBITMAP hbmp = CreateDIBSection(Canvas->Handle, &bmi,
      //   DIB_RGB_COLORS, &dibBits, NULL, 0);

}

//---------------------------------------------------------------------------
void __fastcall TLiveMemoryWindow::Write(int address)
{

}
void __fastcall TLiveMemoryWindow::WMEraseBkgnd(TWMEraseBkgnd& Message)
{
   Message.Result = true;
}
void __fastcall TLiveMemoryWindow::WMDraw(TWMEraseBkgnd& Message)
{

}
//---------------------------------------------------------------------------

