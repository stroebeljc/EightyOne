//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "kb_.h"
#include "kbstatus.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKb *Kb;
//---------------------------------------------------------------------------
__fastcall TKb::TKb(TComponent* Owner)
        : TForm(Owner)
{
        if (CursorMode->ItemIndex==-1) CursorMode->ItemIndex=1;
}
//---------------------------------------------------------------------------
void __fastcall TKb::Button1Click(TObject *Sender)
{
        switch(CursorMode->ItemIndex)
        {
        case 0:
                PCKeySetCursor('5', '6', '7', '8', 0);
                break;
        case 1:
                PCKeySetCursor('5', '6', '7', '8', 1);
                break;
        case 2:
                PCKeySetCursor(toupper(CustomLeft->Text[1]),
                                toupper(CustomDown->Text[1]),
                                toupper(CustomUp->Text[1]),
                                toupper(CustomRight->Text[1]),
                                0);
        }

        PCKeySetCTRL( RadioButton1->Checked? 0 : '0' );

        Close();
}
//---------------------------------------------------------------------------
void __fastcall TKb::CursorModeChange(TObject *Sender)
{
        if (CursorMode->ItemIndex==2)
        {
                CustomLeft->Enabled=true;
                CustomRight->Enabled=true;
                CustomUp->Enabled=true;
                CustomDown->Enabled=true;
        }
        else
        {
                CustomLeft->Enabled=false;
                CustomRight->Enabled=false;
                CustomUp->Enabled=false;
                CustomDown->Enabled=false;
        }

}
//---------------------------------------------------------------------------

