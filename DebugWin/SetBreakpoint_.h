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
        TRadioButton *RadioButtonInRange;
        TEdit *EditAddressHi;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall RadioButtonClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
        void __fastcall UpdateRBStates();

public:		// User declarations
        __fastcall TSetBreakpoint(TComponent* Owner);
        bool EditValue(int& address, BreakpointConditionType& condition);
        bool EditBreakpoint(struct breakpoint& bp);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
#endif
