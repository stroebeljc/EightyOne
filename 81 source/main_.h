//---------------------------------------------------------------------------

#ifndef main_H
#define main_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "dbits.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include "AnimTimer.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TDFlipSurface *DFlipSurface;
        TDImage *DImage1;
        TTimer *Timer1;
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *InsertTape1;
        TMenuItem *NewTape1;
        TMenuItem *CloseTape1;
        TMenuItem *N2;
        TMenuItem *PrinterSetup1;
        TMenuItem *PrintScreen1;
        TMenuItem *PrintListing1;
        TMenuItem *N3;
        TMenuItem *Exit1;
        TMenuItem *View1;
        TMenuItem *Zoom1;
        TMenuItem *N1001;
        TMenuItem *N2001;
        TMenuItem *N4001;
        TMenuItem *UserDefined1;
        TMenuItem *RenderStyle1;
        TMenuItem *Fast1;
        TMenuItem *Accurate1;
        TMenuItem *Options1;
        TMenuItem *Joystick1;
        TMenuItem *Keyboard1;
        TMenuItem *Sound1;
        TMenuItem *Display1;
        TMenuItem *Speed1;
        TMenuItem *Tape1;
        TMenuItem *Help1;
        TMenuItem *HelpTopics2;
        TMenuItem *KeyboardMap1;
        TMenuItem *N1;
        TMenuItem *AboutEightyOne1;
        TMenuItem *N4;
        TMenuItem *LoadSnapshot1;
        TMenuItem *SaveSnapshot1;
        TOpenDialog *LoadSnapDialog;
        TSaveDialog *SaveSnapDialog;
        TToolBar *ToolBar;
        TMenuItem *Toolbar1;
        TToolButton *ResetButton;
        TToolButton *InsertTapeButton;
        TToolButton *ToolButton3;
        TToolButton *CloseTapeButton;
        TImageList *ImageList1;
        TMenuItem *N5;
        TMenuItem *ResetZX811;
        TMenuItem *ViewPrinter;
        TMenuItem *Interlaced;
        TDImage *DImage2;
        TTimer *Timer2;
        TDImage *DImage3;
        TMenuItem *WavLoadBtn;
        TMenuItem *DebugWin;
        TMenuItem *Tools1;
        TMenuItem *SmoothScaling1;
        TMenuItem *PauseZX81;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall N1001Click(TObject *Sender);
        void __fastcall N2001Click(TObject *Sender);
        void __fastcall N4001Click(TObject *Sender);
        void __fastcall UserDefined1Click(TObject *Sender);
        void __fastcall Accurate1Click(TObject *Sender);
        void __fastcall Fast1Click(TObject *Sender);
        void __fastcall Speed1Click(TObject *Sender);
        void __fastcall Display1Click(TObject *Sender);
        void __fastcall Keyboard1Click(TObject *Sender);
        void __fastcall KeyboardMap1Click(TObject *Sender);
        void __fastcall AboutEightyOne1Click(TObject *Sender);
        void __fastcall InsertTape1Click(TObject *Sender);
        void __fastcall SaveSnapshot1Click(TObject *Sender);
        void __fastcall LoadSnapshot1Click(TObject *Sender);
        void __fastcall Tape1Click(TObject *Sender);
        void __fastcall CloseTape1Click(TObject *Sender);
        void __fastcall NewTape1Click(TObject *Sender);
        void __fastcall Toolbar1Click(TObject *Sender);
        void __fastcall ResetButtonClick(TObject *Sender);
        void __fastcall InsertTapeButtonClick(TObject *Sender);
        void __fastcall CloseTapeButtonClick(TObject *Sender);
        void __fastcall ResetZX811Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ViewPrinterClick(TObject *Sender);
        void __fastcall InterlacedClick(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall WavLoadBtnClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall DebugWinClick(TObject *Sender);
        void __fastcall PauseZX81Click(TObject *Sender);
private:	// User declarations
        int fps;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        int BaseWidth;
        int BaseHeight;
        bool FullScreen;
        int SaveX, SaveY, SaveW, SaveH;
        void __fastcall AppMessage(TMsg &Msg, bool &Handled);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
