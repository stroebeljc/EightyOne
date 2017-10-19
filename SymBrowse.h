//---------------------------------------------------------------------------

#ifndef SymBrowseH
#define SymBrowseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TSymbolBrowser : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox1;
        void __fastcall ListBox1ContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall ShowMemory1Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ListBox1KeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
        __fastcall TSymbolBrowser(TComponent* Owner);

        void __fastcall RefreshContent(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TSymbolBrowser *SymbolBrowser;
//---------------------------------------------------------------------------
#endif
