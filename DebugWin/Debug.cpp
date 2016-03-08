/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Debug.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Debug.h"
#include "zx81.h"
#include "z80.h"
#include "zx81config.h"
#include "hist.h"

#include "main_.h"
#include "EditValue_.h"
#include "EditFlags.h"
#include "memoryWindow.h"
#include "symbolstore.h"
#include "SymBrowse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDbg *Dbg;

// TODO
//
// ability to disable the breakpoint w/o deleting it
// re-calculate breakpoints if symbols change???

extern int frametstates, rowcounter, RasterY, HSYNC_generator, NMI_generator;
extern unsigned char shift_store;

extern unsigned char memory[];
extern int lastMemoryReadAddr, lastMemoryWriteAddr;

const int historySize = 2000;
AnsiString HistoryLog[historySize];
int HistoryPos=0;

int recentHistory[4];
int recentHistoryPos=0;

void DebugUpdate(void)
{
        static int NMISaveSingleStep=-1, INTSaveSingleStep=-1;
        static int lastpc;
        int i;

        i=z80.pc.w;
        if (lastpc!=z80.pc.w)
        {
                recentHistory[recentHistoryPos] = i;
                recentHistoryPos = (recentHistoryPos + 1) & 3;

                if (Dbg->EnableHistory->Checked)
                {
                        HistoryLog[HistoryPos++] = Dbg->Disassemble(&i);
                        if (HistoryPos==historySize) HistoryPos=0;
                }

                lastpc=z80.pc.w;
        }

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
                Dbg->NMIRetAddr = getbyte(z80.sp.w) + 256*getbyte(z80.sp.w + 1);
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
                Dbg->INTRetAddr = getbyte(z80.sp.w) + 256*getbyte(z80.sp.w + 1);
                INTSaveSingleStep = zx81.single_step;
                zx81.single_step=0;
        }
        if (z80.pc.w == Dbg->INTRetAddr)
        {
                Dbg->INTRetAddr=-1;
                zx81.single_step=INTSaveSingleStep;
        }
        if (Dbg->INTRetAddr!=-1) return;

        int lmr = lastMemoryReadAddr;
        lastMemoryReadAddr = -1;
        int lmw = lastMemoryWriteAddr;
        lastMemoryWriteAddr = -1;
        int lpi = Dbg->lastPortInAddr;
        Dbg->lastPortInAddr = -1;
        int lpo = Dbg->lastPortOutAddr;
        Dbg->lastPortOutAddr = -1;

        if (Dbg->ExecBreakPointHit(z80.pc.w) ||
                Dbg->MemoryReadHit(lmr) ||
                Dbg->MemoryWriteHit(lmw) ||
                Dbg->PortInHit(lpi) ||
                Dbg->PortOutHit(lpo))
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

        Dbg->Symbols->Enabled = symbolstore::romLoaded() || symbolstore::fileLoaded();
        Dbg->SymRom->Enabled = symbolstore::romLoaded();
        Dbg->SymApp->Enabled = symbolstore::fileLoaded();
}
//---------------------------------------------------------------------------
bool TDbg::AddBreakPoint(int Addr, bool Perm, int Type)
{
        // type 0 = execute
        // type 1 = mem read
        // type 2 = mem write
        const AnsiString types("xrwio");
        const int maxBreakpoints = 99;

        if (Breakpoints == maxBreakpoints)
                return false;

        int i;
        for(i=0; i<Breakpoints; i++)
        {
                if (Breakpoint[i].Addr == Addr && Breakpoint[i].Type == Type)
                {
                        // already exists
                        return false;
                }
        }

        Breakpoint[Breakpoints].Addr=Addr;
        Breakpoint[Breakpoints].Permanent=Perm;
        Breakpoint[Breakpoints].Type=Type;
        AnsiString t(types[Type + 1]);
        AnsiString str;
        if (t == "x")
        {
                str = t + " " + symbolstore::addressToSymbolOrHex(Addr);
        }
        else
        {
                str = t + " $" + Hex16(Addr);
        }

        if (Perm)
        {
                BPList->Cells[0][Breakpoints]=str;
        }
        else
        {
                BPList->Cells[0][Breakpoints]="("+str+")";
        }

        Breakpoints++;
        BPList->RowCount++;

        return true;
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

bool TDbg::BPHit(int Addr, int Type, int& idx)
{
        for (idx = 0; idx < Breakpoints; ++idx)
        {
                if (Breakpoint[idx].Addr == Addr && Breakpoint[idx].Type == Type)
                {
                        BPList->Row=idx;
                        return true;
                }
        }

        return false;
}

bool TDbg::ExecBreakPointHit(int Addr)
{
        int idx;
        bool hit = BPHit(Addr, BP_EXE, idx);
        if (hit && !Breakpoint[idx].Permanent)
        {
                DelBreakPoint(Addr);
        }

        return hit;
}

bool TDbg::MemoryReadHit(int Addr)
{
        int idx;
        return BPHit(Addr, BP_RD, idx);
}

bool TDbg::MemoryWriteHit(int Addr)
{
        int idx;
        return BPHit(Addr, BP_WR, idx);
}

bool TDbg::PortInHit(int Addr)
{
        int idx;
        return BPHit(Addr, BP_IN, idx);
}

bool TDbg::PortOutHit(int Addr)
{
        int idx;
        return BPHit(Addr, BP_OUT, idx);
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
        int i;

        for(i=1; i<=num.Length(); i++)
        {
                if (num[i]>='0' && num[i]<='9') val = val*16 + num[i]-'0';
                else if (num[i]>='a' && num[i]<='f') val = val*16 + 10 + num[i]-'a';
                else if (num[i]>='A' && num[i]<='F') val = val*16 + 10 + num[i]-'A';
                else return(val);
        }
        return(val);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Hex16(int Value)
{
        return AnsiString::IntToHex(Value,4);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Hex8(int Value)
{
        return AnsiString::IntToHex(Value,2);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Bin8(int Value)
{
        char arry[9] = {0};
        for (int i = 7; i >= 0; --i)
        {
                arry[i] = (Value & 1) + '0';
                Value >>= 1;
        }

        return AnsiString(arry);
}
//---------------------------------------------------------------------------

// set the caption and other good stuff for a register label
//
void TDbg::SetLabelInfo(TLabel* label, int value, int valueWidth)
{
        // the tag is used for the editvalue function
        label->Tag = value;

        // the caption is fairly standard
        label->Caption = "$"+AnsiString::IntToHex(label->Tag, valueWidth);
        if (valueWidth == 4)
        {
                // ahaa!
                AnsiString hint("");

                // enable showHint if there is a symbol for the given 16bit value
                label->ShowHint = symbolstore::addressToSymbol(label->Tag, hint);
                label->Hint = hint;

                // lowlight the label if there's a hint to show
                //
                label->Color = label->ShowHint ? clInfoBk : clBtnFace;
        }
}

void TDbg::UpdateVals(void)
{
        int i, Stack;

        switch(zx81.machine)
        {
        case MACHINEACE:
                Stack=getbyte(0x3C3B)+256*getbyte(0x3C3C) -2;

                AceStk0->Caption = "$"+Hex16(Stack); AceStkVal0->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk1->Caption = "$"+Hex16(Stack); AceStkVal1->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk2->Caption = "$"+Hex16(Stack); AceStkVal2->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk3->Caption = "$"+Hex16(Stack); AceStkVal3->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk4->Caption = "$"+Hex16(Stack); AceStkVal4->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk5->Caption = "$"+Hex16(Stack); AceStkVal5->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk6->Caption = "$"+Hex16(Stack); AceStkVal6->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk7->Caption = "$"+Hex16(Stack); AceStkVal7->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                break;

        case MACHINESPEC48:
                GroupBoxSpectra->Enabled = zx81.spectraColourSwitchOn;
                SpectraModeLabel->Enabled = zx81.spectraColourSwitchOn;
                SpectraMode->Enabled = zx81.spectraColourSwitchOn;
                SpectraMode->Caption = "$"+Hex8(zx81.spectraMode);
                break;

        default:
                RowCount->Caption = rowcounter;
                Scanline->Caption = RasterY;
                ShiftReg->Caption = Bin8(shift_store);
                TStates->Caption = frametstates;
                NMIGen->Caption = NMI_generator ? "On":"Off";
                HSYNCGen->Caption = HSYNC_generator ? "On":"Off";

                GroupBoxChroma->Enabled = zx81.chromaColourSwitchOn;
                ChromaColourModeLabel->Enabled = zx81.chromaColourSwitchOn;
                ChromaColourMode->Enabled = zx81.chromaColourSwitchOn;
                ChromaColourMode->Caption = "$"+Hex8(zx81.chromaMode);
                break;
        }

        switch (zx81.romCartridge)
        {
                case ROMCARTRIDGEZXC2:
                        GroupBoxZXC->Caption = "ZXC2";
                        break;
                case ROMCARTRIDGEZXC3:
                        GroupBoxZXC->Caption = "ZXC3";
                        break;
                case ROMCARTRIDGEZXC4:
                        GroupBoxZXC->Caption = "ZXC4";
                        break;
                default:
                        GroupBoxZXC->Caption = "ZXC";
                break;
        }

        bool zxcEnabled = (zx81.romCartridge != ROMCARTRIDGENONE) && (zx81.romCartridge != ROMCARTRIDGESINCLAIR);
        ZXCModeLabel->Enabled = zxcEnabled;
        ZXCMode->Enabled = zxcEnabled;
        ZXCMode->Caption = "$"+Hex8(zx81.zxcPaging);         

        SetLabelInfo(HL, z80.hl.w);
        SetLabelInfo(BC, z80.bc.w);
        SetLabelInfo(DE, z80.de.w);
        SetLabelInfo(HL_, z80.hl_.w);
        SetLabelInfo(BC_, z80.bc_.w);
        SetLabelInfo(DE_, z80.de_.w);
        SetLabelInfo(IX, z80.ix.w);
        SetLabelInfo(IY, z80.iy.w);
        SetLabelInfo(PC, z80.pc.w);
        SetLabelInfo(SP, z80.sp.w);
        SetLabelInfo(IR, (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));
        SetLabelInfo(A, z80.af.b.h, 2);
        SetLabelInfo(A_, z80.af_.b.h, 2);
        F->Caption = Bin8(z80.af.b.l);
        F_->Caption = Bin8(z80.af_.b.l);

        i=z80.sp.w;
        Stack0->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack1->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack2->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack3->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack4->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack5->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack6->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;

        i = recentHistory[(recentHistoryPos+0)&3];
        Disass0->Caption = Disassemble(&i);
        i = recentHistory[(recentHistoryPos+1)&3];
        Disass1->Caption = Disassemble(&i);
        i = recentHistory[(recentHistoryPos+2)&3];
        Disass2->Caption = Disassemble(&i);
        i=z80.pc.w;
        Disass3->Caption = (Disassemble(&i) + "                ").SetLength(50);
        StepOverAddr=i;
        Disass4->Caption = Disassemble(&i);
        Disass5->Caption = Disassemble(&i);
        Disass6->Caption = Disassemble(&i);
        Disass7->Caption = Disassemble(&i);
        Disass8->Caption = Disassemble(&i);
        Disass9->Caption = Disassemble(&i);

        Halt->Caption = z80.halted ? "Yes":"No" ;
        Interrupts->Caption = z80.iff1 ? "Enabled":"Disabled" ;
        IM->Caption = z80.im;

        MemoryWindow->UpdateChanges();

        if (lastIOAccess[0].direction != IO_NONE)
        {
                if (lastIOAccess[0].direction == IO_IN) IOPort0Direction->Caption = "IN"; else IOPort0Direction->Caption = "OUT";
                IOPort0Address->Caption = "$"+AnsiString::IntToHex(lastIOAccess[0].address, 4);
                IOPort0Data->Caption = "$"+Hex8(lastIOAccess[0].data);
        }
        else
        {
                IOPort0Direction->Caption = "-";
                IOPort0Address->Caption = "-";
                IOPort0Data->Caption = "-";
        }

        if (lastIOAccess[1].direction != IO_NONE)
        {
                if (lastIOAccess[1].direction == IO_IN) IOPort1Direction->Caption = "IN"; else IOPort1Direction->Caption = "OUT";
                IOPort1Address->Caption = "$"+AnsiString::IntToHex(lastIOAccess[1].address, 4);
                IOPort1Data->Caption = "$"+Hex8(lastIOAccess[1].data);
        }
        else
        {
                IOPort1Direction->Caption = "-";
                IOPort1Address->Caption = "-";
                IOPort1Data->Caption = "-";
        }

        if (lastIOAccess[2].direction != IO_NONE)
        {
                if (lastIOAccess[2].direction == IO_IN) IOPort2Direction->Caption = "IN"; else IOPort2Direction->Caption = "OUT";
                IOPort2Address->Caption = "$"+AnsiString::IntToHex(lastIOAccess[2].address, 4);
                IOPort2Data->Caption = "$"+Hex8(lastIOAccess[2].data);
        }
        else
        {
                IOPort2Direction->Caption = "-";
                IOPort2Address->Caption = "-";
                IOPort2Data->Caption = "-";
        }

        if (lastIOAccess[3].direction != IO_NONE)
        {
                if (lastIOAccess[3].direction == IO_IN) IOPort3Direction->Caption = "IN"; else IOPort3Direction->Caption = "OUT";
                IOPort3Address->Caption = "$"+AnsiString::IntToHex(lastIOAccess[3].address, 4);
                IOPort3Data->Caption = "$"+Hex8(lastIOAccess[3].data);
        }
        else
        {
                IOPort3Direction->Caption = "-";
                IOPort3Address->Caption = "-";
                IOPort3Data->Caption = "-";
        }

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
void TDbg::EnableValues(bool enable)
{
        HL->Enabled = enable;
        BC->Enabled = enable;
        DE->Enabled = enable;
        HL_->Enabled = enable;
        BC_->Enabled = enable;
        DE_->Enabled = enable;
        IX->Enabled = enable;
        IY->Enabled = enable;
        PC->Enabled = enable;
        SP->Enabled = enable;
        IR->Enabled = enable;
        A->Enabled = enable;
        A_->Enabled = enable;
        F->Enabled = enable;
        F_->Enabled = enable;

        Stack0->Enabled = enable;
        Stack1->Enabled = enable;
        Stack2->Enabled = enable;
        Stack3->Enabled = enable;
        Stack4->Enabled = enable;
        Stack5->Enabled = enable;
        Stack6->Enabled = enable;

        Disass0->Enabled = enable;
        Disass1->Enabled = enable;
        Disass2->Enabled = enable;
        Disass3->Enabled = enable;
        Disass4->Enabled = enable;
        Disass5->Enabled = enable;
        Disass6->Enabled = enable;
        Disass7->Enabled = enable;
        Disass8->Enabled = enable;
        Disass9->Enabled = enable;

        Halt->Enabled = enable;
        Interrupts->Enabled = enable;
        IM->Enabled = enable;

        RowCount->Enabled = enable;
        Scanline->Enabled = enable;
        ShiftReg->Enabled = enable;

        NMIGen->Enabled = enable;
        HSYNCGen->Enabled = enable;

        AceStk0->Enabled=enable; AceStkVal0->Enabled=enable;
        AceStk1->Enabled=enable; AceStkVal1->Enabled=enable;
        AceStk2->Enabled=enable; AceStkVal2->Enabled=enable;
        AceStk3->Enabled=enable; AceStkVal3->Enabled=enable;
        AceStk4->Enabled=enable; AceStkVal4->Enabled=enable;
        AceStk5->Enabled=enable; AceStkVal5->Enabled=enable;
        AceStk6->Enabled=enable; AceStkVal6->Enabled=enable;
        AceStk7->Enabled=enable; AceStkVal7->Enabled=enable;
}

void TDbg::DisableVals(void)
{
        EnableValues(false);
}

void TDbg::EnableVals(void)
{
        EnableValues(true);
}
//---------------------------------------------------------------------------
__fastcall TDbg::TDbg(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DoNext=false;
        Breakpoints=0;
        NMIRetAddr=INTRetAddr=-1;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        ClientHeight=GroupBox2->Top+GroupBox2->Height+4;
        ClientWidth=GroupBoxZX81->Left+GroupBoxZX81->Width+4;

        BPList->DefaultColWidth = BPList->Width;

        ResetLastIOAccesses();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::RunStopClick(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        if(!zx81_stop)
        {
                MemoryWindow->ClearChanges();
        }
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
        MemoryWindow->ClearChanges();
        zx81_stop=0;
        zx81.single_step=1;
        DoNext=true;
}
//---------------------------------------------------------------------------
#include "DbgDissassem.cpp"


void __fastcall TDbg::StepOverClick(TObject *Sender)
{
        MemoryWindow->ClearChanges();
        AddBreakPoint(StepOverAddr,false,BP_EXE);
        //zx81_stop=false;
        RunStopClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::AddrBrkBtnClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int Addr = 0;
        if (EditValue->Edit2(Addr, 2))
        {
                AddBreakPoint(Addr, true, BP_EXE);
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
        if (!SkipNMIBtn->Checked)
                NMIRetAddr=-1;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SkipINTBtnClick(TObject *Sender)
{
        if (!SkipINTBtn->Checked)
                INTRetAddr=-1;
}
//---------------------------------------------------------------------------

void TDbg::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("DEBUG","Top",Top);
        Left = ini->ReadInteger("DEBUG","Left",Left);

        if (Form1->DebugWin->Checked)
        {
                //Show();
                Form1->DebugWin->Checked=false;
        }
}

void TDbg::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("DEBUG","Top",Top);
        ini->WriteInteger("DEBUG","Left",Left);
}


void __fastcall TDbg::HistoryClick(TObject *Sender)
{
        int i;

        if (HistoryBox->Visible)
        {
                HistoryBox->Close();
                return;
        }

        HistoryBox->Text->Clear();

        i=HistoryPos-1;
        if (i==-1) i=historySize-1;
        while(i!=HistoryPos)
        {
                if (HistoryLog[i]!="") HistoryBox->Text->Lines->Add(HistoryLog[i]);
                --i;
                if (i==-1) i=historySize-1;
        }
        HistoryBox->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::NMIGenClick(TObject *Sender)
{
        NMI_generator = !NMI_generator;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HSYNCGenClick(TObject *Sender)
{
        HSYNC_generator = !HSYNC_generator;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DoEditReg(WORD& value)
{
        EditValue->CentreOn(this);

        int n = value;
        if (EditValue->Edit2(n, 2))
        {
                value = n;
                UpdateVals();
        }
}

void __fastcall TDbg::DoEditReg(BYTE& value)
{
        EditValue->CentreOn(this);

        int n = value;
        if (EditValue->Edit2(n, 1))
        {
                value = n;
                UpdateVals();
        }
}

//---------------------------------------------------------------------------

void __fastcall TDbg::HLClick(TObject *Sender)
{
        DoEditReg(z80.hl.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BCClick(TObject *Sender)
{
        DoEditReg(z80.bc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DEClick(TObject *Sender)
{
        DoEditReg(z80.de.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HL_Click(TObject *Sender)
{
        DoEditReg(z80.hl_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DE_Click(TObject *Sender)
{
        DoEditReg(z80.de_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BC_Click(TObject *Sender)
{
        DoEditReg(z80.bc_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IXClick(TObject *Sender)
{
        DoEditReg(z80.ix.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IYClick(TObject *Sender)
{
        DoEditReg(z80.iy.w);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::SPClick(TObject *Sender)
{
        DoEditReg(z80.sp.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::PCClick(TObject *Sender)
{
        DoEditReg(z80.pc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AClick(TObject *Sender)
{
        DoEditReg(z80.af.b.h);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::A_Click(TObject *Sender)
{
        DoEditReg(z80.af_.b.h);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::InterruptsClick(TObject *Sender)
{
        // DI - IFF1=IFF2=0
        // EI - IFF1=IFF2=1;

        z80.iff1=z80.iff2= !z80.iff1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IMClick(TObject *Sender)
{
        if (++z80.im == 3) z80.im=0;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DoEditStack(int offs)
{
        EditValue->CentreOn(this);

        int i = 2 * offs + z80.sp.w;
        int v = getbyte(i)+256*getbyte(i+1);
        if (EditValue->Edit2(v, 2))
        {
                setbyte(i, v&255);
                setbyte(i+1, v>>8);
                UpdateVals();
        }
}

void __fastcall TDbg::Stack0Click(TObject *Sender)
{
        TLabel* label = (TLabel*)Sender;
        int len = label->Name.Length();
        int idx = label->Name.SubString(len, 1).ToInt();
        DoEditStack(idx);
}
//---------------------------------------------------------------------------

// dereferences the stack values. requires the stack
// window value labels to be labelled with a number according to the
// index on the stack. (SP-0) = 0, (SP-2) = 1, etc.
//
void __fastcall TDbg::Stack0MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        TLabel* label = (TLabel*)Sender;
        int len = label->Name.Length();
        int idx = label->Name.SubString(len, 1).ToInt();

        int i = z80.sp.w;
        i += 2 * idx;
        SetMenuContent(getbyte(i)+256*getbyte(i+1));
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IRClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int v;
        v = (z80.i << 8) | (z80.r7 & 128) | z80.r;
        if (EditValue->Edit2(v, 2))
        {
                z80.r = v&127;
                z80.r7 = v&128;
                z80.i = (v>>8)&255;
                UpdateVals();
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FClick(TObject *Sender)
{
        // this relies on the textbox that displays the flag bits
        // being exactly 8 char_width pixels wide.
        //
        TPoint cp = ScreenToClient(Mouse->CursorPos);
        cp.x -= F->Left + 2;
        cp.y -= F->Top;
        int f = cp.x / (F->Width / 8);

        int bit = 0x80 >> f;
        z80.af.b.l ^= bit;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::F_Click(TObject *Sender)
{
        // see above.
        //
        TPoint cp = ScreenToClient(Mouse->CursorPos);
        cp.x -= F_->Left + 2;
        cp.y -= F_->Top;
        int f = (cp.x) / (F_->Width / 8);

        int bit = 0x80 >> f;
        z80.af_.b.l ^= bit;
        UpdateVals();
}
//---------------------------------------------------------------------------


void TDbg::SetMenuContent(int memloc)
{
     MemDumpFromHere1->Tag = memloc;
     AddBreak1->Tag = memloc;
}
//---------------------------------------------------------------------------


void __fastcall TDbg::BCMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.bc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DEMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.de.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HLMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.hl.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BC_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.bc_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DE_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.de_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HL_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.hl_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IXMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.ix.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IYMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.iy.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IRMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        int ir = (z80.i << 8) | (z80.r7 & 128) | (z80.r & 127);
        SetMenuContent(ir);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SPMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.sp.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::PCMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.pc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::MemDumpFromHere1Click(TObject *Sender)
{
        MemoryWindow->Show();
        MemoryWindow->BaseAddress = MemDumpFromHere1->Tag;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AddBreak1Click(TObject *Sender)
{
        TMenuItem* mi = (TMenuItem*)Sender;
        AddBreakPoint(AddBreak1->Tag, true, mi->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AceStk0Click(TObject *Sender)
{
//        fred;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AceStkVal0Click(TObject *Sender)
{
//        bert
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SymbolsClick(TObject *Sender)
{
        SymbolBrowser->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::MemoryClick(TObject *Sender)
{
        MemoryWindow->BaseAddress = z80.pc.w;
        MemoryWindow->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::WriteBrkBtnClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int Addr = 0;
        if (EditValue->Edit2(Addr, 2))
        {
                AddBreakPoint(Addr, true, BP_WR);
        }                                      
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ReadBrkBtnClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int Addr = 0;
        if (EditValue->Edit2(Addr, 2))
        {
                AddBreakPoint(Addr, true, BP_RD);
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::OutBrkBtnClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int Addr = 0;
        if (EditValue->Edit2(Addr, 2))
        {
                AddBreakPoint(Addr, true, BP_OUT);
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::InBrkBtnClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int Addr = 0;
        if (EditValue->Edit2(Addr, 2))
        {
                AddBreakPoint(Addr, true, BP_IN);
        }
}
//---------------------------------------------------------------------------

void ResetLastIOAccesses()
{
        for (int i = 0; i < 4; i++)
        {
                Dbg->lastIOAccess[i].direction = IO_NONE;
        }
}
//---------------------------------------------------------------------------

void LogInAccess(int address, BYTE data)
{
        Dbg->lastPortInAddr = address;

        for (int i = 2; i >= 0; i--)
        {
                  Dbg->lastIOAccess[i+1] = Dbg->lastIOAccess[i];
        }

        Dbg->lastIOAccess[0].direction = IO_IN;
        Dbg->lastIOAccess[0].address = address;
        Dbg->lastIOAccess[0].data = data;
}
//---------------------------------------------------------------------------

void LogOutAccess(int address, BYTE data)
{
        Dbg->lastPortOutAddr = address;

        for (int i = 2; i >= 0; i--)
        {
                  Dbg->lastIOAccess[i+1] = Dbg->lastIOAccess[i];
        }

        Dbg->lastIOAccess[0].direction = IO_OUT;
        Dbg->lastIOAccess[0].address = address;
        Dbg->lastIOAccess[0].data = data;
}
//---------------------------------------------------------------------------

