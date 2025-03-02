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

#ifndef FullScreenH
#define FullScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>

struct FullScr
{
        int Width, Height, Bpp;
        bool Stretch;
        bool WhiteLetterbox;
};

extern struct FullScr FScreen;

//---------------------------------------------------------------------------
class TFSSettings : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TComboBox *ModeList;
        TRadioButton *Letterbox;
        TRadioButton *Stretch;
        TCheckBox *White;
        TLabel *Label2;
        TRadioButton *GDIBtn;
        TRadioButton *DDrawBtn;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ModeListChange(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall GDIBtnClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFSSettings(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);

};
//---------------------------------------------------------------------------
extern PACKAGE TFSSettings *FSSettings;
//---------------------------------------------------------------------------
#endif
