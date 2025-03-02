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

#ifndef SearchSequence_H
#define SearchSequence_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <vector>

//---------------------------------------------------------------------------
class TSearchSequence : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditSequence;
        TButton *Cancel;
        TButton *Find;
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall FindClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TSearchSequence(TComponent* Owner);
        void CentreOn(TForm* parent);
        bool EditSequenceBytes(AnsiString& searchText, std::vector<int>& bytes);
        bool ValidateSearchSequence(std::vector<int>& bytes);
};
//---------------------------------------------------------------------------
extern PACKAGE TSearchSequence *SearchSequence;
//---------------------------------------------------------------------------
#endif
