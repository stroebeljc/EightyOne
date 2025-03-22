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

#ifndef BasicListingFormatInfo_H
#define BasicListingFormatInfo_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TBasicListingFormatInfoForm : public TForm
{
__published:	// IDE-managed Components
        TButton *btnOK;
        TLabel *Label26;
        TLabel *Label27;
        TLabel *Label28;
        TLabel *Label25;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *Label35;
        TLabel *Label36;
        void __fastcall Weblink1Click(TObject *Sender);
        void __fastcall Weblink2Click(TObject *Sender);
        void __fastcall Weblink3Click(TObject *Sender);
private:	// User declarations
        void OpenWebsite();
public:		// User declarations
        __fastcall TBasicListingFormatInfoForm(TComponent* Owner);
        void SetActivePage(int index);
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicListingFormatInfoForm *BasicListingFormatInfoForm;
//---------------------------------------------------------------------------
#endif
