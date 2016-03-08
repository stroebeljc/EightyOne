//---------------------------------------------------------------------------

#ifndef Keyboard_H
#define Keyboard_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TKeyboard : public TForm
{
__published:	// IDE-managed Components
        TImage *zx81kb;
        TImage *zx80kb;
        TImage *acekb;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        __fastcall TKeyboard(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboard *Keyboard;
//---------------------------------------------------------------------------
#endif
