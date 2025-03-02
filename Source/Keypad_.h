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

#ifndef Keypad_H
#define Keypad_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TKeypad128 : public TForm
{
__published:	// IDE-managed Components
        TImage *KeypadUk;
        TImage *KeypadSpanish;
        TTimer *Tracker;
        void __fastcall TrackerTimer(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall KeypadDblClick(TObject *Sender);
private:	// User declarations
        int keyboardLeft;
        int keyboardTop;
public:		// User declarations
        __fastcall TKeypad128(TComponent* Owner);
        void __fastcall EnableTimer(bool enable);
        void SelectUKLayout();
        void SelectSpanishLayout();
};
//---------------------------------------------------------------------------
extern PACKAGE TKeypad128 *Keypad128;
//---------------------------------------------------------------------------
#endif
