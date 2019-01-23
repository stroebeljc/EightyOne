//---------------------------------------------------------------------------

#ifndef BasicLoaderOptions_H
#define BasicLoaderOptions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iniFiles.hpp"

//---------------------------------------------------------------------------
class TLoadBasicListingOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *cboTokeniseRemContents;
        TCheckBox *cboTokeniseStringContents;
        TButton *btnOK;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TCheckBox *cboDiscardRedundantSpaces;
        TLabel *Label4;
        TCheckBox *cboAcceptAlternateKeywordSpelling;
        TLabel *Label5;
        TComboBox *cboLineNumberIncrement;
        TLabel *Label6;
private:	// User declarations
public:		// User declarations
        __fastcall TLoadBasicListingOptionsForm(TComponent* Owner);
        bool GetTokeniseRemContents();
        bool GetTokeniseStringContents();
        bool GetDiscardRedundantSpaces();
        bool GetAcceptAlternateKeywordSpelling();
        int GetAutomaticLineNumberIncrement();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoadBasicListingOptionsForm *LoadBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif
