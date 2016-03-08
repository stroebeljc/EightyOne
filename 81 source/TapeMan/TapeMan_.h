//---------------------------------------------------------------------------

#ifndef TapeMan_H
#define TapeMan_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>

#include <stdio.h>
//---------------------------------------------------------------------------
#define BYTE unsigned char
#define MAXFILES 99
#define HEADER_ID "EO81"

class TTape : public TForm
{
__published:	// IDE-managed Components
        TButton *DeleteBlock;
        TButton *ExtractBlock;
        TButton *SaveTapeBtn;
        TButton *InsertBlock;
        TButton *OK;
        TButton *LoadTapeBtn;
        THeaderControl *HeaderControl1;
        TStringGrid *StringGrid;
        TOpenDialog *LoadTapeDialog;
        TSaveDialog *SaveTapeDialog;
        TOpenDialog *InsertBlockDialog;
        TSaveDialog *ExtractBlockDialog;
        TCheckBox *AutoRewind;
        TButton *NewTapeBtn;
        TButton *BlockUp;
        TButton *BlockDown;
        void __fastcall InsertBlockClick(TObject *Sender);
        void __fastcall StringGridDblClick(TObject *Sender);
        void __fastcall LoadTapeBtnClick(TObject *Sender);
        void __fastcall ExtractBlockClick(TObject *Sender);
        void __fastcall SaveTapeBtnClick(TObject *Sender);
        void __fastcall DeleteBlockClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall NewTapeBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall BlockUpClick(TObject *Sender);
        void __fastcall BlockDownClick(TObject *Sender);
private:	// User declarations
        void __fastcall WMGetMinMaxInfo(TWMGetMinMaxInfo &Message);
        int LoadBlock(AnsiString FileName);
        int SaveBlock(AnsiString filename,FILE * fp, int Block);
        BYTE *NewFile;
        long NewFileLen;
        void RecalcCurPos();
        AnsiString FileName, BlockName;
        int CurrentPosition;
        BYTE *File[MAXFILES];
        bool Changed;
        AnsiString RemoveExt(AnsiString Fname);
        AnsiString GetExt(AnsiString Fname);

        BEGIN_MESSAGE_MAP
          MESSAGE_HANDLER(WM_GETMINMAXINFO, TWMGetMinMaxInfo, WMGetMinMaxInfo)
        END_MESSAGE_MAP(TForm)


public:		// User declarations
        int SaveTapeFile(AnsiString FileName);
        int LoadTapeFile(AnsiString FileName);
        void Rewind(void);
        void NextFile(void);
        BYTE *GetCurrentFile(void);
        int GetCurrentFileLen(void);
        bool FindFile(AnsiString Name);
        void CloseTape(void);
        void NewTape(void);
        void AddBlock(AnsiString Name, BYTE *ptr, int len);
        bool IsTape(AnsiString FileName);
        __fastcall TTape(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTape *Tape;
//---------------------------------------------------------------------------
#endif
