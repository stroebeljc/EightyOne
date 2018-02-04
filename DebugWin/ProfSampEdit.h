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
private:	// User declarations
        bool _cancelled;
public:		// User declarations
        __fastcall TProfileSampleEdit(TComponent* Owner);

        bool __fastcall EditValues(AnsiString tag, class ProfileDetail& pd);
};
//---------------------------------------------------------------------------
extern PACKAGE TProfileSampleEdit *ProfileSampleEdit;
//---------------------------------------------------------------------------
#endif
