//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Debug.h"
#include "zx81.h"
#include "z80.h"
#include "zx81config.h"

#include "main_.h"
#include "AddBreak_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDbg *Dbg;

extern int rowcounter, RasterY, HSYNC_generator, NMI_generator;
extern unsigned char shift_store;

extern unsigned char memory[];

void DebugUpdate(void)
{
        static int NMISaveSingleStep=-1, INTSaveSingleStep=-1;

        if (Dbg->NMIRetAddr==-1 && NMISaveSingleStep!=-1)
        {
                zx81.single_step=NMISaveSingleStep;
                NMISaveSingleStep=-1;
        }
        if (Dbg->INTRetAddr==-1 && INTSaveSingleStep!=-1)
        {
                zx81.single_step=INTSaveSingleStep;
                INTSaveSingleStep=-1;
        }


        if (z80.pc.w==0x66 && Dbg->SkipNMIBtn->Checked
                && Dbg->NMIRetAddr==-1 && Dbg->Continuous->Checked)
        {
                Dbg->NMIRetAddr = memory[z80.sp.w] + 256*memory[z80.sp.w + 1];
                NMISaveSingleStep = zx81.single_step;
                zx81.single_step=0;
        }
        if (z80.pc.w == Dbg->NMIRetAddr)
        {
                Dbg->NMIRetAddr=-1;
                zx81.single_step=NMISaveSingleStep;
        }
        if (Dbg->NMIRetAddr!=-1) return;

        if (z80.pc.w==0x38 && Dbg->SkipINTBtn->Checked
                && Dbg->INTRetAddr==-1 && Dbg->Continuous->Checked)
        {
                Dbg->INTRetAddr = memory[z80.sp.w] + 256*memory[z80.sp.w + 1];
                INTSaveSingleStep = zx81.single_step;
                zx81.single_step=0;
        }
        if (z80.pc.w == Dbg->INTRetAddr)
        {
                Dbg->INTRetAddr=-1;
                zx81.single_step=INTSaveSingleStep;
        }
        if (Dbg->INTRetAddr!=-1) return;

        if (Dbg->BreakPointHit(z80.pc.w))
        {
                //zx81_stop=1;
                Dbg->DoNext=0;
                Dbg->UpdateVals();
                //zx81.single_step=Dbg->Continuous->Checked;
                Dbg->RunStopClick(NULL);
        }


        if (Dbg->DoNext)
        {
                zx81_stop=1;
                Dbg->DoNext=0;
                Dbg->UpdateVals();
                zx81.single_step=Dbg->Continuous->Checked;
        }

        if (Dbg->Continuous->Checked==true && Dbg->Visible==true)
                Dbg->UpdateVals();
}
//---------------------------------------------------------------------------
void TDbg::AddBreakPoint(int Addr, bool Perm)
{
        Breakpoint[Breakpoints].Addr=Addr;
        Breakpoint[Breakpoints].Permanent=Perm;
        if (Perm) BPList->Cells[0][Breakpoints]="$"+Hex16(Addr);
        else  BPList->Cells[0][Breakpoints]="($"+Hex16(Addr)+")";

        Breakpoints++;
        BPList->RowCount++;
}

void TDbg::DelBreakPoint(int Addr)
{
        int i;
        for(i=0; i<Breakpoints; i++)
        {
                if ((Breakpoint[i].Addr == Addr) || (i==Addr-100000))
                {
                        int j;
                        for(j=i; j<Breakpoints;j++)
                        {
                                Breakpoint[j] = Breakpoint[j+1];
                                BPList->Cells[0][j] = BPList->Cells[0][j+1];
                        }
                        Breakpoints--;
                        BPList->RowCount--;
                }
        }
}

bool TDbg::BreakPointHit(int Addr)
{
        int i;
        for(i=0; i<Breakpoints; i++)
        {
                if (Breakpoint[i].Addr == Addr)

                {
                        if (!Breakpoint[i].Permanent) DelBreakPoint(Addr);
                        BPList->Row=i;
                        return(true);
                }
        }
        return(false);
}
void TDbg::DelTempBreakPoints(void)
{
        int i;
        for(i=0; i<Breakpoints; i++)
                if (!Breakpoint[i].Permanent) DelBreakPoint(Breakpoint[i].Addr);
}
//---------------------------------------------------------------------------
int TDbg::Hex2Dec(AnsiString num)
{
        int val=0;
        int i=1;

        for(i=1; i<=num.Length(); i++)
        {
                if (num[i]>='0' && num[i]<='9') val = val*16 + num[i]-'0';
                else if (num[i]>='a' && num[i]<='f') val = val*16 + 10 + num[i]-'a';
                else if (num[i]>='A' && num[i]<='F') val = val*16 + 10 + num[i]-'A';
                else return(-1);
        }
        return(val);
}

AnsiString TDbg::Hex16(int Value)
{
        char temp[8];
        int i;
        AnsiString retval;

        itoa(Value, temp, 16);

        retval=temp;

        for(i=1; i<=retval.Length(); i++) retval[i]=toupper(retval[i]);
        while(retval.Length()<4) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Hex8(int Value)
{
        char temp[8];
        int i;
        AnsiString retval;

        itoa(Value, temp, 16);

        retval=temp;

        for(i=1; i<=retval.Length(); i++) retval[i]=toupper(retval[i]);
        while(retval.Length()<2) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Bin8(int Value)
{
        char temp[16];
        int i;
        AnsiString retval;

        itoa(Value, temp, 2);

        retval=temp;

        while(retval.Length()<8) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
void TDbg::UpdateVals(void)
{
        int i;

        HL->Caption = "$"+Hex16(z80.hl.w);
        BC->Caption = "$"+Hex16(z80.bc.w);
        DE->Caption = "$"+Hex16(z80.de.w);
        HL_->Caption = "$"+Hex16(z80.hl_.w);
        BC_->Caption = "$"+Hex16(z80.bc_.w);
        DE_->Caption = "$"+Hex16(z80.de_.w);
        IX->Caption = "$"+Hex16(z80.ix.w);
        IY->Caption = "$"+Hex16(z80.iy.w);
        PC->Caption = "$"+Hex16(z80.pc.w);
        SP->Caption = "$"+Hex16(z80.sp.w);
        IR->Caption = "$"+Hex16((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
        A->Caption = "$"+Hex8(z80.af.b.h);
        A_->Caption = "$"+Hex8(z80.af_.b.h);
        F->Caption = Bin8(z80.af.b.l);
        F_->Caption = Bin8(z80.af_.b.l);

        i=z80.sp.w;
        Stack0->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack1->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack2->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack3->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack4->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack5->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack6->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;
        Stack7->Caption = "$"+Hex16( memory[i] + 256 * memory[i+1] ); i+=2;

        i=z80.pc.w;
        Disass0->Caption = Disassemble(&i);
        StepOverAddr=i;
        Disass1->Caption = Disassemble(&i);
        Disass2->Caption = Disassemble(&i);
        Disass3->Caption = Disassemble(&i);
        Disass4->Caption = Disassemble(&i);
        Disass5->Caption = Disassemble(&i);
        Disass6->Caption = Disassemble(&i);
        Disass7->Caption = Disassemble(&i);

        Halt->Caption = z80.halted ? "Yes":"No" ;
        Interupts->Caption = z80.iff1 ? "Enabled":"Disabled" ;
        IM->Caption = z80.im;

        RowCount->Caption = rowcounter;
        Scanline->Caption = RasterY;
        ShiftReg->Caption = Bin8(shift_store);

        NMIGen->Caption = NMI_generator ? "On":"Off";
        HSYNCGen->Caption = HSYNC_generator ? "On":"Off";

        if (zx81_stop)
        {
                RunStop->Caption = "Run";
                SingleStep->Enabled = true;
                StepOver->Enabled = true;
                EnableVals();
                DelTempBreakPoints();
        }
        else
        {
                RunStop->Caption = "Stop";
                SingleStep->Enabled = false;
                StepOver->Enabled = false;
                if (!Continuous->Checked) DisableVals();
        }

}
//---------------------------------------------------------------------------
void TDbg::DisableVals(void)
{
        HL->Enabled = false;
        BC->Enabled = false;
        DE->Enabled = false;
        HL_->Enabled = false;
        BC_->Enabled = false;
        DE_->Enabled = false;
        IX->Enabled = false;
        IY->Enabled = false;
        PC->Enabled = false;
        SP->Enabled = false;
        IR->Enabled = false;
        A->Enabled = false;
        A_->Enabled = false;
        F->Enabled = false;
        F_->Enabled = false;

        Stack0->Enabled = false;
        Stack1->Enabled = false;
        Stack2->Enabled = false;
        Stack3->Enabled = false;
        Stack4->Enabled = false;
        Stack5->Enabled = false;
        Stack6->Enabled = false;
        Stack7->Enabled = false;

        Disass0->Enabled = false;
        Disass1->Enabled = false;
        Disass2->Enabled = false;
        Disass3->Enabled = false;
        Disass4->Enabled = false;
        Disass5->Enabled = false;
        Disass6->Enabled = false;
        Disass7->Enabled = false;


        Halt->Enabled = false;
        Interupts->Enabled = false;
        IM->Enabled = false;

        RowCount->Enabled = false;
        Scanline->Enabled = false;
        ShiftReg->Enabled = false;

        NMIGen->Enabled = false;
        HSYNCGen->Enabled = false;
}
void TDbg::EnableVals(void)
{
        HL->Enabled = true;
        BC->Enabled = true;
        DE->Enabled = true;
        HL_->Enabled = true;
        BC_->Enabled = true;
        DE_->Enabled = true;
        IX->Enabled = true;
        IY->Enabled = true;
        PC->Enabled = true;
        SP->Enabled = true;
        IR->Enabled = true;
        A->Enabled = true;
        A_->Enabled = true;
        F->Enabled = true;
        F_->Enabled = true;

        Stack0->Enabled = true;
        Stack1->Enabled = true;
        Stack2->Enabled = true;
        Stack3->Enabled = true;
        Stack4->Enabled = true;
        Stack5->Enabled = true;
        Stack6->Enabled = true;
        Stack7->Enabled = true;

        Disass0->Enabled = true;
        Disass1->Enabled = true;
        Disass2->Enabled = true;
        Disass3->Enabled = true;
        Disass4->Enabled = true;
        Disass5->Enabled = true;
        Disass6->Enabled = true;
        Disass7->Enabled = true;


        Halt->Enabled = true;
        Interupts->Enabled = true;
        IM->Enabled = true;

        RowCount->Enabled = true;
        Scanline->Enabled = true;
        ShiftReg->Enabled = true;

        NMIGen->Enabled = true;
        HSYNCGen->Enabled = true;
}
//---------------------------------------------------------------------------
__fastcall TDbg::TDbg(TComponent* Owner)
        : TForm(Owner)
{
        DoNext=false;
        Breakpoints=0;
        NMIRetAddr=INTRetAddr=-1;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::RunStopClick(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormClose(TObject *Sender, TCloseAction &Action)
{
        zx81.single_step=0;
        Form1->DebugWin->Checked=false;
        zx81_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormShow(TObject *Sender)
{
        if (Continuous->Checked==true) zx81.single_step=1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ContinuousClick(TObject *Sender)
{
        if (Continuous->Checked==true)
        {
                zx81.single_step=1;
                EnableVals();
        }
        else
        {
                zx81.single_step=0;
                DisableVals();
        }

        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SingleStepClick(TObject *Sender)
{
        zx81_stop=0;
        zx81.single_step=1;
        DoNext=true;
}
//---------------------------------------------------------------------------
#include "DbgDissassem.cpp"


void __fastcall TDbg::StepOverClick(TObject *Sender)
{
        AddBreakPoint(StepOverAddr,false);
        //zx81_stop=false;
        RunStopClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::AddBrkBtnClick(TObject *Sender)
{
        int Addr=-1;
        AnsiString text;

        NewBreakForm->ShowModal();
        if (NewBreakForm->Value->Text!="")
        {
                text=NewBreakForm->Value->Text;

                if (text[1] == '$')
                {
                        text = text.SubString(2, text.Length()-1);
                        Addr = Hex2Dec(text);
                }
                else    Addr = atoi(text.c_str());

                if (Addr!=-1) AddBreakPoint(Addr, true);
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DelBrkBtnClick(TObject *Sender)
{
        if (BPList->Row < (BPList->RowCount-1))
                DelBreakPoint(BPList->Row + 100000);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::SkipNMIBtnClick(TObject *Sender)
{
        if (!SkipNMIBtn->Checked) NMIRetAddr=-1;        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SkipINTBtnClick(TObject *Sender)
{
        if (!SkipINTBtn->Checked) INTRetAddr=-1;        
}
//---------------------------------------------------------------------------

