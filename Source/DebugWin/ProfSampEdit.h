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

#ifndef ProfSampEditH
#define ProfSampEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TProfileSampleEdit : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *EditTag;
        TEdit *EditStart;
        TEdit *EditEnd;
        TButton *ButtonOK;
        TLabel *Label4;
        void __fastcall ButtonOKClick(TObject *Sender);
        void __fastcall EditDblClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall EditStartChange(TObject *Sender);
        void __fastcall EditEndChange(TObject *Sender);
private:	// User declarations
        bool _valid;
        class ProfileDetail* _pd;
        void (*_completion)(bool);
        void SetOKButtonStatus();
        int Count();

public:		// User declarations
        __fastcall TProfileSampleEdit(TComponent* Owner);

        void __fastcall EditValues(ProfileDetail* pd, void(*)(bool));
};
//---------------------------------------------------------------------------
extern PACKAGE TProfileSampleEdit *ProfileSampleEdit;
//---------------------------------------------------------------------------
#endif
