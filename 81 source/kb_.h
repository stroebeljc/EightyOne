//---------------------------------------------------------------------------

#ifndef kb_H
#define kb_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TKb : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TComboBox *CursorMode;
        TButton *Button1;
        TEdit *CustomUp;
        TEdit *CustomDown;
        TEdit *CustomLeft;
        TEdit *CustomRight;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TLabel *Label2;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall CursorModeChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TKb(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TKb *Kb;
//---------------------------------------------------------------------------
#endif
