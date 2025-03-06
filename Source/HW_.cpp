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

#include <dir.h>
#include <dirent.h>
#include <vcl4.h>
#include <ctype.h>

#pragma hdrstop

#include "HW_.h"
#include "main_.h"
#include "keyboard_.h"
#include "kb_.h"
#include "kbstatus.h"
#include "zx81config.h"
#include "accdraw_.h"
#include "speed_.h"
#include "sound.h"
#include "serialport.h"
#include "zx81.h"
#include "ace.h"
#include "spec48.h"
#include "artifacts_.h"
#include "utils.h"
#include "Plus3Drives.h"
#include "ide.h"
#include "interface1.h"
#include "debug.h"
#include "symbolstore.h"
#include "SymBrowse.h"
#include "Chroma\Chroma.h"
#include "Spectra\Spectra.h"
#include "LiveMemoryWindow_.h"
#include "BasicLister\BasicLister_.h"
#include "BasicLister\IBasicLister.h"
#include "zx81\zx81BasicLister.h"
#include "zx81\zx80BasicLister.h"
#include "spectrum\spec48BasicLister.h"
#include "spectrum\spec128BasicLister.h"
#include "zx97config.h"
#include "SoundForm.h"

#include <set>
extern std::set<int> dirtyBird;

extern "C" BYTE ZX1541Mem[];

extern void HWSetMachine(int machine, int speccy);
extern int zx81_do_scanline(SCANLINE *CurScanLine);
extern int zx80_do_scanline(SCANLINE *CurScanLine);
extern void InitPatches(int machineType);
extern bool LoadRomCartridgeFile(char *filename);
extern int RomCartridgeCapacity;
extern int LoadDock(char *Filename);

static AnsiString programmableJoystickLeft;
static AnsiString programmableJoystickRight;
static AnsiString programmableJoystickUp;
static AnsiString programmableJoystickDown;
static AnsiString programmableJoystickFire;

extern char* zxpandSDCardFolderRoot;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OffBtn"
#pragma link "OffBtn"
#pragma resource "*.dfm"
THW *HW;
//---------------------------------------------------------------------------
void HWSetMachine(int machine, int speccy)
{
        switch(machine)
        {
        case MACHINEZX80:   HW->ZX80BtnClick(NULL);   break;
        case MACHINEZX81:   HW->ZX81BtnClick(NULL);   break;
        case MACHINEACE:    HW->AceBtnClick(NULL);    break;
        case MACHINETS1500: HW->TS1500BtnClick(NULL); break;
        case MACHINETS1000: HW->TS1000BtnClick(NULL); break;
        case MACHINER470:   HW->R470BtnClick(NULL);   break;
        case MACHINETK85:   HW->TK85BtnClick(NULL);   break;
        case MACHINELAMBDA: HW->LambdaBtnClick(NULL); break;
        case MACHINEZX97LE: HW->ZX97LEBtnClick(NULL); break;
        case MACHINESPECTRUM:
                switch(speccy)
                {
                case SPECCY16:     HW->Spec16BtnClick(NULL);   break;
                case SPECCY48:     HW->Spec48BtnClick(NULL);   break;
                case SPECCYPLUS:   HW->SpecPlusBtnClick(NULL); break;
                case SPECCYTC2048: HW->TC2048BtnClick(NULL);   break;
                case SPECCYTC2068: HW->TC2068BtnClick(NULL);   break;
                case SPECCYTS2068: HW->TS2068BtnClick(NULL);   break;
                case SPECCY128:    HW->Spec128BtnClick(NULL);  break;
                case SPECCYPLUS2:  HW->SpecP2BtnClick(NULL);   break;
                case SPECCYPLUS2A: HW->SpecP2aBtnClick(NULL);  break;
                case SPECCYPLUS3:  HW->SpecP3BtnClick(NULL);   break;
                }
                break;
        }

        const bool disableResetStatus = false;
        HW->UpdateHardwareSettings(disableResetStatus);
}

__fastcall THW::THW(TComponent* Owner)
        : TForm(Owner)
{
        programmableJoystickLeft  = JoystickLeft1.Character;
        programmableJoystickRight = JoystickRight1.Character;
        programmableJoystickUp    = JoystickUp1.Character;
        programmableJoystickDown  = JoystickDown1.Character;
        programmableJoystickFire  = JoystickFire1.Character;

        NoMicrodrivesComboBox->ItemIndex = 0;
        RomCartridgeCapacity  = 0;
        RamPackHeight         = RamPackBox->Height;
        Machine->ActivePage   = Sinclair;
        Advanced->ActivePage  = Interfaces;
        DriveAType->ItemIndex = SelectEntry(DriveAType, "3\" Single-Sided (180K)");
        DriveBType->ItemIndex = SelectEntry(DriveBType, "None");
        ZXCFRAM->ItemIndex    = SelectEntry(ZXCFRAM,    "1024K");
        ZX81BtnClick(NULL);

        SaveToInternalSettings(); // save in case there is no INI file
        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        SetUpRomCartridges();

        ResetRequired = true;

        const bool disableResetStatus = false;
        UpdateHardwareSettings(disableResetStatus);
}
//---------------------------------------------------------------------------

AnsiString getMachineRoot(AnsiString fullRomName)
{
        // return the first part of the rom name up to but excluding the first '.'
        char* p1 = fullRomName.c_str();
        char* p2 = strchr(p1, '.');
        if (p2 == NULL) return fullRomName;
        int len = p2 - p1;
        return fullRomName.SubString(1, len);
}
//---------------------------------------------------------------------------

void THW::SetUpRomCartridges()
{
        ts1510RomCartridges.push_back(RomCartridgeEntry("79001 Supermath",         ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79002 States & Capitals", ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79003 Chess",             ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79004 Flight Simulation", ts1510RomsFolder));

        tc2068RomCartridges.push_back(RomCartridgeEntry("335.910.922 Spectrum Emulator", tc2068RomsFolder));
        tc2068RomCartridges.push_back(RomCartridgeEntry("335.911.940 TimeWord v1.2",     tc2068RomsFolder));

        ts2068RomCartridges.push_back(RomCartridgeEntry("72000 Budgeter",          ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("73000 Flight Simulator",  ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("73001 States & Capitals", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74000 Pinball",           ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74001 Androids",          ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74004 Casino 1",          ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74005 Crazybugs",         ts2068RomsFolder));

        ts2068RomCartridges.push_back(RomCartridgeEntry("2068 Programmer's Cartridge Toolkit", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("Hot-Z 2068 v2.51 AROS",               ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("Hot-Z 2068 v2.51 EXROM",              ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("Spectrum Emulator",                   ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("Zebra OS-64 v1.72",                   ts2068RomsFolder));

        sinclairRomCartridges.push_back(RomCartridgeEntry("G9R Space Raiders",         if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G10R Chess",                if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G12R Planetoids",           if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G13R Hungry Horace",        if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G22R Backgammon",           if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G24R Horace & The Spiders", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G27R Jet Pac",              if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G28R Pssst",                if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G29R Tranz Am",             if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G30R Cookie",               if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Spectrum System Test",      if2RomsFolder));

        sinclairRomCartridges.push_back(RomCartridgeEntry("Gyruss",                 if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Loco Motion",            if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Montezuma's Revenge #1", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Montezuma's Revenge #2", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Popeye",                 if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Q-Bert",                 if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Return Of The Jedi",     if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("Star Wars",              if2RomsFolder));

        sinclairRomCartridges.push_back(RomCartridgeEntry("Spectrum+2 Test Program", spectrumPlus2RomsFolder));
}

void zx81_writebyteProxy(int address, int data)
{
        zx81_writebyte(address, data);
        dirtyBird.insert(address);
}

void spec48_writebyteProxy(int address, int data)
{
        spec48_writebyte(address, data);
        dirtyBird.insert(address);
}

void ace_writebyteProxy(int address, int data)
{
        ace_writebyte(address, data);
        dirtyBird.insert(address);
}

void __fastcall THW::OKClick(TObject *Sender)
{
        const bool disableResetStatus = false;
        UpdateHardwareSettings(disableResetStatus);
        Close();
}

void __fastcall THW::ApplyClick(TObject *Sender)
{
        const bool disableResetStatus = false;
        UpdateHardwareSettings(disableResetStatus);
}

void THW::UpdateHardwareSettings(bool disableReset)
{
        bool machineChanged = (NewMachine != emulator.machine);
        emulator.machine = (CFGBYTE)NewMachine;
        spectrum.model = NewSpec;

        CloseLiveMemoryWindow(machineChanged);

        strcpy(machine.CurRom, RomBox->Text.c_str());

        AnsiString romBase = DetermineRomBase();
        ConfigureSymbolFile(romBase);
        ConfigureCharacterBitmapFile(romBase);

        ConfigureMachineSettings();
        ConfigureRamTop();
        ConfigureDefaultRamSettings();
        DetermineRamSizeLabel(NewMachineName);
        ConfigureRom();
        ConfigureColour();
        Configure8K16KRam();
        ConfigureBasicLister();
        ConfigureMultifaceRom();
        ConfigureZXpand();
        ConfigureRomCartridge();
        ConfigureCharacterGenerator();
        ConfigureHiRes();
        ConfigureSound();
        ConfigureSpeech();
        ConfigureJoystick();
        ConfigureM1Not();
        ConfigureDisplayArtifacts();
        ConfigureKeypad();
        ConfigureInterfaces();

        ConfigureIDE();
        ConfigureIDERom();
        ConfigureFDC();
        ConfigureFDCRom();

        ConfigureModem();
        ConfigurePrinterCentronicsPort();
        ConfigureRzxSupport();

        spectrum.kbissue = Issue2->Checked;

        zx81.improvedWait = ImprovedWait->Checked;
        zx81.FloatingPointHardwareFix = FloatingPointHardwareFix->Checked;

        Form1->InWaveLoader->Enabled = true;
        Form1->OutWaveLoader->Enabled = true;

        SelectGroupboxVisibility();

        AccurateInit(false);
        Speed->Recalc(NULL);
        PCKbInit();
        Form1->ConnectSpectrum128Keypad->Hint = StringReplace(Form1->ConnectSpectrum128Keypad->Hint, "#", GetKeypadMultiplyKey(), TReplaceFlags() << rfReplaceAll);
        Kb->UpdateCursors();

        if (disableReset)
        {
                ResetRequired = false;
        }
        else if (ResetRequired)
        {
                machine.initialise();
                ResetRequired = false;
        }

        Keyboard->KbChange();

        InitialiseSound(machineChanged);

        Form1->EnableAnnotationOptions();

        machine.drivebusy = -1;

        InitPatches(NewMachine);

        ResetDisplaySize();

        ResetDebugger();

        if (Dbg->Visible) Dbg->UpdateVals();

        SaveToInternalSettings(); // save copy to keep user choices
        ZX97Dialog->UpdateMachine(Hwform.ZX97Form);

        UpdateApplyButton();
}
                             
void THW::LoadFromInternalSettings()
{
        if (Hwform.MachineName == "ZX80")               ZX80BtnClick(NULL);
        else if (Hwform.MachineName == "16K Spectrum")  Spec16BtnClick(NULL);
        else if (Hwform.MachineName == "48K Spectrum")  Spec48BtnClick(NULL);
        else if (Hwform.MachineName == "48K Spectrum+") SpecPlusBtnClick(NULL);
        else if (Hwform.MachineName == "Spectrum 128")  Spec128BtnClick(NULL);
        else if (Hwform.MachineName == "Spectrum +2")   SpecP2BtnClick(NULL);
        else if (Hwform.MachineName == "Spectrum +2A")  SpecP2aBtnClick(NULL);
        else if (Hwform.MachineName == "Spectrum +3")   SpecP3BtnClick(NULL);
        else if (Hwform.MachineName == "TS1000")        TS1000BtnClick(NULL);
        else if (Hwform.MachineName == "TS1500")        TS1500BtnClick(NULL);
        else if (Hwform.MachineName == "TC2048")        TC2048BtnClick(NULL);
        else if (Hwform.MachineName == "TC2068")        TC2068BtnClick(NULL);
        else if (Hwform.MachineName == "TS2068")        TS2068BtnClick(NULL);
        else if (Hwform.MachineName == "Lambda 8300")   LambdaBtnClick(NULL);
        else if (Hwform.MachineName == "Ringo R-470")   R470BtnClick(NULL);
        else if (Hwform.MachineName == "TK85")          TK85BtnClick(NULL);
        else if (Hwform.MachineName == "ZX97 Lite")     ZX97LEBtnClick(NULL);
        else if (Hwform.MachineName == "Jupiter Ace")   AceBtnClick(NULL);
        else                                            ZX81BtnClick(NULL);

        if (Hwform.ZXpandChecked && JoystickBox->Items->Strings[JoystickBox->Items->Count - 1] != "ZXpand")
        {
                JoystickBox->Items->Add("ZXpand");
        }

        RomBox->ItemIndex = FindEntry(RomBox, Hwform.RomBoxText);
        if (RomBox->ItemIndex == -1)
        {
                RomBox->Text = Hwform.RomBoxText;
        }

        RamPackBox->ItemIndex            = FindEntry(RamPackBox,              Hwform.RamPackBoxText);
        SoundCardBox->ItemIndex          = SelectEntry(SoundCardBox,          Hwform.SoundCardBoxText);
        ChrGenBox->ItemIndex             = SelectEntry(ChrGenBox,             Hwform.ChrGenBoxText);
        HiResBox->ItemIndex              = SelectEntry(HiResBox,              Hwform.HiResBoxText);
        ColourBox->ItemIndex             = SelectEntry(ColourBox,             Hwform.ColourBoxText);
        SpeechBox->ItemIndex             = SelectEntry(SpeechBox,             Hwform.SpeechBoxText);
        JoystickBox->ItemIndex           = SelectEntry(JoystickBox,           Hwform.JoystickBoxText);
        RomCartridgeBox->ItemIndex       = SelectEntry(RomCartridgeBox,       Hwform.RomCartridgeBoxText);
        ZXC1ConfigurationBox->ItemIndex  = SelectEntry(ZXC1ConfigurationBox,  Hwform.ZXC1ConfigurationBoxText);
        DriveAType->ItemIndex            = SelectEntry(DriveAType,            Hwform.DriveATypeText);
        DriveBType->ItemIndex            = SelectEntry(DriveBType,            Hwform.DriveBTypeText);
        ZXCFRAM->ItemIndex               = SelectEntry(ZXCFRAM,               Hwform.ZXCFRAMText);
        IDEBox->ItemIndex                = SelectEntry(IDEBox,                Hwform.IDEBoxText);
        FDCBox->ItemIndex                = SelectEntry(FDCBox,                Hwform.FDCBoxText);
        NoMicrodrivesComboBox->ItemIndex = SelectEntry(NoMicrodrivesComboBox, Hwform.NoMicrodrivesComboBoxText);

        IDERomBox->ItemIndex = FindEntry(IDERomBox, Hwform.IDERomBoxText);
        if (IDERomBox->ItemIndex == -1)
        {
                IDERomBox->Text = Hwform.IDERomBoxText;
        }

        FDCRomBox->ItemIndex = FindEntry(FDCRomBox, Hwform.FDCRomBoxText);
        if (FDCRomBox->ItemIndex == -1)
        {
                FDCRomBox->Text = Hwform.FDCRomBoxText;
        }

        RomCartridgeFileBox->Text         = Hwform.RomCartridgeFileBoxText;
        SinclairRomCartridgeFileBox->Text = Hwform.SinclairRomCartridgeFileBoxText;
        TS1510RomCartridgeFileBox->Text   = Hwform.TS1510RomCartridgeFileBoxText;
        TC2068RomCartridgeFileBox->Text   = Hwform.TC2068RomCartridgeFileBoxText;
        TS2068RomCartridgeFileBox->Text   = Hwform.TS2068RomCartridgeFileBoxText;

        programmableJoystickLeft          = Hwform.ProgrammableJoystickLeft;
        programmableJoystickRight         = Hwform.ProgrammableJoystickRight;
        programmableJoystickUp            = Hwform.ProgrammableJoystickUp;
        programmableJoystickDown          = Hwform.ProgrammableJoystickDown;
        programmableJoystickFire          = Hwform.ProgrammableJoystickFire;

        SpecDrum->Checked                      = Hwform.SpecDrumChecked;
        ProtectROM->Checked                    = Hwform.ProtectROMChecked;
        NTSC->Checked                          = Hwform.NTSCChecked;
        EnableLowRAM->Checked                  = Hwform.EnableLowRAMChecked;
        M1Not->Checked                         = Hwform.M1NotChecked;
        ImprovedWait->Checked                  = Hwform.ImprovedWaitChecked;
        TS2050->Checked                        = Hwform.TS2050Checked;
        Issue2->Checked                        = Hwform.Issue2Checked;
        KMouse->Checked                        = Hwform.KMouseChecked;
        Multiface->Checked                     = Hwform.MultifaceChecked;
        ZXPrinter->Checked                     = Hwform.ZXPrinterChecked;
        FloatingPointHardwareFix->Checked      = Hwform.FloatingPointHardwareFixChecked;
        uSource->Checked                       = Hwform.uSourceChecked;
        Z80Assembler->Checked                  = Hwform.Z80AssemblerChecked;
        Memocalc->Checked                      = Hwform.MemocalcChecked;
//        Memotext->Checked                      = Hwform.MemotextChecked;

        ZX97Dialog->UpdateFormSettings(Hwform.ZX97Form);

        //---- APPLY THE SETTINGS ----

        JoystickBoxChange(JoystickBox);

        if (RomCartridgeFileBox->Text.Length() > 0)
        {
                RomCartridgeFileBox->SelStart = RomCartridgeFileBox->Text.Length() - 1;
                RomCartridgeFileBox->SelLength = 0;
        }

        // Do this after all ZXpand dependencies have been assigned
        SetZXpandState(Hwform.ZXpandChecked, ZXpand->Enabled);

        IDEBoxChange(NULL);
        FDCBoxChange(NULL);

        if (ZX80Btn->Down || ZX81Btn->Down || Spec16Btn->Down || Spec48Btn->Down || SpecPlusBtn->Down || Spec128Btn->Down)
        {
                Machine->ActivePage = Sinclair;
        }
        else if (SpecP2Btn->Down || SpecP2aBtn->Down || SpecP3Btn->Down)
        {
                Machine->ActivePage = Amstrad;
        }
        else if (TS1000Btn->Down || TS1500Btn->Down  || TC2048Btn->Down || TS2068Btn->Down || TC2068Btn->Down)
        {
                Machine->ActivePage = Timex;
        }
        else if (ZX97LEBtn->Down)
        {
                Machine->ActivePage = HomeBrew;
        }
        else
        {
                Machine->ActivePage = Others;
        }
}

void THW::SaveToInternalSettings()
{
        Hwform.MachineName                     = NewMachineName;

        Hwform.RomBoxText                      = RomBox->Text;
        Hwform.RamPackBoxText                  = RamPackBox->Text;
        Hwform.SoundCardBoxText                = SoundCardBox->Text;
        Hwform.ChrGenBoxText                   = ChrGenBox->Text;
        Hwform.HiResBoxText                    = HiResBox->Text;
        Hwform.ColourBoxText                   = ColourBox->Text;
        Hwform.SpeechBoxText                   = SpeechBox->Text;
        Hwform.JoystickBoxText                 = JoystickBox->Text;
        Hwform.RomCartridgeBoxText             = RomCartridgeBox->Text;
        Hwform.RomCartridgeFileBoxText         = RomCartridgeFileBox->Text;
        Hwform.SinclairRomCartridgeFileBoxText = SinclairRomCartridgeFileBox->Text;
        Hwform.TS1510RomCartridgeFileBoxText   = TS1510RomCartridgeFileBox->Text;
        Hwform.TC2068RomCartridgeFileBoxText   = TC2068RomCartridgeFileBox->Text;
        Hwform.TS2068RomCartridgeFileBoxText   = TS2068RomCartridgeFileBox->Text;
        Hwform.ZXC1ConfigurationBoxText        = ZXC1ConfigurationBox->Text;
        Hwform.FDCBoxText                      = FDCBox->Text;
        Hwform.IDEBoxText                      = IDEBox->Text;
        Hwform.FDCRomBoxText                   = FDCRomBox->Text;
        Hwform.IDERomBoxText                   = IDERomBox->Text;
        Hwform.NoMicrodrivesComboBoxText       = NoMicrodrivesComboBox->Text;

        Hwform.ProgrammableJoystickLeft        = programmableJoystickLeft;
        Hwform.ProgrammableJoystickRight       = programmableJoystickRight;
        Hwform.ProgrammableJoystickUp          = programmableJoystickUp;
        Hwform.ProgrammableJoystickDown        = programmableJoystickDown;
        Hwform.ProgrammableJoystickFire        = programmableJoystickFire;

        Hwform.DriveATypeText                  = DriveAType->Text;
        Hwform.DriveBTypeText                  = DriveBType->Text;
        Hwform.ZXCFRAMText                     = ZXCFRAM->Text;

        Hwform.ZXpandChecked                   = ZXpand->Checked;
        Hwform.SpecDrumChecked                 = SpecDrum->Checked;
        Hwform.ProtectROMChecked               = ProtectROM->Checked;
        Hwform.NTSCChecked                     = NTSC->Checked;
        Hwform.EnableLowRAMChecked             = EnableLowRAM->Checked;
        Hwform.M1NotChecked                    = M1Not->Checked;
        Hwform.ImprovedWaitChecked             = ImprovedWait->Checked;
        Hwform.TS2050Checked                   = TS2050->Checked;
        Hwform.Issue2Checked                   = Issue2->Checked;
        Hwform.KMouseChecked                   = KMouse->Checked;
        Hwform.MultifaceChecked                = Multiface->Checked;
        Hwform.ZXPrinterChecked                = ZXPrinter->Checked;
        Hwform.FloatingPointHardwareFixChecked = FloatingPointHardwareFix->Checked;
        Hwform.uSourceChecked                  = uSource->Checked;
        Hwform.Z80AssemblerChecked             = Z80Assembler->Checked;
        Hwform.MemocalcChecked                 = Memocalc->Checked;
//        Hwform.MemotextChecked                 = Memotext->Checked;

        ZX97Dialog->RetrieveFormSettings(Hwform.ZX97Form);
}

void THW::ConfigureInterfaces()
{
        spectrum.usource = uSource->Checked;
        spectrum.kmouse  = KMouse->Checked;

        zx81.z80Assembler = Z80Assembler->Checked;
        zx81.memocalc     = Memocalc->Checked;
//        zx81.memotext     = Memotext->Checked;

        if (Hwform.Z80AssemblerChecked != Z80Assembler->Checked)
        {
                zx81.z80AssemblerOn = Z80Assembler->Checked;
        }

        if (Hwform.MemocalcChecked != Memocalc->Checked)
        {
                zx81.memocalcOn = Memocalc->Checked;
        }
        
//        if (Hwform.MemotextChecked != Memotext->Checked)
//        {
//                zx81.memotextOn = Memotext->Checked;
//        }

        Form1->BuildMemotechInterfaceSelection();
}

void THW::Configure8K16KRam()
{
        bool enable8K16KProtectOption = EnableLowRAM->Checked && (emulator.machine != MACHINESPECTRUM && emulator.machine != MACHINER470 && emulator.machine != MACHINEACE && emulator.machine != MACHINEZX97LE);
        Form1->WriteProtect8KRAM->Enabled = enable8K16KProtectOption;
        zx81.shadowROM = !EnableLowRAM->Checked && zx81.truehires != HIRESG007;
        zx81.RAM816k = EnableLowRAM->Checked;
        if (!Form1->WriteProtect8KRAM->Enabled)
        {
                zx81.RAM816kWriteProtected = 0;
        }
        else
        {
                zx81.RAM816kWriteProtected = (CFGBYTE)(Form1->WriteProtect8KRAM->Checked ? 1 : 0);
        }
}

void THW::ConfigureRzxSupport()
{
        Form1->RZX1->Enabled = false;
        if (emulator.machine == MACHINESPECTRUM) Form1->RZX1->Enabled = true;
}

void THW::ResetDebugger()
{
        Dbg->Reset();
}

void THW::ConfigureDisplayArtifacts()
{
        Artifacts->TrackBarChange(NULL);
        Artifacts->ConfigureDotCrawlOption();

        bool spec48 = (emulator.machine == MACHINESPECTRUM && spectrum.model<SPECCY128);
        if (!spec48)
        {
                Artifacts->DotCrawl1->Checked = false;
        }

        Artifacts->ForceVibrantColours(machine.colour == COLOURCHROMA);
}

void THW::ConfigureM1Not()
{
        zx81.m1not = M1Not->Checked? 49152 : 32768;

        if (emulator.machine == MACHINEZX97LE)
        {
                zx81.m1not = 49152;
        }
}

void THW::ConfigureModem()
{
        if (TS2050->Checked == 1) { machine.ts2050 = 1; Form1->TS20501->Enabled = true; }
        else { machine.ts2050 = 0; Form1->TS20501->Enabled = false; }
}

void THW::ConfigurePrinterCentronicsPort()
{
        if (machine.floppytype == FLOPPYOPUSD
                || machine.floppytype == FLOPPYPLUSD
                || machine.floppytype == FLOPPYDISCIPLE
                || spectrum.model == SPECCYPLUS2A
                || spectrum.model == SPECCYPLUS3)
        {
                Form1->PrinterPort->Enabled = true;
                if (machine.floppytype == FLOPPYOPUSD)
                        Form1->PrinterPort->Caption = "Opus Printer Port";
                if (machine.floppytype == FLOPPYDISCIPLE)
                        Form1->PrinterPort->Caption = "DISCiPLE Printer Port";
                if (machine.floppytype == FLOPPYPLUSD)
                        Form1->PrinterPort->Caption = "Plus D Printer Port";
                if (spectrum.model == SPECCYPLUS2A)
                        Form1->PrinterPort->Caption = "+2A/+3 Printer Port";
                if (spectrum.model == SPECCYPLUS3)
                        Form1->PrinterPort->Caption = "+2A/+3 Printer Port";
        }
        else
        {
                Form1->PrinterPort->Enabled = false;
                Form1->PrinterPort->Caption = "Centronics Printer Port";
        }
}

void THW::SelectGroupboxVisibility()
{
        Dbg->GroupBoxZX81->Visible = false;
        Dbg->GroupBoxAce->Visible = false;
        Dbg->GroupBoxChroma->Visible = false;
        Dbg->GroupBoxSpectra->Visible = false;
        Dbg->GroupBoxZXC->Visible = false;

        switch(emulator.machine)
        {
        case MACHINEACE:
                Dbg->GroupBoxAce->Visible = true;
                break;

        case MACHINESPECTRUM:
                Dbg->GroupBoxSpectra->Visible = Form1->SpectraColourEnable->Visible;
                Dbg->GroupBoxZXC->Visible = Form1->SpectraColourEnable->Visible;
                break;

        default:
                Dbg->GroupBoxZX81->Visible = Form1->ChromaColourEnable->Visible;
                Dbg->GroupBoxChroma->Visible = Form1->ChromaColourEnable->Visible;
                Dbg->GroupBoxZXC->Visible = Form1->ChromaColourEnable->Visible;
                break;
        }
}

void THW::ConfigureRamTop()
{
        if (NewMachine == MACHINESPECTRUM)
        {
                zx81.RAMTOP = 65535;
                machine.ace96k = 0;
        }
        else if (RamPackBox->Items->Strings[RamPackBox->ItemIndex] == "96K")
        {
                zx81.RAMTOP = 65535;
                machine.ace96k = 1;
        }
        else if (emulator.machine == MACHINEZX97LE)
        {
                zx81.RAMTOP = 65535;
        }
        else
        {
                int kp = machine.baseRamSize;
                if (RamPackBox->ItemIndex!= 0)
                {
                        int rp = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                        kp = machine.ramPackSupplementsInternalRam ? kp + rp : rp;
                }
                zx81.RAMTOP = (kp << 10) + 16383;
                if (RamPackBox->ItemIndex == 6) zx81.RAMTOP = 65535;
                if (emulator.machine == MACHINEACE) zx81.RAMTOP -= 3072;
                machine.ace96k = 0;
        }
}

void THW::InitialiseSound(bool machineChanged)
{
        if (Sound.ReInitialise(NULL, machine.fps, 0, 0, 0, NULL)) MessageBox(NULL, "", "Sound Error", 0);

        if (machineChanged)
        {
                if ( ((emulator.machine == MACHINESPECTRUM || emulator.machine == MACHINEACE || emulator.machine == MACHINELAMBDA) && !Form1->Sound1->Checked)
                || ( (emulator.machine!= MACHINESPECTRUM && emulator.machine!= MACHINEACE && emulator.machine!= MACHINELAMBDA) && (machine.colour != COLOURCHROMA) && Form1->Sound1->Checked) )
                {
                        Form1->Sound1Click(NULL);
                }
        }

        zx81.vsyncsound = Form1->Sound1->Checked;
}

void THW::CloseLiveMemoryWindow(bool machineChanged)
{
        if (machineChanged && LiveMemoryWindow && LiveMemoryWindow->Visible)
                LiveMemoryWindow->Close();
}

void THW::DetermineRamSizeLabel(AnsiString newMachineName)
{
        AnsiString name = newMachineName;

        if (AceBtn->Down)
        {
                int i = 3;
                AnsiString Mem;

                if (RamPackBox->ItemIndex!= 0)
                        i+= atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());

                name = i;
                name += "K Jupiter Ace";
        }
        else if (RamPackBox->Visible)
        {
                int totalRam = machine.baseRamSize;
                if (RamPackBox->ItemIndex!= 0)
                {
                        int ramPack = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                        totalRam = machine.ramPackSupplementsInternalRam ? totalRam + ramPack : ramPack;
                }
                AnsiString ramSize = totalRam;
                name = ramSize + "K " + name;
        }

        strcpy(emulator.machinename, name.c_str());
        Form1->StatusBar1->Panels->Items[0]->Text = name;
}

void THW::ConfigureMultifaceRom()
{
        spectrum.MFVersion = MFNONE;
        Multiface->Enabled = false;

        if (NewMachine == MACHINESPECTRUM)
        {
                switch(NewSpec)
                {
                case SPECCY16:
                case SPECCY48:
                case SPECCYPLUS:
                case SPECCYTC2048:
                case SPECCY128:
                case SPECCYPLUS2:
                        spectrum.MFVersion = MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        break;

                case SPECCYPLUS2A:
                case SPECCYPLUS3:
                        spectrum.MFVersion = MFPLUS3;
                        Multiface->Caption = "Multiface 3";
                        Multiface->Enabled = true;
                        break;
                case SPECCYTC2068:
                case SPECCYTS2068:
                        spectrum.MFVersion = MFNONE;
                        Multiface->Enabled = false;
                        break;
                }
        }

        if (!Multiface->Checked) spectrum.MFVersion = MFNONE;
}

void THW::ConfigureRom()
{
        switch(NewMachine)
        {
        case MACHINEZX80:
                strcpy(emulator.ROM80, machine.CurRom);
                break;

        case MACHINEZX81:
                strcpy(emulator.ROM81, machine.CurRom);
                break;

        case MACHINER470:
                strcpy(emulator.ROMR470, machine.CurRom);
                break;

        case MACHINETK85:
                strcpy(emulator.ROMTK85, machine.CurRom);
                break;

        case MACHINETS1000:
                strcpy(emulator.ROMTS1000, machine.CurRom);
                break;

        case MACHINEACE:
                strcpy(emulator.ROMACE, machine.CurRom);
                break;

        case MACHINETS1500:
                strcpy(emulator.ROMTS1500, machine.CurRom);
                break;

        case MACHINELAMBDA:
                strcpy(emulator.ROMLAMBDA, machine.CurRom);
                break;

        case MACHINEZX97LE:
                strcpy(emulator.ROM97LE, machine.CurRom);
                break;

        case MACHINESPECTRUM:
                switch(NewSpec)
                {
                case SPECCY16:
                        spectrum.RAMBanks = 1;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSP16, machine.CurRom);
                        break;

                case SPECCY48:
                        spectrum.RAMBanks = 3;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSP48, machine.CurRom);
                        break;

                case SPECCYPLUS:
                        spectrum.RAMBanks = 3;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSPP, machine.CurRom);
                        break;

                case SPECCYTC2048:
                        spectrum.RAMBanks = 3;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMTC2048, machine.CurRom);
                        break;

                case SPECCYTC2068:
                        spectrum.RAMBanks = 3;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMTC2068, machine.CurRom);
                        break;

                case SPECCYTS2068:
                        spectrum.RAMBanks = 3;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMTS2068, machine.CurRom);
                        break;

                case SPECCY128:
                        spectrum.RAMBanks = 8;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSP128, machine.CurRom);
                        break;

                case SPECCYPLUS2:
                        spectrum.RAMBanks = 8;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSPP2, machine.CurRom);
                        break;

                case SPECCYPLUS2A:
                        spectrum.RAMBanks = 8;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSPP2A, machine.CurRom);
                        break;

                case SPECCYPLUS3:
                        spectrum.RAMBanks = 8;
                        spectrum.ROMBanks = 1;
                        strcpy(emulator.ROMSPP3, machine.CurRom);
                        break;
                }
                break;
        }
}

void THW::ConfigureZXpand()
{
        zx81.zxpand = 0;

        switch(NewMachine)
        {
        case MACHINEZX80:
                zx81.zxpand = (ZXpand->Checked == true);
                break;

        case MACHINEZX81:
        case MACHINETS1000:
        case MACHINETS1500:
                zx81.zxpand = (ZXpand->Checked == true);
                ZXpand->Caption = "ZXpand+";
                break;
        }

        CreateZXpand();
}

void THW::ConfigureBasicLister()
{
        if (BasicLister->ListerAvailable())
        {
                BasicLister->Clear();
                BasicLister->SetBasicLister(NULL);
        }

        Form1->BasicListerOption->Enabled = false;

        if (!strcmp(machine.CurRom, "zx80.rom"))
        {
                BasicLister->SetBasicLister(new zx80BasicLister(zx81.zxpand));
                Form1->BasicListerOption->Enabled = true;
        }
        else if (!strcmp(machine.CurRom, "zx81.edition1.rom") ||
                 !strcmp(machine.CurRom, "zx81.edition2.rom") ||
                 !strcmp(machine.CurRom, "zx81.edition3.rom") ||
                 !strcmp(machine.CurRom, "ts1500.rom") ||
                 !strcmp(machine.CurRom, "tk85.rom"))
        {
                BasicLister->SetBasicLister(new zx81BasicLister(zx81.zxpand));
                Form1->BasicListerOption->Enabled = true;
        }
        else if (!strcmp(machine.CurRom, "spectrum48.rom") ||
                 !strcmp(machine.CurRom, "spectrum48.spanish.rom"))
        {
                BasicLister->SetBasicLister(new spec48BasicLister());
                Form1->BasicListerOption->Enabled = true;
        }
        else if (!strcmp(machine.CurRom, "spectrum128.rom") ||
                 !strcmp(machine.CurRom, "spectrum128.spanish.rom") ||
                 !strcmp(machine.CurRom, "spectrum+2.rom") ||
                 !strcmp(machine.CurRom, "spectrum+3.version4-0.rom") ||
                 !strcmp(machine.CurRom, "spectrum+3.version4-1.rom") ||
                 !strcmp(machine.CurRom, "spectrum+2.french.rom") ||
                 !strcmp(machine.CurRom, "spectrum+2.spanish.rom") ||
                 !strcmp(machine.CurRom, "spectrum+3.version4-0.spanish.rom") ||
                 !strcmp(machine.CurRom, "spectrum+3.version4-1.spanish.rom"))
        {
                BasicLister->SetBasicLister(new spec128BasicLister());
                Form1->BasicListerOption->Enabled = true;
        }
        
        if (!BasicLister->ListerAvailable() && BasicLister->Visible)
        {
                BasicLister->Close();
        }
}

void THW::ConfigureColour()
{
        bool prevChromaColourSwitchOn  = (machine.colour == COLOURCHROMA);
        bool prevSpectraColourSwitchOn = (machine.colour == COLOURSPECTRA);

        switch(NewMachine)
        {
        case MACHINEACE:
                machine.colour = (CFGBYTE)((ColourBox->ItemIndex > 0) ? COLOURACE : COLOURDISABLED);
                break;
        case MACHINESPECTRUM:
                machine.colour = (CFGBYTE)((ColourBox->ItemIndex > 0) ? COLOURSPECTRA : COLOURSINCLAIR);
                break;
        default:
                machine.colour = (CFGBYTE)ColourBox->ItemIndex;
                break;
        }

        Artifacts->SelectRGBOutput(machine.colour == COLOURSPECTRA);
        Artifacts->ForceVibrantColours(machine.colour == COLOURCHROMA);
        if (machine.colour == COLOURSPECTRA)
        {
                Form1->DisplayArt->Checked = false;
                Artifacts->Visible = false;
        }
        Form1->DisplayArt->Enabled = (machine.colour != COLOURSPECTRA);

        /*
        if (machine.colour == COLOURCHROMA)
        {
                Form1->Sound1->Checked = true;
        }
        else if (machine.colour == COLOURDISABLED)
        {
                Form1->Sound1->Checked = false;
        }
        */
        
        ConfigureChroma(prevChromaColourSwitchOn);
        ConfigureSpectra(prevSpectraColourSwitchOn);
}

void THW::ConfigureChroma(bool prevChromaColourSwitchOn)
{
        zx81.chromaColourSwitchOn = (machine.colour == COLOURCHROMA);
        if (!prevChromaColourSwitchOn && zx81.chromaColourSwitchOn)
        {
                InitialiseChroma();
        }
        Form1->ChromaColourEnable->Checked = zx81.chromaColourSwitchOn;
        Form1->ChromaColourEnable->Enabled = zx81.chromaColourSwitchOn;
}

void THW::ConfigureSpectra(bool prevSpectraColourSwitchOn)
{
        spectrum.spectraColourSwitchOn = (machine.colour == COLOURSPECTRA);
        if (!prevSpectraColourSwitchOn && spectrum.spectraColourSwitchOn)
        {
                InitialiseSpectra();
        }
        Form1->SpectraColourEnable->Checked = spectrum.spectraColourSwitchOn;
        Form1->SpectraColourEnable->Enabled = spectrum.spectraColourSwitchOn;
}

void THW::ConfigureRomCartridge()
{
        int romcartridgetype = UpdateRomCartridgeControls(emulator.machine, spectrum.model);
        romcartridge.type = (CFGBYTE)romcartridgetype;

        if ((romcartridge.type == ROMCARTRIDGENONE) || (RomCartridgeFileBox->Text.Trim() == ""))
        {
                RomCartridgeCapacity = 0;
                RomCartridgeBox->ItemIndex = ROMCARTRIDGENONE;
                LoadDock((char *)"");
                *(emulator.romcartridgefilepath) = '\0';
        }
        else
        {
                AnsiString romCartridgeFilePath = RomCartridgeFileBox->Text;

                if (romcartridge.type == ROMCARTRIDGESINCLAIR)
                {
                        vector<RomCartridgeEntry>::iterator iter;

                        for (iter = sinclairRomCartridges.begin(); iter != sinclairRomCartridges.end(); iter++)
                        {
                                if (romCartridgeFilePath == iter->Title)
                                {
                                        romCartridgeFilePath = AnsiString(emulator.cwd) + iter->Path + iter->Title + ".rom";
                                        break;
                                }
                        }

                        AnsiString path = FileNameGetPath(romCartridgeFilePath);
                        if (path.Length() == 0)
                        {
                                romCartridgeFilePath = if2RomsFolder + romCartridgeFilePath;
                        }
                }
                else if (romcartridge.type == ROMCARTRIDGETS1510)
                {
                        vector<RomCartridgeEntry>::iterator iter;

                        for (iter = ts1510RomCartridges.begin(); iter != ts1510RomCartridges.end(); iter++)
                        {
                                if (romCartridgeFilePath == iter->Title)
                                {
                                        romCartridgeFilePath = AnsiString(emulator.cwd) + iter->Path + iter->Title + ".rom";
                                        break;
                                }
                        }

                        AnsiString path = FileNameGetPath(romCartridgeFilePath);
                        if (path.Length() == 0)
                        {
                                romCartridgeFilePath = ts1510RomsFolder + romCartridgeFilePath;
                        }
                }
                else if (romcartridge.type == ROMCARTRIDGETS2068)
                {
                        vector<RomCartridgeEntry>::iterator iter;

                        for (iter = ts2068RomCartridges.begin(); iter != ts2068RomCartridges.end(); iter++)
                        {
                                if (romCartridgeFilePath == iter->Title)
                                {
                                        romCartridgeFilePath = AnsiString(emulator.cwd) + iter->Path + iter->Title + ".dck";
                                        break;
                                }
                        }

                        AnsiString path = FileNameGetPath(romCartridgeFilePath);
                        if (path.Length() == 0)
                        {
                                romCartridgeFilePath = ts2068RomsFolder + romCartridgeFilePath;
                        }
                }
                else if (romcartridge.type == ROMCARTRIDGETC2068)
                {
                        vector<RomCartridgeEntry>::iterator iter;

                        for (iter = tc2068RomCartridges.begin(); iter != tc2068RomCartridges.end(); iter++)
                        {
                                if (romCartridgeFilePath == iter->Title)
                                {
                                        romCartridgeFilePath = AnsiString(emulator.cwd) + iter->Path + iter->Title + ".dck";
                                        break;
                                }
                        }

                        AnsiString path = FileNameGetPath(romCartridgeFilePath);
                        if (path.Length() == 0)
                        {
                                romCartridgeFilePath = tc2068RomsFolder + romCartridgeFilePath;
                        }
                }
                else if (romcartridge.type == ROMCARTRIDGEZXC1)
                {
                        romcartridge.zxc1Configuration = (ZXC1TYPE)ZXC1ConfigurationBox->ItemIndex;
                }

                int loadSuccessful;

                if (emulator.machine == MACHINESPECTRUM && (spectrum.model == SPECCYTS2068 || spectrum.model == SPECCYTC2068))
                {
                        loadSuccessful = LoadDock(romCartridgeFilePath.c_str());
                }
                else
                {
                        LoadDock((char *)"");
                        loadSuccessful = LoadRomCartridgeFile(romCartridgeFilePath.c_str());
                }

                strcpy(emulator.romcartridgefilepath, romCartridgeFilePath.c_str());

                if (!loadSuccessful)
                {
                        AnsiString msg;
                        msg = "Failed to load cartridge file:\n\n";
                        msg += romCartridgeFilePath;
                        Application->MessageBox(msg.c_str(), "Error", MB_OK | MB_ICONERROR);

                        LoadDock((char *)"");
                }
        }
}

int THW::DetermineRomCartridgeType(AnsiString cartridgeText, int machine, int spectrumModel)
{
        int cartridgeType;

        if (cartridgeText == "Sinclair")
        {
                cartridgeType = ROMCARTRIDGESINCLAIR;
        }
        else if (cartridgeText == "ZXC1")
        {
                cartridgeType = ROMCARTRIDGEZXC1;
        }
        else if (cartridgeText == "ZXC2")
        {
                cartridgeType = ROMCARTRIDGEZXC2;
        }
        else if (cartridgeText == "ZXC3")
        {
                cartridgeType = ROMCARTRIDGEZXC3;
        }
        else if (cartridgeText == "ZXC4")
        {
                cartridgeType = ROMCARTRIDGEZXC4;
        }
        else if (cartridgeText == "Standard 16K")
        {
                cartridgeType = ROMCARTRIDGE16K;
        }
        else if (cartridgeText == "Timex")
        {
                if (machine == MACHINESPECTRUM)
                {
                        if (spectrumModel == SPECCYTS2068)
                        {
                                cartridgeType = ROMCARTRIDGETS2068;
                        }
                        else
                        {
                                cartridgeType = ROMCARTRIDGETC2068;
                        }
                }
                else
                {
                        cartridgeType = ROMCARTRIDGETS1510;
                }
        }
        else
        {
                cartridgeType = ROMCARTRIDGENONE;
        }

        return cartridgeType;
}

int THW::UpdateRomCartridgeControls(int machine, int spectrumModel)
{
        int romcartridgetype = DetermineRomCartridgeType(RomCartridgeBox->Text, machine, spectrumModel);

        bool spectrumSinclairSelected = (romcartridgetype == ROMCARTRIDGESINCLAIR);
        bool zx81TimexSelected = (romcartridgetype == ROMCARTRIDGETS1510);
        bool ts2068Selected = (romcartridgetype == ROMCARTRIDGETS2068);
        bool tc2068Selected = (romcartridgetype == ROMCARTRIDGETC2068);

        bool noneSelected = (romcartridgetype == ROMCARTRIDGENONE);
        bool zxc1Selected = (romcartridgetype == ROMCARTRIDGEZXC1);

        SinclairRomCartridgeFileBox->Enabled = spectrumSinclairSelected;
        SinclairRomCartridgeFileBox->Visible = spectrumSinclairSelected;

        TC2068RomCartridgeFileBox->Enabled = tc2068Selected;
        TC2068RomCartridgeFileBox->Visible = tc2068Selected;

        TS2068RomCartridgeFileBox->Enabled = ts2068Selected;
        TS2068RomCartridgeFileBox->Visible = ts2068Selected;

        TS1510RomCartridgeFileBox->Enabled = zx81TimexSelected;
        TS1510RomCartridgeFileBox->Visible = zx81TimexSelected;

        if (spectrumSinclairSelected)
        {
                SinclairRomCartridgeFileBox->Text = SinclairRomCartridgeFileBox->Text.Trim();
                RomCartridgeFileBox->Text = SinclairRomCartridgeFileBox->Text;
        }
        else if (tc2068Selected)
        {
                TC2068RomCartridgeFileBox->Text = TC2068RomCartridgeFileBox->Text.Trim();
                RomCartridgeFileBox->Text = TC2068RomCartridgeFileBox->Text;
        }
        else if (ts2068Selected)
        {
                TS2068RomCartridgeFileBox->Text = TS2068RomCartridgeFileBox->Text.Trim();
                RomCartridgeFileBox->Text = TS2068RomCartridgeFileBox->Text;
        }
        else if (zx81TimexSelected)
        {
                TS1510RomCartridgeFileBox->Text = TS1510RomCartridgeFileBox->Text.Trim();
                RomCartridgeFileBox->Text = TS1510RomCartridgeFileBox->Text;
        }

        RomCartridgeFileBox->Enabled = !spectrumSinclairSelected && !zx81TimexSelected && !tc2068Selected && !ts2068Selected && !noneSelected;
        RomCartridgeFileBox->Visible = !spectrumSinclairSelected && !zx81TimexSelected && !tc2068Selected && !ts2068Selected;

        BrowseRomCartridge->Enabled = !noneSelected;

        ZXC1ConfigurationBox->Enabled = zxc1Selected;
        ZXC1ConfigurationBox->Visible = zxc1Selected;

        if (zxc1Selected)
        {
                RomCartridgeFileBox->Left = 277;
                RomCartridgeFileBox->Width = 101;

                if (ZXC1ConfigurationBox->ItemIndex == -1)
                {
                        ZXC1ConfigurationBox->ItemIndex = 0;
                }
        }
        else
        {
                RomCartridgeFileBox->Left = 184;
                RomCartridgeFileBox->Width = 194;
        }

        return romcartridgetype;
}

void THW::ConfigureCharacterGenerator()
{
        if ((zx81.chrgen != CHRGENQS) && (ChrGenBox->ItemIndex == CHRGENQS))
        {
                zx81.enableQSchrgen = false;
        }
        else if (ChrGenBox->ItemIndex != CHRGENQS)
        {
                zx81.enableQSchrgen = false;
        }

        if (ChrGenBox->Text == "Lambda")
        {
                zx81.chrgen = CHRGENLAMBDA;
                zx81.extfont = 1;
        }
        else
        {
                zx81.chrgen = (CFGBYTE)ChrGenBox->ItemIndex;
                zx81.extfont = 0;
        }
        
        Form1->QSChrEnable->Checked = zx81.enableQSchrgen;
        Form1->QSChrEnable->Enabled = (zx81.chrgen == CHRGENQS);

        if ((zx81.chrgen == CHRGENDK) || (zx81.chrgen == CHRGENCHR128))
        {
                zx81.maxireg = 0x3F;
        }
        else
        {
                zx81.maxireg = 0x1F;
        }
}

void THW::ConfigureHiRes()
{
        Form1->ResetMemotechHRG->Enabled = false;
        Form1->ResetQuicksilvaHiRes->Enabled = false;

        switch(HiResBox->ItemIndex)
        {
        case 1: zx81.truehires = HIRESWRX; break;
        case 2: zx81.truehires = HIRESG007; break;
        case 3: zx81.truehires = HIRESMEMOTECH;
                Form1->ResetMemotechHRG->Enabled = true;
                break;
        case 4: zx81.truehires = HIRESQUICKSILVA;
                Form1->ResetQuicksilvaHiRes->Enabled = true;
                break;
        default:
        case 0: zx81.truehires = HIRESDISABLED; break;
        }
}

void THW::ConfigureKeypad()
{
        Form1->ConnectSpectrum128Keypad->Enabled = (NewMachine == MACHINESPECTRUM && NewSpec >= SPECCY128);

        if (!Form1->ConnectSpectrum128Keypad->Enabled)
        {
                spectrum.spectrum128Keypad = 0;
        }
        else
        {
                spectrum.spectrum128Keypad = (CFGBYTE)(Form1->ConnectSpectrum128Keypad->Checked ? 1 : 0);
        }
}

void THW::ConfigureSound()
{
        if (NewMachine == MACHINESPECTRUM)
        {
                if (SpecDrum->Checked)
                        spectrum.specdrum = 1;
                else
                        spectrum.specdrum = 0;

                if (NewSpec == SPECCYTS2068)
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 1: machine.aytype = AY_TYPE_TS2068; break;
                        case 0:
                        default: machine.aytype = AY_TYPE_NONE; break;
                        }
                }
                else if (NewSpec == SPECCYTC2068)
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 1: machine.aytype = AY_TYPE_TC2068; break;
                        case 0:
                        default: machine.aytype = AY_TYPE_NONE; break;
                        }
                }
                else
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 4: machine.aytype = AY_TYPE_ZONX; break;
                        case 3: machine.aytype = AY_TYPE_DKTRONICS; break;
                        case 2: machine.aytype = AY_TYPE_FULLER; break;
                        case 1: machine.aytype = AY_TYPE_SINCLAIR; break;
                        case 0:
                        default: machine.aytype = AY_TYPE_NONE; break;
                        }
                }
        }
        else if (NewMachine == MACHINEACE)
        {
                switch (SoundCardBox->ItemIndex)
                {
                case 2: machine.aytype = AY_TYPE_BOLDFIELD; break;
                case 1: machine.aytype = AY_TYPE_ACE_USER; break;
                case 0:
                default: machine.aytype = AY_TYPE_NONE; break;
                }
        }
        else
        {
                switch (SoundCardBox->ItemIndex)
                {
                case 2: machine.aytype = AY_TYPE_QUICKSILVA; break;
                case 1: machine.aytype = AY_TYPE_ZONX; break;
                case 0:
                default: machine.aytype = AY_TYPE_NONE; break;
                }
        }
}

void THW::ConfigureSpeech()
{
        Form1->ResetSpeech->Enabled = (SpeechBox->ItemIndex != 0);

        if (NewMachine == MACHINESPECTRUM)
        {
                if (NewSpec >= SPECCY128)
                {
                        switch (SpeechBox->ItemIndex)
                        {
                        case 4: machine.speech = SPEECH_TYPE_DIGITALKER; break;
                        case 3: machine.speech = SPEECH_TYPE_ORATOR; break;
                        case 2: machine.speech = SPEECH_TYPE_DKTRONICS; break;
                        case 1: machine.speech = SPEECH_TYPE_SWEETTALKER; break;
                        case 0:
                        default: machine.speech = SPEECH_TYPE_NONE; break;
                        }
                }
                else
                {
                        switch (SpeechBox->ItemIndex)
                        {
                        case 5: machine.speech = SPEECH_TYPE_USPEECH; break;
                        case 4: machine.speech = SPEECH_TYPE_DIGITALKER; break;
                        case 3: machine.speech = SPEECH_TYPE_ORATOR; break;
                        case 2: machine.speech = SPEECH_TYPE_DKTRONICS; break;
                        case 1: machine.speech = SPEECH_TYPE_SWEETTALKER; break;
                        case 0:
                        default: machine.speech = SPEECH_TYPE_NONE; break;
                        }
                }
        }
        else if (NewMachine != MACHINEACE)
        {
                switch (SpeechBox->ItemIndex)
                {
                case 5: machine.speech = SPEECH_TYPE_TALKBACK; break;
                case 4: machine.speech = SPEECH_TYPE_DIGITALKER; break;
                case 3: machine.speech = SPEECH_TYPE_MAGECO; break;
                case 2: machine.speech = SPEECH_TYPE_SWEETTALKER; break;
                case 1: machine.speech = SPEECH_TYPE_PARROT; break;
                case 0:
                default: machine.speech = SPEECH_TYPE_NONE; break;
                }
        }
        else
        {
                machine.speech = SPEECH_TYPE_NONE;
        }
}

void THW::ConfigureJoystick()
{
        if (NewMachine == MACHINESPECTRUM)
        {
                if (NewSpec != SPECCYTS2068 && NewSpec != SPECCYTC2068)
                {
                        switch (JoystickBox->ItemIndex)
                        {
                        case 5: machine.joystickInterfaceType = JOYSTICK_PROGRAMMABLE; break;
                        case 4: machine.joystickInterfaceType = JOYSTICK_FULLER; break;
                        case 3: machine.joystickInterfaceType = JOYSTICK_KEMPSTON; break;
                        case 2: machine.joystickInterfaceType = JOYSTICK_INTERFACE2; break;
                        case 1: machine.joystickInterfaceType = JOYSTICK_CURSOR; break;
                        case 0:
                        default: machine.joystickInterfaceType = JOYSTICK_NONE; break;
                        }
                }
                else
                {
                        switch (JoystickBox->ItemIndex)
                        {
                        case 3: machine.joystickInterfaceType = JOYSTICK_PROGRAMMABLE; break;
                        case 2: machine.joystickInterfaceType = JOYSTICK_KEMPSTON; break;
                        case 1: machine.joystickInterfaceType = JOYSTICK_TIMEX; break;
                        case 0:
                        default: machine.joystickInterfaceType = JOYSTICK_NONE; break;
                        }
                }
        }
        else if (NewMachine == MACHINEACE)
        {
                switch (JoystickBox->ItemIndex)
                {
                case 2: machine.joystickInterfaceType = JOYSTICK_PROGRAMMABLE; break;
                case 1: machine.joystickInterfaceType = JOYSTICK_BOLDFIELD; break;
                case 0:
                default: machine.joystickInterfaceType = SPEECH_TYPE_NONE; break;
                }
        }
        else if (NewMachine == MACHINEZX80)
        {
                switch (JoystickBox->ItemIndex)
                {
                case 2: machine.joystickInterfaceType = JOYSTICK_ZXPAND; break;
                case 1: machine.joystickInterfaceType = JOYSTICK_KEMPSTON; break;
                case 0:
                default: machine.joystickInterfaceType = JOYSTICK_NONE; break;
                }
        }
        else
        {
                switch (JoystickBox->ItemIndex)
                {
                case 4: machine.joystickInterfaceType = JOYSTICK_ZXPAND; break;
                case 3: machine.joystickInterfaceType = JOYSTICK_PROGRAMMABLE; break;
                case 2: machine.joystickInterfaceType = JOYSTICK_KEMPSTON; break;
                case 1: machine.joystickInterfaceType = JOYSTICK_CURSOR; break;
                case 0:
                default: machine.joystickInterfaceType = JOYSTICK_NONE; break;
                }
        }

        switch (machine.joystickInterfaceType)
        {
        case JOYSTICK_KEMPSTON:
                JoystickUp1.Data    = 0xF7;
                JoystickDown1.Data  = 0xFB;
                JoystickLeft1.Data  = 0xFD;
                JoystickRight1.Data = 0xFE;
                JoystickFire1.Data  = 0xEF;
                break;

        case JOYSTICK_FULLER:
                JoystickUp1.Data    = 0xFE;
                JoystickDown1.Data  = 0xFD;
                JoystickLeft1.Data  = 0xFB;
                JoystickRight1.Data = 0xF7;
                JoystickFire1.Data  = 0x7F;
                break;

        case JOYSTICK_ZXPAND:
                JoystickUp1.Data    = 0x7F;
                JoystickDown1.Data  = 0xBF;
                JoystickLeft1.Data  = 0xDF;
                JoystickRight1.Data = 0xEF;
                JoystickFire1.Data  = 0xF7;
                break;

        case JOYSTICK_BOLDFIELD:
                JoystickUp1.Data    = 0xFE;
                JoystickDown1.Data  = 0xFD;
                JoystickLeft1.Data  = 0xF7;
                JoystickRight1.Data = 0xFB;
                JoystickFire1.Data  = 0xDF;
                break;

        case JOYSTICK_TIMEX:
                JoystickUp1.Data    = 0xFE;
                JoystickDown1.Data  = 0xFD;
                JoystickLeft1.Data  = 0xFB;
                JoystickRight1.Data = 0xF7;
                JoystickFire1.Data  = 0x7F;

                JoystickUp2.Data    = 0xFE;
                JoystickDown2.Data  = 0xFD;
                JoystickLeft2.Data  = 0xFB;
                JoystickRight2.Data = 0xF7;
                JoystickFire2.Data  = 0x7F;
                break;

        case JOYSTICK_INTERFACE2:
                JoystickUp1.Data    = 0xFD;
                JoystickDown1.Data  = 0xFB;
                JoystickLeft1.Data  = 0xEF;
                JoystickRight1.Data = 0xF7;
                JoystickFire1.Data  = 0xFE;

                JoystickUp2.Data    = 0xF7;
                JoystickDown2.Data  = 0xFB;
                JoystickLeft2.Data  = 0xFE;
                JoystickRight2.Data = 0xFD;
                JoystickFire2.Data  = 0xEF;
                break;

        default:
                SetCharacter(JoystickLeftBox,  JoystickLeft1);
                SetCharacter(JoystickRightBox, JoystickRight1);
                SetCharacter(JoystickUpBox,    JoystickUp1);
                SetCharacter(JoystickDownBox,  JoystickDown1);
                SetCharacter(JoystickFireBox,  JoystickFire1);
                break;
        }

        if (machine.joystickInterfaceType != JOYSTICK_NONE)
        {
                InitialiseJoysticks();
        }

        if (JoystickBox->Text != Hwform.JoystickBoxText)
        {
                CFGBYTE defaultController = 0;

                bool joystickInterfaceSelected     = (machine.joystickInterfaceType != JOYSTICK_NONE);
                bool twinJoystickInterfaceSelected = (machine.joystickInterfaceType == JOYSTICK_INTERFACE2 || machine.joystickInterfaceType == JOYSTICK_TIMEX);

                if (joystickInterfaceSelected && (controllerPresent[0] || emulator.UseNumericPadForJoystick1 != 0))
                {
                        machine.joystick1Controller = defaultController;
                        machine.joystick1Connected = 1;
                        machine.joystick1AutoFireEnabled = 0;

                        defaultController++;
                }
                else
                {
                        machine.joystick1Controller = -1;
                        machine.joystick1Connected = 0;
                        machine.joystick1AutoFireEnabled = 0;
                }

                if (twinJoystickInterfaceSelected && (controllerPresent[1] || emulator.UseNumericPadForJoystick2 != 0))
                {
                        machine.joystick2Controller = defaultController;
                        machine.joystick2Connected = 1;
                        machine.joystick2AutoFireEnabled = 0;
                }
                else
                {
                        machine.joystick2Controller = -1;
                        machine.joystick2Connected = 0;
                        machine.joystick2AutoFireEnabled = 0;
                }
        }

        Form1->BuildMenuJoystickSelection();
}

void THW::ConfigureIDE()
{
        Form1->divIDEJumperEClosed->Enabled    = (IDEBox->ItemIndex == FindEntry(IDEBox, "divIDE 57 (R Gal)") || IDEBox->ItemIndex == FindEntry(IDEBox, "divIDE 57 (R\" Gal)"));
        Form1->ZXCFUploadJumperClosed->Enabled = (IDEBox->ItemIndex == FindEntry(IDEBox, "ZXCF"));
        Form1->SimpleIdeRomEnabled->Enabled    = (IDEBox->ItemIndex == FindEntry(IDEBox, "Simple IDE 8-Bit") || IDEBox->ItemIndex == FindEntry(IDEBox, "Simple IDE 16-Bit") ||
                                                  IDEBox->ItemIndex == FindEntry(IDEBox, "Simple IDE CF") || IDEBox->ItemIndex == FindEntry(IDEBox, "Simple +3 8-Bit"));

        machine.HDType = HDNONE;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "ZXCF")                machine.HDType = HDZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "divIDE 57 (R Gal)")   { machine.HDType = HDDIVIDE; machine.divIDEAllRamSupported = false; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "divIDE 57 (R\" Gal)") { machine.HDType = HDDIVIDE; machine.divIDEAllRamSupported = true; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple +3e 8-Bit")    machine.HDType = HDSIMPLE3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "AceCF")               machine.HDType = HDACECF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE CF")       machine.HDType = HDSIMPLECF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE 8-Bit")    machine.HDType = HDSIMPLE8BIT;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE 16-Bit")   machine.HDType = HDSIMPLE16BIT;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "MWCFIde")             machine.HDType = HDSIMPLECF;

        machine.divIDEJumperEClosed    = Form1->divIDEJumperEClosed->Checked;
        machine.zxcfUploadJumperClosed = Form1->ZXCFUploadJumperClosed->Checked;
        machine.simpleIdeRomEnabled    = Form1->SimpleIdeRomEnabled->Checked;

        switch (ZXCFRAM->ItemIndex)
        {
        case 0: machine.ZXCFRAMSize = 128/16;  break;
        case 1: machine.ZXCFRAMSize = 512/16;  break;
        case 2: machine.ZXCFRAMSize = 1024/16; break;
        }
}

void THW::ConfigureFDC()
{
        machine.floppytype = FLOPPYNONE;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "DISCiPLE")       machine.floppytype = FLOPPYDISCIPLE;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "+D")             machine.floppytype = FLOPPYPLUSD;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "+3")             machine.floppytype = FLOPPYPLUS3;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "Opus Discovery") machine.floppytype = FLOPPYOPUSD;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "Beta Disk 128")  machine.floppytype = FLOPPYBETA;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "ZX Interface 1") machine.floppytype = FLOPPYIF1;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "ZX1541")         machine.floppytype = FLOPPYZX1541;
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "Larken")         machine.floppytype = FLOPPYLARKEN81;

        switch(DriveAType->ItemIndex)
        {
        case 0: machine.driveatype = DRIVENONE; break;
        case 1: machine.driveatype = DRIVE3INCHSS; break;
        case 2: machine.driveatype = DRIVE3INCHDS; break;
        case 3: machine.driveatype = DRIVE35INCHDS; break;
        case 4: machine.driveatype = DRIVE35INCHDS; break;
        }

        switch(DriveBType->ItemIndex)
        {
        case 0: machine.drivebtype = DRIVENONE; break;
        case 1: machine.drivebtype = DRIVE3INCHSS; break;
        case 2: machine.drivebtype = DRIVE3INCHDS; break;
        case 3: machine.drivebtype = DRIVE35INCHDS; break;
        case 4: machine.drivebtype = DRIVE35INCHDS; break;
        }

        IF1->MDVNoDrives = NoMicrodrivesComboBox->Text.ToInt();
        Form1->Interface1Ports->Enabled = (machine.floppytype == FLOPPYIF1);

        Form1->DiskDrives1->Enabled = true;
        P3Drive->FormShow(NULL);
}

void THW::ConfigureMachineSettings()
{
        machine.protectROM = ProtectROM->Checked;
        machine.zxprinter = ZXPrinter->Checked;
        machine.NTSC = NTSC->Checked;

        machine.clockspeed = 3250000;
        machine.tperscanline = 207;
        machine.scanlines = machine.NTSC ? 262:310;
        machine.tperframe = machine.tperscanline * machine.scanlines;
        if (emulator.machine == MACHINEZX80)
        {
                machine.tperframe -= 3;
        }

        if (emulator.machine == MACHINELAMBDA)
                machine.tperscanline = 208;

        switch(emulator.machine)
        {
        case MACHINEACE:
                machine.initialise = ace_initialise;
                machine.do_scanline = ace_do_scanline;
                machine.writebyte = ace_writebyteProxy;
                machine.setbyte = ace_writebyte;
                machine.readbyte = ace_readbyte;
                machine.readoperandbyte = ace_readoperandbyte;
                machine.getbyte = ace_readbyte;
                machine.opcode_fetch = ace_opcode_fetch;
                machine.readport = ace_readport;
                machine.writeport = ace_writeport;
                machine.contendmem = ace_contend;
                machine.contendio = ace_contend;
                machine.reset = ace_reset;
                machine.exit = NULL;
                break;

        case MACHINESPECTRUM:
                machine.initialise = spec48_initialise;
                machine.do_scanline = spec48_do_scanline;
                machine.writebyte = spec48_writebyteProxy;
                machine.setbyte = spec48_setbyte;
                machine.readbyte = spec48_readbyte;
                machine.readoperandbyte = spec48_readoperandbyte;
                machine.getbyte = spec48_getbyte;
                machine.opcode_fetch = spec48_opcode_fetch;
                machine.readport = spec48_readport;
                machine.writeport = spec48_writeport;
                machine.contendmem = spec48_contend;
                machine.contendio = spec48_contendio;
                machine.reset = spec48_reset;
                machine.exit = spec48_exit;

                if (spectrum.model == SPECCY16 || spectrum.model == SPECCY48 || spectrum.model == SPECCYPLUS || spectrum.model == SPECCYTC2048)
                {
                        machine.clockspeed = 3500000;
                        machine.tperscanline = 224;
                        spectrum.interruptPosition = 14336;
                        machine.scanlines = 312;
                        machine.fps = 50;
                        machine.tperframe = machine.tperscanline * machine.scanlines;
                }
                else if (spectrum.model == SPECCYTC2068)
                {
                        machine.clockspeed = 3528000;
                        machine.tperscanline = 226;
                        spectrum.interruptPosition = 13560;
                        machine.scanlines = 312;
                        machine.fps = 50;
                        machine.tperframe = machine.tperscanline * machine.scanlines;
                }
                else if (spectrum.model == SPECCYTS2068)
                {
                        machine.clockspeed = 3528000;
                        machine.tperscanline = 226;
                        spectrum.interruptPosition = 10848;
                        machine.scanlines = 262;
                        machine.fps = 60;
                        machine.tperframe = machine.tperscanline * machine.scanlines;
                }
                else
                {
                        machine.clockspeed = 3546900;
                        machine.tperscanline = 228;
                        spectrum.interruptPosition = 14336-228+32;
                        machine.scanlines = 311;
                        machine.fps = 50;
                        machine.tperframe = machine.tperscanline * machine.scanlines;
                }
                break;
        default:
                machine.initialise = zx81_initialise;
                machine.do_scanline = (emulator.machine == MACHINEZX80) ? zx80_do_scanline : zx81_do_scanline;
                machine.writebyte = zx81_writebyteProxy;
                machine.setbyte = zx81_setbyte;
                machine.readbyte = zx81_readbyte;
                machine.readoperandbyte = zx81_readoperandbyte;
                machine.getbyte = zx81_getbyte;
                machine.opcode_fetch = zx81_opcode_fetch;
                machine.readport = zx81_readport;
                machine.writeport = zx81_writeport;
                machine.contendmem = zx81_contend;
                machine.contendio = zx81_contend;
                machine.fps = machine.NTSC ? 60 : 50; // may be overwritten later
                machine.reset = zx81_reset;
                machine.exit = NULL;
                break;
        }
}

AnsiString THW::DetermineRomBase()
{
        AnsiString romBase = emulator.cwd;
        romBase += romsFolder;
        AnsiString rom = romBase + machine.CurRom;

        if (!FileExists(machine.CurRom) && !FileExists(rom))
        {
                romBase = emulator.cwd;
                romBase += romsFolder;
                romBase += replacementRomsFolder;
        }

        return romBase;
}

void THW::ConfigureSymbolFile(AnsiString romBase)
{
        AnsiString sym = romBase;
        sym += ChangeFileExt(machine.CurRom, ".sym");
        symbolstore::reset();
        symbolstore::loadROMSymbols(sym.c_str());
        SymbolBrowser->RefreshContent();
}

void THW::ConfigureCharacterBitmapFile(AnsiString romBase)
{
        AnsiString bmp = romBase;
        bmp += ChangeFileExt(machine.CurRom, ".bmp");
        delete (Graphics::TBitmap*)machine.cset;
        machine.cset = NULL;
        if (!FileExists(bmp))
        {
                // Select character set file based on the machine model
                switch(emulator.machine)
                {
                case MACHINEZX80:
                        bmp = romBase + "zx80.bmp";
                        break;
                case MACHINETS1000:
                case MACHINEZX81:
                        bmp = romBase + "zx81.edition3.bmp";
                        break;
                case MACHINEACE:
                        bmp = romBase + "jupiterace.bmp";
                        break;
                case MACHINETS1500:
                        bmp = romBase + "ts1500.bmp";
                        break;
                case MACHINER470:
                        bmp = romBase + "ringo470.bmp";
                        break;
                case MACHINETK85:
                        bmp = romBase + "tk85.bmp";
                        break;
                case MACHINELAMBDA:
                        bmp = romBase + "lambda.bmp";
                        break;
                case MACHINESPECTRUM:
                        switch (spectrum.model)
                        {
                        case SPECCY128:
                                bmp = romBase + "spectrum128.bmp";
                                break;
                        case SPECCYTC2048:
                                bmp = romBase + "tc2048.bmp";
                                break;
                        case SPECCYTC2068:
                        case SPECCYTS2068:
                                bmp = romBase + "ts2068.bmp";
                                break;
                        case SPECCY48:
                        case SPECCY16:
                        case SPECCYPLUS:
                                bmp = romBase + "spectrum48.bmp";
                                break;
                        case SPECCYPLUS2:
                                bmp = romBase + "spectrum+2.bmp";
                                break;
                        case SPECCYPLUS2A:
                        case SPECCYPLUS3:
                                bmp = romBase + "spectrum+3.version4-1.bmp";
                                break;
                        }
                        break;
                }
        }

        if (FileExists(bmp))
        {
                Graphics::TBitmap* cset = new Graphics::TBitmap;
                cset->LoadFromFile(bmp);
                machine.cset = cset;
        }
}

AnsiString THW::DirectSoundError(unsigned int errorCode)
{
        AnsiString errorText;
        switch (errorCode)
        {
        case 0x8878000A:
                errorText = "DSERR_ALLOCATED:\n\nThe request failed because resources, such as a priority level, were already in use by another caller.";
                break;
        case 0x80070057:
                errorText = "DSERR_INVALIDPARAM:\n\nAn invalid parameter was passed to the returning function.";
                break;
        case 0x80040110:
                errorText = "DSERR_NOAGGREGATION:\n\nThe object does not support aggregation.";
                break;
        case 0x88780078:
                errorText = "DSERR_NODRIVER:\n\nNo sound driver is available for use, or the given GUID is not a valid DirectSound device ID.";
                break;
        case 0x00000007:
                errorText = "DSERR_OUTOFMEMORY:\n\nThe DirectSound subsystem could not allocate sufficient memory to complete the caller's request.";
                break;
        default:
                errorText = "0x" + AnsiString::IntToHex(errorCode, 8);
                break;
        }

        return errorText;
}

void THW::ResetDisplaySize()
{
        if (Form1->N1001->Checked)
        {
                Form1->N2001Click(NULL);
                Form1->N1001Click(NULL);
        }
        else if (Form1->N2001->Checked)
        {
                Form1->N1001Click(NULL);
                Form1->N2001Click(NULL);
        }
        else if (Form1->N4001->Checked)
        {
                Form1->N1001Click(NULL);
                Form1->N4001Click(NULL);
        }
        
        if (Form1->None1->Checked)
        {
                Form1->Small1Click(NULL);
                Form1->None1Click(NULL);
        }
        else if (Form1->Small1->Checked)
        {
                Form1->None1Click(NULL);
                Form1->Small1Click(NULL);
        }
        else if (Form1->Normal1->Checked)
        {
                Form1->None1Click(NULL);
                Form1->Normal1Click(NULL);
        }
        else if (Form1->Large1->Checked)
        {
                Form1->None1Click(NULL);
                Form1->Large1Click(NULL);
        }
        else if (Form1->FullImage1->Checked)
        {
                Form1->None1Click(NULL);
                Form1->FullImage1Click(NULL);
        }
}
//---------------------------------------------------------------------------
void THW::SetZX80Icon()
{
        AnsiString romName;

        if (ZX80Btn->Down)
        {
                romName = RomBox->Text;
        }
        else
        {
                romName = emulator.ROM80;
        }

        AnsiString romRoot = LowerCase(getMachineRoot(romName));
        Graphics::TBitmap* zx80Icon = new Graphics::TBitmap;
        int iconIndex = (romRoot== "zx81" || romRoot == "ts1500") ? 1 : 0;
        ZX80Icons->GetBitmap(iconIndex, zx80Icon);
        ZX80Btn->InactiveGlyph = zx80Icon;
}
//---------------------------------------------------------------------------
void THW::SetSpectrum128Icon()
{
        AnsiString romName;

        if (Spec128Btn->Down)
        {
                romName = RomBox->Text;
        }
        else
        {
                romName = emulator.ROMSP128;
        }

        Graphics::TBitmap* spec128Icon = new Graphics::TBitmap;
        int iconIndex = (romName== "spectrum128.spanish.rom") ? 1 : 0;
        Spec128Icons->GetBitmap(iconIndex, spec128Icon);
        Spec128Btn->InactiveGlyph = spec128Icon;
}
//---------------------------------------------------------------------------

void THW::SetupForZX81(void)
{
        int i;
        AnsiString OldIDE, OldFloppy;

        ZX80Btn->Down = false;
        ZX81Btn->Down = false;
        Spec16Btn->Down = false;
        Spec48Btn->Down = false;
        SpecPlusBtn->Down = false;
        Spec128Btn->Down = false;
        SpecP2Btn->Down = false;
        SpecP2aBtn->Down = false;
        SpecP3Btn->Down = false;
        TS1000Btn->Down = false;
        TS1500Btn->Down = false;
        TC2048Btn->Down = false;
        TC2068Btn->Down = false;
        TS2068Btn->Down = false;
        LambdaBtn->Down = false;
        R470Btn->Down = false;
        TK85Btn->Down = false;
        AceBtn->Down = false;
        ZX97LEBtn->Down = false;

        Form1->SelectJoystick2->Visible = false;
        Form1->ConnectJoystick2->Visible = false;
        Form1->EnableJoystick2AutoFire->Visible = false;
        Form1->divIDEJumperEClosed->Visible = false;
        Form1->ZXCFUploadJumperClosed->Visible = false;
        Form1->SimpleIdeRomEnabled->Visible = false;
        Form1->ConnectSpectrum128Keypad->Visible = false;
        Form1->SpectraColourEnable->Visible = false;

        Form1->ResetMemotechHRG->Visible = true;
        Form1->ResetQuicksilvaHiRes->Visible = true;
        Form1->QSChrEnable->Visible = true;
        Form1->ChromaColourEnable->Visible = true;
        Form1->SwitchOnMemocalc->Visible = true;
//        Form1->SwitchOnMemotext->Visible = true;
        Form1->SwitchOnZ80Assembler->Visible = true;
        Form1->WriteProtect8KRAM->Visible = true;
        Form1->ResetSpeech->Visible = true;

        Form1->ConnectSpectrum128Keypad->Checked = false;

        FloatingPointHardwareFix->Enabled = false;
        ButtonAdvancedMore->Visible = false;

        EnableRomCartridgeOption(true);

        ZXpand->Caption = "ZXpand+";
        Multiface->Caption = "Multiface 128";

        KMouse->Checked = false;
        KMouse->Enabled = false;

        OldFloppy = FDCBox->Items->Strings[FDCBox->ItemIndex];
        while(FDCBox->Items->Count > 1) FDCBox->Items->Delete(FDCBox->Items->Count - 1);
        FDCBox->Items->Strings[0] = "None";
        FDCBox->Items->Add("ZX1541");
        FDCBox->Items->Add("Larken");
        FDCBox->ItemIndex = 0;
        FDCRomBox->ItemIndex = 0;

        for (i = 0; i < FDCBox->Items->Count; i++)
                if (FDCBox->Items->Strings[i] == OldFloppy) FDCBox->ItemIndex = i;

        LabelFDC->Enabled = true;
        FDCBox->Enabled = true;
        FDCBoxChange(NULL);

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count - 1] == "96K")
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count - 1] != "48K")
        {
                RamPackBox->Items->Add("48K");
        }

        if (NewMachine == MACHINETS1500 || NewMachine == MACHINER470 || NewMachine == MACHINETK85)
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        RamPackLbl->Enabled = true; RamPackBox->Enabled = true;
        RamPackBox->ItemIndex = defaultRamPackIndex;
        DisplayTotalRam();

        ChrGenBox->Items->Strings[0] = "Sinclair";
        ChrGenLbl->Enabled = true; ChrGenBox->Enabled = true;
        if (ChrGenBox->ItemIndex == -1) ChrGenBox->ItemIndex = 0;

        if ((!HiResBox->Enabled) || (HiResBox->ItemIndex == -1))
        {
                HiResBox->Items->Strings[0] = "None";
                HiResBox->ItemIndex = 0;
        }
        HiResLbl->Enabled = true; HiResBox->Enabled = true;

        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->Items->Add("Chroma");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;
        ColourLabel->Enabled = true;
        ColourLabel->Caption = "Colour:";

        ProtectROM->Enabled = true;
        NTSC->Enabled = true;
        EnableLowRAM->Enabled = true;
        M1Not->Enabled = true;
        ImprovedWait->Enabled = true;
        TS2050->Enabled = true;
        TS2050Config->Enabled = TS2050->Enabled && TS2050->Checked;

        Issue2->Checked = false;
        Issue2->Enabled = false;

        if (!RamPackBox->Visible)
        {
                Height += RamPackHeight;
                RamPackLbl->Visible = true;
                RamPackBox->Visible = true;
                LabelTotalRAM->Visible = true;
        }

        Multiface->Enabled = false;
        Multiface->Checked = false;

        OldIDE = IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("MWCFIde");
        IDEBox->ItemIndex = 0;
        IDEBox->Enabled = true;
        IDERomBox->ItemIndex = 0;
        LabelIDE->Enabled = true;

        for (i = 0; i < IDEBox->Items->Count; i++)
        {
                if (IDEBox->Items->Strings[i] == OldIDE) IDEBox->ItemIndex = i;
        }

        SoundCardBox->Items->Clear();
        SoundCardBox->Items->Add("None");
        SoundCardBox->Items->Add("ZON X");
        SoundCardBox->Items->Add("Quicksilva");
        SoundCardBox->ItemIndex = 0;
        SoundCardBox->Enabled = true;
        SoundCardLbl->Enabled = true;

        SpeechBox->Items->Clear();
        SpeechBox->Items->Add("None");
        SpeechBox->Items->Add("The Parrot");
        SpeechBox->Items->Add("Sweet Talker");
        SpeechBox->Items->Add("Mageco");
        SpeechBox->Items->Add("S-Pack");
        if (NewMachine != MACHINEZX80)
        {
                SpeechBox->Items->Add("Talk-Back");
        }
        SpeechBox->ItemIndex = 0;
        SpeechBox->Enabled = true;
        SpeechBoxLbl->Enabled = true;

        JoystickBox->Items->Clear();
        JoystickBox->Items->Add("None");
        if (NewMachine != MACHINEZX80)
        {
                JoystickBox->Items->Add("Cursor");
        }
        JoystickBox->Items->Add("Kempston");
        if (NewMachine != MACHINEZX80)
        {
                JoystickBox->Items->Add("Programmable");
        }
        JoystickBox->ItemIndex = 0;
        JoystickBox->Enabled = true;
        JoystickBoxLabel->Enabled = true;
        JoystickUpBox->Text    = "-";
        JoystickDownBox->Text  = "-";
        JoystickLeftBox->Text  = "-";
        JoystickRightBox->Text = "-";
        JoystickFireBox->Text  = "-";
        JoystickUpBox->Enabled    = false;
        JoystickDownBox->Enabled  = false;
        JoystickLeftBox->Enabled  = false;
        JoystickRightBox->Enabled = false;
        JoystickFireBox->Enabled  = false;
        JoystickUpBoxLabel->Enabled    = false;
        JoystickDownBoxLabel->Enabled  = false;
        JoystickLeftBoxLabel->Enabled  = false;
        JoystickRightBoxLabel->Enabled = false;
        JoystickFireBoxLabel->Enabled  = false;
        Form1->ConnectJoystick2->Checked = false;
        Form1->EnableJoystick2AutoFire->Checked = false;

        uSource->Enabled = false;
        uSource->Checked = false;

        SpecDrum->Checked = false;
        SpecDrum->Enabled = false;

        Z80Assembler->Enabled = Z80Assembler->Checked || (!Memocalc->Checked && !Memotext->Checked);
        Memocalc->Enabled     = Memocalc->Checked || (!Z80Assembler->Checked && !Memotext->Checked);
//        Memotext->Enabled     = Memotext->Checked || (!Z80Assembler->Checked && !Memocalc->Checked);

        RomCartridgeBox->Items->Clear();
        RomCartridgeBox->Items->Add("None");

        if (NewMachine != MACHINETS1500)
        {
                RomCartridgeBox->Items->Add("Standard 16K");
                RomCartridgeBox->Items->Add("ZXC1");
                RomCartridgeBox->Items->Add("ZXC2");
                RomCartridgeBox->Items->Add("ZXC3");
                RomCartridgeBox->Items->Add("ZXC4");
        }

        if (NewMachine != MACHINEZX80)
        {
                RomCartridgeBox->Items->Add("Timex");
        }

        RomCartridgeBox->ItemIndex = ROMCARTRIDGENONE;

        UpdateRomCartridgeControls(NewMachine, NULL);

        SetZX80Icon();
        SetSpectrum128Icon();

        Form1->Sound1->Caption = "Video So&und";
        const bool zx81 = true;
        MidiForm->SetComputer(zx81);
}

void THW::SetZXpandState(bool checked, bool enabled)
{
        ZXpand->Checked = checked;
        ZXpand->Enabled = enabled;
        ZXpandEmulationInfo->Visible = checked;
        ButtonZXpandSDCard->Enabled = checked && enabled;
}

void THW::SetupForSpectrum(void)
{
        AnsiString OldIDE, OldFDC;
        int i;

        ZX80Btn->Down = false;
        ZX81Btn->Down = false;
        Spec16Btn->Down = false;
        Spec48Btn->Down = false;
        SpecPlusBtn->Down = false;
        Spec128Btn->Down = false;
        SpecP2Btn->Down = false;
        SpecP2aBtn->Down = false;
        SpecP3Btn->Down = false;
        TS1000Btn->Down = false;
        TS1500Btn->Down = false;
        TC2048Btn->Down = false;
        TC2068Btn->Down = false;
        TS2068Btn->Down = false;
        LambdaBtn->Down = false;
        R470Btn->Down = false;
        TK85Btn->Down = false;
        AceBtn->Down = false;
        ZX97LEBtn->Down = false;

        Form1->SelectJoystick2->Visible = true;
        Form1->ConnectJoystick2->Visible = true;
        Form1->EnableJoystick2AutoFire->Visible = true;
        Form1->divIDEJumperEClosed->Visible = true;
        Form1->ZXCFUploadJumperClosed->Visible = true;
        Form1->SimpleIdeRomEnabled->Visible = true;
        Form1->ConnectSpectrum128Keypad->Visible = true;
        Form1->SpectraColourEnable->Visible = true;
        Form1->ResetSpeech->Visible = true;

        Form1->ResetMemotechHRG->Visible = false;
        Form1->ResetQuicksilvaHiRes->Visible = false;
        Form1->QSChrEnable->Visible = false;
        Form1->ChromaColourEnable->Visible = false;
        Form1->SwitchOnMemocalc->Visible = false;
//        Form1->SwitchOnMemotext->Visible = false;
        Form1->SwitchOnZ80Assembler->Visible = false;
        Form1->WriteProtect8KRAM->Visible = false;

        FloatingPointHardwareFix->Enabled = false;
        FloatingPointHardwareFix->Checked = false;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        machine.plus3arabicPagedOut = 0;

        KMouse->Enabled = true;
        
        ButtonAdvancedMore->Visible = false;

        SetZXpandState(false, false);
        ZXpand->Caption = "ZXpand+";
        if (NewSpec == SPECCYPLUS2A || NewSpec == SPECCYPLUS3)
        {
                Multiface->Caption = "Multiface 3";
        }
        else
        {
                Multiface->Caption = "Multiface 128";
        }

        EnableRomCartridgeOption(true);

        OldFDC = FDCBox->Items->Strings[FDCBox->ItemIndex];
        while(FDCBox->Items->Count > 1) FDCBox->Items->Delete(FDCBox->Items->Count - 1);
        
        FDCBox->Items->Strings[0] = "None";
        FDCBox->Items->Add("ZX Interface 1");
        if (NewSpec != SPECCYTC2068 && NewSpec != SPECCYTS2068)
        {
                FDCBox->Items->Add("Beta Disk 128");
                FDCBox->Items->Add("Opus Discovery");
                FDCBox->Items->Add("DISCiPLE");
                FDCBox->Items->Add("+D");
        }

        FDCBox->ItemIndex = 0;
        for (i = 0; i < FDCBox->Items->Count; i++)
        {
                if (FDCBox->Items->Strings[i] == OldFDC)
                {
                        FDCBox->ItemIndex = i;
                        break;
                }
        }

        FDCRomBox->ItemIndex = 0;
        LabelFDC->Enabled = true;

        FDCBox->Enabled = true;
        FDCBoxChange(NULL);

        SoundCardBox->Items->Clear();
        SoundCardBox->Items->Add("None");
        SoundCardBox->ItemIndex = 0;
        SoundCardBox->Enabled = false;
        SoundCardLbl->Enabled = false;
        if (NewSpec >= SPECCY128 || NewSpec == SPECCY16 || NewSpec == SPECCY48 || NewSpec == SPECCYPLUS || NewSpec == SPECCYTC2048)
        {
                SoundCardBox->Items->Add("Sinclair 128K");
                SoundCardBox->Items->Add("Fuller Box");
                SoundCardBox->Items->Add("dk'tronics");
                SoundCardBox->Items->Add("ZON X");
                SoundCardBox->Enabled = true;
                SoundCardLbl->Enabled = true;
        }
        else if (NewSpec == SPECCYTC2068 || NewSpec == SPECCYTS2068)
        {
                SoundCardBox->Items->Add("Timex");
        }

        SpeechBox->Items->Clear();
        SpeechBox->Items->Add("None");
        SpeechBox->Items->Add("Sweet Talker");
        SpeechBox->Items->Add("dk'tronics");
        SpeechBox->Items->Add("Fuller Orator");
        SpeechBox->Items->Add("S-Pack");
        SpeechBox->ItemIndex = 0;
        SpeechBox->Enabled = true;
        SpeechBoxLbl->Enabled = true;

        JoystickBox->Items->Clear();
        JoystickBox->Items->Add("None");
        if (NewSpec != SPECCYTS2068 && NewSpec != SPECCYTC2068)
        {
                JoystickBox->Items->Add("Cursor (Protek)");
                if (NewSpec == SPECCY16 || NewSpec == SPECCY48 || NewSpec == SPECCYPLUS || NewSpec == SPECCY128 || NewSpec == SPECCYTC2048)
                {
                        JoystickBox->Items->Add("ZX Interface 2");
                }
                else
                {
                        JoystickBox->Items->Add("Sinclair");
                }
                JoystickBox->Items->Add("Kempston");
                JoystickBox->Items->Add("Fuller");
                JoystickBox->ItemIndex = 0;
        }
        else
        {
                JoystickBox->Items->Add("Timex");
                JoystickBox->Items->Add("Kempston");
                JoystickBox->ItemIndex = 1;
        }
        JoystickBox->Items->Add("Programmable");
        JoystickBox->Enabled = true;
        JoystickBoxLabel->Enabled = true;
        JoystickUpBox->Text    = "-";
        JoystickDownBox->Text  = "-";
        JoystickLeftBox->Text  = "-";
        JoystickRightBox->Text = "-";
        JoystickFireBox->Text  = "-";
        JoystickUpBox->Enabled    = false;
        JoystickDownBox->Enabled  = false;
        JoystickLeftBox->Enabled  = false;
        JoystickRightBox->Enabled = false;
        JoystickFireBox->Enabled  = false;
        JoystickUpBoxLabel->Enabled    = false;
        JoystickDownBoxLabel->Enabled  = false;
        JoystickLeftBoxLabel->Enabled  = false;
        JoystickRightBoxLabel->Enabled = false;
        JoystickFireBoxLabel->Enabled  = false;

        uSource->Checked = false;
        uSource->Enabled = false;

        SpecDrum->Checked = false;
        SpecDrum->Enabled = true;

        Form1->ConnectSpectrum128Keypad->Enabled = false;

        RamPackLbl->Enabled = false; RamPackBox->Enabled = false;
        RamPackBox->ItemIndex = -1;

        ChrGenBox->Items->Strings[0] = "Sinclair";
        ChrGenBox->ItemIndex = 0;
        ChrGenLbl->Enabled = false; ChrGenBox->Enabled = false;

        HiResBox->Items->Strings[0] = "Sinclair";
        HiResBox->ItemIndex = 0;
        HiResLbl->Enabled = false; HiResBox->Enabled = false;

        ColourLabel->Caption = "Colour:";

        bool spectraAlreadyAvailable = (ColourBox->Items->Strings[1] == "Spectra");
        if (!spectraAlreadyAvailable)
        {
                while(ColourBox->Items->Count>1) ColourBox->Items->Delete(ColourBox->Items->Count-1);
                ColourBox->Items->Strings[0] = "Sinclair";
                ColourBox->Items->Add("Spectra");
                ColourBox->ItemIndex = 0;
                ColourBox->Enabled = true;
                ColourLabel->Enabled = true;
        }

        ProtectROM->Enabled = true;
        NTSC->Enabled = false;
        NTSC->Checked = false;
        EnableLowRAM->Enabled = false;
        EnableLowRAM->Checked = false;
        M1Not->Enabled = false;
        M1Not->Checked = false;
        ImprovedWait->Enabled = false;
        ImprovedWait->Checked = false;
        TS2050->Enabled = true;
        TS2050Config->Enabled = TS2050->Enabled && TS2050->Checked;

        Issue2->Checked = false;
        Issue2->Enabled = false;

        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible = false;
                RamPackBox->Visible = false;
                LabelTotalRAM->Visible = false;
        }
        Multiface->Enabled = true;

        OldIDE = IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("Simple +3e 8-Bit");
        IDEBox->Items->Add("divIDE 57 (R Gal)");
        IDEBox->Items->Add("divIDE 57 (R\" Gal)");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Simple IDE CF");
        IDEBox->Items->Add("Simple IDE 8-Bit");
        IDEBox->Items->Add("Simple IDE 16-Bit");
        IDEBox->ItemIndex = 0;
        IDEBox->Enabled = true;
        IDERomBox->ItemIndex = 0;
        LabelIDE->Enabled = true;

        for (i = 0; i < IDEBox->Items->Count; i++)
                if (IDEBox->Items->Strings[i] == OldIDE) IDEBox->ItemIndex = i;

        uSource->Enabled = true;

        RomCartridgeBox->Items->Clear();
        RomCartridgeBox->Items->Add("None");

        if (NewSpec != SPECCYTS2068 && NewSpec != SPECCYTC2068)
        {
                RomCartridgeBox->Items->Add("Sinclair");
                RomCartridgeBox->Items->Add("ZXC1");
                RomCartridgeBox->Items->Add("ZXC2");
                RomCartridgeBox->Items->Add("ZXC3");
                RomCartridgeBox->Items->Add("ZXC4");
        }
        else
        {
                RomCartridgeBox->Items->Add("Timex");
        }

        RomCartridgeBox->ItemIndex = ROMCARTRIDGENONE;

        UpdateRomCartridgeControls(NewMachine, NewSpec);

        SetZX80Icon();
        SetSpectrum128Icon();

        Form1->Sound1->Caption = "Beeper So&und";
        const bool zx81 = false;
        MidiForm->SetComputer(zx81);
}
//---------------------------------------------------------------------------

void THW::ConfigureDefaultRamSettings()
{
        machine.baseRamSize = baseRamSize;
        machine.ramPackSupplementsInternalRam = ramPackSupplementsInternalRam;
        machine.defaultRamPackIndex = defaultRamPackIndex;
}
//---------------------------------------------------------------------------

void THW::RefreshDefaultRamSettings()
{
        switch (NewMachine)
        {
        case MACHINEACE:
                baseRamSize = 3;
                ramPackSupplementsInternalRam = true;
                defaultRamPackIndex = 4;
                break;

        case MACHINESPECTRUM:
                if (NewSpec == SPECCY16 || NewSpec == SPECCY48 || NewSpec == SPECCYPLUS || NewSpec == SPECCYTC2048)
                {
                        baseRamSize = (NewSpec == SPECCY16) ? 16 : 48;
                        ramPackSupplementsInternalRam = true;
                }
                else if (NewSpec == SPECCYTS2068 || NewSpec == SPECCYTC2068)
                {
                        baseRamSize = 48;
                        ramPackSupplementsInternalRam = false;
                }
                else
                {
                        baseRamSize = 128;
                        ramPackSupplementsInternalRam = false;
                }
                break;
        default:
                if (NewMachine == MACHINETS1000 || NewMachine == MACHINELAMBDA)
                {
                        baseRamSize = 2;
                        ramPackSupplementsInternalRam = false;
                        defaultRamPackIndex = 4;
                }
                else if (NewMachine == MACHINETS1500 || NewMachine == MACHINETK85 || NewMachine == MACHINER470)
                {
                        baseRamSize = 16;
                        ramPackSupplementsInternalRam = true;
                        defaultRamPackIndex = 0;
                }
                else
                {
                        baseRamSize = 1;
                        ramPackSupplementsInternalRam = false;
                        defaultRamPackIndex = 4;
                }
                break;
        }
}
//---------------------------------------------------------------------------

void THW::DisplayTotalRam()
{
        int totalRam = baseRamSize;
        if (RamPackBox->ItemIndex > 0)
        {
                int ramPack = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                totalRam = ramPackSupplementsInternalRam ? totalRam + ramPack : ramPack;
        }
        AnsiString ramSize = totalRam;
        LabelTotalRAM->Caption = "Total RAM: " + ramSize + "K";
}
//---------------------------------------------------------------------------

void THW::LoadFdcRomBox()
{
        FDCRomBox->Clear();
        FDCRomBox->Visible = true;
        FDCRomBoxLabel->Visible = true;
        FDCRomBoxBrowse->Visible = true;

        if (FDCBox->Text == "ZX Interface 1")
        {
                FDCRomBox->Items->Add("interface1.edition1.rom");
                FDCRomBox->Items->Add("interface1.edition2.rom");
                FDCRomBox->Text = emulator.ROMINTERFACE1;
        }
        else if (FDCBox->Text == "Beta Disk 128")
        {
                FDCRomBox->Items->Add("beta128.trdos.v5-01.rom");
                FDCRomBox->Items->Add("beta128.trdos.v5-02.rom");
                FDCRomBox->Items->Add("beta128.trdos.v5-03.rom");
                FDCRomBox->Text = emulator.ROMBETADISK;
        }
        else if (FDCBox->Text == "Opus Discovery")
        {
                FDCRomBox->Items->Add("discovery.v2-1.rom");
                FDCRomBox->Items->Add("discovery.v2-2.rom");
                FDCRomBox->Items->Add("discovery.v2-22.rom");
                FDCRomBox->Text = emulator.ROMDISCOVERY;
        }
        else if (FDCBox->Text == "DISCiPLE")
        {
                FDCRomBox->Items->Add("disciple.gdos.v2b.rom");
                FDCRomBox->Items->Add("disciple.gdos.v3.rom");
                FDCRomBox->Text = emulator.ROMDISCIPLE;
        }
        else if (FDCBox->Text == "+D")
        {
                FDCRomBox->Items->Add("+d.g+dos.v1-0.rom");
                FDCRomBox->Items->Add("+d.g+dos.v1-a.rom");
                FDCRomBox->Text = emulator.ROMPLUSD;
        }
        else if (FDCBox->Text == "Larken")
        {
                FDCRomBox->Items->Add("larken81.rom");
                FDCRomBox->Text = emulator.ROMLARKEN81;
        }
        else
        {
                FDCRomBox->Text = "";
                FDCRomBox->Visible = false;
                FDCRomBoxLabel->Visible = false;
                FDCRomBoxBrowse->Visible = false;
        }

        FDCRomBox->SelStart = FDCRomBox->Text.Length() - 1;
        FDCRomBox->SelLength = 0;

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void THW::LoadIdeRomBox()
{
        IDERomBox->Clear();
        IDERomBox->Visible = true;
        IDERomBoxLabel->Visible = true;
        IDERomBoxBrowse->Visible = true;

        if (IDEBox->Text == "Simple IDE 8-Bit")
        {
                IDERomBox->Items->Add("zx8blbs.rom");
                IDERomBox->Text = emulator.ROMSIMPLE8BIT;
        }
        else if (IDEBox->Text == "Simple IDE 16-Bit")
        {
                IDERomBox->Items->Add("zxidelbs.rom");
                IDERomBox->Text = emulator.ROMSIMPLE16BIT;
        }
        else if (IDEBox->Text == "Simple IDE CF")
        {
                IDERomBox->Items->Add("zxcflba.rom");
                IDERomBox->Text = emulator.ROMSIMPLECF;
        }
        else if (IDEBox->Text == "MWCFIde")
        {
                IDERomBox->Items->Add("mwcfide.rom");
                IDERomBox->Text = emulator.ROMMWCFIDE;
        }
        else
        {
                IDERomBox->Text = "";
                IDERomBox->Visible = false;
                IDERomBoxLabel->Visible = false;
                IDERomBoxBrowse->Visible = false;
        }

        IDEBox->SelStart = IDEBox->Text.Length() - 1;
        IDEBox->SelLength = 0;

        bool plus3eSupported = (NewMachine == MACHINESPECTRUM && NewSpec >= SPECCYPLUS2A);

        if (plus3eSupported)
        {
                if (IDEBox->Items->Strings[IDEBox->ItemIndex] != "None")
                {
                        AnsiString required = (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple +3e 8-Bit") ? "required" : "supported";
                        AnsiString disableJumper = (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple +3e 8-Bit") ? "" : " Disable the on-board ROM using the jumper.";

                        Plus3eNoticeLabel->Caption = "The +3e ROM is " + required + " by this IDE device. Select an appropriate ROM from the Advanced Settings tab." + disableJumper;
                        Plus3eNoticeLabel->Visible = true;
                }
                else
                {
                        Plus3eNoticeLabel->Visible = false;
                }
        }
        else
        {
                Plus3eNoticeLabel->Visible = false;
        }

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void THW::LoadRomBox()
{
        RomBox->Clear();

        switch (NewMachine)
        {
        case MACHINEZX80:
                RomBox->Items->Add("zx80.rom");
                RomBox->Items->Add("zx81.edition1.rom");
                RomBox->Text = emulator.ROM80;
                break;

        case MACHINEZX81:
                RomBox->Items->Add("zx81.edition1.rom");
                RomBox->Items->Add("zx81.edition2.rom");
                RomBox->Items->Add("zx81.edition3.rom");
                RomBox->Items->Add("tree-forth.rom");
                RomBox->Items->Add("zx81-forth.rom");
                RomBox->Items->Add("zx.aszmic.e04.rom");
                RomBox->Items->Add("zx.aszmic.e07.rom");
                RomBox->Text = emulator.ROM81;
                break;

        case MACHINETS1000:
                RomBox->Items->Add("zx81.edition3.rom");
                RomBox->Items->Add("tree-forth.rom");
                RomBox->Text = emulator.ROMTS1000;
                break;

        case MACHINETS1500:
                RomBox->Items->Add("ts1500.rom");
                RomBox->Items->Add("tree-forth.rom");
                RomBox->Text = emulator.ROMTS1500;
                break;
                
        case MACHINELAMBDA:
                RomBox->Items->Add("lambda8300.rom");
                RomBox->Items->Add("lambda8300colour.rom");
                RomBox->Items->Add("pc8300timex.rom");
                RomBox->Text = emulator.ROMLAMBDA;
                break;

        case MACHINER470:
                RomBox->Items->Add("ringo470.rom");
                RomBox->Text = emulator.ROMR470;
                break;
                
        case MACHINETK85:
                RomBox->Items->Add("tk85.rom");
                RomBox->Text = emulator.ROMTK85;
                break;

        case MACHINEACE:
                RomBox->Items->Add("jupiterace.rom");
                RomBox->Text = emulator.ROMACE;
                break;
        
        case MACHINEZX97LE:
                RomBox->Items->Add("zx97.rom");
                RomBox->Text = emulator.ROM97LE;
                break;

        case MACHINESPECTRUM:
                switch (NewSpec)
                {
                case SPECCY16:
                        RomBox->Items->Add("spectrum48.rom");
                        RomBox->Text = emulator.ROMSP16;
                        break;

                case SPECCY48:
                        RomBox->Items->Add("spectrum48.rom");
                        RomBox->Items->Add("spectrum48.nordic.rom");
                        RomBox->Text = emulator.ROMSP48;
                        break;

                case SPECCYPLUS:
                        RomBox->Items->Add("spectrum48.rom");
                        RomBox->Items->Add("spectrum48.spanish.rom");
                        RomBox->Items->Add("spectrum48.arabic.version1.rom");
                        RomBox->Text = emulator.ROMSPP;
                        break;

                case SPECCY128:
                        RomBox->Items->Add("spectrum128.rom");
                        RomBox->Items->Add("spectrum128.spanish.rom");
                        RomBox->Items->Add("spectrum48.arabic.version1.rom");
                        RomBox->Text = emulator.ROMSP128;
                        break;

                case SPECCYPLUS2:
                        RomBox->Items->Add("spectrum+2.rom");
                        RomBox->Items->Add("spectrum+2.french.rom");
                        RomBox->Items->Add("spectrum+2.spanish.rom");
                        RomBox->Items->Add("spectrum48.arabic.version2.rom");
                        RomBox->Text = emulator.ROMSPP2;
                        break;

                case SPECCYPLUS2A:
                        RomBox->Items->Add("spectrum+3.version4-0.rom");
                        RomBox->Items->Add("spectrum+3.version4-1.rom");
                        RomBox->Items->Add("spectrum+3.version4-0.spanish.rom");
                        RomBox->Items->Add("spectrum+3.version4-1.spanish.rom");
                        RomBox->Items->Add("spectrum48.arabic.version2.rom");
                        RomBox->Items->Add("+3e.divide.v1-43.english.rom");
                        RomBox->Items->Add("+3e.divide.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple+3.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple+3.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple8bit.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple8bit.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple16bit.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple16bit.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simplecf.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simplecf.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.zxcf.v1-43.english.rom");
                        RomBox->Items->Add("+3e.zxcf.v1-43.spanish.rom");
                        RomBox->Text = emulator.ROMSPP2A;
                        break;

                case SPECCYPLUS3:
                        RomBox->Items->Add("spectrum+3.version4-0.rom");
                        RomBox->Items->Add("spectrum+3.version4-1.rom");
                        RomBox->Items->Add("spectrum+3.version4-0.spanish.rom");
                        RomBox->Items->Add("spectrum+3.version4-1.spanish.rom");
                        RomBox->Items->Add("spectrum+3.arabic3-a.english4-0.rom");
                        RomBox->Items->Add("+3e.divide.v1-43.english.rom");
                        RomBox->Items->Add("+3e.divide.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple+3.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple+3.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple8bit.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple8bit.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simple16bit.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simple16bit.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.simplecf.v1-43.english.rom");
                        RomBox->Items->Add("+3e.simplecf.v1-43.spanish.rom");
                        RomBox->Items->Add("+3e.zxcf.v1-43.english.rom");
                        RomBox->Items->Add("+3e.zxcf.v1-43.spanish.rom");
                        RomBox->Text = emulator.ROMSPP3;
                        break;

                case SPECCYTC2048:
                        RomBox->Items->Add("tc2048.rom");
                        RomBox->Text = emulator.ROMTC2048;
                        break;

                case SPECCYTC2068:
                        RomBox->Items->Add("ts2068.rom");
                        RomBox->Text = emulator.ROMTC2068;
                        break;

                 case SPECCYTS2068:
                        RomBox->Items->Add("ts2068.rom");
                        RomBox->Text = emulator.ROMTS2068;
                        break;
                }
        }

        RomBox->SelStart = RomBox->Text.Length() - 1;
        RomBox->SelLength = 0;

        RomBoxChange(NULL);
}

//---------------------------------------------------------------------------

void __fastcall THW::ZX80BtnClick(TObject *Sender)
{
        if (ZX80Btn->Down) return;

        NewMachine = MACHINEZX80;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false, true);
        ZXpand->Caption = "ZXpand";
        ZX80Btn->Down = true;
        NewMachineName = ZX80Btn->Caption;
        LoadRomBox();
        ImprovedWait->Enabled = false;
        ImprovedWait->Checked = false;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        IDEBoxChange(NULL);

        Form1->SwitchOnMemocalc->Visible = false;
//        Form1->SwitchOnMemotext->Visible = false;
        Form1->SwitchOnZ80Assembler->Visible = false;

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX81BtnClick(TObject *Sender)
{
        if (ZX81Btn->Down) return;

        NewMachine = MACHINEZX81;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false, true);
        ZX81Btn->Down = true;
        NewMachineName = ZX81Btn->Caption;
        LoadRomBox();
        FloatingPointHardwareFix->Enabled = true;
        NTSC->Checked = false;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec48BtnClick(TObject *Sender)
{
        if (Spec48Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCY48;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec48Btn->Down = true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled = true;

        Issue2->Enabled = true;
        NewMachineName = Spec48Btn->Caption;
        LoadRomBox();

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec128BtnClick(TObject *Sender)
{
        if (Spec128Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCY128;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec128Btn->Down = true;

        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
        Form1->ConnectSpectrum128Keypad->Enabled = true;

        NewMachineName = Spec128Btn->Caption;
        LoadRomBox();

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecPlusBtnClick(TObject *Sender)
{
        if (SpecPlusBtn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYPLUS;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecPlusBtn->Down = true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled = true;

        Issue2->Enabled = true;
        NewMachineName = SpecPlusBtn->Caption;
        LoadRomBox();

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec16BtnClick(TObject *Sender)
{
        if (Spec16Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCY16;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec16Btn->Down = true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled = true;

        Issue2->Enabled = true;
        Issue2->Checked = true;
        NewMachineName = Spec16Btn->Caption;
        LoadRomBox();

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2BtnClick(TObject *Sender)
{
        if (SpecP2Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYPLUS2;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP2Btn->Down = true;

        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
        Form1->ConnectSpectrum128Keypad->Enabled = true;

        JoystickBox->ItemIndex = SelectEntry(JoystickBox, "Sinclair");

        NewMachineName = SpecP2Btn->Caption;
        LoadRomBox();

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2aBtnClick(TObject *Sender)
{
        if (SpecP2aBtn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYPLUS2A;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP2aBtn->Down = true;
        Multiface->Caption = "Multiface 3";

        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
        Form1->ConnectSpectrum128Keypad->Enabled = true;

        JoystickBox->ItemIndex = SelectEntry(JoystickBox, "Sinclair");

        NewMachineName = SpecP2aBtn->Caption;
        LoadRomBox();

        IDEBoxChange(NULL);

        while(FDCBox->Items->Count > 2) FDCBox->Items->Delete(FDCBox->Items->Count - 1);
        FDCBox->Items->Strings[1] = "+3";
        FDCBox->ItemIndex = 0;

        FDCBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP3BtnClick(TObject *Sender)
{
        if (SpecP3Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYPLUS3;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP3Btn->Down = true;
        Multiface->Caption = "Multiface 3";

        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
        Form1->ConnectSpectrum128Keypad->Enabled = true;

        JoystickBox->ItemIndex = SelectEntry(JoystickBox, "Sinclair");

        NewMachineName = SpecP3Btn->Caption;
        LoadRomBox();

        IDEBoxChange(NULL);

        while(FDCBox->Items->Count > 1) FDCBox->Items->Delete(FDCBox->Items->Count - 1);
        FDCBox->Items->Strings[0] = "+3";
        FDCBox->ItemIndex = 0;

        FDCBox->Enabled = false;
        FDCBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1000BtnClick(TObject *Sender)
{
        if (TS1000Btn->Down) return;

        NewMachine = MACHINETS1000;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        TS1000Btn->Down = true;
        NewMachineName = TS1000Btn->Caption;
        FloatingPointHardwareFix->Checked = false;
        LoadRomBox();
        ColourLabel->Caption = "Color:";
        NTSC->Checked = true;
        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1500BtnClick(TObject *Sender)
{
        if (TS1500Btn->Down) return;

        NewMachine = MACHINETS1500;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        M1Not->Enabled = false;
        M1Not->Checked = true;
        TS1500Btn->Down = true;
        NewMachineName = TS1500Btn->Caption;
        FloatingPointHardwareFix->Checked = false;
        LoadRomBox();
        ColourLabel->Caption = "Color:";
        NTSC->Checked = true;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaBtnClick(TObject *Sender)
{
        if (LambdaBtn->Down) return;

        NewMachine = MACHINELAMBDA;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false, false);
        LambdaBtn->Down = true;
        NewMachineName = LambdaBtn->Caption;
        LoadRomBox();
        NTSC->Checked = true;
        ChrGenBox->Items->Strings[0] = "Lambda";
        ChrGenBox->ItemIndex = 0;
        ChrGenBox->Enabled = false;
        ChrGenLbl->Enabled = false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;
        HiResBox->ItemIndex = 0;
        HiResBox->Enabled = false;
        HiResLbl->Enabled = false;
        RomCartridgeBox->Enabled = false;
        RomCartridgeFileBox->Enabled = false;
        TS1510RomCartridgeFileBox->Enabled = false;
        TC2068RomCartridgeFileBox->Enabled = false;
        TS2068RomCartridgeFileBox->Enabled = false;
        SinclairRomCartridgeFileBox->Enabled = false;
        BrowseRomCartridge->Enabled = false;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::R470BtnClick(TObject *Sender)
{
        if (R470Btn->Down) return;

        NewMachine = MACHINER470;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false, false);
        R470Btn->Down = true;
        NewMachineName = R470Btn->Caption;
        LoadRomBox();
        NTSC->Checked = true;
        EnableLowRAM->Checked = false;
        EnableLowRAM->Enabled = false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TK85BtnClick(TObject *Sender)
{
        if (TK85Btn->Down) return;

        NewMachine = MACHINETK85;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false, false);
        TK85Btn->Down = true;
        NewMachineName = TK85Btn->Caption;
        LoadRomBox();
        NTSC->Checked = true;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::AceBtnClick(TObject *Sender)
{
        if (AceBtn->Down) return;

        NewMachine = MACHINEACE;
        RefreshDefaultRamSettings();
        SetupForZX81();
        IDEBox->Enabled = true;
        LabelIDE->Enabled = true;

        AceBtn->Down = true;
        NewMachineName = AceBtn->Caption;
        LoadRomBox();
        NTSC->Checked = false;
        NTSC->Enabled = false;
        EnableLowRAM->Checked = false;
        EnableLowRAM->Enabled = false;
        M1Not->Checked = false;
        M1Not->Enabled = false;
        ImprovedWait->Checked = false;
        ImprovedWait->Enabled = false;
        FloatingPointHardwareFix->Checked = false;
        HiResBox->ItemIndex = 0;
        HiResBox->Enabled = false;
        HiResLbl->Enabled = false;
        ChrGenBox->Items->Strings[0] = "Ace";
        ChrGenBox->ItemIndex = 0;
        ChrGenBox->Enabled = false;
        ChrGenLbl->Enabled = false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("ETI");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;   
        SpeechBox->Items->Clear();
        SpeechBox->Items->Add("None");
        SpeechBox->ItemIndex = 0;
        SpeechBox->Enabled = false;
        SpeechBoxLbl->Enabled = false;
        SoundCardBox->Items->Clear();
        SoundCardBox->Items->Add("None");
        SoundCardBox->Items->Add("Ace User");
        SoundCardBox->Items->Add("Boldfield");
        SoundCardBox->ItemIndex = 0;
        SoundCardBox->Enabled = true;
        SoundCardLbl->Enabled = true;
        JoystickBox->Items->Clear();
        JoystickBox->Items->Add("None");
        JoystickBox->Items->Add("Boldfield");
        JoystickBox->Items->Add("Programmable");
        JoystickBox->ItemIndex = 0;
        JoystickBox->Enabled = true;
        JoystickBoxLabel->Enabled = true;
        JoystickUpBox->Text    = "-";
        JoystickDownBox->Text  = "-";
        JoystickLeftBox->Text  = "-";
        JoystickRightBox->Text = "-";
        JoystickFireBox->Text  = "-";
        JoystickUpBox->Enabled    = false;
        JoystickDownBox->Enabled  = false;
        JoystickLeftBox->Enabled  = false;
        JoystickRightBox->Enabled = false;
        JoystickFireBox->Enabled  = false;
        JoystickUpBoxLabel->Enabled    = false;
        JoystickDownBoxLabel->Enabled  = false;
        JoystickLeftBoxLabel->Enabled  = false;
        JoystickRightBoxLabel->Enabled = false;
        JoystickFireBoxLabel->Enabled  = false;
        FDCBox->Items->Clear();
        FDCBox->Items->Add("None");
        FDCBox->ItemIndex = 0;
        FDCBox->Enabled = false;
        LabelFDC->Enabled = false;
        IDEBox->Items->Clear();
        IDEBox->Items->Add("None");
        IDEBox->ItemIndex = 0;
        IDEBox->Enabled = false;
        LabelIDE->Enabled = false;
        IDERomBox->ItemIndex = 0;
        FDCRomBox->ItemIndex = 0;
        RamPackBox->Items->Add("96K");
        EnableRomCartridgeOption(false);
        SetZXpandState(false, false);
        IDEBoxChange(NULL);

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        Form1->SelectJoystick2->Visible = false;
        Form1->ConnectJoystick2->Visible = false;
        Form1->EnableJoystick2AutoFire->Visible = false;
        Form1->divIDEJumperEClosed->Visible = false;
        Form1->ZXCFUploadJumperClosed->Visible = false;
        Form1->SimpleIdeRomEnabled->Visible = false;
        Form1->ConnectSpectrum128Keypad->Visible = false;
        Form1->SpectraColourEnable->Visible = false;
        Form1->ResetMemotechHRG->Visible = false;
        Form1->ResetQuicksilvaHiRes->Visible = false;
        Form1->QSChrEnable->Visible = false;
        Form1->ChromaColourEnable->Visible = false;
        Form1->SwitchOnMemocalc->Visible = false;
//        Form1->SwitchOnMemotext->Visible = false;
        Form1->SwitchOnZ80Assembler->Visible = false;
        Form1->WriteProtect8KRAM->Visible = false;
        Form1->ResetSpeech->Visible = false;

        Form1->ConnectSpectrum128Keypad->Checked = false;

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2048BtnClick(TObject *Sender)
{
        if (TC2048Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYTC2048;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TC2048Btn->Down = true;

        Issue2->Enabled = false;
        Issue2->Checked = false;

        EnableRomCartridgeOption(false);

        NewMachineName = TC2048Btn->Caption;
        LoadRomBox();
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;
        JoystickBox->ItemIndex = SelectEntry(JoystickBox, "Kempston");

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2068BtnClick(TObject *Sender)
{
        if (TC2068Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYTC2068;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TC2068Btn->Down = true;
        Multiface->Enabled = false;

        SpeechBox->Enabled = false;
        SpeechBoxLbl->Enabled = false;

        uSource->Checked = false;
        uSource->Enabled = false;

        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Timex");
        SoundCardBox->Enabled = false;
        SoundCardLbl->Enabled = false;

        Issue2->Enabled = false;
        Issue2->Checked = false;

        EnableRomCartridgeOption(true);

        NewMachineName = TC2068Btn->Caption;
        LoadRomBox();
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2068BtnClick(TObject *Sender)
{
        if (TS2068Btn->Down) return;

        NewMachine = MACHINESPECTRUM;
        NewSpec = SPECCYTS2068;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TS2068Btn->Down = true;
        Multiface->Enabled = false;

        SpeechBox->Enabled = false;
        SpeechBoxLbl->Enabled = false;

        uSource->Checked = false;
        uSource->Enabled = false;
        
        SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Timex");;
        SoundCardBox->Enabled = false;
        SoundCardLbl->Enabled = false;

        Issue2->Enabled = false;
        Issue2->Checked = false;

        NTSC->Checked = true;

        EnableRomCartridgeOption(true);

        NewMachineName = TS2068Btn->Caption;
        LoadRomBox();
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;

        int index = FindEntry(IDEBox, "Simple +3e 8-Bit");
        if (IDEBox->ItemIndex == index) IDEBox->ItemIndex = 0;
        IDEBox->Items->Delete(index);

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX97LEBtnClick(TObject *Sender)
{
        if (ZX97LEBtn->Down) return;

        NewMachine = MACHINEZX97LE;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        ZX97LEBtn->Down = true;
        NewMachineName = ZX97LEBtn->Caption;
        LoadRomBox();
        ChrGenBox->ItemIndex = SelectEntry(ChrGenBox, "CHR$128");
        ChrGenBox->Enabled = false;
        HiResBox->ItemIndex = SelectEntry(HiResBox, "WRX");
        HiResBox->Enabled = false;
        EnableLowRAM->Checked = true;
        EnableLowRAM->Enabled = false;
        M1Not->Checked = true;
        M1Not->Enabled = false;
        ImprovedWait->Checked = false;
        ImprovedWait->Enabled = false;
        FloatingPointHardwareFix->Checked = false;
        RamPackBox->Enabled = false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = true;
        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible = false;
                RamPackBox->Visible = false;
                LabelTotalRAM->Visible = false;
        }
        ButtonAdvancedMore->Visible = true;

        Z80Assembler->Checked = false;
        Z80Assembler->Enabled = false;
        Memocalc->Checked = false;
        Memocalc->Enabled = false;
//        Memotext->Checked = false;
//        Memotext->Enabled = false;

        IDEBoxChange(NULL);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2050Click(TObject *Sender)
{
        TS2050Config->Enabled = TS2050->Enabled && TS2050->Checked;

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::FormShow(TObject *Sender)
{
        LoadFromInternalSettings();  // restore form settings from copy
        ResetRequired = false;
        UpdateApplyButton();
}
//---------------------------------------------------------------------------
void __fastcall THW::TS2050ConfigClick(TObject *Sender)
{
        SerialConfig->ShowModal();
}
//---------------------------------------------------------------------------

void THW::SaveSettings(TIniFile* ini)
{
        AccessIniFile(ini, Write);

        WriteNVMemory(divIDEMem, 1,  8192,  "divide.nv");
        WriteNVMemory(ZXCFMem,   64, 16384, "zxcf.nv");
        WriteNVMemory(ZX1541Mem, 1,  8192,  "zx1541.nv");
}
//---------------------------------------------------------------------------

void THW::LoadSettings(TIniFile* ini)
{
        AccessIniFile(ini, Read);    // Read the ini file into the internal copy
        LoadFromInternalSettings();  // restore form settings from copy

        ReadNVMemory(divIDEMem, 1,  8192,  "divide.nv");
        ReadNVMemory(ZXCFMem,   64, 16384, "zxcf.nv");
        ReadNVMemory(ZX1541Mem, 1,  8192,  "zx1541.nv");

        if (Form1->Hardware1->Checked) Show();
}
//---------------------------------------------------------------------------

void THW::AccessIniFile(TIniFile* ini, IniFileAccessType accessType)
{
        //---- MACHINE ----

        int temp=Top;
        AccessIniFileInteger(ini, accessType, "HARDWARE", "Top",  temp);
        Top=temp;
        temp=Left;
        AccessIniFileInteger(ini, accessType, "HARDWARE", "Left", temp);
        Left=temp;

        AccessIniFileString(ini, accessType, "HARDWARE", "MachineName", Hwform.MachineName);

        AccessIniFileString(ini, accessType, "HARDWARE", "ROM80",     emulator.ROM80);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROM81",     emulator.ROM81);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSP16",   emulator.ROMSP16);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSP48",   emulator.ROMSP48);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSPP",    emulator.ROMSPP);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSP128",  emulator.ROMSP128);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSPP2",   emulator.ROMSPP2);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSPP2A",  emulator.ROMSPP2A);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSPP3",   emulator.ROMSPP3);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTS1000", emulator.ROMTS1000);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTS1500", emulator.ROMTS1500);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTC2048", emulator.ROMTC2048);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTC2068", emulator.ROMTC2068);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTS2068", emulator.ROMTS2068);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMLAMBDA", emulator.ROMLAMBDA);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMTK85",   emulator.ROMTK85);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMACE",    emulator.ROMACE);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMR470",   emulator.ROMR470);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROM97LE",   emulator.ROM97LE);

        //---- INTERFACES TAB ----

        AccessIniFileString(ini, accessType, "HARDWARE", "RamPack",            Hwform.RamPackBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "Sound",              Hwform.SoundCardBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "CharacterGenerator", Hwform.ChrGenBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "HighResolution",     Hwform.HiResBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "Colour",             Hwform.ColourBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "Speech",             Hwform.SpeechBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "Joystick",           Hwform.JoystickBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "RomCartridge",       Hwform.RomCartridgeBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "Dock",               emulator.ROMDock);
        AccessIniFileString(ini, accessType, "HARDWARE", "ZXC1Configuration",  Hwform.ZXC1ConfigurationBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "RomCartridgeFile",   Hwform.RomCartridgeFileBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "SinclairRomCartridgeFile", Hwform.SinclairRomCartridgeFileBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "TS1510RomCartridgeFile",   Hwform.TS1510RomCartridgeFileBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "TC2068RomCartridgeFile",   Hwform.TC2068RomCartridgeFileBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "TS2068RomCartridgeFile",   Hwform.TS2068RomCartridgeFileBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "JoystickLeft",       Hwform.ProgrammableJoystickLeft);
        AccessIniFileString(ini, accessType, "HARDWARE", "JoystickRight",      Hwform.ProgrammableJoystickRight);
        AccessIniFileString(ini, accessType, "HARDWARE", "JoystickUp",         Hwform.ProgrammableJoystickUp);
        AccessIniFileString(ini, accessType, "HARDWARE", "JoystickDown",       Hwform.ProgrammableJoystickDown);
        AccessIniFileString(ini, accessType, "HARDWARE", "JoystickFire",       Hwform.ProgrammableJoystickFire);

        AccessIniFileBoolean(ini, accessType, "HARDWARE", "MicroSource",       Hwform.uSourceChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "ZXpand",            Hwform.ZXpandChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "SpecDrum",          Hwform.SpecDrumChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "TS2050",            Hwform.TS2050Checked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "KempstonMouse",     Hwform.KMouseChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "Multiface",         Hwform.MultifaceChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "ZXPrinter",         Hwform.ZXPrinterChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "Z80Assembler",      Hwform.Z80AssemblerChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "Memocalc",          Hwform.MemocalcChecked);
//        AccessIniFileBoolean(ini, accessType, "HARDWARE", "Memotext",          Hwform.MemotextChecked);

        //---- DRIVES TAB ----

        AccessIniFileString(ini, accessType, "HARDWARE", "FDCType",    Hwform.FDCBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "DriveAType", Hwform.DriveATypeText);
        AccessIniFileString(ini, accessType, "HARDWARE", "DriveBType", Hwform.DriveBTypeText);
        AccessIniFileString(ini, accessType, "HARDWARE", "IDEType",    Hwform.IDEBoxText);
        AccessIniFileString(ini, accessType, "HARDWARE", "ZXCFRAM",    Hwform.ZXCFRAMText);

        AccessIniFileString(ini, accessType, "HARDWARE", "ROMINTERFACE1",  emulator.ROMINTERFACE1);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMBETADISK128", emulator.ROMBETADISK);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMDISCOVERY",   emulator.ROMDISCOVERY);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMDISCIPLE",    emulator.ROMDISCIPLE);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMPLUSD",       emulator.ROMPLUSD);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSIMPLE8BIT",  emulator.ROMSIMPLE8BIT);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSIMPLE16BIT", emulator.ROMSIMPLE16BIT);
        AccessIniFileString(ini, accessType, "HARDWARE", "ROMSIMPLECF",    emulator.ROMSIMPLECF);

        //---- ADVANCED TAB ----

        AccessIniFileString(ini,  accessType, "HARDWARE", "RomFile",                  Hwform.RomBoxText);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "ProtectRom",               Hwform.ProtectROMChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "NTSC",                     Hwform.NTSCChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "8KRAM",                    Hwform.EnableLowRAMChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "M1Not",                    Hwform.M1NotChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "ImprovedWait",             Hwform.ImprovedWaitChecked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "Issue2EarBehaviour",       Hwform.Issue2Checked);
        AccessIniFileBoolean(ini, accessType, "HARDWARE", "FloatingPointHardwareFix", Hwform.FloatingPointHardwareFixChecked);

        //---- ZX97 DIALOG BOX ----

        AccessIniFileInteger(ini, accessType, "ZX97", "SaveRAM",    Hwform.ZX97Form.saveram);
        AccessIniFileInteger(ini, accessType, "ZX97", "08k",        Hwform.ZX97Form.protect08);
        AccessIniFileInteger(ini, accessType, "ZX97", "abk",        Hwform.ZX97Form.protectab);
        AccessIniFileInteger(ini, accessType, "ZX97", "b0",         Hwform.ZX97Form.protectb0);
        AccessIniFileInteger(ini, accessType, "ZX97", "b115",       Hwform.ZX97Form.protectb115);
        AccessIniFileInteger(ini, accessType, "ZX97", "SwapRAMROM", Hwform.ZX97Form.bankswitch);

}

void THW::WriteNVMemory(BYTE* memory, int size, int count, const char* fileName)
{
        char FilePath[256];

        strcpy(FilePath, emulator.cwd);
        strcat(FilePath, nvMemoryFolder);
        strcat(FilePath, fileName);

        FILE* f = fopen(FilePath, "wb");
        if (f)
        {
                fwrite(memory, size, count, f);
                fclose(f);
        }
}

void THW::ReadNVMemory(BYTE* memory, int size, int count, const char* fileName)
{
        char FilePath[256];

        strcpy(FilePath, emulator.cwd);
        strcat(FilePath, nvMemoryFolder);
        strcat(FilePath, fileName);

        FILE* f = fopen(FilePath, "rb");
        if (f)
        {
                fread(memory, size, count, f);
                fclose(f);
        }
}

void __fastcall THW::BrowseROMClick(TObject *Sender)
{
        if (BrowseROMFile(romsFolder, RomBox))
        {
                SetZX80Icon();
                SetSpectrum128Icon();

                UpdateApplyButton();
        }
}

bool THW::BrowseROMFile(AnsiString folder, TComboBox* romBox)
{
        AnsiString Path;
        char cPath[512];

        Path = emulator.cwd;
        Path += folder;

        RomSelect->Title = "Select ROM File";
        RomSelect->Filter = "ROM Files (*.rom;*.bin)|*.rom;*.bin";
        RomSelect->InitialDir = Path;
        RomSelect->FileName = romBox->Text;
        if (RomSelect->FileName.Length() == 0 || *(RomSelect->FileName.AnsiLastChar()) == '\\')
        {
                RomSelect->FileName = "";
        }

        if (!RomSelect->Execute())
        {
                return false;
        }

        AnsiString selectedRomPath = FileNameGetPath(RomSelect->FileName);

        strcpy(cPath, Path.c_str());
        if (Path == selectedRomPath)
        {
                Path = RomSelect->FileName;
        }
        else
        {
                Path = RomSelect->FileName;
        }

        romBox->Text = Path;
        romBox->SelStart = romBox->Text.Length() - 1;
        romBox->SelLength = 0;

        return true;
}
//---------------------------------------------------------------------------

void THW::EnableRomCartridgeOption(bool enable)
{
        RomCartridgeLabel->Enabled = enable;
        RomCartridgeBox->Enabled = enable;
        RomCartridgeFileBox->Enabled = enable;
        TS1510RomCartridgeFileBox->Enabled = enable;
        TS2068RomCartridgeFileBox->Enabled = enable;
        TC2068RomCartridgeFileBox->Enabled = enable;
        SinclairRomCartridgeFileBox->Enabled = enable;

        RomCartridgeBox->ItemIndex = ROMCARTRIDGENONE;

        RomCartridgeFileBox->Text = "";
        TS1510RomCartridgeFileBox->Text = "";
        TS2068RomCartridgeFileBox->Text = "";
        TC2068RomCartridgeFileBox->Text = "";
        SinclairRomCartridgeFileBox->Text = "";

        UpdateRomCartridgeControls(NewMachine, NewSpec);
}                        
//---------------------------------------------------------------------------

void __fastcall THW::RamPackBoxChange(TObject *Sender)
{
        DisplayTotalRam();

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::NTSCClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::EnableLowRAMClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::M1NotClick(TObject *Sender)
{
        UpdateApplyButton();
}               
//---------------------------------------------------------------------------

void __fastcall THW::SoundCardBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ChrGenBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::HiResBoxChange(TObject *Sender)
{
        if ((HiResBox->Items->Strings[HiResBox->ItemIndex] == "Quicksilva") ||
             (HiResBox->Items->Strings[HiResBox->ItemIndex] == "Memotech"))
        {
                ProtectROM->Checked = true;
        }

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveATypeChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveBTypeChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Interface1ConfigureClick(TObject *Sender)
{
        IF1->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall THW::MultifaceClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::IDEBoxChange(TObject *Sender)
{
        ZXCFLabel->Visible = false;
        ZXCFRAM->Visible = false;
        Form1->divIDEJumperEClosed->Enabled = false;
        Form1->ZXCFUploadJumperClosed->Enabled = false;
        Form1->SimpleIdeRomEnabled->Enabled = false;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "MWCFIde")
        {
                RamPackBox->ItemIndex = SelectEntry(RamPackBox, "32K");
                M1Not->Checked = true;
        }
        else if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "AceCF")
        {
                RamPackBox->ItemIndex = SelectEntry(RamPackBox, "96K");
        }
        else if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "ZXCF")
        {
                Form1->ZXCFUploadJumperClosed->Enabled = true;
                ZXCFLabel->Visible = true;
                ZXCFRAM->Visible = true;
        }
        else if ((IDEBox->Items->Strings[IDEBox->ItemIndex] == "divIDE 57 (R Gal)") ||
                 (IDEBox->Items->Strings[IDEBox->ItemIndex] == "divIDE 57 (R\" Gal)"))
        {
                Form1->divIDEJumperEClosed->Enabled = true;
        }
        else if (IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE 8-Bit" || IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE 16-Bit" ||
                 IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple IDE CF" || IDEBox->Items->Strings[IDEBox->ItemIndex] == "Simple +3 8-Bit")
        {
                Form1->SimpleIdeRomEnabled->Enabled = true;
        }

        LoadIdeRomBox();

        DisplayTotalRam();

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::FDCBoxChange(TObject *Sender)
{
        if (FDCBox->Items->Strings[FDCBox->ItemIndex] == "Larken")
        {
                EnableLowRAM->Checked = true;
        }

        bool SpectrumPlus3Drive = (FDCBox->Items->Strings[FDCBox->ItemIndex] == "+3");
        DriveAType->Visible      = SpectrumPlus3Drive;
        DriveBType->Visible      = SpectrumPlus3Drive;
        DriveATypeLabel->Visible = SpectrumPlus3Drive;
        DriveBTypeLabel->Visible = SpectrumPlus3Drive;

        bool interface1Drive = (FDCBox->Items->Strings[FDCBox->ItemIndex] == "ZX Interface 1");
        Interface1Configure->Visible    = interface1Drive;
        NoMicrodrivesLabel->Visible     = interface1Drive;
        NoMicrodrivesComboBox->Visible  = interface1Drive;

        if (interface1Drive)
        {
                NoMicrodrivesComboBox->ItemIndex = IF1->MDVNoDrives;
        }

        LoadFdcRomBox();

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXpandClick(TObject *Sender)
{
        SetZXpandState(ZXpand->Checked,true);

        if (HW->Visible && HW->Enabled)
        {
                if (ZXpand->Checked)
                {
                        JoystickBox->Items->Add("ZXpand");

                        int zxpandHiResIndex = SelectEntry(HiResBox, "WRX");
                        int zxpandRamPackIndex = SelectEntry(RamPackBox, "32K");
                        int zxpandSoundCardIndex = SelectEntry(SoundCardBox, "ZON X");
                        int zxpandJoystickIndex = SelectEntry(JoystickBox, "ZXpand");

                        bool allFacilitiesSelected = (HiResBox->ItemIndex == zxpandHiResIndex) &&
                                                     (RamPackBox->ItemIndex == zxpandRamPackIndex) &&
                                                     (SoundCardBox->ItemIndex == zxpandSoundCardIndex) &&
                                                     EnableLowRAM->Checked &&
                                                     (JoystickBox->ItemIndex == zxpandJoystickIndex);

                        if (!allFacilitiesSelected)
                        {
                                int ret = Application->MessageBox("Automatically select the following ZXpand+ facilities?\n\n32K RAM Pack, RAM in 8K-16K Region, WRX High Resolution,\nZON X Sound, ZXpand Joystick", "ZXpand+ Configuration", MB_YESNO | MB_ICONQUESTION);

                                if (ret == IDYES)
                                {
                                        HiResBox->ItemIndex = zxpandHiResIndex;
                                        RamPackBox->ItemIndex = zxpandRamPackIndex;
                                        SoundCardBox->ItemIndex = zxpandSoundCardIndex;;
                                        EnableLowRAM->Checked = true;
                                        JoystickBox->ItemIndex = zxpandJoystickIndex;
                                        JoystickBoxChange(JoystickBox);
                                }
                        }
                }
                else
                {
                        if (JoystickBox->Text == "ZXpand")
                        {
                                JoystickBox->ItemIndex = 0;
                        }
                        JoystickBox->Items->Delete(JoystickBox->Items->Count - 1);
                }
        }

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

int THW::SelectEntry(TComboBox* comboBox, AnsiString text)
{
        int index = FindEntry(comboBox, text);
        if (index == -1)
        {
                index = 0;
        }

        return index;
}
//---------------------------------------------------------------------------

int THW::FindEntry(TComboBox* comboBox, AnsiString text)
{
        for (int i = 0; i < comboBox->Items->Count; i++)
        {
                if (comboBox->Items->Strings[i] == text)
                {
                        return i;
                }
        }

        return -1;
}
//---------------------------------------------------------------------------

void __fastcall THW::BrowseRomCartridgeClick(TObject *Sender)
{
        AnsiString Path;

        Path = emulator.cwd;

        int romcartridgetype = DetermineRomCartridgeType(RomCartridgeBox->Text, NewMachine, NewSpec);

        if (romcartridgetype == ROMCARTRIDGESINCLAIR)
        {
                Path += if2RomsFolder;
        }
        else if (romcartridgetype == ROMCARTRIDGETS1510)
        {
                Path += ts1510RomsFolder;
        }
        else if (romcartridgetype == ROMCARTRIDGETS2068)
        {
                Path += ts2068RomsFolder;
        }
        else if (romcartridgetype == ROMCARTRIDGETC2068)
        {
                Path += tc2068RomsFolder;
        }
        else
        {
                Path += romCartridgeFolder;
        }

        RomSelect->Title = "Select ROM Cartridge File";
        RomSelect->Filter = "ROM Cartridge Files (*.rom;*.bin;*.dck)|*.rom;*.bin;*.dck";
        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomCartridgeFileBox->Text;
        if (RomSelect->FileName.Length() == 0 || *(RomSelect->FileName.AnsiLastChar()) == '\\')
        {
                RomSelect->FileName = "";
        }
        if (!RomSelect->Execute()) return;

        AnsiString selectedRomPath = FileNameGetPath(RomSelect->FileName);

        Path = RomSelect->FileName;

        RomCartridgeFileBox->Text = Path;
        RomCartridgeFileBox->SelStart = 0;

        TS1510RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TS1510RomCartridgeFileBox->SelStart = RomCartridgeFileBox->SelStart;
        TS1510RomCartridgeFileBox->SelLength = RomCartridgeFileBox->SelLength;

        TC2068RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TC2068RomCartridgeFileBox->SelStart = RomCartridgeFileBox->SelStart;
        TC2068RomCartridgeFileBox->SelLength = RomCartridgeFileBox->SelLength;

        TS2068RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TS2068RomCartridgeFileBox->SelStart = RomCartridgeFileBox->SelStart;
        TS2068RomCartridgeFileBox->SelLength = RomCartridgeFileBox->SelLength;

        SinclairRomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        SinclairRomCartridgeFileBox->SelStart = RomCartridgeFileBox->SelStart;
        SinclairRomCartridgeFileBox->SelLength = RomCartridgeFileBox->SelLength;
}
//---------------------------------------------------------------------------

void __fastcall THW::RomCartridgeBoxChange(TObject *Sender)
{
        RomCartridgeFileBox->Text = "";
        SinclairRomCartridgeFileBox->Text = "";
        TS1510RomCartridgeFileBox->Text = "";
        TS2068RomCartridgeFileBox->Text = "";
        TC2068RomCartridgeFileBox->Text = "";

        UpdateRomCartridgeControls(NewMachine, NewSpec);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SinclairRomCartridgeFileBoxChange(TObject *Sender)
{
        if (SinclairRomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = SinclairRomCartridgeFileBox->Text;
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2068RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TC2068RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TC2068RomCartridgeFileBox->Text;
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2068RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TS2068RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TS2068RomCartridgeFileBox->Text;
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1510RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TS1510RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TS1510RomCartridgeFileBox->Text;
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::ColourBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::RomBoxChange(TObject *Sender)
{
        if (ZX80Btn->Down)
        {
                bool zx81Ed1ROM = (RomBox->Text == "zx81.edition1.rom");
                FloatingPointHardwareFix->Enabled = zx81Ed1ROM;
                if (!zx81Ed1ROM)
                {
                        FloatingPointHardwareFix->Checked = false;
                }
        }
        else if (ZX81Btn->Down)
        {
                if (RomBox->Text == "tree-forth.rom")
                {
                        NTSC->Checked = true;
                }
                else if (RomBox->Text == "zx81-forth.rom" || RomBox->Text == "zx.aszmic.e04.rom.rom" || RomBox->Text == "zx.aszmic.e07.rom.rom")
                {
                        NTSC->Checked = false;
                }
        }

        UpdateApplyButton();

        SetZX80Icon();
        SetSpectrum128Icon();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXPrinterClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------
  
void __fastcall THW::FloatingPointHardwareFixClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------
                
void __fastcall THW::ButtonZXpandSDCardClick(TObject *Sender)
{
        AnsiString root(zxpandSDCardFolderRoot);
        AnsiString replaced = StringReplace(root, "/", "\\", TReplaceFlags() << rfReplaceAll);
        ShellExecute(NULL, "explore", replaced.c_str(), NULL, NULL, SW_NORMAL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ButtonAdvancedMoreClick(TObject *Sender)
{
        if (ZX97Dialog->ShowModal()==mrOk)
        {
                ResetRequired = true;
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::ImprovedWaitClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::uSourceClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::FormCreate(TObject *Sender)
{
        PopulateRomCartridgeTS1510List();
        PopulateRomCartridgeTS2068List();
        PopulateRomCartridgeTC2068List();
        PopulateRomCartridgeSinclairList();
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTS1510List()
{
        vector<RomCartridgeEntry>::iterator iter;

        TS1510RomCartridgeFileBox->Items->Clear();

        for (iter = ts1510RomCartridges.begin(); iter != ts1510RomCartridges.end(); iter++)
        {
                AddRomCartridgeFile(TS1510RomCartridgeFileBox, iter, ".rom");
        }
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTS2068List()
{
        vector<RomCartridgeEntry>::iterator iter;

        TS2068RomCartridgeFileBox->Items->Clear();

        for (iter = ts2068RomCartridges.begin(); iter != ts2068RomCartridges.end(); iter++)
        {
                AddRomCartridgeFile(TS2068RomCartridgeFileBox, iter, ".dck");
        }
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTC2068List()
{
        vector<RomCartridgeEntry>::iterator iter;

        TC2068RomCartridgeFileBox->Items->Clear();

        for (iter = tc2068RomCartridges.begin(); iter != tc2068RomCartridges.end(); iter++)
        {
                AddRomCartridgeFile(TC2068RomCartridgeFileBox, iter, ".dck");
        }
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeSinclairList()
{
        vector<RomCartridgeEntry>::iterator iter;

        SinclairRomCartridgeFileBox->Items->Clear();

        for (iter = sinclairRomCartridges.begin(); iter != sinclairRomCartridges.end(); iter++)
        {
                AddRomCartridgeFile(SinclairRomCartridgeFileBox, iter, ".rom");
        }
}
//---------------------------------------------------------------------------
void THW::AddRomCartridgeFile(TComboBox* romCartridgeFileBox, RomCartridgeEntry* romCartridgeEntry, AnsiString fileExt)
{                                                  
        AnsiString romPath = emulator.cwd;
        romPath += romCartridgeEntry->Path;
        romPath += romCartridgeEntry->Title;
        romPath += fileExt;

        if (FileExists(romPath))
        {
                romCartridgeFileBox->Items->Add(romCartridgeEntry->Title);
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXpandEmulationInfoClick(TObject *Sender)
{
        AnsiString path = emulator.cwd;

        ShellExecute(NULL, "open", "Notepad.exe", "ZXpand readme.txt", path.c_str(), SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpeechBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::CancelClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickBoxChange(TObject *Sender)
{
        AnsiString joystickInterfaceType = ((TEdit*)Sender)->Text;
        bool keySelectionEnabled = false;

        if (joystickInterfaceType == "Programmable")
        {
                JoystickUpBox->Text    = programmableJoystickUp;
                JoystickDownBox->Text  = programmableJoystickDown;
                JoystickLeftBox->Text  = programmableJoystickLeft;
                JoystickRightBox->Text = programmableJoystickRight;
                JoystickFireBox->Text  = programmableJoystickFire;

                keySelectionEnabled = true;
        }
        else
        {
                AnsiString joystickUpText    = "-";
                AnsiString joystickDownText  = "-";
                AnsiString joystickLeftText  = "-";
                AnsiString joystickRightText = "-";
                AnsiString joystickFireText  = "-";

                if (joystickInterfaceType == "Cursor" || joystickInterfaceType == "Cursor (Protek)")
                {
                        joystickUpText    = "7";
                        joystickDownText  = "6";
                        joystickLeftText  = "5";
                        joystickRightText = "8";
                        joystickFireText  = "0";
                }
                else if (joystickInterfaceType == "ZX Interface 2" || joystickInterfaceType == "Sinclair")
                {
                        UpdateSinclairJoystickKeys();
                }

                JoystickUpBox->Text    = joystickUpText;
                JoystickDownBox->Text  = joystickDownText;
                JoystickLeftBox->Text  = joystickLeftText;
                JoystickRightBox->Text = joystickRightText;
                JoystickFireBox->Text  = joystickFireText;
        }

        JoystickUpBox->Enabled    = keySelectionEnabled;
        JoystickDownBox->Enabled  = keySelectionEnabled;
        JoystickLeftBox->Enabled  = keySelectionEnabled;
        JoystickRightBox->Enabled = keySelectionEnabled;
        JoystickFireBox->Enabled  = keySelectionEnabled;

        JoystickUpBoxLabel->Enabled    = keySelectionEnabled;
        JoystickDownBoxLabel->Enabled  = keySelectionEnabled;
        JoystickLeftBoxLabel->Enabled  = keySelectionEnabled;
        JoystickRightBoxLabel->Enabled = keySelectionEnabled;
        JoystickFireBoxLabel->Enabled  = keySelectionEnabled;

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void THW::UpdateSinclairJoystickKeys()
{
        if (Form1->ConnectJoystick1->Checked || !Form1->ConnectJoystick2->Checked)
        {
                JoystickUpBox->Text    = "9";
                JoystickDownBox->Text  = "8";
                JoystickLeftBox->Text  = "6";
                JoystickRightBox->Text = "7";
                JoystickFireBox->Text  = "0";
        }
        else
        {
                JoystickUpBox->Text    = "4";
                JoystickDownBox->Text  = "3";
                JoystickLeftBox->Text  = "1";
                JoystickRightBox->Text = "2";
                JoystickFireBox->Text  = "5";
        }
}
//---------------------------------------------------------------------------

bool THW::ValidCharacter(TEdit* textBox, char newKey)
{
        bool invalid = (!isalnum(newKey) && newKey != ' ' && newKey != '^' && newKey != '.' && newKey != '#') ||
                        (textBox != JoystickLeftBox  && newKey == *(JoystickLeftBox->Text.c_str())) ||
                        (textBox != JoystickRightBox && newKey == *(JoystickRightBox->Text.c_str())) ||
                        (textBox != JoystickDownBox  && newKey == *(JoystickDownBox->Text.c_str())) ||
                        (textBox != JoystickUpBox    && newKey == *(JoystickUpBox->Text.c_str())) ||
                        (textBox != JoystickFireBox  && newKey == *(JoystickFireBox->Text.c_str()));

        return !invalid;
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        TEdit* textBox = (TEdit*)Sender;

        if (Button == mbLeft)
        {
                textBox->SelStart = 0;
                textBox->SelLength = 1;
        }
}
//---------------------------------------------------------------------------
     
void __fastcall THW::JoystickBoxExit(TObject *Sender)
{
        TEdit* textBox = (TEdit*)Sender;

        if (!ValidCharacter(textBox, *(textBox->Text.c_str())))
        {
                textBox->SetFocus();
                textBox->SelStart = 0;
                textBox->SelLength = 1;
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickBoxKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        TEdit* textBox = (TEdit*)Sender;
        AnsiString keyString = (char)Key;
        char key;

        if (Key == VK_SHIFT)
        {
                bool supportRightShift = (NewMachine == MACHINESPECTRUM || NewMachine == MACHINEACE);
                bool rightShiftPressed = IsKeyPressed(VK_RSHIFT);
                key = (supportRightShift && emulator.UseRShift && rightShiftPressed) ? '.' : '^';
                if (NewKey(textBox, key))
                {
                        textBox->Text = key;
                }
        }
        else if (Key == VK_CONTROL)
        {
                key = '.';
                if (NewKey(textBox, key))
                {
                        textBox->Text = key;
                }
        }
        else if (Key == VK_RETURN)
        {
                key = '#';
                if (NewKey(textBox, key))
                {
                        textBox->Text = key;
                }
        }

        textBox->SelStart = 0;
        textBox->SelLength = 1;
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickBoxKeyPress(TObject *Sender, char &Key)
{
        TEdit* textBox = (TEdit*)Sender;
        AnsiString keyString = (char)Key;
        char key = *(keyString.UpperCase().c_str());

        if (!NewKey(textBox, key))
        {
                Key = 0;
                textBox->SelStart = 0;
                textBox->SelLength = 1;
        }
}
//---------------------------------------------------------------------------

bool THW::NewKey(TEdit* textBox, char key)
{
        if (ValidCharacter(textBox, key))
        {
                if (textBox == JoystickUpBox)    programmableJoystickUp    = key;
                if (textBox == JoystickDownBox)  programmableJoystickDown  = key;
                if (textBox == JoystickLeftBox)  programmableJoystickLeft  = key;
                if (textBox == JoystickRightBox) programmableJoystickRight = key;
                if (textBox == JoystickFireBox)  programmableJoystickFire  = key;

                SelectNext(textBox, true, true);

                return true;
        }

        return false;
}
//---------------------------------------------------------------------------

void THW::SetCharacter(TEdit* textBox, KeyInfo& keyInfo)
{
        char c = *(textBox->Text.c_str());

        if (keyInfo.Character == '^')
        {
                c = VK_SHIFT;
        }
        else if (keyInfo.Character == '.')
        {
                c = VK_CONTROL;
        }
        else if (keyInfo.Character == '#')
        {
                c = VK_RETURN;
        }

        keyInfo.Character = c;
        DetermineKeyMatrixPosition(NewMachine, keyInfo);
}
//---------------------------------------------------------------------------

void __fastcall THW::DefaultsButtonClick(TObject *Sender)
{
        SoundCardBox->ItemIndex    = 0;
        ChrGenBox->ItemIndex       = 0;
        HiResBox->ItemIndex        = 0;
        ColourBox->ItemIndex       = 0;
        SpeechBox->ItemIndex       = 0;
        JoystickBox->ItemIndex     = 0;
        RomCartridgeBox->ItemIndex = 0;
        FDCBox->ItemIndex          = 0;
        IDEBox->ItemIndex          = 0;
        RomBox->ItemIndex          = 0;

        DriveAType->ItemIndex = SelectEntry(DriveAType, "3\" Single-Sided (180K)");
        DriveBType->ItemIndex = SelectEntry(DriveBType, "None");
        ZXCFRAM->ItemIndex    = SelectEntry(ZXCFRAM,    "1024K");
        
        uSource->Checked                  = false;
        KMouse->Checked                   = false;
        Multiface->Checked                = false;
        ZXPrinter->Checked                = false;
        SpecDrum->Checked                 = false;
        TS2050->Checked                   = false;
        ZXpand->Checked                   = false;
        EnableLowRAM->Checked             = false;
        M1Not->Checked                    = false;
        ImprovedWait->Checked             = false;
        FloatingPointHardwareFix->Checked = false;
        Issue2->Checked                   = false;
        NTSC->Checked                     = false;
        Z80Assembler->Checked             = false;
        Memocalc->Checked                 = false;
//        Memotext->Checked                 = false;
        ProtectROM->Checked               = true;

        programmableJoystickLeft  = "O";
        programmableJoystickRight = "P";
        programmableJoystickUp    = "Q";
        programmableJoystickDown  = "A";
        programmableJoystickFire  = "M";

        EnableRomCartridgeOption(true);

        SoundCardBoxChange(NULL);
        ChrGenBoxChange(NULL);
        HiResBoxChange(NULL);
        ColourBoxChange(NULL);
        SpeechBoxChange(NULL);
        JoystickBoxChange(NULL);
        RomCartridgeBoxChange(NULL);
        FDCBoxChange(NULL);
        IDEBoxChange(NULL);
        RomBoxChange(NULL);

        SelectDefaultRom();

        switch (NewMachine)
        {
        case MACHINEZX80:
                break;

        case MACHINEZX81:
                break;

        case MACHINEACE:
                EnableRomCartridgeOption(false);
                break;

        case MACHINETS1500:
                NTSC->Checked = true;
                break;

        case MACHINELAMBDA:
                NTSC->Checked = true;
                EnableRomCartridgeOption(false);
                break;

        case MACHINEZX97LE:
                ChrGenBox->ItemIndex = SelectEntry(ChrGenBox, "CHR$128");
                HiResBox->ItemIndex  = SelectEntry(HiResBox,  "WRX");
                EnableLowRAM->Checked = true;
                M1Not->Checked = true;
                break;

        case MACHINER470:
                NTSC->Checked = true;
                EnableRomCartridgeOption(false);
                break;
                
        case MACHINETK85:
                NTSC->Checked = true;
                EnableRomCartridgeOption(false);
                break;

        case MACHINETS1000:
                NTSC->Checked = true;
                break;

        case MACHINESPECTRUM:
                switch (NewSpec)
                {
                        case SPECCY16:
                                Issue2->Checked = true;
                                break;

                        case SPECCY48:
                                break;

                        case SPECCYPLUS:
                                break;

                        case SPECCYTC2048:
                                ChrGenBox->ItemIndex   = SelectEntry(ChrGenBox,   "Timex");
                                HiResBox->ItemIndex    = SelectEntry(HiResBox,    "Timex");
                                ColourBox->ItemIndex   = SelectEntry(ColourBox,   "Timex");
                                JoystickBox->ItemIndex = SelectEntry(JoystickBox, "Kempston");
                                EnableRomCartridgeOption(false);
                                break;

                        case SPECCYTC2068:
                                SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Timex");
                                ChrGenBox->ItemIndex    = SelectEntry(ChrGenBox,    "Timex");
                                HiResBox->ItemIndex     = SelectEntry(HiResBox,     "Timex");
                                ColourBox->ItemIndex    = SelectEntry(ColourBox,    "Timex");
                                JoystickBox->ItemIndex  = SelectEntry(JoystickBox,  "Timex");
                                break;

                        case SPECCYTS2068:
                                ChrGenBox->ItemIndex    = SelectEntry(ChrGenBox,   "Timex");
                                HiResBox->ItemIndex     = SelectEntry(HiResBox,    "Timex");
                                ColourBox->ItemIndex    = SelectEntry(ColourBox,   "Timex");
                                JoystickBox->ItemIndex  = SelectEntry(JoystickBox, "Timex");
                                NTSC->Checked = true;
                                break;

                        case SPECCY128:
                                SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
                                break;

                        case SPECCYPLUS2:
                                SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
                                JoystickBox->ItemIndex  = SelectEntry(JoystickBox,  "Sinclair");
                                break;

                        case SPECCYPLUS2A:
                                SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
                                JoystickBox->ItemIndex  = SelectEntry(JoystickBox,  "Sinclair");
                                break;

                        case SPECCYPLUS3:
                                SoundCardBox->ItemIndex = SelectEntry(SoundCardBox, "Sinclair 128K");
                                JoystickBox->ItemIndex  = SelectEntry(JoystickBox,  "Sinclair");
                                break;
                }
                break;
        }

        if (RamPackBox->Visible)
        {
                RamPackBox->ItemIndex = 0;
                RamPackBoxChange(NULL);

                AnsiString defaultRam = (AceBtn->Down) ? "19K" : "16K";

                while (LabelTotalRAM->Caption != ("Total RAM: " + defaultRam))
                {
                        RamPackBox->ItemIndex = RamPackBox->ItemIndex + 1;
                        DisplayTotalRam();
                }
        }

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void THW::SelectDefaultRom()
{
        switch (NewMachine)
        {
        case MACHINEZX80:
                RomBox->Text = "zx80.rom";
                break;

        case MACHINEZX81:
                RomBox->Text = "zx81.edition3.rom";
                break;
                
        case MACHINEACE:
                RomBox->Text = "jupiterace.rom";
                break;

        case MACHINETS1500:
                RomBox->Text = "ts1500.rom";
                break;

        case MACHINELAMBDA:
                RomBox->Text = "lambda8300.rom";
                break;

        case MACHINEZX97LE:
                RomBox->Text = "zx97.rom";
                break;

        case MACHINER470:
                RomBox->Text = "ringo470.rom";
                break;

        case MACHINETK85:
                RomBox->Text = "tk85.rom";
                break;

        case MACHINETS1000:
                RomBox->Text = "zx81.edition3.rom";
                break;

        case MACHINESPECTRUM:
                switch (NewSpec)
                {
                        case SPECCY16:
                        case SPECCY48:
                        case SPECCYPLUS:
                                RomBox->Text = "spectrum48.rom";
                                break;

                        case SPECCYTC2048:
                                RomBox->Text = "tc2048.rom";
                                break;

                        case SPECCYTC2068:
                        case SPECCYTS2068:
                                RomBox->Text = "ts2068.rom";
                                break;

                        case SPECCY128:
                                RomBox->Text = "spectrum128.rom";
                                break;

                        case SPECCYPLUS2:
                                RomBox->Text = "spectrum+2.rom";
                                break;

                        case SPECCYPLUS2A:
                        case SPECCYPLUS3:
                                RomBox->Text = "spectrum+3.version4-1.rom";
                                break;
                }
                break;             
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::KMouseClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecDrumClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickLeftBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickRightBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickUpBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickDownBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::JoystickFireBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ProtectROMClick(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Issue2Click(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXCFRAMChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::RestoreButtonClick(TObject *Sender)
{
        LoadFromInternalSettings();  // restore form settings from copy
        UpdateRomCartridgeControls(NewMachine, NewSpec);
        ResetRequired = false;
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::FDCRomBoxBrowseClick(TObject *Sender)
{
        AnsiString folder = romsFolder;
        folder += fdcRomsFolder;

        if (BrowseROMFile(folder, FDCRomBox))
        {
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::IDERomBoxBrowseClick(TObject *Sender)
{
        AnsiString folder = romsFolder;
        folder += ideRomsFolder;

        if (BrowseROMFile(folder, IDERomBox))
        {
                UpdateApplyButton();
        }
}
//---------------------------------------------------------------------------

void THW::ConfigureIDERom()
{
        if (IDEBox->Text == "Simple IDE 8-Bit")
        {
                strcpy(emulator.ROMSIMPLE8BIT, IDERomBox->Text.c_str());
        }
        else if (IDEBox->Text == "Simple IDE 16-Bit")
        {
                strcpy(emulator.ROMSIMPLE16BIT, IDERomBox->Text.c_str());
        }
        else if (IDEBox->Text == "Simple IDE CF")
        {
                strcpy(emulator.ROMSIMPLECF, IDERomBox->Text.c_str());
        }
        else if (IDEBox->Text == "MWCFIde")
        {
                strcpy(emulator.ROMMWCFIDE, IDERomBox->Text.c_str());
        }
}
//---------------------------------------------------------------------------

void THW::ConfigureFDCRom()
{
        if (FDCBox->Text == "Beta Disk 128")
        {
                strcpy(emulator.ROMBETADISK, FDCRomBox->Text.c_str());
        }
        else if (FDCBox->Text == "Opus Discovery")
        {
                strcpy(emulator.ROMDISCOVERY, FDCRomBox->Text.c_str());
        }
        else if (FDCBox->Text == "DISCiPLE")
        {
                strcpy(emulator.ROMDISCIPLE, FDCRomBox->Text.c_str());
        }
        else if (FDCBox->Text == "+D")
        {
                strcpy(emulator.ROMPLUSD, FDCRomBox->Text.c_str());
        }
        else if (FDCBox->Text == "Larken")
        {
                strcpy(emulator.ROMLARKEN81, FDCRomBox->Text.c_str());
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::FDCRomBoxChange(TObject *Sender)
{
        UpdateApplyButton();        
}
//---------------------------------------------------------------------------

void __fastcall THW::IDERomBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::NoMicrodrivesComboBoxChange(TObject *Sender)
{
        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::Z80AssemblerClick(TObject *Sender)
{
        if (Z80Assembler->Checked)
        {
                Memocalc->Checked = false;
//                Memotext->Checked = false;
        }

        Z80Assembler->Enabled = (!Memotext->Checked     && !Memocalc->Checked);
        Memocalc->Enabled     = (!Z80Assembler->Checked && !Memotext->Checked);
//        Memotext->Enabled     = (!Z80Assembler->Checked && !Memocalc->Checked);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

void __fastcall THW::MemocalcClick(TObject *Sender)
{
        if (Memocalc->Checked)
        {
//                Memotext->Checked = false;
                Z80Assembler->Checked = false;
        }

        Z80Assembler->Enabled = (!Memotext->Checked     && !Memocalc->Checked);
        Memocalc->Enabled     = (!Z80Assembler->Checked && !Memotext->Checked);
//        Memotext->Enabled     = (!Z80Assembler->Checked && !Memocalc->Checked);

        UpdateApplyButton();
}
//---------------------------------------------------------------------------

/*
void __fastcall THW::MemotextClick(TObject *Sender)
{
        if (Memotext->Checked)
        {
                Memocalc->Checked = false;
                Z80Assembler->Checked = false;
        }

        Z80Assembler->Enabled = (!Memotext->Checked     && !Memocalc->Checked);
        Memocalc->Enabled     = (!Z80Assembler->Checked && !Memotext->Checked);
        Memotext->Enabled     = (!Z80Assembler->Checked && !Memocalc->Checked);

        UpdateApplyButton();
}
*/
//---------------------------------------------------------------------------

void THW::UpdateApplyButton()
{
        bool settingsChanged = (NewMachineName != Hwform.MachineName);

        settingsChanged |= (RomBox->Text                           != Hwform.RomBoxText);
        settingsChanged |= (RamPackBox->Text                       != Hwform.RamPackBoxText);
        settingsChanged |= (SoundCardBox->Text                     != Hwform.SoundCardBoxText);
        settingsChanged |= (ChrGenBox->Text                        != Hwform.ChrGenBoxText);
        settingsChanged |= (HiResBox->Text                         != Hwform.HiResBoxText);
        settingsChanged |= (ColourBox->Text                        != Hwform.ColourBoxText);
        settingsChanged |= (SpeechBox->Text                        != Hwform.SpeechBoxText);
        settingsChanged |= (RomCartridgeBox->Text                  != Hwform.RomCartridgeBoxText);
        settingsChanged |= (ZXC1ConfigurationBox->Text             != Hwform.ZXC1ConfigurationBoxText);
        settingsChanged |= (JoystickBox->Text                      != Hwform.JoystickBoxText);
        settingsChanged |= (DriveAType->Text                       != Hwform.DriveATypeText);
        settingsChanged |= (DriveBType->Text                       != Hwform.DriveBTypeText);
        settingsChanged |= (ZXCFRAM->Text                          != Hwform.ZXCFRAMText);
        settingsChanged |= (IDEBox->Text                           != Hwform.IDEBoxText);
        settingsChanged |= (FDCBox->Text                           != Hwform.FDCBoxText);
        settingsChanged |= (IDERomBox->Text                        != Hwform.IDERomBoxText);
        settingsChanged |= (FDCRomBox->Text                        != Hwform.FDCRomBoxText);
        settingsChanged |= (NoMicrodrivesComboBox->Text            != Hwform.NoMicrodrivesComboBoxText);

        settingsChanged |= (RomCartridgeFileBox->Text              != Hwform.RomCartridgeFileBoxText);
        settingsChanged |= (SinclairRomCartridgeFileBox->Text      != Hwform.SinclairRomCartridgeFileBoxText);
        settingsChanged |= (TS1510RomCartridgeFileBox->Text        != Hwform.TS1510RomCartridgeFileBoxText);
        settingsChanged |= (TC2068RomCartridgeFileBox->Text        != Hwform.TC2068RomCartridgeFileBoxText);
        settingsChanged |= (TS2068RomCartridgeFileBox->Text        != Hwform.TS2068RomCartridgeFileBoxText);

        settingsChanged |= (programmableJoystickLeft               != Hwform.ProgrammableJoystickLeft);
        settingsChanged |= (programmableJoystickRight              != Hwform.ProgrammableJoystickRight);
        settingsChanged |= (programmableJoystickUp                 != Hwform.ProgrammableJoystickUp);
        settingsChanged |= (programmableJoystickDown               != Hwform.ProgrammableJoystickDown);
        settingsChanged |= (programmableJoystickFire               != Hwform.ProgrammableJoystickFire);

        settingsChanged |= (SpecDrum->Checked                      != Hwform.SpecDrumChecked);
        settingsChanged |= (ProtectROM->Checked                    != Hwform.ProtectROMChecked);
        settingsChanged |= (NTSC->Checked                          != Hwform.NTSCChecked);
        settingsChanged |= (EnableLowRAM->Checked                  != Hwform.EnableLowRAMChecked);
        settingsChanged |= (M1Not->Checked                         != Hwform.M1NotChecked);
        settingsChanged |= (ImprovedWait->Checked                  != Hwform.ImprovedWaitChecked);
        settingsChanged |= (TS2050->Checked                        != Hwform.TS2050Checked);
        settingsChanged |= (Issue2->Checked                        != Hwform.Issue2Checked);
        settingsChanged |= (KMouse->Checked                        != Hwform.KMouseChecked);
        settingsChanged |= (Multiface->Checked                     != Hwform.MultifaceChecked);
        settingsChanged |= (ZXPrinter->Checked                     != Hwform.ZXPrinterChecked);
        settingsChanged |= (FloatingPointHardwareFix->Checked      != Hwform.FloatingPointHardwareFixChecked);
        settingsChanged |= (uSource->Checked                       != Hwform.uSourceChecked);
        settingsChanged |= (ZXpand->Checked                        != Hwform.ZXpandChecked);
        settingsChanged |= (Z80Assembler->Checked                  != Hwform.Z80AssemblerChecked);
        settingsChanged |= (Memocalc->Checked                      != Hwform.MemocalcChecked);
//        settingsChanged |= (Memotext->Checked                      != Hwform.MemotextChecked);

        ResetRequired |= settingsChanged;

        Apply->Enabled = ResetRequired;
        RestoreButton->Enabled = Apply->Enabled;
}
//---------------------------------------------------------------------------

void __fastcall THW::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->Hardware1->Checked=false;
}
//---------------------------------------------------------------------------

