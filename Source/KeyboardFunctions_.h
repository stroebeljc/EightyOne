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
        void __fastcall FormStartDock(TObject *Sender,
          TDragDockObject *&DragObject);
        void __fastcall FormEndDock(TObject *Sender, TObject *Target,
          int X, int Y);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall zx80IntegralFunctionsDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TKeyboardFunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboardFunctions *KeyboardFunctions;
//---------------------------------------------------------------------------
#endif
