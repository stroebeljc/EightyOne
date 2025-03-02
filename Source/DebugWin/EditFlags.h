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

#ifndef EditFlagsH
#define EditFlagsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditFlag : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *F7;
        TCheckBox *F6;
        TCheckBox *F5;
        TCheckBox *F4;
        TCheckBox *F3;
        TCheckBox *F2;
        TCheckBox *F1;
        TCheckBox *F0;
        TButton *OK;
        TButton *Cancel;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label4;
        TLabel *Label6;
        TLabel *Label7;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CancelClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TEditFlag(TComponent* Owner);
        int Edit(int value);
        bool Edit2(int& value);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditFlag *EditFlag;
//---------------------------------------------------------------------------
#endif
