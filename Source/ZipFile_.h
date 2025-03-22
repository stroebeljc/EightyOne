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

#ifndef ZipFile_H
#define ZipFile_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>

//---------------------------------------------------------------------------
class TZipFile : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox;
        TButton *OK;
        TLabel *Label1;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ListBoxDblClick(TObject *Sender);
private:	// User declarations
        bool Cancelled;
        AnsiString LastFile;
public:		// User declarations
        __fastcall TZipFile(TComponent* Owner);
        AnsiString ExpandZIP(AnsiString Path, AnsiString Filter);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TZipFile *ZipFile;
//---------------------------------------------------------------------------
#endif
