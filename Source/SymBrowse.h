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
        void __fastcall ListBox1DblClick(TObject *Sender);
private:	// User declarations
        AnsiString _searchString;
        DWORD _lastTick;

public:		// User declarations
        __fastcall TSymbolBrowser(TComponent* Owner);

        void __fastcall RefreshContent(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TSymbolBrowser *SymbolBrowser;
//---------------------------------------------------------------------------
#endif
