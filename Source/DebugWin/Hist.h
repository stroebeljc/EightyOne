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

//---------------------------------------------------------------------------

#ifndef HistH
#define HistH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "Debug.h"
//---------------------------------------------------------------------------
class THistoryBox : public TForm
{
__published:	// IDE-managed Components
        TRichEdit *Text;
        TStatusBar *StatusBar1;
        TToolBar *ToolBar1;
        TToolButton *ToolButtonReload;
        TToolButton *ToolButtonClear;
        TToolButton *ToolButton4;
        TToolButton *ToolButton3;
        TToolButton *ReverseToolButtonReverse;
        TToolButton *ToolButton2;
        TToolButton *ToolButton0K8K;
        TToolButton *ToolButton5;
        TToolButton *ToolButton8K16K;
        TToolButton *ToolButton7;
        TToolButton *ToolButton16K32K;
        TToolButton *ToolButton6;
        TToolButton *ToolButton32K48K;
        TToolButton *ToolButton9;
        TToolButton *ToolButton48K64K;
        TToolButton *ToolButton11;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ToolButtonReloadClick(TObject *Sender);
        void __fastcall ToolButtonClearClick(TObject *Sender);
        void __fastcall ReverseToolButtonReverseClick(TObject *Sender);
        void __fastcall ToolButton0K8KClick(TObject *Sender);
        void __fastcall ToolButton8K16KClick(TObject *Sender);
        void __fastcall ToolButton16K32KClick(TObject *Sender);
        void __fastcall ToolButton32K48KClick(TObject *Sender);
        void __fastcall ToolButton48K64KClick(TObject *Sender);
private:	// User declarations
        bool reverseOrder;
        bool show0K8KAddresses;
        bool show8K16KAddresses;
        bool show16K32KAddresses;
        bool show32K48KAddresses;
        bool show48K64KAddresses;
public:		// User declarations
        __fastcall THistoryBox(TComponent* Owner);
        bool __fastcall ReverseOrder();
        bool __fastcall Show0K8KAddresses();
        bool __fastcall Show8K16KAddresses();
        bool __fastcall Show16K32KAddresses();
        bool __fastcall Show32K48KAddresses();
        bool __fastcall Show48K64KAddresses();
};
//---------------------------------------------------------------------------
extern PACKAGE THistoryBox *HistoryBox;
//---------------------------------------------------------------------------
#endif
