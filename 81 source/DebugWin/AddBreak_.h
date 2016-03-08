//---------------------------------------------------------------------------

#ifndef AddBreak_H
#define AddBreak_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TNewBreakForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *Value;
        TButton *OKBtn;
        TButton *CancelBtn;
        void __fastcall CancelBtnClick(TObject *Sender);
        void __fastcall OKBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TNewBreakForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNewBreakForm *NewBreakForm;
//---------------------------------------------------------------------------
#endif
