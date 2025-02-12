//---------------------------------------------------------------------------

#ifndef Keypad_H
#define Keypad_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TKeypad128 : public TForm
{
__published:	// IDE-managed Components
        TImage *KeypadUk;
        TImage *KeypadSpanish;
        TTimer *Tracker;
        void __fastcall TrackerTimer(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall KeypadDblClick(TObject *Sender);
private:	// User declarations
        int keyboardLeft;
        int keyboardTop;
public:		// User declarations
        __fastcall TKeypad128(TComponent* Owner);
        void __fastcall EnableTimer(bool enable);
        void SelectUKLayout();
        void SelectSpanishLayout();
};
//---------------------------------------------------------------------------
extern PACKAGE TKeypad128 *Keypad128;
//---------------------------------------------------------------------------
#endif
