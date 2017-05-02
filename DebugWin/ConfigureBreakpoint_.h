//---------------------------------------------------------------------------

#ifndef ConfigureBreakpoint_H
#define ConfigureBreakpoint_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TConfigureBreakpoint : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TButton *Cancel;
        TLabel *Label1;
        TEdit *EditAddress;
        TLabel *Label2;
        TEdit *EditTStates;
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CancelClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TConfigureBreakpoint(TComponent* Owner);
        bool EditValues(int& address, int len, int& tStates);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigureBreakpoint *ConfigureBreakpoint;
//---------------------------------------------------------------------------
#endif
