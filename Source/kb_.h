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
 * kb_.h
 *
 */

//---------------------------------------------------------------------------

#ifndef kb_H
#define kb_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TKb : public TForm
{
__published:	// IDE-managed Components
        TLabel *CursorKeysLabel;
        TComboBox *CursorMode;
        TButton *OK;
        TEdit *CustomUp;
        TEdit *CustomDown;
        TEdit *CustomLeft;
        TEdit *CustomRight;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TLabel *CtrlKeyMapsToLabel;
        TCheckBox *UseRightShiftCheckBox;
        TCheckBox *UseNumericPadForJoystickCheckBox;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CursorModeChange(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall UseNumericPadForJoystickCheckBoxClick(
          TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TKb(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void UpdateCursors();
};
//---------------------------------------------------------------------------
extern PACKAGE TKb *Kb;
//---------------------------------------------------------------------------
#endif
