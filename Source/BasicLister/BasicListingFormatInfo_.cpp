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

#include "BasicListingFormatInfo_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBasicListingFormatInfoForm *BasicListingFormatInfoForm;
//---------------------------------------------------------------------------
__fastcall TBasicListingFormatInfoForm::TBasicListingFormatInfoForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TBasicListingFormatInfoForm::SetActivePage(int index)
{
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink1Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink2Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink3Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------
void TBasicListingFormatInfoForm::OpenWebsite()
{
        ShellExecute(NULL, "open", "http://freestuff.grok.co.uk/zxtext2p/index.html", NULL, NULL, SW_SHOW);
}
