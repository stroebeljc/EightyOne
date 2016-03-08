//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Keyboard_.h"
#include "main_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKeyboard *Keyboard;
//---------------------------------------------------------------------------
__fastcall TKeyboard::TKeyboard(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TKeyboard::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->KeyboardMap1->Checked=false;
}
//---------------------------------------------------------------------------

