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
 * zx97Config.h
 *
 */

//---------------------------------------------------------------------------

#ifndef zx97ConfigH
#define zx97ConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <IniFiles.hpp>
#include "zx81config.h"
#include "Utils.h"
//---------------------------------------------------------------------------
class TZX97Dialog : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *Protect08;
        TLabel *Label1;
        TCheckBox *ProtectAB;
        TCheckBox *ProtectB0;
        TCheckBox *ProtectB115;
        TCheckBox *SaveRAM;
        TButton *OK;
        TCheckBox *SwapRAMROM;
        TButton *Cancel;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CancelClick(TObject *Sender);
private:	// User declarations
        void SaveToInternalSettings();
        void LoadFromInternalSettings();
        void AccessIniFile(TIniFile* ini, IniFileAccessType accessType);
        ZX97 Zx97form;
public:		// User declarations
        __fastcall TZX97Dialog(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void UpdateRequired(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TZX97Dialog *ZX97Dialog;
//---------------------------------------------------------------------------
#endif
