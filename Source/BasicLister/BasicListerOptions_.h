//---------------------------------------------------------------------------

#ifndef BasicListerOptions_H
#define BasicListerOptions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IniFiles.hpp"

//---------------------------------------------------------------------------
class TSaveBasicListingOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *cboOutputAsCharacterCodes;
        TButton *btnOK;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TCheckBox *cboOutputNonAsciiAsCharacterCodes;
private:	// User declarations
public:		// User declarations
        __fastcall TSaveBasicListingOptionsForm(TComponent* Owner);
        bool GetOutputTokensAsCharacterCodes();
        bool GetOutputNonAsciiAsCharacterCodes();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TSaveBasicListingOptionsForm *SaveBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif
