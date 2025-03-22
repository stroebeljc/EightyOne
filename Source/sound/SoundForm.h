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

#ifndef midifrmH
#define midifrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TMidiForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TComboBox *MidiBox;
        TButton *OK;
        TGroupBox *GroupBox2;
        TTrackBar *ChAVol;
        TTrackBar *ChBVol;
        TTrackBar *ChCVol;
        TTrackBar *BeeperVol;
        TLabel *Label1;
        TLabel *BeeperLabel;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TCheckBox *AYMute;
        TCheckBox *BeeperMute;
        TLabel *Label6;
        TLabel *Label7;
        TRadioButton *RadioButtonMono;
        TRadioButton *RadioButtonACB;
        TGroupBox *VideoSoundGroupBox;
        TCheckBox *BeeperExcludeHSyncs;
        TLabel *Label8;
        TCheckBox *SpeechMute;
        TTrackBar *SpeechVol;
        TTrackBar *SpecDrumVol;
        TLabel *Label9;
        TCheckBox *SpecDrumMute;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall MidiChange(TObject *Sender);
        void __fastcall BeeperExcludeHSyncsClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TMidiForm(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);
        void SetComputer(bool zx81);
};
//---------------------------------------------------------------------------
extern PACKAGE TMidiForm *MidiForm;
//---------------------------------------------------------------------------
#endif
