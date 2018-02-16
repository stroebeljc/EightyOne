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
        TLabel *BreakAddress;
        TEdit *EditAddress;
        TLabel *Label2;
        TEdit *EditAddressArgument;
        TLabel *BreakAddressArgument;
        TComboBox *BreakCondition;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall BreakConditionChange(TObject *Sender);
        void __fastcall EditAddressArgumentChange(TObject *Sender);
        void __fastcall EditAddressChange(TObject *Sender);
private:	// User declarations
        bool cancelled;
        void __fastcall UpdateRBStates();

public:		// User declarations
        __fastcall TSetBreakpoint(TComponent* Owner);
        void SetTitle(AnsiString& title);
        bool EditTSetBreakpoint(int& address, int len, int& tStates);
        bool EditBreakpoint(struct breakpoint& bp);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
#endif
