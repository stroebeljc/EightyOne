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
        TImage *Image1;
private:	// User declarations
public:		// User declarations
        __fastcall TLiveMemoryWindow(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------
#endif
