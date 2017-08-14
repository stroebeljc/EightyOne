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
private:	// User declarations
        void __fastcall WMEraseBkgnd(TWMEraseBkgnd &Message);
        void __fastcall WMDraw(TMessage &Msg);

        Graphics::TBitmap *pBitmap;
        int _o;
public:		// User declarations
        __fastcall TLiveMemoryWindow(TComponent* Owner);
        void __fastcall Write(int address);
};
//---------------------------------------------------------------------------
extern PACKAGE TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------
#endif
