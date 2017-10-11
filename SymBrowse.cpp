//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SymBrowse.h"
#include "symbolstore.h"
#include "MemoryWindow.h"
#include "Debug.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSymbolBrowser *SymbolBrowser;
//---------------------------------------------------------------------------
__fastcall TSymbolBrowser::TSymbolBrowser(TComponent* Owner)
        : TForm(Owner)
{
        ListBox1->Width=ClientWidth;
        ListBox1->Height=ClientHeight-StatusBar1->Height;
}
//---------------------------------------------------------------------------


void __fastcall TSymbolBrowser::RefreshContent(void)
{
        ListBox1->Items->Clear();

        symbolstore::beginenumeration();
        AnsiString sym;
        char type;
        int val;
        while(symbolstore::enumerate(sym, val, type))
        {
                sym = "    " + sym + "                       ";
                sym.SetLength(24);
                sym[1] = type;
                sym += IntToHex(val, 4);
                ListBox1->Items->Add(sym);
        }
}

//---------------------------------------------------------------------------



void __fastcall TSymbolBrowser::ListBox1ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
        int item = ListBox1->TopIndex + MousePos.y / ListBox1->ItemHeight;

        Dbg->MemDumpFromHere1->Tag = 0;

        if (item < ListBox1->Items->Count)
        {
                ListBox1->ItemIndex=item;
                AnsiString sym(ListBox1->Items->Strings[item].SubString(5,20));
                symbolstore::symbolToAddress(sym.Trim(), Dbg->MemDumpFromHere1->Tag);
        }

        Dbg->AddBreak1->Tag = Dbg->MemDumpFromHere1->Tag;
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::ShowMemory1Click(TObject *Sender)
{
        MemoryWindow->BaseAddress = Dbg->MemDumpFromHere1->Tag;
        MemoryWindow->Show();
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::AddBreakpoint1Click(TObject *Sender)
{
        TMenuItem* mi = (TMenuItem*)Sender;
        Dbg->AddBreakPoint(Dbg->AddBreak1->Tag, true, mi->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::FormShow(TObject *Sender)
{
        RefreshContent();
}
//---------------------------------------------------------------------------

