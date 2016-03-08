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
 * Debug.h
 *
 */

//---------------------------------------------------------------------------

#ifndef DebugH
#define DebugH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <IniFiles.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

enum
{
        BP_EXE, BP_RD, BP_WR, BP_IN, BP_OUT
};

struct breakpoint
{
        int Addr;
        bool Permanent;

        // one of BP_EXE, BP_RD, BP_WR, BP_IN, BP_OUT
        int Type;
};

enum IODirection
{
        IO_NONE,
        IO_OUT,
        IO_IN
};

struct LastIOAccess
{
        IODirection direction;
        int address;
        BYTE data;
};

extern void DebugUpdate(void);
extern void ResetLastIOAccesses();
extern void LogInAccess(int address, BYTE data);
extern void LogOutAccess(int address, BYTE data);

class TDbg : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *HL;
        TLabel *BC;
        TLabel *DE;
        TLabel *A;
        TLabel *F;
        TLabel *Label6;
        TLabel *HL_;
        TLabel *BC_;
        TLabel *DE_;
        TLabel *A_;
        TLabel *F_;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *IX;
        TLabel *Label18;
        TLabel *IY;
        TLabel *Label20;
        TLabel *IR;
        TLabel *Label22;
        TLabel *SP;
        TLabel *Label24;
        TLabel *PC;
        TLabel *Label7;
        TLabel *IM;
        TLabel *Label9;
        TLabel *Interrupts;
        TLabel *Label11;
        TLabel *Halt;
        TGroupBox *GroupBox2;
        TLabel *Disass0;
        TLabel *Disass1;
        TLabel *Disass2;
        TLabel *Disass3;
        TLabel *Disass4;
        TLabel *Disass5;
        TLabel *Disass6;
        TLabel *Disass7;
        TLabel *Disass8;
        TLabel *Disass9;
        TGroupBox *GroupBoxZX81;
        TLabel *Label39;
        TLabel *Label10;
        TLabel *Label19;
        TLabel *Label21;
        TLabel *Label23;
        TLabel *NMIGen;
        TLabel *Scanline;
        TLabel *RowCount;
        TLabel *ShiftReg;
        TGroupBox *GroupBox4;
        TLabel *HSYNCGen;
        TGroupBox *GroupBox5;
        TLabel *Stack0;
        TLabel *Stack1;
        TLabel *Stack2;
        TLabel *Stack3;
        TLabel *Stack4;
        TLabel *Stack5;
        TLabel *Stack6;
        TButton *AddrBrkBtn;
        TButton *DelBrkBtn;
        TStringGrid *BPList;
        TLabel *Label27;
        TLabel *TStates;
        TGroupBox *GroupBoxAce;
        TLabel *AceStk0;
        TLabel *AceStk1;
        TLabel *AceStk2;
        TLabel *AceStk3;
        TLabel *AceStk4;
        TLabel *AceStk5;
        TLabel *AceStk6;
        TLabel *AceStk7;
        TLabel *Label37;
        TLabel *Label38;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TLabel *Label43;
        TLabel *Label44;
        TLabel *Label45;
        TLabel *AceStkVal0;
        TLabel *AceStkVal1;
        TLabel *AceStkVal2;
        TLabel *AceStkVal3;
        TLabel *AceStkVal4;
        TLabel *AceStkVal5;
        TLabel *AceStkVal6;
        TLabel *AceStkVal7;
        TLabel *Label28;
        TLabel *Label29;
        TPopupMenu *MemDumpPopup;
        TMenuItem *MemDumpFromHere1;
        TLabel *Label8;
        TMenuItem *AddBreak1;
        TMenuItem *OnExecute1;
        TMenuItem *OnRead1;
        TMenuItem *OnWrite1;
        TButton *WriteBrkBtn;
        TButton *ReadBrkBtn;
        TGroupBox *Execute;
        TButton *SingleStep;
        TButton *StepOver;
        TButton *RunStop;
        TCheckBox *Continuous;
        TCheckBox *SkipNMIBtn;
        TCheckBox *SkipINTBtn;
        TGroupBox *GroupBox3;
        TCheckBox *EnableHistory;
        TButton *History;
        TGroupBox *IOPorts;
        TButton *Memory;
        TButton *Symbols;
        TLabel *SymRom;
        TLabel *SymApp;
        TButton *OutBrkBtn;
        TButton *InBrkBtn;
        TLabel *IOPort0Address;
        TLabel *IOPort2Address;
        TLabel *IOPort1Address;
        TLabel *IOPort3Address;
        TLabel *IOPort0Data;
        TLabel *IOPort0Direction;
        TLabel *IOPort2Data;
        TLabel *IOPort1Data;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *IOPort3Data;
        TLabel *IOPort3Direction;
        TLabel *IOPort1Direction;
        TLabel *IOPort2Direction;
        TGroupBox *GroupBoxChroma;
        TLabel *ChromaColourModeLabel;
        TLabel *ChromaColourMode;
        TGroupBox *GroupBoxZXC;
        TLabel *ZXCModeLabel;
        TLabel *ZXCMode;
        TGroupBox *GroupBoxSpectra;
        TLabel *SpectraModeLabel;
        TLabel *SpectraMode;
        TLabel *Label17;
        TLabel *Label25;
        TLabel *Label26;
        TLabel *Label30;
        void __fastcall RunStopClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ContinuousClick(TObject *Sender);
        void __fastcall SingleStepClick(TObject *Sender);
        void __fastcall StepOverClick(TObject *Sender);
        void __fastcall DelBrkBtnClick(TObject *Sender);
        void __fastcall SkipNMIBtnClick(TObject *Sender);
        void __fastcall SkipINTBtnClick(TObject *Sender);
        void __fastcall HistoryClick(TObject *Sender);
        void __fastcall NMIGenClick(TObject *Sender);
        void __fastcall HSYNCGenClick(TObject *Sender);
        void __fastcall HLClick(TObject *Sender);
        void __fastcall BCClick(TObject *Sender);
        void __fastcall DEClick(TObject *Sender);
        void __fastcall HL_Click(TObject *Sender);
        void __fastcall DE_Click(TObject *Sender);
        void __fastcall BC_Click(TObject *Sender);
        void __fastcall IXClick(TObject *Sender);
        void __fastcall IYClick(TObject *Sender);
        void __fastcall SPClick(TObject *Sender);
        void __fastcall PCClick(TObject *Sender);
        void __fastcall AClick(TObject *Sender);
        void __fastcall A_Click(TObject *Sender);
        void __fastcall InterruptsClick(TObject *Sender);
        void __fastcall IMClick(TObject *Sender);
        void __fastcall Stack0Click(TObject *Sender);
        void __fastcall IRClick(TObject *Sender);
        void __fastcall FClick(TObject *Sender);
        void __fastcall F_Click(TObject *Sender);
        void __fastcall BCMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall DEMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall HLMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall BC_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall DE_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall HL_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall IXMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall IYMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall SPMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall PCMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall MemDumpFromHere1Click(TObject *Sender);
        void __fastcall Stack0MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall IRMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall AceStk0Click(TObject *Sender);
        void __fastcall AceStkVal0Click(TObject *Sender);
        void __fastcall AddrBrkBtnClick(TObject *Sender);
        void __fastcall SymbolsClick(TObject *Sender);
        void __fastcall AddBreak1Click(TObject *Sender);
        void __fastcall MemoryClick(TObject *Sender);
        void __fastcall WriteBrkBtnClick(TObject *Sender);
        void __fastcall ReadBrkBtnClick(TObject *Sender);
        void __fastcall OutBrkBtnClick(TObject *Sender);
        void __fastcall InBrkBtnClick(TObject *Sender);
private:	// User declarations
        void EnableValues(bool enable);
        void EnableVals(void);
        void DisableVals(void);
        void SetMenuContent(int memloc);
        AnsiString StrRep(AnsiString Text, int Pos, int Len, AnsiString NewText);

        struct breakpoint Breakpoint[99];
        int Breakpoints;
        bool BPHit(int Addr, int Type, int& idx);
        void DelTempBreakPoints(void);

        // default values are evil. never ever use them >:)
        void SetLabelInfo(TLabel* label, int value, int valueWidth = 4);

        int StepOverAddr;
        void __fastcall DoEditStack(int);
        void __fastcall DoEditReg(WORD&);
        void __fastcall DoEditReg(BYTE&);


public:		// User declarations
        __fastcall TDbg(TComponent* Owner);
        int NMIRetAddr, INTRetAddr;
        void UpdateVals(void);
        bool DoNext;

        bool ExecBreakPointHit(int Addr);
        bool MemoryReadHit(int Addr);
        bool MemoryWriteHit(int Addr);
        bool PortInHit(int Addr);
        bool PortOutHit(int Addr);

        bool AddBreakPoint(int Addr, bool Perm, int type);
        void DelBreakPoint(int Addr);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        AnsiString Disassemble(int *Ad);
        AnsiString Hex16(int value);
        AnsiString Hex8(int value);
        AnsiString Bin8(int Value);
        int Hex2Dec(AnsiString num);

        LastIOAccess lastIOAccess[4];
        int lastPortInAddr, lastPortOutAddr;     
};
//---------------------------------------------------------------------------
extern PACKAGE TDbg *Dbg;
//---------------------------------------------------------------------------
#endif
