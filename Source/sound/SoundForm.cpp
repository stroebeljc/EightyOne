//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <mmsystem.h>
#pragma hdrstop

#include "sound\sound.h"
#include "sound\midi.h"
#include "sound\SoundForm.h"
#include "zx81config.h"
#include "main_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMidiForm *MidiForm;


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

        MidiBox->ItemIndex=1;
        FreqBox->ItemIndex=3;
        Btn8Bit->Checked=true;
        Midi.Start();

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::OKClick(TObject *Sender)
{
        Form1->Midi1->Checked=false;
        MidiChange(NULL);
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::FormDestroy(TObject *Sender)
{
        Midi.Stop();
}
//---------------------------------------------------------------------------
void __fastcall TMidiForm::MidiChange(TObject *Sender)
{
        static int LastMidiItem=0;
        int Channels, SampleRate;

        Channels = BtnStereo ? 2:1;

        switch (FreqBox->ItemIndex)
        {
        case 0: SampleRate = 11025; break;
        case 1: SampleRate = 16000; break;
        case 2: SampleRate = 22050; break;
        case 3: SampleRate = 32000; break;
        case 4: SampleRate = 44100; break;
        case 5: SampleRate = 48000; break;
        default: SampleRate = 22000; break;
        }

        Sound.VolumeLevel[0]= AYMute->Checked ? 0:ChAVol->Max - ChAVol->Position;
        Sound.VolumeLevel[1]= AYMute->Checked ? 0:ChBVol->Max - ChBVol->Position;
        Sound.VolumeLevel[2]= AYMute->Checked ? 0:ChCVol->Max - ChCVol->Position;
        Sound.VolumeLevel[3]= BeeperMute->Checked ? 0:BeeperVol->Max - BeeperVol->Position;

        if (LastMidiItem!=MidiBox->ItemIndex)
        {
                Midi.Stop();
                Midi.Device = MidiBox->ItemIndex -2;
                Midi.Start();
                LastMidiItem=MidiBox->ItemIndex;
        }

        Sound.ReInitialise(0,0,8,SampleRate, Channels);

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
        ini->WriteBool("MIDI","8Bit",Btn8Bit->Checked);
        ini->WriteBool("MIDI","16Bit",Btn16Bit->Checked);
        ini->WriteBool("MIDI","Stereo",BtnStereo->Checked);
        ini->WriteInteger("MIDI","Frequency",FreqBox->ItemIndex);
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
        Btn8Bit->Checked=ini->ReadBool("MIDI","8Bit",Btn8Bit->Checked);
        Btn16Bit->Checked=ini->ReadBool("MIDI","16Bit",Btn16Bit->Checked);
        BtnStereo->Checked=ini->ReadBool("MIDI","Stereo",BtnStereo->Checked);
        FreqBox->ItemIndex=ini->ReadInteger("MIDI","Frequency",FreqBox->ItemIndex);

        OKClick(NULL);

        MidiChange(NULL);
}

