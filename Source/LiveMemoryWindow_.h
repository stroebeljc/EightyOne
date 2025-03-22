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
        TMenuItem *Zoom1;
        TMenuItem *ROM1;
        TMenuItem *N816K1;
        TMenuItem *N1632K1;
        TMenuItem *N848K1;
        TMenuItem *WholeMemory1;
        TMenuItem *N032;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall Reset1Click(TObject *Sender);
        void __fastcall Reads1Click(TObject *Sender);
        void __fastcall Writes1Click(TObject *Sender);
        void __fastcall Touches1Click(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall ROM1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        void __fastcall WMEraseBkgnd(TWMEraseBkgnd &Message);

        RGBQUAD* _pbits;
        HBITMAP _hdib;
        BYTE _writes[65536];
        BYTE _reads[65536];
        int _count;
        int _memStart, _memEnd;

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
