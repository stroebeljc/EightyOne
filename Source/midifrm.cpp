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
#include <windows.h>
#include <mmsystem.h>
#pragma hdrstop

#include "midifrm.h"
#include "midi.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMidiForm *MidiForm;

extern unsigned int VolumeLevel[4];

//---------------------------------------------------------------------------
__fastcall TMidiForm::TMidiForm(TComponent* Owner)
        : TForm(Owner)
{
        int i,Devices;
        MIDIOUTCAPS moc;
        TIniFile *ini;

        Devices=midiOutGetNumDevs();

        while(MidiBox->Items->Count) MidiBox->Items->Delete(0);

        MidiBox->Items->Add("Nothing");
        MidiBox->Items->Add("Midi Mapper");

        for(i=0;i<Devices;i++)
        {
                if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
                        MidiBox->Items->Add(moc.szPname);
        }

        MidiDev=-1;
        MidiBox->ItemIndex=1;
        MidiStart();

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::FormDestroy(TObject *Sender)
{
        MidiStop();
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::MidiChange(TObject *Sender)
{
        static int LastMidiItem=0;

        VolumeLevel[0] = AYMute->Checked ? 0:ChAVol->Max - ChAVol->Position;
        VolumeLevel[1] = AYMute->Checked ? 0:ChBVol->Max - ChBVol->Position;
        VolumeLevel[2] = AYMute->Checked ? 0:ChCVol->Max - ChCVol->Position;
        VolumeLevel[3] = BeeperMute->Checked ? 0:BeeperVol->Max - BeeperVol->Position;

        if (LastMidiItem!=MidiBox->ItemIndex)
        {
                MidiStop();
                MidiDev = MidiBox->ItemIndex -2;
                MidiStart();
                LastMidiItem=MidiBox->ItemIndex;
        }
}
//---------------------------------------------------------------------------
void TMidiForm::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("MIDI","Top",Top);
        ini->WriteInteger("MIDI","Left",Left);
        ini->WriteInteger("MIDI","ChAVol",ChAVol->Position);
        ini->WriteInteger("MIDI","ChBVol",ChBVol->Position);
        ini->WriteInteger("MIDI","ChCVol",ChCVol->Position);
        ini->WriteInteger("MIDI","BeeperVol",BeeperVol->Position);
        ini->WriteInteger("MIDI","MidiDev",MidiBox->ItemIndex);
        ini->WriteBool("MIDI","AYMute",AYMute->Checked);
        ini->WriteBool("MIDI","BeeperMute",BeeperMute->Checked);
}

void TMidiForm::LoadSettings(TIniFile *ini)
{
        Top=ini->ReadInteger("MIDI","Top",Top);
        Left=ini->ReadInteger("MIDI","Left",Left);
        ChAVol->Position=ini->ReadInteger("MIDI","ChAVol",ChAVol->Position);
        ChBVol->Position=ini->ReadInteger("MIDI","ChBVol",ChBVol->Position);
        ChCVol->Position=ini->ReadInteger("MIDI","ChCVol",ChCVol->Position);
        BeeperVol->Position=ini->ReadInteger("MIDI","BeeperVol",BeeperVol->Position);
        MidiBox->ItemIndex=ini->ReadInteger("MIDI","MidiDev",MidiBox->ItemIndex);
        AYMute->Checked=ini->ReadBool("MIDI","AYMute",AYMute->Checked);
        BeeperMute->Checked=ini->ReadBool("MIDI","BeeperMute",BeeperMute->Checked);
        MidiChange(NULL);
}

