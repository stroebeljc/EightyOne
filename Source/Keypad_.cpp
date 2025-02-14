//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "Keypad_.h"
#include "Keyboard_.h"
#include "main_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKeypad128 *Keypad128;

//---------------------------------------------------------------------------
__fastcall TKeypad128::TKeypad128(TComponent* Owner)
        : TForm(Owner)
{
        keyboardLeft = -10000;
        keyboardTop = -10000;
}
//---------------------------------------------------------------------------

void __fastcall TKeypad128::KeypadDblClick(TObject *Sender)
{
        Keyboard->KeyboardDblClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TKeypad128::EnableTimer(bool enable)
{
        Tracker->Enabled = enable;
}
//---------------------------------------------------------------------------
void __fastcall TKeypad128::TrackerTimer(TObject *Sender)
{
        if ((Keyboard->Left + Keyboard->Width + Keyboard->KeypadOffset != Keypad128->Left) ||
            (Keyboard->Top != Keypad128->Top))
        {
                if ((Keyboard->Left != keyboardLeft) ||(Keyboard->Top != keyboardTop))
                {
                        Keypad128->Left = Keyboard->Left + Keyboard->Width + Keyboard->KeypadOffset;
                        Keypad128->Top = Keyboard->Top;
                }
                else
                {
                        Keyboard->Left = Keypad128->Left - Keyboard->KeypadOffset - Keyboard->Width;
                        Keyboard->Top = Keypad128->Top;
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
void __fastcall TKeypad128::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        Form1->FormKeyDown(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TKeypad128::FormKeyPress(TObject *Sender, char &Key)
{
        Form1->FormKeyPress(Sender, Key);
}
//---------------------------------------------------------------------------
void __fastcall TKeypad128::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        Form1->FormKeyUp(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void TKeypad128::SelectUKLayout()
{
        KeypadUk->Visible = true;
        KeypadSpanish->Visible = false;
}
//---------------------------------------------------------------------------
void TKeypad128::SelectSpanishLayout()
{
        KeypadSpanish->Visible = true;
        KeypadUk->Visible = false;
}