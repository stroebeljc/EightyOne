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

#include "EditFlags.h"
#include "debug.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditFlag *EditFlag;
//---------------------------------------------------------------------------
__fastcall TEditFlag::TEditFlag(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditFlag::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TEditFlag::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TEditFlag::Edit(int value)
{
        int val;

        Top = Dbg->Top + (Dbg->Height - Height) /2;
        Left = Dbg->Left + (Dbg->Width - Width) /2;

        cancelled=false;

        F7->Checked = value&128;
        F6->Checked = value&64;
        F5->Checked = value&32;
        F4->Checked = value&16;
        F3->Checked = value&8;
        F2->Checked = value&4;
        F1->Checked = value&2;
        F0->Checked = value&1;

        ShowModal();
        if (cancelled) return(value);

        val=0;
        if (F7->Checked) val |= 128;
        if (F6->Checked) val |= 64;
        if (F5->Checked) val |= 32;
        if (F4->Checked) val |= 16;
        if (F3->Checked) val |= 8;
        if (F2->Checked) val |= 4;
        if (F1->Checked) val |= 2;
        if (F0->Checked) val |= 1;

        return(val);
}


bool TEditFlag::Edit2(int& value)
{
        int val;

        Top = Dbg->Top + (Dbg->Height - Height) /2;
        Left = Dbg->Left + (Dbg->Width - Width) /2;

        cancelled=false;

        F7->Checked = value&128;
        F6->Checked = value&64;
        F5->Checked = value&32;
        F4->Checked = value&16;
        F3->Checked = value&8;
        F2->Checked = value&4;
        F1->Checked = value&2;
        F0->Checked = value&1;

        ShowModal();
        if (cancelled) return(value);

        val=0;
        if (F7->Checked) val |= 128;
        if (F6->Checked) val |= 64;
        if (F5->Checked) val |= 32;
        if (F4->Checked) val |= 16;
        if (F3->Checked) val |= 8;
        if (F2->Checked) val |= 4;
        if (F1->Checked) val |= 2;
        if (F0->Checked) val |= 1;

        return(val);
}
