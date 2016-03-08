//---------------------------------------------------------------------------

#ifndef wavload_H
#define wavload_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>

#include "WavFile.h"
//---------------------------------------------------------------------------
class TWavLoad : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBar;
        TButton *Load;
        TOpenDialog *LoadWavDialog;
        TImage *Image1;
        TLabel *Status;
        TButton *PlayBtn;
        TButton *SaveBtn;
        TButton *OKBtn;
        TCheckBox *AutoBtn;
        TRadioButton *LeftBtn;
        TRadioButton *RightBtn;
        TLabel *StereoTxt1;
        TLabel *StereoTxt2;
        void __fastcall LoadClick(TObject *Sender);
        void __fastcall ScrollBarChange(TObject *Sender);
        void __fastcall PlayBtnClick(TObject *Sender);
        void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall OKBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall LeftBtnClick(TObject *Sender);
        void __fastcall RightBtnClick(TObject *Sender);
private:	// User declarations
        int TStatesSample;
        int TapePos;
        bool Playing;
        int Threshold;
        int CurrentTStates, ScreenCounter;
        AnsiString StatusText;
        int Channel;
public:		// User declarations
        __fastcall TWavLoad(TComponent* Owner);
        void ClearImage();
        void UpdateImage();
        bool GetEarState();
        void ClockTick(int TStates);
        void Stop();
        void Start();
        void DoCaption(AnsiString Message);
};
//---------------------------------------------------------------------------
extern PACKAGE TWavLoad *WavLoad;
//---------------------------------------------------------------------------
#endif
