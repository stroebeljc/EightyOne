//---------------------------------------------------------------------------

#ifndef zxprinterH
#define zxprinterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TPrinter : public TForm
{
__published:	// IDE-managed Components
        TImage *DisplayImage;
        TScrollBar *ScrollBar;
        TButton *SaveImage;
        TButton *ClearImage;
        TSaveDialog *SaveDialog;
        TButton *OptionsBtn;
        void __fastcall DrawImage(void);
        void __fastcall ScrollBarChange(TObject *Sender);
        void __fastcall ClearImageClick(TObject *Sender);
        void __fastcall SaveImageClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall OptionsBtnClick(TObject *Sender);
private:	// User declarations
        int StaticWidth;
        void ScrollOutput(void);
        void OutputBit(void);
        void OutputLine(void);
        void ResetPrinter(void);
        void ClearBitmap(Graphics::TBitmap *Bitmap);
        Graphics::TBitmap *PrinterOutput;
        bool MotorOn, StylusActive, OnPaper, EncoderWheel;
        int XPos, YPos, Counter1, Counter2;
public:		// User declarations
        __fastcall TPrinter(TComponent* Owner);
        void WritePort(unsigned char Data);
        unsigned char ReadPort(void);
        void ClockTick(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TPrinter *Printer;
//---------------------------------------------------------------------------
#endif
