//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AddBreak_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNewBreakForm *NewBreakForm;
//---------------------------------------------------------------------------
__fastcall TNewBreakForm::TNewBreakForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::CancelBtnClick(TObject *Sender)
{
        Value->Text="";
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::OKBtnClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::FormShow(TObject *Sender)
{
        Value->Text="";        
}
//---------------------------------------------------------------------------
