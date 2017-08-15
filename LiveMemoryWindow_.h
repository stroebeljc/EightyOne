//---------------------------------------------------------------------------

#ifndef LiveMemoryWindow_H
#define LiveMemoryWindow_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TLiveMemoryWindow : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *Re1;
        TMenuItem *Reset1;
        TMenuItem *View1;
        TMenuItem *Reads1;
        TMenuItem *Writes1;
        TMenuItem *Touches1;
        TStatusBar *StatusBar1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall Reset1Click(TObject *Sender);
        void __fastcall Reads1Click(TObject *Sender);
        void __fastcall Writes1Click(TObject *Sender);
        void __fastcall Touches1Click(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
private:	// User declarations
        void __fastcall WMEraseBkgnd(TWMEraseBkgnd &Message);

        RGBQUAD* _pbits;
        HBITMAP _hdib;
        BYTE _writes[65536];
        BYTE _reads[65536];
        int _count;

public:		// User declarations
        __fastcall TLiveMemoryWindow(TComponent* Owner);
        void __fastcall Reset();
        void __fastcall Write(unsigned short address);
        void __fastcall Read(unsigned short address);
        void __fastcall Update(void);

BEGIN_MESSAGE_MAP
   MESSAGE_HANDLER(WM_ERASEBKGND,TWMEraseBkgnd,WMEraseBkgnd)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------
#endif
