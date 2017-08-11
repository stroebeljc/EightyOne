//---------------------------------------------------------------------------

#include <vcl.h>
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
}
//---------------------------------------------------------------------------
