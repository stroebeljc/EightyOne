//---------------------------------------------------------------------------

#ifndef ProfSampEditH
#define ProfSampEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TProfileSampleEdit : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *EditTag;
        TEdit *EditStart;
        TEdit *EditEnd;
        TButton *ButtonOK;
        void __fastcall ButtonOKClick(TObject *Sender);
        void __fastcall EditDblClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        bool _valid;
        class ProfileDetail* _pd;
        void (*_completion)(bool, AnsiString);

public:		// User declarations
        __fastcall TProfileSampleEdit(TComponent* Owner);

        void __fastcall EditValues(AnsiString tag, ProfileDetail* pd, void(*)(bool, AnsiString));
};
//---------------------------------------------------------------------------
extern PACKAGE TProfileSampleEdit *ProfileSampleEdit;
//---------------------------------------------------------------------------
#endif
