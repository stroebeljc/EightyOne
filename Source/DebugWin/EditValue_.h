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

#ifndef EditValue_H
#define EditValue_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditValue : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditVal;
        TButton *OK;
        TButton *Cancel;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall EditValChange(TObject *Sender);
private:	// User declarations
        bool cancelled;
        bool Translate(AnsiString str, int& value);
        AnsiString IntToBin(int Value, int digits);
        AnsiString IntToDec(int value);
        int maxValue;
public:		// User declarations
        __fastcall TEditValue(TComponent* Owner);
        bool Edit2(int& value, int len, char format = '$');
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditValue *EditValue;
//---------------------------------------------------------------------------
#endif
