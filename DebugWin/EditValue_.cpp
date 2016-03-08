/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
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
 *
 *
 * EditValue.cpp
 */
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditValue_.h"
#include "symbolstore.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditValue *EditValue;
//---------------------------------------------------------------------------
__fastcall TEditValue::TEditValue(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditValue::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TEditValue::OKClick(TObject *Sender)
{
        Close();
}

void TEditValue::CentreOn(TForm* parent)
{
        Top = parent->Top + (parent->Height - Height) /2;
        Left = parent->Left + (parent->Width - Width) /2;
}


// returns true or false depending on whether the gathering of a value was
// succesful. The data is both passed in and returned in value. This is
// unchanged in the case when gathering fails.
//
bool TEditValue::Edit2(int& value,int len)
{
        AnsiString NewVal;

        cancelled=false;
        EditVal->Text = "$" + IntToHex(value, len*2);

        ActiveControl = EditVal;
        EditVal->SelectAll();
        ShowModal();

        AnsiString result = EditVal->Text.Trim();

        if (cancelled || result.Length() == 0)
        {
                return false;
        }

        int addr;
        if (symbolstore::symbolToAddress(result, addr))
        {
                result = "0x" + IntToHex(addr, 4);
        }

        // replace the zx-world hex identifier with the standard c library one.
        //
        if (result[1] == '$')
        {
                result = "0x" + result.SubString(2, result.Length() - 1);
        }


        // which allows us to use a ninja converter.
        // suddenly 0xABCD, $ABCD, 01234 [octal] and decimal are all valid.
        //
        char* endPtr;
        int tval = int(strtol(result.c_str(), &endPtr, 0));
        if (*endPtr != 0)
        {
                // and we know when the user entered junk...
                return false;
        }

        value = tval;
        return true;
}

void __fastcall TEditValue::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key == VK_ESCAPE)
        {
                cancelled = true;
                Close();
        }
}
//---------------------------------------------------------------------------

