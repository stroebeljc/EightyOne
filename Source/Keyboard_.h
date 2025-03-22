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

#ifndef Keyboard_H
#define Keyboard_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TKeyboard : public TForm
{
__published:	// IDE-managed Components
        TImage *zx81kb;
        TImage *zx80kb;
        TImage *acekb;
        TImage *ts1500kb;
        TImage *lambdakb;
        TImage *tk85kb;
        TImage *zx97kb;
        TImage *r470kb;
        TImage *ts1000kb;
        TImage *spec48kb;
        TImage *spec128kb;
        TImage *spec128kbSpanish;
        TImage *ts2068kb;
        TImage *tc2048kb;
        TImage *spec16kb;
        TImage *zx81zxpandkb;
        TImage *zx80zxpandkb;
        TImage *specPlus2kb;
        TImage *specPlus3kb;
        TImage *ts1000zxpandkb;
        TImage *specPlus2kbSpanish;
        TImage *specPlus3kbSpanish;
        TImage *spec48kbNordic;
        TImage *ts1500zxpandkb;
        TImage *spec128kbArabic;
        TImage *specPlus2kbArabic;
        TImage *specPlus3kbArabic;
        TImage *tc2068kbTimeWord;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall KeyboardDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TKeyboard(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void KbChange(void);

        static const int FunctionsOffset = 20;
        static const int KeypadOffset = 20;
};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboard *Keyboard;
//---------------------------------------------------------------------------
#endif
