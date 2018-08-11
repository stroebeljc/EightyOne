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
        keyboardLeft = -10000;
        keyboardTop = -10000;
}
//---------------------------------------------------------------------------
                     
void __fastcall TKeyboardFunctions::zx80IntegralFunctionsDblClick(
      TObject *Sender)
{
        Keyboard->KeyboardDblClick(this);        
}
//---------------------------------------------------------------------------

void __fastcall TKeyboardFunctions::EnableTimer(bool enable)
{
        Tracker->Enabled = enable;
}
//---------------------------------------------------------------------------
void __fastcall TKeyboardFunctions::TrackerTimer(TObject *Sender)
{
        if ((Keyboard->Left + Keyboard->Width + Keyboard->FunctionsOffset != KeyboardFunctions->Left) ||
            (Keyboard->Top != KeyboardFunctions->Top))
        {
                if ((Keyboard->Left != keyboardLeft) ||(Keyboard->Top != keyboardTop))
                {
                        KeyboardFunctions->Left = Keyboard->Left + Keyboard->Width + Keyboard->FunctionsOffset;
                        KeyboardFunctions->Top = Keyboard->Top;
                }
                else
                {
                        Keyboard->Left = KeyboardFunctions->Left - Keyboard->FunctionsOffset - Keyboard->Width;
                        Keyboard->Top = KeyboardFunctions->Top;
                }

                keyboardLeft = Keyboard->Left;
                keyboardTop = Keyboard->Top;

                Tracker->Interval = 25;
        }
        else if (Tracker->Interval < 800)
        {
                Tracker->Interval *= 2;
        }
}
//---------------------------------------------------------------------------

