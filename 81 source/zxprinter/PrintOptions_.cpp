//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PrintOptions_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPrintOptions *PrintOptions;
//---------------------------------------------------------------------------
__fastcall TPrintOptions::TPrintOptions(TComponent* Owner)
        : TForm(Owner)
{
        BitSpeed=25;
        LineSpeed=16;

        BitRate->Position = 100-25;
        LineRate->Position = 100-16;

}
//---------------------------------------------------------------------------
void __fastcall TPrintOptions::BitRateChange(TObject *Sender)
{
        BitRateTxt->Caption = ((int) BitRate->Position);
        BitRateTxt->Caption = "Carriage Speed: "+BitRateTxt->Caption;
        BitSpeed = 100 - BitRate->Position;
}
//---------------------------------------------------------------------------
void __fastcall TPrintOptions::LineRateChange(TObject *Sender)
{
        LineRateTxt->Caption = ((int) LineRate->Position);
        LineRateTxt->Caption = "Paper Feed Speed: "+LineRateTxt->Caption;
        LineSpeed = 100 - LineRate->Position;
}
//---------------------------------------------------------------------------
void __fastcall TPrintOptions::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
