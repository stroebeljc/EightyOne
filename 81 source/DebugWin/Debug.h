//---------------------------------------------------------------------------

#ifndef DebugH
#define DebugH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

struct breakpoint
{
        int Addr;
        bool Permanent;
};

extern "C" void DebugUpdate(void);

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
        TLabel *Interupts;
        TLabel *Label11;
        TLabel *Halt;
        TGroupBox *GroupBox2;
        TLabel *Label8;
        TLabel *Disass0;
        TLabel *Disass1;
        TLabel *Disass2;
        TLabel *Disass3;
        TLabel *Disass4;
        TLabel *Disass5;
        TLabel *Disass6;
        TLabel *Disass7;
        TGroupBox *GroupBox3;
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
        TButton *RunStop;
        TButton *SingleStep;
        TButton *StepOver;
        TCheckBox *Continuous;
        TLabel *HSYNCGen;
        TGroupBox *GroupBox5;
        TLabel *Stack0;
        TLabel *Stack1;
        TLabel *Stack2;
        TLabel *Stack3;
        TLabel *Stack4;
        TLabel *Stack5;
        TLabel *Stack6;
        TLabel *Stack7;
        TLabel *Label17;
        TLabel *Label25;
        TButton *AddBrkBtn;
        TButton *DelBrkBtn;
        TStringGrid *BPList;
        TLabel *Label26;
        TCheckBox *SkipNMIBtn;
        TCheckBox *SkipINTBtn;
        void __fastcall RunStopClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ContinuousClick(TObject *Sender);
        void __fastcall SingleStepClick(TObject *Sender);
        void __fastcall StepOverClick(TObject *Sender);
        void __fastcall AddBrkBtnClick(TObject *Sender);
        void __fastcall DelBrkBtnClick(TObject *Sender);
        void __fastcall SkipNMIBtnClick(TObject *Sender);
        void __fastcall SkipINTBtnClick(TObject *Sender);
private:	// User declarations
        void EnableVals(void);
        void DisableVals(void);
        AnsiString Hex16(int value);
        AnsiString Hex8(int value);
        AnsiString Bin8(int Value);
        int Hex2Dec(AnsiString num);
        AnsiString Disassemble(int *Ad);
        AnsiString StrRep(AnsiString Text, int Pos, int Len, AnsiString NewText);
        struct breakpoint Breakpoint[99];
        int Breakpoints;
        int StepOverAddr;
public:		// User declarations
        __fastcall TDbg(TComponent* Owner);
        int NMIRetAddr, INTRetAddr;
        void UpdateVals(void);
        bool DoNext;
        bool BreakPointHit(int Addr);
        void AddBreakPoint(int Addr, bool Perm);
        void DelBreakPoint(int Addr);
        void DelTempBreakPoints(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TDbg *Dbg;
//---------------------------------------------------------------------------
#endif
