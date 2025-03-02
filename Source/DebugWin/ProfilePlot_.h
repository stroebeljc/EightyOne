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

#ifndef ProfilePlot_H
#define ProfilePlot_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TProfilePlot : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBarHorizontal;
        TStatusBar *StatusBar1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ScrollBarHorizontalChange(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        class ProfileDetail* _pd;
        void InitScrollbar();
        void UpdateMinMax(ProfileDetail* pd);
        int _mouseX;
        
public:		// User declarations
        __fastcall TProfilePlot(TComponent* Owner);
        void __fastcall PlotTGraph(class ProfileDetail* pd, AnsiString caption);
        void ShowLast();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfilePlot *ProfilePlot;
//---------------------------------------------------------------------------
#endif
