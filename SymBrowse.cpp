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
        ListBox1->Height=ClientHeight;
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

        Dbg->MemDumpPopup->Tag = Dbg->MemDumpFromHere1->Tag;
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::ShowMemory1Click(TObject *Sender)
{
        MemoryWindow->BaseAddress = Dbg->MemDumpFromHere1->Tag;
        MemoryWindow->Show();
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::FormShow(TObject *Sender)
{
        RefreshContent();
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::ListBox1KeyPress(TObject *Sender,
      char &Key)
{
        int startidx = ListBox1->ItemIndex + 1;

        for (int i = 0; i < ListBox1->Items->Count; ++i)
        {
                int idx = (startidx + i) % ListBox1->Items->Count;
                AnsiString s = ListBox1->Items->Strings[idx];
                char x = s[5];
                if (x == (char)Key)
                {
                       ListBox1->ItemIndex = idx;
                       return;
                }
        }
}
//---------------------------------------------------------------------------

