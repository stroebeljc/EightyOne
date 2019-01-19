//---------------------------------------------------------------------------

#ifndef BasicListingFormatInfo_H
#define BasicListingFormatInfo_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TBasicListingFormatInfoForm : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet3;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *Weblink1;
        TLabel *Label21;
        TLabel *Weblink2;
        TLabel *Label23;
        TLabel *Weblink3;
        TButton *btnOK;
        TLabel *Label20;
        TLabel *Label22;
        TLabel *Label24;
        void __fastcall Weblink1Click(TObject *Sender);
        void __fastcall Weblink2Click(TObject *Sender);
        void __fastcall Weblink3Click(TObject *Sender);
private:	// User declarations
        void OpenWebsite();
public:		// User declarations
        __fastcall TBasicListingFormatInfoForm(TComponent* Owner);
        void TBasicListingFormatInfoForm::SetActivePage(int index);
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicListingFormatInfoForm *BasicListingFormatInfoForm;
//---------------------------------------------------------------------------
#endif
