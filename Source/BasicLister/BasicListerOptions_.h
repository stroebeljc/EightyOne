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
        TButton *btnOK;
        TGroupBox *GroupBoxSaveOptions;
        TCheckBox *cboOutputFullWidthLineNumbers;
        TCheckBox *cboOutputRemTokensAsCharacterCodes;
        TCheckBox *cboOutputNonAsciiAsCharacterCodes;
        TCheckBox *cboOutputStringTokensAsCharacterCodes;
        TCheckBox *cboLimitLineLengths;
        TCheckBox *cboOutputVariableNamesInLowercase;
        TGroupBox *GroupBoxDisplayOptions;
        TLabel *LabelTextScaling;
        TComboBox *cboTextScaling;
private:	// User declarations
public:		// User declarations
        __fastcall TSaveBasicListingOptionsForm(TComponent* Owner);
        bool GetOutputRemTokensAsCharacterCodes();
        bool GetOutputStringTokensAsCharacterCodes();
        bool GetOutputNonAsciiAsCharacterCodes();
        bool GetOutputVariableNamesInLowercase();
        bool GetLimitLineLengths();
        bool GetOutputFullWidthLineNumbers();
        int GetScalingFator();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TSaveBasicListingOptionsForm *SaveBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif
