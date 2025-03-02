/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <vcl4.h>
#pragma hdrstop

#include <Clipbrd.hpp>

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
                sym.SetLength(28);
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
                AnsiString sym(ListBox1->Items->Strings[item].SubString(5,24));
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
        int isPrintable = isprint(Key);

        long timeSinceLastPress = GetTickCount() - _lastTick;
        if (timeSinceLastPress > 500 || !isPrintable) {
                _searchString = "";
        }
        _lastTick = GetTickCount();

        if (!isPrintable) {
                if (0x03 == Key) {
                        // ctrl-c ... copy string to clipboard
                        AnsiString x = ListBox1->Items->Strings[ListBox1->ItemIndex].SubString(5,24).Trim();
                        Clipboard()->SetTextBuf((wchar_t *)WideString(x.c_str()));
                }
                return;
        }

        _searchString += Key;
        int startidx = ListBox1->ItemIndex + 1;

        for (int i = 0; i < ListBox1->Items->Count; ++i)
        {
                int idx = (startidx + i) % ListBox1->Items->Count;
                AnsiString x = ListBox1->Items->Strings[idx].SubString(5,_searchString.Length());
                if (x.AnsiCompareIC(_searchString) == 0)
                {
                       ListBox1->ItemIndex = idx;
                       return;
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TSymbolBrowser::ListBox1DblClick(TObject *Sender)
{
        int addr, item = ListBox1->ItemIndex;
        AnsiString sym(ListBox1->Items->Strings[item].SubString(5,24));
        symbolstore::symbolToAddress(sym.Trim(), addr);
        breakpoint bp(addr, BP_EXE);
        Dbg->AddBreakPoint(bp);
}
//---------------------------------------------------------------------------

