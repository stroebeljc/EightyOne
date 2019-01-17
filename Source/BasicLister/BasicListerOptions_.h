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
        TCheckBox *cboOutputRemTokensAsCharacterCodes;
        TButton *btnOK;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TCheckBox *cboOutputNonAsciiAsCharacterCodes;
        TCheckBox *cboOutputStringTokensAsCharacterCodes;
        TLabel *Label4;
        TLabel *Label5;
        TCheckBox *cboLimitLineLengths;
private:	// User declarations
public:		// User declarations
        __fastcall TSaveBasicListingOptionsForm(TComponent* Owner);
        bool GetOutputRemTokensAsCharacterCodes();
        bool GetOutputStringTokensAsCharacterCodes();
        bool GetOutputNonAsciiAsCharacterCodes();
        bool GetLimitLineLengths();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TSaveBasicListingOptionsForm *SaveBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif
