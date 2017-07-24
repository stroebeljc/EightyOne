//---------------------------------------------------------------------------

#ifndef SetBreakpoint_H
#define SetBreakpoint_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Debug.h"

//---------------------------------------------------------------------------
class TSetBreakpoint : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TButton *Cancel;
        TLabel *Label1;
        TEdit *EditAddress;
        TRadioButton *RadioButtonEqual;
        TRadioButton *RadioButtonLessThan;
        TRadioButton *RadioButtonGreaterThan;
        TLabel *Label2;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TSetBreakpoint(TComponent* Owner);
        bool EditValue(int& address, TDbg::BreakpointConditionType& condition);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
#endif
