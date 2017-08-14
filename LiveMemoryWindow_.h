//---------------------------------------------------------------------------

#ifndef LiveMemoryWindow_H
#define LiveMemoryWindow_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TLiveMemoryWindow : public TForm
{
__published:	// IDE-managed Components
        void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
        RGBQUAD* _pbits;
        HBITMAP _hdib;
        BYTE _writes[65536];
        BYTE _reads[65536];
        int _count;

public:		// User declarations
        __fastcall TLiveMemoryWindow(TComponent* Owner);
        void __fastcall Write(int address);
        void __fastcall Read(int address);
        void __fastcall Update(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------
#endif
