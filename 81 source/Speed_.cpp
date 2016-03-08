//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main_.h"
#include "Speed_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSpeed *Speed;
//---------------------------------------------------------------------------
__fastcall TSpeed::TSpeed(TComponent* Owner)
        : TForm(Owner)
{
        DirtyFastTStates=64000;
        DirtySlowTStates=16000;
        AccurateScanlines=312;
}
//---------------------------------------------------------------------------
void __fastcall TSpeed::Button1Click(TObject *Sender)
{
        Form1->Speed1->Checked=false;
        Close();
}
//---------------------------------------------------------------------------


void __fastcall TSpeed::FormShow(TObject *Sender)
{
        ChangeMode();
}
//---------------------------------------------------------------------------
 void TSpeed::ChangeMode()
{
        if (Form1->Fast1->Checked)
        {
                FastPanel->Visible=true;
                AccuratePanel->Visible=false;
        }
        else
        {
                FastPanel->Visible=false;
                AccuratePanel->Visible=true;
        }
}

void __fastcall TSpeed::Recalc(TObject *Sender)
{
        DirtyFastTStates=64000;
        DirtySlowTStates=16000;
        AccurateScanlines=312;

        if (FastMode->Position>5) DirtyFastTStates *= 1<<(FastMode->Position-5);
        if (FastMode->Position<5) DirtyFastTStates /= 1<<(5-FastMode->Position);
        if (SlowMode->Position>5) DirtySlowTStates *= 1<<(SlowMode->Position-5);
        if (SlowMode->Position<5) DirtySlowTStates /= 1<<(5-SlowMode->Position);
        if (AccurateSpeed->Position>4) AccurateScanlines *= 1<<(AccurateSpeed->Position-4);
        if (AccurateSpeed->Position<4) AccurateScanlines /= 1<<(4-AccurateSpeed->Position);
}
//---------------------------------------------------------------------------

