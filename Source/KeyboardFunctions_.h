//---------------------------------------------------------------------------

#ifndef KeyboardFunctions_H
#define KeyboardFunctions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TKeyboardFunctions : public TForm
{
__published:	// IDE-managed Components
        TImage *zx80IntegralFunctions;
        TTimer *Tracker;
        void __fastcall zx80IntegralFunctionsDblClick(TObject *Sender);
        void __fastcall TrackerTimer(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
        int keyboardLeft;
        int keyboardTop;
public:		// User declarations
        __fastcall TKeyboardFunctions(TComponent* Owner);
        void __fastcall EnableTimer(bool enable);
};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboardFunctions *KeyboardFunctions;
//---------------------------------------------------------------------------
#endif
