//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "KeyboardFunctions_.h"
#include "Keyboard_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKeyboardFunctions *KeyboardFunctions;
//---------------------------------------------------------------------------
__fastcall TKeyboardFunctions::TKeyboardFunctions(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TKeyboardFunctions::FormStartDock(TObject *Sender,
      TDragDockObject *&DragObject)
{
        Keyboard->Left = KeyboardFunctions->Left - Keyboard->Width;
        Keyboard->Top = KeyboardFunctions->Top;
}
//---------------------------------------------------------------------------

void __fastcall TKeyboardFunctions::FormEndDock(TObject *Sender,
      TObject *Target, int X, int Y)
{
        Keyboard->Left = KeyboardFunctions->Left - Keyboard->Width;
        Keyboard->Top = KeyboardFunctions->Top;
}
//---------------------------------------------------------------------------

void __fastcall TKeyboardFunctions::FormResize(TObject *Sender)
{
        Keyboard->Left = KeyboardFunctions->Left - Keyboard->Width;
        Keyboard->Top = KeyboardFunctions->Top;

}
//---------------------------------------------------------------------------


void __fastcall TKeyboardFunctions::zx80IntegralFunctionsDblClick(
      TObject *Sender)
{
        Keyboard->KeyboardDblClick(this);        
}
//---------------------------------------------------------------------------

