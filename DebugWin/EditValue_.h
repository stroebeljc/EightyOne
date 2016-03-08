//---------------------------------------------------------------------------

#ifndef EditValue_H
#define EditValue_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditValue : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditVal;
        TButton *OK;
        TButton *Cancel;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TEditValue(TComponent* Owner);
        bool Edit2(int& value,int len);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditValue *EditValue;
//---------------------------------------------------------------------------
#endif
