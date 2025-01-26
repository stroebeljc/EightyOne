//--------------------------------------------------------------------------

#include <dir.h>
#include <dirent.h>
#include <vcl4.h>
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

//extern "C" void sound_ay_init(void);
extern "C" BYTE ZX1541Mem[];

extern void HWSetMachine(int machine, int speccy);
extern int zx81_do_scanline(SCANLINE *CurScanLine);
extern int zx80_do_scanline(SCANLINE *CurScanLine);
extern void InitPatches(int machineType);
extern bool LoadRomCartridgeFile(char *filename);
extern int RomCartridgeCapacity;
extern int LoadDock(char *Filename);

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
        case MACHINEZX80: HW->ZX80BtnClick(NULL);break;
        case MACHINEZX81: HW->ZX81BtnClick(NULL);break;
        case MACHINEACE: HW->AceBtnClick(NULL);break;
        case MACHINETS1500: HW->TS1500BtnClick(NULL);break;
        case MACHINETS1000: HW->TS1000BtnClick(NULL);break;
        case MACHINER470: HW->R470BtnClick(NULL);break;
        case MACHINETK85: HW->TK85BtnClick(NULL);break;
        case MACHINELAMBDA: HW->LambdaBtnClick(NULL);break;
        case MACHINEZX97LE: HW->ZX97LEBtnClick(NULL);break;
        case MACHINESPECTRUM:
                switch(speccy)
                {
                case SPECCY16: HW->Spec16BtnClick(NULL);break;
                case SPECCY48: HW->Spec48BtnClick(NULL);break;
                case SPECCYPLUS: HW->SpecPlusBtnClick(NULL);break;
                case SPECCYTC2048: HW->TC2048BtnClick(NULL);break;
                case SPECCYTC2068: HW->TC2068BtnClick(NULL);break;
                case SPECCYTS2068: HW->TS2068BtnClick(NULL);break;
                case SPECCY128: HW->Spec128BtnClick(NULL);break;
                case SPECCYPLUS2: HW->SpecP2BtnClick(NULL);break;
                case SPECCYPLUS2A: HW->SpecP2aBtnClick(NULL);break;
                }
                break;
        }

        const bool reinitialiseStatus = true;
        const bool disableResetStatus = false;
        HW->UpdateHardwareSettings(reinitialiseStatus, disableResetStatus);
}

__fastcall THW::THW(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        RomCartridgeCapacity = 0;
        RamPackHeight=RamPackBox->Height;
        Machine->ActivePage=Sinclair;
        Advanced->ActivePage=Interfaces;
        FloppyDrives->TabVisible=true;
        DriveAType->ItemIndex=1;
        DriveBType->ItemIndex=0;
        ZXCFRAM->ItemIndex=1;
        ZX81BtnClick(NULL);

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        SetUpRomCartridges();

        ResetRequired=true;
        const bool reinitialiseStatus = true;
        const bool disableResetStatus = false;
        UpdateHardwareSettings(reinitialiseStatus, disableResetStatus);
}
//---------------------------------------------------------------------------

AnsiString getMachineRoot(AnsiString fullRomName)
{
        // return the first part of the rom name up to but excluding the first '.'
        char* p1 = fullRomName.c_str();
        char* p2 = strchr(p1, '.');
        if (p2 == NULL) return fullRomName;
        int len = p2 - p1;
        return fullRomName.SubString(1,len);
}
//---------------------------------------------------------------------------

void THW::SetUpRomCartridges()
{
        ts1510RomCartridges.push_back(RomCartridgeEntry("79001 Supermath", ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79002 States & Capitals", ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79003 Chess", ts1510RomsFolder));
        ts1510RomCartridges.push_back(RomCartridgeEntry("79004 Flight Simulation", ts1510RomsFolder));

        tc2068RomCartridges.push_back(RomCartridgeEntry("335.910.922 Spectrum Emulator", tc2068RomsFolder));
        tc2068RomCartridges.push_back(RomCartridgeEntry("335.911.940 TimeWord v1.2", tc2068RomsFolder));

        ts2068RomCartridges.push_back(RomCartridgeEntry("72000 Budgeter", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("73000 Flight Simulator", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("73001 States & Capitals", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74000 Pinball", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74001 Androids", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74004 Casino 1", ts2068RomsFolder));
        ts2068RomCartridges.push_back(RomCartridgeEntry("74005 Crazybugs", ts2068RomsFolder));

        sinclairRomCartridges.push_back(RomCartridgeEntry("G9R Space Raiders", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G10R Chess", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G12R Planetoids", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G13R Hungry Horace", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G22R Backgammon", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G24R Horace & The Spiders", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G27R Jet Pac", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G28R Pssst", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G29R Tranz Am", if2RomsFolder));
        sinclairRomCartridges.push_back(RomCartridgeEntry("G30R Cookie", if2RomsFolder));              
        sinclairRomCartridges.push_back(RomCartridgeEntry("Spectrum System Test", if2RomsFolder));
}

#include <set>
extern std::set<int> dirtyBird;
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
        const bool reinitialiseStatus = false;
        const bool disableResetStatus = false;
        UpdateHardwareSettings(reinitialiseStatus, disableResetStatus);
}

void THW::UpdateHardwareSettings(bool reinitialise, bool disableReset)
{
        bool machineChanged = (NewMachine != emulator.machine);
        emulator.machine = (CFGBYTE)NewMachine;
        spectrum.model = NewSpec;

        CloseLiveMemoryWindow(machineChanged);

        RomBox->Text = RomBox->Text.LowerCase();
        strcpy(machine.CurRom, AnsiString(RomBox->Text).c_str());

        AnsiString romBase = DetermineRomBase();
        ConfigureSymbolFile(romBase);
        ConfigureCharacterBitmapFile(romBase);

        ConfigureMachineSettings();
        ConfigureRamTop();
        ConfigureDefaultRamSettings();
        DetermineRamSizeLabel(NewMachineName);
        ConfigureColour();
        Configure8K16KRam();
        ConfigureBasicLister();
        ConfigureRom();
        ConfigureMultifaceRom();
        ConfigureZXpand();
        ConfigureRomCartridge();
        ConfigureCharacterGenerator();
        ConfigureHiRes();
        ConfigureSound();
        ConfigureSpeech();
        ConfigureM1Not();
        ConfigureDisplayArtifacts();

        ConfigureSpectrumIDE();
        ConfigureFDC();
        ConfigureModem();
        ConfigurePrinterCentronicsPort();
        ConfigureRzxSupport();

        spectrum.usource = uSource->Checked;
        spectrum.kbissue = Issue2->Checked;
        spectrum.kmouse = KMouse->Checked;

        zx81.improvedWait = ImprovedWait->Checked;
        zx81.shadowROM = !EnableLowRAM->Checked && zx81.truehires != HIRESG007;
        zx81.RAM816k = EnableLowRAM->Checked;
        zx81.RAM816kWriteProtected = Form1->WriteProtect8KRAM->Checked;
        zx81.FloatingPointHardwareFix = FloatingPointHardwareFix->Checked;

        Form1->InWaveLoader->Enabled=true;
        Form1->OutWaveLoader->Enabled=true;

        SelectGroupboxVisibility();

        AccurateInit(false);
        Speed->Recalc(NULL);
        PCKbInit();
        Kb->UpdateCursors();

        if (ResetRequired && !disableReset)
        {
                machine.initialise();
                ResetRequired=false;
        }

        Keyboard->KbChange();

        InitialiseSound(machineChanged);

        Form1->EnableAnnotationOptions();

        spectrum.drivebusy = -1;

        InitPatches(NewMachine);

        ResetDisplaySize();

        ResetDebugger();

        if (!reinitialise) Close();

        if (Dbg->Visible) Dbg->UpdateVals();

        ReInitialiseSound();

        SaveInternalSettings();
}

void THW::ReloadFromInternalSettings()
{
        if (Hwform.MachineName=="ZX80") ZX80BtnClick(NULL);
        else if (Hwform.MachineName=="Spec16") Spec16BtnClick(NULL);
        else if (Hwform.MachineName=="Spec48") Spec48BtnClick(NULL);
        else if (Hwform.MachineName=="SpecPlus") SpecPlusBtnClick(NULL);
        else if (Hwform.MachineName=="Spec128") Spec128BtnClick(NULL);
        else if (Hwform.MachineName=="SpecP2") SpecP2BtnClick(NULL);
        else if (Hwform.MachineName=="SpecP2A") SpecP2aBtnClick(NULL);
        else if (Hwform.MachineName=="SpecP3") SpecP3BtnClick(NULL);
        else if (Hwform.MachineName=="TS1000") TS1000BtnClick(NULL);
        else if (Hwform.MachineName=="TS1500") TS1500BtnClick(NULL);
        else if (Hwform.MachineName=="TC2048") TC2048BtnClick(NULL);
        else if (Hwform.MachineName=="TC2068") TC2068BtnClick(NULL);
        else if (Hwform.MachineName=="TS2068") TS2068BtnClick(NULL);
        else if (Hwform.MachineName=="Lambda") LambdaBtnClick(NULL);
        else if (Hwform.MachineName=="R470") R470BtnClick(NULL);
        else if (Hwform.MachineName=="TK85") TK85BtnClick(NULL);
        else if (Hwform.MachineName=="ZX97LE") ZX97LEBtnClick(NULL);
        else if (Hwform.MachineName=="ACE") AceBtnClick(NULL);
        else ZX81BtnClick(NULL);

        RamPackBox->ItemIndex=Hwform.RamPackBoxItemIndex;
        SoundCardBox->ItemIndex=Hwform.SoundCardBoxItemIndex;
        ChrGenBox->ItemIndex=Hwform.ChrGenBoxItemIndex;
        HiResBox->ItemIndex=Hwform.HiResBoxItemIndex;
        ColourBox->ItemIndex=Hwform.ColourBoxItemIndex;
        SpeechBox->ItemIndex=Hwform.SpeechBoxItemIndex;
        RomCartridgeBox->ItemIndex=Hwform.RomCartridgeBoxItemIndex;
        RomCartridgeFileBox->Text=Hwform.RomCartridgeFileBoxText;
        ZXC1ConfigurationBox->ItemIndex=Hwform.ZXC1ConfigurationBoxItemIndex;
        if (RomCartridgeFileBox->Text.Length() > 0)
        {
                RomCartridgeFileBox->SelStart=RomCartridgeFileBox->Text.Length()-1;
                RomCartridgeFileBox->SelLength=0;
        }
        SinclairRomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TS1510RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TC2068RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        TS2068RomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        DriveAType->ItemIndex=Hwform.DriveATypeItemIndex;
        DriveBType->ItemIndex=Hwform.DriveBTypeItemIndex;
        FDC->ItemIndex=Hwform.FDCItemIndex;
        Autoboot->Checked=Hwform.AutobootChecked;
        ZXpand->Enabled=Hwform.ZXpandEnabled;
        SpecDrum->Enabled=Hwform.SpecDrumEnabled;
        SpecDrum->Checked=Hwform.SpecDrumChecked;
        ProtectROM->Checked=Hwform.ProtectROMChecked;
        NTSC->Checked=Hwform.NTSCChecked;
        EnableLowRAM->Checked=Hwform.EnableLowRAMChecked;
        M1Not->Checked=Hwform.M1NotChecked;
        ImprovedWait->Checked=Hwform.ImprovedWaitChecked;
        TS2050->Checked=Hwform.TS2050Checked;
        Issue2->Checked=Hwform.Issue2Checked;
        KMouse->Checked=Hwform.KMouseChecked;
        Form1->divIDEJumperEClosed->Checked=Hwform.Form1divIDEJumperEClosedChecked;
        Multiface->Checked=Hwform.MultifaceChecked;
        ZXPrinter->Checked=Hwform.ZXPrinterChecked;
        FloatingPointHardwareFix->Checked=Hwform.FloatingPointHardwareFixChecked;
        Upload->Checked=Hwform.UploadChecked;
        ZXCFRAM->ItemIndex=Hwform.ZXCFRAMItemIndex;
        IDEBox->ItemIndex=Hwform.IDEBoxItemIndex;
        uSource->Checked=Hwform.uSourceChecked;

        // Do this after all ZXpand dependencies have been assigned.
        ZXpand->Checked=Hwform.ZXpandChecked;

        SetZXpandState(ZXpand->Checked,ZXpand->Enabled);
        IDEBoxChange(NULL);
        FDCChange(NULL);

        if (ZX80Btn->Down || ZX81Btn->Down || Spec16Btn->Down || Spec48Btn->Down || SpecPlusBtn->Down || Spec128Btn->Down)
        {
                Machine->ActivePage=Sinclair;
        }
        else if (SpecP2Btn->Down || SpecP2aBtn->Down || SpecP3Btn->Down)
        {
                Machine->ActivePage=Amstrad;
        }
        else if (TS1000Btn->Down || TS1500Btn->Down  || TC2048Btn->Down || TS2068Btn->Down || TC2068Btn->Down)
        {
                Machine->ActivePage=Timex;
        }
        else if (ZX97LEBtn->Down)
        {
                Machine->ActivePage=HomeBrew;
        }
        else
        {
                Machine->ActivePage=Others;
        }
}

void THW::SaveInternalSettings()
{
        if (ZX80Btn->Down) Hwform.MachineName="ZX80";
        else if (Spec16Btn->Down) Hwform.MachineName="Spec16";
        else if (Spec48Btn->Down) Hwform.MachineName="Spec48";
        else if (SpecPlusBtn->Down) Hwform.MachineName="SpecPlus";
        else if (Spec128Btn->Down) Hwform.MachineName="Spec128";
        else if (SpecP2Btn->Down) Hwform.MachineName="SpecP2";
        else if (SpecP2aBtn->Down) Hwform.MachineName="SpecP2A";
        else if (SpecP3Btn->Down) Hwform.MachineName="SpecP3";
        else if (TS1000Btn->Down) Hwform.MachineName="TS1000";
        else if (TS1500Btn->Down) Hwform.MachineName="TS1500";
        else if (TC2048Btn->Down) Hwform.MachineName="TC2048";
        else if (TC2068Btn->Down) Hwform.MachineName="TC2068";
        else if (TS2068Btn->Down) Hwform.MachineName="TS2068";
        else if (LambdaBtn->Down) Hwform.MachineName="Lambda";
        else if (R470Btn->Down) Hwform.MachineName="R470";
        else if (TK85Btn->Down) Hwform.MachineName="TK85";
        else if (ZX97LEBtn->Down) Hwform.MachineName="ZX97LE";
        else if (AceBtn->Down) Hwform.MachineName="ACE";
        else Hwform.MachineName="ZX81";

        Hwform.RamPackBoxItemIndex=RamPackBox->ItemIndex;
        Hwform.SoundCardBoxItemIndex=SoundCardBox->ItemIndex;
        Hwform.ChrGenBoxItemIndex=ChrGenBox->ItemIndex;
        Hwform.HiResBoxItemIndex=HiResBox->ItemIndex;
        Hwform.ColourBoxItemIndex=ColourBox->ItemIndex;
        Hwform.SpeechBoxItemIndex=SpeechBox->ItemIndex;
        Hwform.RomCartridgeBoxItemIndex=RomCartridgeBox->ItemIndex;
        Hwform.RomCartridgeFileBoxText=RomCartridgeFileBox->Text;
        Hwform.ZXC1ConfigurationBoxItemIndex=ZXC1ConfigurationBox->ItemIndex;
        Hwform.DriveATypeItemIndex=DriveAType->ItemIndex;
        Hwform.DriveBTypeItemIndex=DriveBType->ItemIndex;
        Hwform.FDCItemIndex=FDC->ItemIndex;
        Hwform.AutobootChecked=Autoboot->Checked;
        Hwform.ZXpandChecked=ZXpand->Checked;
        Hwform.ZXpandEnabled=ZXpand->Enabled;
        Hwform.SpecDrumChecked=SpecDrum->Checked;
        Hwform.SpecDrumEnabled=SpecDrum->Enabled;
        Hwform.ProtectROMChecked=ProtectROM->Checked;
        Hwform.NTSCChecked=NTSC->Checked;
        Hwform.EnableLowRAMChecked=EnableLowRAM->Checked;
        Hwform.M1NotChecked=M1Not->Checked;
        Hwform.ImprovedWaitChecked=ImprovedWait->Checked;
        Hwform.TS2050Checked=TS2050->Checked;
        Hwform.Issue2Checked=Issue2->Checked;
        Hwform.KMouseChecked=KMouse->Checked;
        Hwform.Form1divIDEJumperEClosedChecked=Form1->divIDEJumperEClosed->Checked;
        Hwform.MultifaceChecked=Multiface->Checked;
        Hwform.ZXPrinterChecked=ZXPrinter->Checked;
        Hwform.FloatingPointHardwareFixChecked=FloatingPointHardwareFix->Checked;
        Hwform.UploadChecked=Upload->Checked;
        Hwform.ZXCFRAMItemIndex=ZXCFRAM->ItemIndex;
        Hwform.IDEBoxItemIndex=IDEBox->ItemIndex;
        Hwform.uSourceChecked=uSource->Checked;
}

void THW::Configure8K16KRam()
{
        bool enable8K16KProtectOption = EnableLowRAM->Checked && (emulator.machine != MACHINESPECTRUM && emulator.machine != MACHINER470 && emulator.machine != MACHINEACE && emulator.machine != MACHINEZX97LE);
        Form1->WriteProtect8KRAM->Enabled = enable8K16KProtectOption;
        if (!Form1->WriteProtect8KRAM->Enabled)
        {
                Form1->WriteProtect8KRAM->Checked = false;
        }
}

void THW::ConfigureRzxSupport()
{
        Form1->RZX1->Enabled=false;
        if (emulator.machine==MACHINESPECTRUM) Form1->RZX1->Enabled=true;
}

void THW::ReInitialiseSound()
{
        int r = Sound.ReInitialise(NULL, machine.fps,0,0,0);
        if (r)
        {
                AnsiString err = "EightyOne is unable to run. DirectSound creation failed, reporting error " + DirectSoundError(r);
                MessageBox(NULL, err.c_str(), "Error", 0);
                Application->Terminate();
        }
}

void THW::ResetDebugger()
{
        Dbg->Reset();
}

void THW::ConfigureDisplayArtifacts()
{
        Artifacts->TrackBarChange(NULL);
        Artifacts->ConfigureDotCrawlOption();

        bool spec48 = (emulator.machine==MACHINESPECTRUM && spectrum.model<SPECCY128);
        if (!spec48)
        {
                Artifacts->DotCrawl1->Checked = false;
        }

        Artifacts->ForceVibrantColours(machine.colour == COLOURCHROMA);
}

void THW::ConfigureM1Not()
{
        zx81.m1not = M1Not->Checked? 49152 : 32768;

        if (emulator.machine==MACHINEZX97LE)
        {
                zx81.m1not=49152;
        }
}

void THW::ConfigureModem()
{
        if (TS2050->Checked==1) { machine.ts2050=1; Form1->TS20501->Enabled=true; }
        else { machine.ts2050=0; Form1->TS20501->Enabled=false; }
}

void THW::ConfigurePrinterCentronicsPort()
{
        if (spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.model==SPECCYPLUS2A
                || spectrum.model==SPECCYPLUS3)
        {
                Form1->PrinterPort1->Enabled=true;
                if (spectrum.floppytype==FLOPPYOPUSD)
                        Form1->PrinterPort1->Caption="Opus Printer Port";
                if (spectrum.floppytype==FLOPPYDISCIPLE)
                        Form1->PrinterPort1->Caption="DISCiPLE Printer Port";
                if (spectrum.floppytype==FLOPPYPLUSD)
                        Form1->PrinterPort1->Caption="Plus D Printer Port";
                if (spectrum.model==SPECCYPLUS2A)
                        Form1->PrinterPort1->Caption="+2A/+3 Printer Port";
                if (spectrum.model==SPECCYPLUS3)
                        Form1->PrinterPort1->Caption="+2A/+3 Printer Port";
        }
        else
        {
                Form1->PrinterPort1->Enabled=false;
                Form1->PrinterPort1->Caption="Centronics Printer Port";
        }
}

void THW::SelectGroupboxVisibility()
{
        Dbg->GroupBoxZX81->Visible=false;
        Dbg->GroupBoxAce->Visible=false;
        Dbg->GroupBoxChroma->Visible=false;
        Dbg->GroupBoxSpectra->Visible=false;
        Dbg->GroupBoxZXC->Visible=false;

        switch(emulator.machine)
        {
        case MACHINEACE:
                Dbg->GroupBoxAce->Visible=true;
                break;

        case MACHINESPECTRUM:
                Dbg->GroupBoxSpectra->Visible=Form1->SpectraColourEnable->Visible;
                Dbg->GroupBoxZXC->Visible=Form1->SpectraColourEnable->Visible;
                break;

        default:
                Dbg->GroupBoxZX81->Visible=Form1->ChromaColourEnable->Visible;
                Dbg->GroupBoxChroma->Visible=Form1->ChromaColourEnable->Visible;
                Dbg->GroupBoxZXC->Visible=Form1->ChromaColourEnable->Visible;
                break;
        }
}

void THW::ConfigureRamTop()
{
        if (NewMachine == MACHINESPECTRUM)
        {
                zx81.RAMTOP = 65535;
                machine.ace96k=0;
        }
        else if (RamPackBox->Items->Strings[RamPackBox->ItemIndex]=="96K")
        {
                zx81.RAMTOP = 65535;
                machine.ace96k=1;
        }
        else if (emulator.machine==MACHINEZX97LE)
        {
                zx81.RAMTOP=65535;
        }
        else
        {
                int kp = machine.baseRamSize;
                if (RamPackBox->ItemIndex!=0)
                {
                        int rp = atoi(AnsiString(RamPackBox->Items->Strings[RamPackBox->ItemIndex]).c_str());
                        kp = machine.ramPackSupplementsInternalRam ? kp + rp : rp;
                }
                zx81.RAMTOP = (kp << 10) + 16383;
                if (RamPackBox->ItemIndex == 6) zx81.RAMTOP = 65535;
                if (emulator.machine==MACHINEACE) zx81.RAMTOP -= 3072;
                machine.ace96k=0;
        }
}

void THW::InitialiseSound(bool machineChanged)
{
        Sound.InitDevices();

        if (machineChanged)
        {
                if ( ((emulator.machine==MACHINESPECTRUM || emulator.machine==MACHINEACE || emulator.machine==MACHINELAMBDA) && !Form1->Sound1->Checked)
                || ( (emulator.machine!=MACHINESPECTRUM && emulator.machine!=MACHINEACE && emulator.machine!=MACHINELAMBDA) && (machine.colour != COLOURCHROMA) && Form1->Sound1->Checked) )
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
                int i=3;
                AnsiString Mem;

                if (RamPackBox->ItemIndex!=0)
                        i+=atoi(AnsiString(RamPackBox->Items->Strings[RamPackBox->ItemIndex]).c_str());

                name = i;
                name += "K Jupiter Ace";
        }
        else if (RamPackBox->Visible)
        {
                int totalRam = machine.baseRamSize;
                if (RamPackBox->ItemIndex!=0)
                {
                        int ramPack = atoi(AnsiString(RamPackBox->Items->Strings[RamPackBox->ItemIndex]).c_str());
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
        spectrum.MFVersion=MFNONE;
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
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        break;

                case SPECCYPLUS2A:
                case SPECCYPLUS3:
                        spectrum.MFVersion=MFPLUS3;
                        Multiface->Caption = "Multiface 3";
                        Multiface->Enabled = true;
                        break;
                case SPECCYTC2068:
                case SPECCYTS2068:
                        spectrum.MFVersion=MFNONE;
                        Multiface->Enabled = false;
                        break;
                }
        }

        if (!Multiface->Checked) spectrum.MFVersion=MFNONE;
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
                        spectrum.RAMBanks=1;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP16, machine.CurRom);
                        break;

                case SPECCY48:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP48, machine.CurRom);
                        break;

                case SPECCYPLUS:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSPP, machine.CurRom);
                        break;

                case SPECCYTC2048:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMTC2048, machine.CurRom);
                        break;

                case SPECCYTC2068:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMTC2068, machine.CurRom);
                        break;

                case SPECCYTS2068:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMTS2068, machine.CurRom);
                        break;

                case SPECCY128:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP128, machine.CurRom);
                        break;

                case SPECCYPLUS2:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMSPP2, machine.CurRom);
                        break;

                case SPECCYPLUS2A:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Simple +3e 8-Bit")
                                strcpy(emulator.ROMSPP3E, machine.CurRom);
                        else strcpy(emulator.ROMSPP2A, machine.CurRom);
                        break;

                case SPECCYPLUS3:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Simple +3e 8-Bit")
                                strcpy(emulator.ROMSPP3E, machine.CurRom);
                        else strcpy(emulator.ROMSPP3, machine.CurRom);
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
        bool prevChromaColourSwitchOn = (machine.colour == COLOURCHROMA);
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
        Form1->ChromaColourEnable->Enabled = zx81.chromaColourSwitchOn;
        Form1->ChromaColourEnable->Checked = zx81.chromaColourSwitchOn;
        Form1->ChromaColourEnable->Visible = (NewMachine == MACHINEZX80) || (NewMachine == MACHINEZX81) ||
                                             (NewMachine == MACHINETS1000) || (NewMachine == MACHINETS1500);
}

void THW::ConfigureSpectra(bool prevSpectraColourSwitchOn)
{
        spectrum.spectraColourSwitchOn = (machine.colour == COLOURSPECTRA);
        if (!prevSpectraColourSwitchOn && spectrum.spectraColourSwitchOn)
        {
                InitialiseSpectra();
        }
        Form1->SpectraColourEnable->Enabled = spectrum.spectraColourSwitchOn;
        Form1->SpectraColourEnable->Checked = spectrum.spectraColourSwitchOn;
        Form1->SpectraColourEnable->Visible = (NewMachine == MACHINESPECTRUM) && ((NewSpec != SPECCYTC2048) && (NewSpec != SPECCYTC2068) && (NewSpec != SPECCYTS2068));
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
                        UnicodeString msg = "Failed to load cartridge file:\n\n";
                        msg += romCartridgeFilePath;
                        Application->MessageBox(msg.c_str(), L"Error", MB_OK | MB_ICONERROR);

                        LoadDock((char *)"");
                }

                ResetRequired=true;
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
                RomCartridgeFileBox->Width = 91;

                if (ZXC1ConfigurationBox->ItemIndex == -1)
                {
                        ZXC1ConfigurationBox->ItemIndex = 0;
                }
        }
        else
        {
                RomCartridgeFileBox->Left = 184;
                RomCartridgeFileBox->Width = 184;
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
        }
        else
        {
                zx81.chrgen = (CFGBYTE)ChrGenBox->ItemIndex;
        }
        Form1->QSChrEnable->Checked = zx81.enableQSchrgen;
        Form1->QSChrEnable->Enabled = (zx81.chrgen == CHRGENQS);
        Form1->QSChrEnable->Visible = (NewMachine != MACHINEACE) && (NewMachine != MACHINESPECTRUM);

        zx81.extfont=0;
        if ((zx81.chrgen==CHRGENDK) || (zx81.chrgen==CHRGENCHR128))
                zx81.maxireg = 0x3F;
        else
                zx81.maxireg = 0x1F;
        if (ChrGenBox->Text == "Lambda") zx81.extfont=1;
}

void THW::ConfigureHiRes()
{
        Form1->ResetMemotechHRG->Enabled = false;
        Form1->ResetMemotechHRG->Visible = (NewMachine != MACHINEACE) && (NewMachine != MACHINESPECTRUM);

        Form1->ResetQuicksilvaHiRes->Enabled = false;
        Form1->ResetQuicksilvaHiRes->Visible = (NewMachine != MACHINEACE) && (NewMachine != MACHINESPECTRUM);

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

void THW::ConfigureSound()
{
        if (NewMachine == MACHINESPECTRUM)
        {
                if (SpecDrum->Checked)
                        spectrum.specdrum=1;
                else
                        spectrum.specdrum=0;

                if (NewSpec == SPECCYTS2068)
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 1: machine.aytype=AY_TYPE_TS2068; break;
                        case 0:
                        default: machine.aytype=AY_TYPE_NONE; break;
                        }
                }
                else if (NewSpec == SPECCYTC2068)
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 1: machine.aytype=AY_TYPE_TC2068; break;
                        case 0:
                        default: machine.aytype=AY_TYPE_NONE; break;
                        }
                }
                else
                {
                        switch (SoundCardBox->ItemIndex)
                        {
                        case 4: machine.aytype=AY_TYPE_ZONX; break;
                        case 3: machine.aytype=AY_TYPE_DKTRONICS; break;
                        case 2: machine.aytype=AY_TYPE_FULLER; break;
                        case 1: machine.aytype=AY_TYPE_SINCLAIR; break;
                        case 0:
                        default: machine.aytype=AY_TYPE_NONE; break;
                        }
                }
        }
        else if (NewMachine == MACHINEACE)
        {
                switch (SoundCardBox->ItemIndex)
                {
                case 2: machine.aytype=AY_TYPE_BOLDFIELD; break;
                case 1: machine.aytype=AY_TYPE_ACE_USER; break;
                case 0:
                default: machine.aytype=AY_TYPE_NONE; break;
                }
        }
        else
        {
                switch (SoundCardBox->ItemIndex)
                {
                case 2: machine.aytype=AY_TYPE_QUICKSILVA; break;
                case 1: machine.aytype=AY_TYPE_ZONX; break;
                case 0:
                default: machine.aytype=AY_TYPE_NONE; break;
                }
        }
}

void THW::ConfigureSpeech()
{
        Form1->ResetSpeech->Enabled = (SpeechBox->ItemIndex != 0);
        Form1->ResetSpeech->Visible = SpeechBox->Enabled;

        if (NewMachine == MACHINESPECTRUM)
        {
                if (NewSpec >= SPECCY128)
                {
                        switch (SpeechBox->ItemIndex)
                        {
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
                        case 4: machine.speech = SPEECH_TYPE_USPEECH; break;
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
                case 4: machine.speech = SPEECH_TYPE_TALKBACK; break;
                case 3: machine.speech = SPEECH_TYPE_MAGECO; break;
                case 2: machine.speech = SPEECH_TYPE_SWEETTALKER; break;
                case 1:
                {
                        machine.speech = SPEECH_TYPE_PARROT;
                        break;
                }
                case 0:
                default: machine.speech = SPEECH_TYPE_NONE; break;
                }
        }
        else
        {
                machine.speech = SPEECH_TYPE_NONE;
        }
}

void THW::ConfigureSpectrumIDE()
{
        Form1->divIDEJumperEClosed->Visible = (NewMachine == MACHINESPECTRUM) && ((NewSpec != SPECCYTC2048) && (NewSpec != SPECCYTC2068) && (NewSpec != SPECCYTS2068));

        spectrum.HDType=HDNONE;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF") spectrum.HDType=HDZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE 57 (R Gal)") { spectrum.HDType=HDDIVIDE; spectrum.divIDEAllRamSupported = false; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE 57 (R'' Gal)") { spectrum.HDType=HDDIVIDE; spectrum.divIDEAllRamSupported = true; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Simple +3e 8-Bit") spectrum.HDType=HDPLUS3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="AceCF") spectrum.HDType=HDACECF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") spectrum.HDType=HDPITERSCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") spectrum.HDType=HDPITERS8B;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") spectrum.HDType=HDPITERS16B;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="MWCFIde") spectrum.HDType=HDPITERSCF;
        spectrum.divIDEJumperEClosed = Form1->divIDEJumperEClosed->Checked;
        spectrum.UploadJumperZXCF=Upload->Checked;

        switch(ZXCFRAM->ItemIndex)
        {
        case 0: spectrum.ZXCFRAMSize=128/16; break;
        case 1: spectrum.ZXCFRAMSize=512/16; break;
        case 2: spectrum.ZXCFRAMSize=1024/16; break;
        }
}

void THW::ConfigureFDC()
{
        spectrum.floppytype=FLOPPYNONE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="DISCiPLE") spectrum.floppytype=FLOPPYDISCIPLE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Plus D") spectrum.floppytype=FLOPPYPLUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="+3") spectrum.floppytype=FLOPPYPLUS3;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Opus Discovery") spectrum.floppytype=FLOPPYOPUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Beta Disk") spectrum.floppytype=FLOPPYBETA;
        if (FDC->Items->Strings[FDC->ItemIndex]=="ZX Interface 1") spectrum.floppytype=FLOPPYIF1;
        if (FDC->Items->Strings[FDC->ItemIndex]=="ZX1541") spectrum.floppytype=FLOPPYZX1541;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Larken") spectrum.floppytype=FLOPPYLARKEN81;

        if (Autoboot->Checked) spectrum.autoboot=true;
        else spectrum.autoboot=false;

        switch(DriveAType->ItemIndex)
        {
        case 0: spectrum.driveatype=DRIVENONE; break;
        case 1: spectrum.driveatype=DRIVE3INCHSS; break;
        case 2: spectrum.driveatype=DRIVE3INCHDS; break;
        case 3: spectrum.driveatype=DRIVE35INCHDS; break;
        case 4: spectrum.driveatype=DRIVE35INCHDS; break;
        }

        switch(DriveBType->ItemIndex)
        {
        case 0: spectrum.drivebtype=DRIVENONE; break;
        case 1: spectrum.drivebtype=DRIVE3INCHSS; break;
        case 2: spectrum.drivebtype=DRIVE3INCHDS; break;
        case 3: spectrum.drivebtype=DRIVE35INCHDS; break;
        case 4: spectrum.drivebtype=DRIVE35INCHDS; break;
        }

        Form1->DiskDrives1->Enabled=true;
        P3Drive->FormShow(NULL);
}

void THW::ConfigureMachineSettings()
{
        machine.protectROM = ProtectROM->Checked;
        machine.zxprinter = ZXPrinter->Checked;
        machine.NTSC = NTSC->Checked;

        machine.clockspeed=3250000;
        machine.tperscanline=207;
        machine.scanlines=machine.NTSC ? 262:310;
        machine.tperframe = machine.tperscanline * machine.scanlines;
        if (emulator.machine == MACHINEZX80)
        {
                machine.tperframe -= 3;
        }

        if (emulator.machine==MACHINELAMBDA)
                machine.tperscanline=208;

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
                machine.reset = NULL;
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

                if (spectrum.model==SPECCY16 || spectrum.model==SPECCY48 || spectrum.model==SPECCYPLUS || spectrum.model==SPECCYTC2048)
                {
                        machine.clockspeed=3500000;
                        machine.tperscanline=224;
                        spectrum.intposition=14336;
                        machine.scanlines=312;
                        machine.fps=50;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
                }
                else if (spectrum.model==SPECCYTC2068)
                {
                        machine.clockspeed=3528000;
                        machine.tperscanline=226;
                        spectrum.intposition=13560;
                        machine.scanlines=312;
                        machine.fps=50;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
                }
                else if (spectrum.model==SPECCYTS2068)
                {
                        machine.clockspeed=3528000;
                        machine.tperscanline=226;
                        spectrum.intposition=10848;
                        machine.scanlines=262;
                        machine.fps=60;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
                }
                else
                {
                        machine.clockspeed=3546900;
                        machine.tperscanline=228;
                        spectrum.intposition=14336-228+32;
                        machine.scanlines=311;
                        machine.fps=50;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
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
                machine.fps = machine.NTSC ? 60:50; // may be overwritten later
                machine.reset = zx81_reset;
                machine.exit = NULL;
                break;
        }
}

AnsiString THW::DetermineRomBase()
{
        AnsiString romBase = emulator.cwd;
        romBase += romsFolder;

        AnsiString rom = machine.CurRom;

        if (!FileExists(rom))
        {
                rom = romBase + machine.CurRom;
        }
        if (!FileExists(rom))
        {
                ShowMessage("ROM file for this system not found. Go to:\n\n"
                "Options > Hardware > Advanced Settings > ROM File.");
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

        ZX80Btn->Down=false;
        ZX81Btn->Down=false;
        Spec16Btn->Down=false;
        Spec48Btn->Down=false;
        SpecPlusBtn->Down=false;
        Spec128Btn->Down=false;
        SpecP2Btn->Down=false;
        SpecP2aBtn->Down=false;
        SpecP3Btn->Down=false;
        TS1000Btn->Down=false;
        TS1500Btn->Down=false;
        TC2048Btn->Down=false;
        TC2068Btn->Down=false;
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;

        FloatingPointHardwareFix->Enabled = false;
        ButtonAdvancedMore->Visible = false;

        EnableRomCartridgeOption(true);

        ZXpand->Caption = "ZXpand+";
        Multiface->Caption = "Multiface 128";

        KMouse->Checked = false;
        KMouse->Enabled = false;

        OldFloppy=FDC->Items->Strings[FDC->ItemIndex];
        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->Items->Add("ZX1541");
        FDC->Items->Add("Larken");
        FDC->ItemIndex=0;

        for(i=0;i<FDC->Items->Count;i++)
                if (FDC->Items->Strings[i]==OldFloppy) FDC->ItemIndex=i;

        FDC->Enabled=true;
        DriveAType->Enabled=false;
        DriveBType->Enabled=false;

        ResetRequired=true;

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count-1] == "96K")
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count-1] != "48K")
        {
                RamPackBox->Items->Add("48K");
        }

        if (NewMachine == MACHINETS1500 || NewMachine == MACHINER470 || NewMachine == MACHINETK85)
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        RamPackLbl->Enabled=true; RamPackBox->Enabled=true;
        RamPackBox->ItemIndex = defaultRamPackIndex;
        DisplayTotalRam();

        ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenLbl->Enabled=true; ChrGenBox->Enabled=true;
        if (ChrGenBox->ItemIndex==-1) ChrGenBox->ItemIndex=0;

        if ((!HiResBox->Enabled) || (HiResBox->ItemIndex==-1))
        {
                HiResBox->Items->Strings[0]="None";
                HiResBox->ItemIndex=0;
        }
        HiResLbl->Enabled=true; HiResBox->Enabled=true;

        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->Items->Add("Chroma");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;
        ColourLabel->Enabled=true;
        ColourLabel->Caption = "Colour:";

        ProtectROM->Enabled=true;
        NTSC->Enabled=true;
        EnableLowRAM->Enabled=true;
        M1Not->Enabled=true;
        ImprovedWait->Enabled=true;
        TS2050->Enabled=true;
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;

        Issue2->Checked=false;
        Issue2->Enabled=false;

        if (!RamPackBox->Visible)
        {
                Height += RamPackHeight;
                RamPackLbl->Visible=true;
                RamPackBox->Visible=true;
                LabelTotalRAM->Visible=true;
        }

        Multiface->Enabled=false;
        Multiface->Checked=false;

        OldIDE=IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("MWCFIde");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=true;
        LabelIDE->Enabled=true;
        LabelFDC->Enabled=true;
        for(i=0;i<IDEBox->Items->Count;i++)
        {
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;
        }

        SoundCardBox->Items->Clear();
        SoundCardBox->Items->Add("None");
        SoundCardBox->Items->Add("ZON X");
        SoundCardBox->Items->Add("Quicksilva");
        SoundCardBox->ItemIndex=0;
        SoundCardBox->Enabled=true;
        SoundCardLbl->Enabled=true;

        SpeechBox->Items->Clear();
        SpeechBox->Items->Add("None");
        SpeechBox->Items->Add("The Parrot");
        SpeechBox->Items->Add("Sweet Talker");
        SpeechBox->Items->Add("Mageco");
        if (NewMachine != MACHINEZX80)
        {
                SpeechBox->Items->Add("Talk-Back");
        }
        SpeechBox->ItemIndex=0;
        SpeechBox->Enabled=true;
        SpeechBoxLbl->Enabled=true;

        uSource->Enabled=false;
        uSource->Checked=false;

        SpecDrum->Checked=false;
        SpecDrum->Enabled=false;

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
        ZXpand->Checked=checked;
        ZXpand->Enabled=enabled;
        ZXpandEmulationInfo->Visible=checked;
        ButtonZXpandSDCard->Enabled = checked && enabled;
}

void THW::SetupForSpectrum(void)
{
        AnsiString OldIDE,OldFDC;
        int i;

        ZX80Btn->Down=false;
        ZX81Btn->Down=false;
        Spec16Btn->Down=false;
        Spec48Btn->Down=false;
        SpecPlusBtn->Down=false;
        Spec128Btn->Down=false;
        SpecP2Btn->Down=false;
        SpecP2aBtn->Down=false;
        SpecP3Btn->Down=false;
        TS1000Btn->Down=false;
        TS1500Btn->Down=false;
        TC2048Btn->Down=false;
        TC2068Btn->Down=false;
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;

        FloatingPointHardwareFix->Enabled = false;
        FloatingPointHardwareFix->Checked = false;

        machine.plus3arabicPagedOut=0;

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

        ResetRequired=true;

        EnableRomCartridgeOption(true);

        OldFDC=FDC->Items->Strings[FDC->ItemIndex];
        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->Items->Add("ZX Interface 1");
        FDC->Items->Add("Beta Disk");
        FDC->Items->Add("Opus Discovery");
        FDC->Items->Add("DISCiPLE");
        FDC->Items->Add("Plus D");

        FDC->ItemIndex=0;
        for(i=0;i<FDC->Items->Count;i++)
        {
                if (FDC->Items->Strings[i]==OldFDC) FDC->ItemIndex=i;
        }

        FDC->Enabled=true;
        FDCChange(NULL);

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
                SoundCardBox->Enabled=true;
                SoundCardLbl->Enabled=true;
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
        SpeechBox->ItemIndex = 0;
        SpeechBox->Enabled=true;
        SpeechBoxLbl->Enabled=true;

        uSource->Checked=false;
        uSource->Enabled=false;

        SpecDrum->Checked=false;
        SpecDrum->Enabled=true;

        RamPackLbl->Enabled=false; RamPackBox->Enabled=false;
        RamPackBox->ItemIndex=-1;

        ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenBox->ItemIndex=0;
        ChrGenLbl->Enabled=false; ChrGenBox->Enabled=false;

        HiResBox->Items->Strings[0]="Sinclair";
        HiResBox->ItemIndex=0;
        HiResLbl->Enabled=false; HiResBox->Enabled=false;

        ColourLabel->Caption = "Colour:";

        bool spectraAlreadyAvailable = (ColourBox->Items->Strings[1] == "Spectra");
        if (!spectraAlreadyAvailable)
        {
                while(ColourBox->Items->Count>1) ColourBox->Items->Delete(ColourBox->Items->Count-1);
                ColourBox->Items->Strings[0]="Sinclair";
                ColourBox->Items->Add("Spectra");
                ColourBox->ItemIndex=0;
                ColourBox->Enabled=true;
                ColourLabel->Enabled=true;
        }

        ProtectROM->Enabled=true;
        NTSC->Enabled=false;
        NTSC->Checked=false;
        EnableLowRAM->Enabled=false;
        EnableLowRAM->Checked=false;
        M1Not->Enabled=false;
        M1Not->Checked=false;
        ImprovedWait->Enabled=false;
        ImprovedWait->Checked=false;
        TS2050->Enabled=true;
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;

        Issue2->Checked=false;
        Issue2->Enabled=false;

        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible=false;
                RamPackBox->Visible=false;
                LabelTotalRAM->Visible=false;
        }
        Multiface->Enabled=true;

        OldIDE=IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("Simple +3e 8-Bit");
        IDEBox->Items->Add("divIDE 57 (R Gal)");
        IDEBox->Items->Add("divIDE 57 (R'' Gal)");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Pera Putnik CF");
        IDEBox->Items->Add("Pera Putnik 8-Bit");
        IDEBox->Items->Add("Pera Putnik 16-Bit");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=true;
        LabelIDE->Enabled=true;
        LabelFDC->Enabled=true;

        for(i=0;i<IDEBox->Items->Count;i++)
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;

        uSource->Enabled=true;

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
                if (NewSpec==SPECCY16 || NewSpec==SPECCY48 || NewSpec==SPECCYPLUS || NewSpec==SPECCYTC2048)
                {
                        baseRamSize = (NewSpec==SPECCY16) ? 16 : 48;
                        ramPackSupplementsInternalRam = true;
                }
                else if (NewSpec==SPECCYTS2068 || NewSpec==SPECCYTC2068)
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

void THW::DisplayTotalRam()
{
        int totalRam = baseRamSize;
        if (RamPackBox->ItemIndex>0)
        {
                int ramPack = atoi(AnsiString(RamPackBox->Items->Strings[RamPackBox->ItemIndex]).c_str());
                totalRam = ramPackSupplementsInternalRam ? totalRam + ramPack : ramPack;
        }
        AnsiString ramSize = totalRam;
        LabelTotalRAM->Caption = "Total RAM: " + ramSize + "K";
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX80BtnClick(TObject *Sender)
{
        if (ZX80Btn->Down) return;

        NewMachine=MACHINEZX80;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        ZXpand->Caption = "ZXpand";
        ZX80Btn->Down=true;
        NewMachineName=ZX80Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("zx80.rom");
        RomBox->Items->Add("zx81.edition1.rom");
        RomBox->Text = emulator.ROM80;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        RomBoxChange(NULL);
        ImprovedWait->Enabled = false;
        ImprovedWait->Checked = false;

        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX81BtnClick(TObject *Sender)
{
        if (ZX81Btn->Down) return;

        NewMachine=MACHINEZX81;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        ZX81Btn->Down=true;
        NewMachineName=ZX81Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("zx81.edition1.rom");
        RomBox->Items->Add("zx81.edition2.rom");
        RomBox->Items->Add("zx81.edition3.rom");
        RomBox->Text = emulator.ROM81;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        FloatingPointHardwareFix->Enabled = true;
        NTSC->Checked=false;

        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec48BtnClick(TObject *Sender)
{
        if (Spec48Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCY48;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec48Btn->Down=true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled=true;

        Issue2->Enabled=true;
        NewMachineName=Spec48Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum48.rom");
        RomBox->Items->Add("spectrum48.nordic.rom");
        RomBox->Text = emulator.ROMSP48;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec128BtnClick(TObject *Sender)
{
        if (Spec128Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCY128;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec128Btn->Down=true;

        SoundCardBox->ItemIndex = 1;

        NewMachineName=Spec128Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum128.rom");
        RomBox->Items->Add("spectrum128.spanish.rom");
        RomBox->Items->Add("spectrum48.arabic.version1.rom");
        RomBox->Text = emulator.ROMSP128;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;

        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecPlusBtnClick(TObject *Sender)
{
        if (SpecPlusBtn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYPLUS;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecPlusBtn->Down=true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled=true;

        Issue2->Enabled=true;
        NewMachineName=SpecPlusBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum48.rom");
        RomBox->Items->Add("spectrum48.spanish.rom");
        RomBox->Items->Add("spectrum48.arabic.version1.rom");
        RomBox->Text = emulator.ROMSPP;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec16BtnClick(TObject *Sender)
{
        if (Spec16Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCY16;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        Spec16Btn->Down=true;

        SpeechBox->Items->Add("�Speech");

        uSource->Enabled=true;

        Issue2->Enabled=true;
        Issue2->Checked=true;
        NewMachineName=Spec16Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum48.rom");
        RomBox->Text = emulator.ROMSP16;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2BtnClick(TObject *Sender)
{
        if (SpecP2Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYPLUS2;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP2Btn->Down=true;

        SoundCardBox->ItemIndex = 1;

        NewMachineName=SpecP2Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum+2.rom");
        RomBox->Items->Add("spectrum+2.french.rom");
        RomBox->Items->Add("spectrum+2.spanish.rom");
        RomBox->Items->Add("spectrum48.arabic.version2.rom");
        RomBox->Text = emulator.ROMSPP2;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2aBtnClick(TObject *Sender)
{
        if (SpecP2aBtn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYPLUS2A;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP2aBtn->Down=true;
        Multiface->Caption = "Multiface 3";

        SoundCardBox->ItemIndex = 1;

        NewMachineName=SpecP2aBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum+3.version4-0.rom");
        RomBox->Items->Add("spectrum+3.version4-1.rom");
        RomBox->Items->Add("spectrum+3.version4-0.spanish.rom");
        RomBox->Items->Add("spectrum+3.version4-1.spanish.rom");
        RomBox->Items->Add("spectrum48.arabic.version2.rom");
        RomBox->Text = emulator.ROMSPP2A;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP3BtnClick(TObject *Sender)
{
        if (SpecP3Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYPLUS3;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        SpecP3Btn->Down=true;
        Multiface->Caption = "Multiface 3";

        FloppyDrives->TabVisible=true;

        SoundCardBox->ItemIndex = 1;

        NewMachineName=SpecP3Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spectrum+3.version4-0.rom");
        RomBox->Items->Add("spectrum+3.version4-1.rom");
        RomBox->Items->Add("spectrum+3.version4-0.spanish.rom");
        RomBox->Items->Add("spectrum+3.version4-1.spanish.rom");
        RomBox->Items->Add("spectrum+3.arabic3-a.english4-0.rom");
        RomBox->Text = emulator.ROMSPP3;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        IDEBoxChange(NULL);

        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="+3";
        FDC->ItemIndex=0;

        FDC->Enabled=false;
        FDCChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1000BtnClick(TObject *Sender)
{
        if (TS1000Btn->Down) return;

        NewMachine=MACHINETS1000;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        TS1000Btn->Down=true;
        NewMachineName=TS1000Btn->Caption;
        FloatingPointHardwareFix->Checked = false;
        RomBox->Clear();
        RomBox->Items->Add("zx81.edition3.rom");
        RomBox->Text = emulator.ROMTS1000;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ColourLabel->Caption = "Color:";
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1500BtnClick(TObject *Sender)
{
        if (TS1500Btn->Down) return;

        NewMachine=MACHINETS1500;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,true);
        TS1500Btn->Down=true;
        NewMachineName=TS1500Btn->Caption;
        FloatingPointHardwareFix->Checked = false;
        RomBox->Clear();
        RomBox->Items->Add("ts1500.rom");
        RomBox->Text = emulator.ROMTS1500;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ColourLabel->Caption = "Color:";
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaBtnClick(TObject *Sender)
{
        if (LambdaBtn->Down) return;

        NewMachine=MACHINELAMBDA;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        LambdaBtn->Down=true;
        NewMachineName=LambdaBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("lambda8300.rom");
        RomBox->Items->Add("lambda8300colour.rom");
        RomBox->Text = emulator.ROMLAMBDA;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        ChrGenBox->Items->Strings[0] = "Lambda";
        ChrGenBox->ItemIndex=0;
        ChrGenBox->Enabled=false;
        ChrGenLbl->Enabled=false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;
        HiResBox->ItemIndex=0;
        HiResBox->Enabled=false;
        HiResLbl->Enabled=false;
        RomCartridgeBox->Enabled = false;
        RomCartridgeFileBox->Enabled = false;
        TS1510RomCartridgeFileBox->Enabled = false;
        TC2068RomCartridgeFileBox->Enabled = false;
        TS2068RomCartridgeFileBox->Enabled = false;
        SinclairRomCartridgeFileBox->Enabled = false;
        BrowseRomCartridge->Enabled = false;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::R470BtnClick(TObject *Sender)
{
        if (R470Btn->Down) return;

        NewMachine=MACHINER470;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        R470Btn->Down=true;
        NewMachineName=R470Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("ringo470.rom");
        RomBox->Text = emulator.ROMR470;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        EnableLowRAM->Checked=false;
        EnableLowRAM->Enabled=false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TK85BtnClick(TObject *Sender)
{
        if (TK85Btn->Down) return;

        NewMachine=MACHINETK85;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        TK85Btn->Down=true;
        NewMachineName=TK85Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("tk85.rom");
        RomBox->Text = emulator.ROMTK85;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;
        EnableRomCartridgeOption(false);
        FloatingPointHardwareFix->Checked = false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::AceBtnClick(TObject *Sender)
{
        if (AceBtn->Down) return;

        NewMachine=MACHINEACE;
        RefreshDefaultRamSettings();
        SetupForZX81();
        IDEBox->Enabled=true;
        LabelIDE->Enabled=true;

        AceBtn->Down=true;
        NewMachineName=AceBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("jupiterace.rom");
        RomBox->Text = emulator.ROMACE;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=false;
        NTSC->Enabled=false;
        EnableLowRAM->Checked=false;
        EnableLowRAM->Enabled=false;
        M1Not->Checked=false;
        M1Not->Enabled=false;
        ImprovedWait->Checked=false;
        ImprovedWait->Enabled=false;
        FloatingPointHardwareFix->Checked = false;
        HiResBox->ItemIndex=0;
        HiResBox->Enabled=false;
        HiResLbl->Enabled=false;
        ChrGenBox->Items->Strings[0]="Ace";
        ChrGenBox->ItemIndex=0;
        ChrGenBox->Enabled=false;
        ChrGenLbl->Enabled=false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("ETI");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;   
        SpeechBox->Items->Clear();
        SpeechBox->Items->Add("None");
        SpeechBox->ItemIndex=0;
        SpeechBox->Enabled=false;
        SpeechBoxLbl->Enabled=false;
        SoundCardBox->Items->Clear();
        SoundCardBox->Items->Add("None");
        SoundCardBox->Items->Add("Ace User");
        SoundCardBox->Items->Add("Boldfield");
        SoundCardBox->ItemIndex=0;
        SoundCardBox->Enabled=true;
        SoundCardLbl->Enabled=true;
        IDEBox->Items->Add("AceCF");
        RamPackBox->Items->Add("96K");
        EnableRomCartridgeOption(false);
        SetZXpandState(false,false);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2048BtnClick(TObject *Sender)
{
        if (TC2048Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYTC2048;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TC2048Btn->Down=true;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        EnableRomCartridgeOption(false);

        NewMachineName=TC2048Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("tc2048.rom");
        RomBox->Text = emulator.ROMTC2048;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2068BtnClick(TObject *Sender)
{
        if (TC2068Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYTC2068;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TC2068Btn->Down=true;
        Multiface->Enabled = false;

        SpeechBox->Enabled = false;
        SpeechBoxLbl->Enabled = false;

        uSource->Checked = false;
        uSource->Enabled = false;

        SoundCardBox->ItemIndex = 1;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NTSC->Checked=false;

        EnableRomCartridgeOption(true);

        NewMachineName=TC2068Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("ts2068.rom");
        RomBox->Text = emulator.ROMTC2068;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;

        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}

//---------------------------------------------------------------------------

void __fastcall THW::TS2068BtnClick(TObject *Sender)
{
        if (TS2068Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYTS2068;
        RefreshDefaultRamSettings();
        SetupForSpectrum();
        TS2068Btn->Down=true;
        Multiface->Enabled = false;

        SpeechBox->Enabled = false;
        SpeechBoxLbl->Enabled = false;

        uSource->Checked = false;
        uSource->Enabled = false;
        
        SoundCardBox->ItemIndex = 1;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NTSC->Checked=true;

        EnableRomCartridgeOption(true);

        NewMachineName=TS2068Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("ts2068.rom");
        RomBox->Text = emulator.ROMTS2068;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ChrGenBox->Items->Strings[0] = "Timex";
        ChrGenBox->ItemIndex = 0;
        HiResBox->Items->Strings[0] = "Timex";
        HiResBox->ItemIndex = 0;
        ColourBox->Items->Strings[0] = "Timex";
        ColourBox->ItemIndex = 0;
        ColourBox->Enabled = false;
        ColourLabel->Caption = "Color:";
        ColourLabel->Enabled = false;

        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX97LEBtnClick(TObject *Sender)
{
        if (ZX97LEBtn->Down) return;

        NewMachine=MACHINEZX97LE;
        RefreshDefaultRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        ZX97LEBtn->Down=true;
        NewMachineName=ZX97LEBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("zx97.rom");
        RomBox->Text = emulator.ROM97LE;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ChrGenBox->ItemIndex=3;
        ChrGenBox->Enabled=false;
        HiResBox->ItemIndex=1;
        HiResBox->Enabled=false;
        EnableLowRAM->Checked=true;
        EnableLowRAM->Enabled=false;
        M1Not->Checked=true;
        M1Not->Enabled=false;
        ImprovedWait->Checked=false;
        ImprovedWait->Enabled=false;
        FloatingPointHardwareFix->Checked = false;
        RamPackBox->Enabled=false;
        ColourBox->Items->Clear();
        ColourBox->Items->Add("None");
        ColourBox->Items->Add("Lambda");
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=true;
        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible=false;
                RamPackBox->Visible=false;
                LabelTotalRAM->Visible=false;
        }
        ButtonAdvancedMore->Visible = true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2050Click(TObject *Sender)
{
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::FormShow(TObject *Sender)
{
        ReloadFromInternalSettings();
        ResetRequired=false;
}
//---------------------------------------------------------------------------
void __fastcall THW::TS2050ConfigClick(TObject *Sender)
{
        SerialConfig->ShowModal();
}
//---------------------------------------------------------------------------
void THW::SaveSettings(TIniFile *ini)
{
        AnsiString Rom, MachineName;
        FILE *f;
        char FileName[256];

        if (ZX80Btn->Down) MachineName="ZX80";
        else if (Spec16Btn->Down) MachineName="Spec16";
        else if (Spec48Btn->Down) MachineName="Spec48";
        else if (SpecPlusBtn->Down) MachineName="SpecPlus";
        else if (Spec128Btn->Down) MachineName="Spec128";
        else if (SpecP2Btn->Down) MachineName="SpecP2";
        else if (SpecP2aBtn->Down) MachineName="SpecP2A";
        else if (SpecP3Btn->Down) MachineName="SpecP3";
        else if (TS1000Btn->Down) MachineName="TS1000";
        else if (TS1500Btn->Down) MachineName="TS1500";
        else if (TC2048Btn->Down) MachineName="TC2048";
        else if (TC2068Btn->Down) MachineName="TC2068";
        else if (TS2068Btn->Down) MachineName="TS2068";
        else if (LambdaBtn->Down) MachineName="Lambda";
        else if (R470Btn->Down) MachineName="R470";
        else if (TK85Btn->Down) MachineName="TK85";
        else if (ZX97LEBtn->Down) MachineName="ZX97LE";
        else if (AceBtn->Down) MachineName="ACE";
        else MachineName="ZX81";

        ini->WriteInteger("HWARE","Top",Top);
        ini->WriteInteger("HWARE","Left",Left);
        ini->WriteString("HWARE","MachineName",MachineName);

        ini->WriteInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        ini->WriteInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        ini->WriteInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        ini->WriteInteger("HWARE","HiRes",HiResBox->ItemIndex);
        ini->WriteInteger("HWARE","Colour",ColourBox->ItemIndex);
        ini->WriteInteger("HWARE","Speech",SpeechBox->ItemIndex);
        ini->WriteInteger("HWARE","RomCartridge",RomCartridgeBox->ItemIndex);
        ini->WriteString("HWARE","RomCartridgeFile",RomCartridgeFileBox->Text);
        ini->WriteInteger("HWARE","ZXC1Configuration",ZXC1ConfigurationBox->ItemIndex);

        ini->WriteInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        ini->WriteInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        ini->WriteString("DRIVES","DriveA", spectrum.driveaimg);
        ini->WriteString("DRIVES","DriveB", spectrum.drivebimg);
        ini->WriteInteger("HWARE","FDCType",FDC->ItemIndex);
        ini->WriteBool("HWARE","Autoboot",Autoboot->Checked);
        ini->WriteBool("HWARE","uSource",uSource->Checked);
        ini->WriteBool("HWARE","ZXpand",ZXpand->Checked);
        ini->WriteBool("HWARE","ZXpandEnabled",ZXpand->Enabled);
        ini->WriteBool("HWARE","SpecDrum",SpecDrum->Checked);
        ini->WriteBool("HWARE","SpecDrumEnabled",SpecDrum->Enabled);

        if (ATA_GetHDF(0)) Rom=ATA_GetHDF(0); else Rom="NULL";
        ini->WriteString("DRIVES","HD0", Rom);
        if (ATA_GetHDF(1)) Rom=ATA_GetHDF(1); else Rom="NULL";
        ini->WriteString("DRIVES","HD1", Rom);
        for (int i = 0; i < IF1->MDVNoDrives; i++)
        {
            Rom="NULL"; if (IF1->MDVGetFileName(i)) Rom=IF1->MDVGetFileName(i);
            ini->WriteString("DRIVES","MDV"+AnsiString(i), Rom);
        }

        ini->WriteBool("HWARE","ProtectRom",ProtectROM->Checked);
        ini->WriteBool("HWARE","NTSC",NTSC->Checked);
        ini->WriteBool("HWARE","LowRAM",EnableLowRAM->Checked);
        ini->WriteBool("HWARE","M1NOT",M1Not->Checked);
        ini->WriteBool("HWARE","IMPROVED_WAIT",ImprovedWait->Checked);
        ini->WriteBool("HWARE","TS2050",TS2050->Checked);
        ini->WriteBool("HWARE","Iss2Kb",Issue2->Checked);
        ini->WriteBool("HWARE","KMouse",KMouse->Checked);
        ini->WriteBool("HWARE","divIDEWP",Form1->divIDEJumperEClosed->Checked);
        ini->WriteBool("HWARE","ZXCFWP",Upload->Checked);
        ini->WriteBool("HWARE","MFace",Multiface->Checked);
        ini->WriteBool("HWARE","ZXPrinter",ZXPrinter->Checked);
        ini->WriteInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        ini->WriteInteger("HWARE","HDRIVE",IDEBox->ItemIndex);
        ini->WriteBool("HWARE","FloatingPointHardwareFix",FloatingPointHardwareFix->Checked);

        Rom=emulator.ROM80; ini->WriteString("HWARE","ROM80",Rom);
        Rom=emulator.ROM81; ini->WriteString("HWARE","ROM81",Rom);
        Rom=emulator.ROMSP16; ini->WriteString("HWARE","ROMSP16",Rom);
        Rom=emulator.ROMSP48; ini->WriteString("HWARE","ROMSP48",Rom);
        Rom=emulator.ROMSPP; ini->WriteString("HWARE","ROMSPP",Rom);
        Rom=emulator.ROMSP128; ini->WriteString("HWARE","ROMSP128",Rom);

        Rom=emulator.ROMSPP2; ini->WriteString("HWARE","ROMSPP2",Rom);
        Rom=emulator.ROMSPP2A; ini->WriteString("HWARE","ROMSPP2A",Rom);
        Rom=emulator.ROMSPP3; ini->WriteString("HWARE","ROMSPP3",Rom);

        Rom=emulator.ROMTS1000; ini->WriteString("HWARE","ROMTS1000",Rom);
        Rom=emulator.ROMTS1500; ini->WriteString("HWARE","ROMTS1500",Rom);
        Rom=emulator.ROMTC2048; ini->WriteString("HWARE","ROMTC2048",Rom);
        Rom=emulator.ROMTC2068; ini->WriteString("HWARE","ROMTC2068",Rom);
        Rom=emulator.ROMTS2068; ini->WriteString("HWARE","ROMTS2068",Rom);

        Rom=emulator.ROMLAMBDA; ini->WriteString("HWARE","ROMLAMBDA",Rom);
        Rom=emulator.ROMTK85; ini->WriteString("HWARE","ROMTK85",Rom);
        Rom=emulator.ROMACE; ini->WriteString("HWARE","ROMACE",Rom);
        Rom=emulator.ROMR470; ini->WriteString("HWARE","ROMR470",Rom);

        Rom=emulator.ROM97LE; ini->WriteString("HWARE","ROM97LE",Rom);

        Rom=emulator.ROMSPP3E; ini->WriteString("HWARE","ROMSPP3E",Rom);
        Rom=emulator.ROMDock; ini->WriteString("HWARE","Dock",Rom);
        Rom=emulator.ROMZX8BIT; ini->WriteString("HWARE","ZX8BIT",Rom);
        Rom=emulator.ROMZX16BIT; ini->WriteString("HWARE","ZX16BIT",Rom);
        Rom=emulator.ROMZXCF; ini->WriteString("HWARE","ZXCF",Rom);

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "divide.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(divIDEMem, 8192, 1, f);
                fclose(f);
        }

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "zxcf.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(ZXCFMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "zx1541.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(ZX1541Mem, 1, 8192, f);
                fclose(f);
        }
}

void THW::LoadSettings(TIniFile *ini)
{
        AnsiString Rom;
        char FileName[256];
        FILE *f;

        Top=ini->ReadInteger("HWARE","Top",Top);
        Left=ini->ReadInteger("HWARE","Left",Left);

        Rom=emulator.ROM80; Rom=ini->ReadString("HWARE","ROM80",Rom).LowerCase(); strcpy(emulator.ROM80, Rom.c_str());
        Rom=emulator.ROM81; Rom=ini->ReadString("HWARE","ROM81",Rom).LowerCase(); strcpy(emulator.ROM81, Rom.c_str());
        Rom=emulator.ROMACE; Rom=ini->ReadString("HWARE","ROMACE",Rom).LowerCase(); strcpy(emulator.ROMACE, Rom.c_str());
        Rom=emulator.ROMTS1000; Rom=ini->ReadString("HWARE","ROMTS1000",Rom).LowerCase(); strcpy(emulator.ROMTS1000, Rom.c_str());
        Rom=emulator.ROMTS1500; Rom=ini->ReadString("HWARE","ROMTS1500",Rom).LowerCase(); strcpy(emulator.ROMTS1500, Rom.c_str());
        Rom=emulator.ROMTC2048; Rom=ini->ReadString("HWARE","ROMTC2048",Rom).LowerCase(); strcpy(emulator.ROMTC2048, Rom.c_str());
        Rom=emulator.ROMTC2068; Rom=ini->ReadString("HWARE","ROMTC2068",Rom).LowerCase(); strcpy(emulator.ROMTC2068, Rom.c_str());
        Rom=emulator.ROMTS2068; Rom=ini->ReadString("HWARE","ROMTS2068",Rom).LowerCase(); strcpy(emulator.ROMTS2068, Rom.c_str());
        Rom=emulator.ROMLAMBDA; Rom=ini->ReadString("HWARE","ROMLAMBDA",Rom).LowerCase(); strcpy(emulator.ROMLAMBDA, Rom.c_str());
        Rom=emulator.ROMTK85; Rom=ini->ReadString("HWARE","ROMTK85",Rom).LowerCase(); strcpy(emulator.ROMTK85, Rom.c_str());
        Rom=emulator.ROM97LE; Rom=ini->ReadString("HWARE","ROM97LE",Rom).LowerCase(); strcpy(emulator.ROM97LE, Rom.c_str());
        Rom=emulator.ROMR470; Rom=ini->ReadString("HWARE","ROMR470",Rom).LowerCase(); strcpy(emulator.ROMR470, Rom.c_str());
        Rom=emulator.ROMSP16; Rom=ini->ReadString("HWARE","ROMSP16",Rom).LowerCase(); strcpy(emulator.ROMSP16, Rom.c_str());
        Rom=emulator.ROMSP48; Rom=ini->ReadString("HWARE","ROMSP48",Rom).LowerCase(); strcpy(emulator.ROMSP48, Rom.c_str());
        Rom=emulator.ROMSPP; Rom=ini->ReadString("HWARE","ROMSPP",Rom).LowerCase(); strcpy(emulator.ROMSPP, Rom.c_str());
        Rom=emulator.ROMSP128; Rom=ini->ReadString("HWARE","ROMSP128",Rom).LowerCase(); strcpy(emulator.ROMSP128, Rom.c_str());
        Rom=emulator.ROMSPP2; Rom=ini->ReadString("HWARE","ROMSPP2",Rom).LowerCase(); strcpy(emulator.ROMSPP2, Rom.c_str());
        Rom=emulator.ROMSPP2A; Rom=ini->ReadString("HWARE","ROMSPP2A",Rom).LowerCase(); strcpy(emulator.ROMSPP2A, Rom.c_str());
        Rom=emulator.ROMSPP3; Rom=ini->ReadString("HWARE","ROMSPP3",Rom).LowerCase(); strcpy(emulator.ROMSPP3, Rom.c_str());
        Rom=emulator.ROMSPP3E; Rom=ini->ReadString("HWARE","ROMSPP3E",Rom).LowerCase(); strcpy(emulator.ROMSPP3E, Rom.c_str());
        Rom=emulator.ROMDock; Rom=ini->ReadString("HWARE","Dock",Rom).LowerCase(); strcpy(emulator.ROMDock, Rom.c_str());
        Rom=emulator.ROMZX8BIT; Rom=ini->ReadString("HWARE","ZX8BIT",Rom).LowerCase(); strcpy(emulator.ROMZX8BIT, Rom.c_str());
        Rom=emulator.ROMZX16BIT; Rom=ini->ReadString("HWARE","ZX16BIT",Rom).LowerCase(); strcpy(emulator.ROMZX16BIT, Rom.c_str());
        Rom=emulator.ROMZXCF; Rom=ini->ReadString("HWARE","ZXCF",Rom).LowerCase(); strcpy(emulator.ROMZXCF, Rom.c_str());

        Hwform.MachineName=ini->ReadString("HWARE","MachineName","ZX81");

        Hwform.RamPackBoxItemIndex=ini->ReadInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        Hwform.SoundCardBoxItemIndex=ini->ReadInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        Hwform.ChrGenBoxItemIndex=ini->ReadInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        Hwform.HiResBoxItemIndex=ini->ReadInteger("HWARE","HiRes",HiResBox->ItemIndex);
        Hwform.ColourBoxItemIndex=ini->ReadInteger("HWARE","Colour",ColourBox->ItemIndex);
        Hwform.SpeechBoxItemIndex=ini->ReadInteger("HWARE","Speech",SpeechBox->ItemIndex);
        Hwform.RomCartridgeBoxItemIndex=ini->ReadInteger("HWARE","RomCartridge",RomCartridgeBox->ItemIndex);
        Hwform.RomCartridgeFileBoxText=ini->ReadString("HWARE","RomCartridgeFile","");
        Hwform.ZXC1ConfigurationBoxItemIndex=ini->ReadInteger("HWARE","ZXC1Configuration",ZXC1ConfigurationBox->ItemIndex);

        Hwform.DriveATypeItemIndex=ini->ReadInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        Hwform.DriveBTypeItemIndex=ini->ReadInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        Hwform.FDCItemIndex=ini->ReadInteger("HWARE","FDCType",FDC->ItemIndex);
        Hwform.AutobootChecked=ini->ReadBool("HWARE","Autoboot",Autoboot->Checked);

        Rom=ini->ReadString("DRIVES","DriveA", spectrum.driveaimg);
        strcpy(spectrum.driveaimg, Rom.c_str());
        Rom=ini->ReadString("DRIVES","DriveB", spectrum.drivebimg);
        strcpy(spectrum.drivebimg, Rom.c_str());

        Rom=ini->ReadString("DRIVES","HD0", "NULL");
        if (Rom!="NULL") ATA_LoadHDF(0,Rom.c_str());
        Rom=ini->ReadString("DRIVES","HD1", "NULL");
        if (Rom!="NULL") ATA_LoadHDF(1,Rom.c_str());

        if (ATA_GetHDF(0)) Rom=ATA_GetHDF(0); else Rom="NULL";
        ini->WriteString("DRIVES","HD0", Rom);
        if (ATA_GetHDF(1)) Rom=ATA_GetHDF(1); else Rom="NULL";
        ini->WriteString("DRIVES","HD1", Rom);

        for (int i = 0; i < IF1->MDVNoDrives; i++)
        {
            Rom=ini->ReadString("DRIVES","MDV"+AnsiString(i), "NULL");
            if (Rom!="NULL") IF1->MDVSetFileName(i,Rom.c_str());
        }

        Hwform.ZXpandChecked=ini->ReadBool("HWARE","ZXpand",ZXpand->Checked);
        Hwform.ZXpandEnabled=ini->ReadBool("HWARE","ZXpandEnabled",ZXpand->Enabled);
        Hwform.SpecDrumChecked=ini->ReadBool("HWARE","SpecDrum",SpecDrum->Checked);
        Hwform.SpecDrumEnabled=ini->ReadBool("HWARE","SpecDrumEnabled",SpecDrum->Enabled);
        Hwform.ProtectROMChecked=ini->ReadBool("HWARE","ProtectRom",ProtectROM->Checked);
        Hwform.NTSCChecked=ini->ReadBool("HWARE","NTSC",NTSC->Checked);
        Hwform.EnableLowRAMChecked=ini->ReadBool("HWARE","LowRAM",EnableLowRAM->Checked);
        Hwform.M1NotChecked=ini->ReadBool("HWARE","M1NOT",M1Not->Checked);
        Hwform.ImprovedWaitChecked=ini->ReadBool("HWARE","IMPROVED_WAIT",ImprovedWait->Checked);
        Hwform.TS2050Checked=ini->ReadBool("HWARE","TS2050",TS2050->Checked);
        Hwform.Issue2Checked=ini->ReadBool("HWARE","Iss2Kb",Issue2->Checked);
        Hwform.KMouseChecked=ini->ReadBool("HWARE","KMouse",KMouse->Checked);
        Hwform.Form1divIDEJumperEClosedChecked=ini->ReadBool("HWARE","divIDEWP",Form1->divIDEJumperEClosed->Checked);
        Hwform.MultifaceChecked=ini->ReadBool("HWARE","MFace",Multiface->Checked);
        Hwform.ZXPrinterChecked=ini->ReadBool("HWARE","ZXPrinter",ZXPrinter->Checked);
        Hwform.FloatingPointHardwareFixChecked=ini->ReadBool("HWARE","FloatingPointHardwareFix",FloatingPointHardwareFix->Checked);

        Hwform.UploadChecked=ini->ReadBool("HWARE","ZXCFWP",Upload->Checked);
        Hwform.ZXCFRAMItemIndex=ini->ReadInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        Hwform.IDEBoxItemIndex=ini->ReadInteger("HWARE","HDRIVE",IDEBox->ItemIndex);

        Hwform.uSourceChecked=ini->ReadBool("HWARE","uSource",uSource->Checked);

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "zxcf.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(ZXCFMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "divide.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(divIDEMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName, emulator.cwd);
        strcat(FileName, nvMemoryFolder);
        strcat(FileName, "zx1541.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(ZX1541Mem, 1, 8192, f);
                fclose(f);
        }

        ReloadFromInternalSettings();
}

void __fastcall THW::BrowseROMClick(TObject *Sender)
{
        AnsiString Path;
        char cPath[512];

        Path = emulator.cwd;
        Path += romsFolder;

        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomBox->Text;
        if (RomSelect->FileName.Length() == 0 || *(RomSelect->FileName.LastChar()) == '\\')
        {
                RomSelect->FileName = "";
        }

        if (!RomSelect->Execute())
        {
                return;
        }
        
        AnsiString selectedRomPath = FileNameGetPath(RomSelect->FileName);

        strcpy(cPath, Path.c_str());
        if (Path == selectedRomPath)
        {
                Path=RomSelect->FileName;
                Path=RemovePath(Path);
        }
        else
        {
            Path = RomSelect->FileName;
        }
        
        RomBox->Text=Path;
        RomBox->SelStart=0;
        ResetRequired=true;

        SetZX80Icon();
        SetSpectrum128Icon();
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

        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::NTSCClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::EnableLowRAMClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::M1NotClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaColourClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::SoundCardBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::ChrGenBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::HiResBoxChange(TObject *Sender)
{
        if ((HiResBox->Items->Strings[HiResBox->ItemIndex]=="Quicksilva") ||
             (HiResBox->Items->Strings[HiResBox->ItemIndex]=="Memotech"))
        {
                ProtectROM->Checked=true;
        }
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveATypeChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveBTypeChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::IF1ConfigClick(TObject *Sender)
{
        IF1->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall THW::MultifaceClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::IDEBoxChange(TObject *Sender)
{
        Upload->Visible=false;
        ZXCFLabel->Visible=false;
        ZXCFRAM->Visible=false;
        Form1->divIDEJumperEClosed->Enabled = false;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="MWCFIde")
        {
                RamPackBox->ItemIndex = RamPackBox->Items->Count-2;
                M1Not->Checked=true;
        }

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="AceCF")
                RamPackBox->ItemIndex = RamPackBox->Items->Count-1;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
        {
                Upload->Visible=true;
                ZXCFLabel->Visible=true;
                ZXCFRAM->Visible=true;
        }

        if ((IDEBox->Items->Strings[IDEBox->ItemIndex]).Pos("divIDE"))
        {
                Form1->divIDEJumperEClosed->Enabled = true;
                Form1->divIDEJumperEClosed->Checked = true;
        }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Simple +3e 8-Bit") RomBox->Text = emulator.ROMSPP3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik CF") RomBox->Text = emulator.ROMZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 8-Bit") RomBox->Text = emulator.ROMZX8BIT;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Pera Putnik 16-Bit") RomBox->Text = emulator.ROMZX16BIT;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="None")
        {
                if (SpecP2aBtn->Down) RomBox->Text = emulator.ROMSPP2A;
                if (SpecP3Btn->Down) RomBox->Text = emulator.ROMSPP3;
                if (Spec16Btn->Down) RomBox->Text = emulator.ROMSP16;
                if (Spec48Btn->Down) RomBox->Text = emulator.ROMSP48;
                if (SpecPlusBtn->Down) RomBox->Text = emulator.ROMSPP;
                if (Spec128Btn->Down) RomBox->Text = emulator.ROMSP128;
                if (SpecP2Btn->Down) RomBox->Text = emulator.ROMSPP2;
                if (TC2048Btn->Down) RomBox->Text = emulator.ROMTC2048;
                if (TC2068Btn->Down) RomBox->Text = emulator.ROMTC2068;
                if (TS2068Btn->Down) RomBox->Text = emulator.ROMTS2068;
        }

        DisplayTotalRam();

        ResetRequired=true;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
}
//---------------------------------------------------------------------------

void __fastcall THW::FDCChange(TObject *Sender)
{
        if (FDC->Items->Strings[FDC->ItemIndex]=="Larken")
                EnableLowRAM->Checked=true;

        if (FDC->Items->Strings[FDC->ItemIndex]=="+3")
        {
                DriveAType->Enabled=true;
                DriveBType->Enabled=true;
                DriveAType->Visible=true;
                DriveBType->Visible=true;
                LabelA->Visible=true;
                LabelB->Visible=true;
        }
        else
        {
                DriveAType->Enabled=false;
                DriveBType->Enabled=false;
                DriveAType->Visible=false;
                DriveBType->Visible=false;
                LabelA->Visible=false;
                LabelB->Visible=false;
        }

        if (FDC->Items->Strings[FDC->ItemIndex]=="Beta Disk")
                Autoboot->Visible=true;
        else    Autoboot->Visible=false;

        if (FDC->Items->Strings[FDC->ItemIndex]=="ZX Interface 1")
        {
                IF1Config->Visible=true;
                IF1Config->Enabled=true;
                Form1->IFace1->Enabled=true;
        }
        else
        {
                IF1Config->Visible=false;
                Form1->IFace1->Enabled=false;
        }
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::uSpeechClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXpandClick(TObject *Sender)
{
        ResetRequired=true;
        SetZXpandState(ZXpand->Checked,true);

        if (HW->Visible && ZXpand->Checked)
        {
                bool allFacilitiesSelected = (HiResBox->ItemIndex == 1) &&
                                             (RamPackBox->ItemIndex == 5) &&
                                             (SoundCardBox->ItemIndex == 1) &&
                                             EnableLowRAM->Checked;

                if (!allFacilitiesSelected)
                {
                        int ret = Application->MessageBox(L"Automatically select the following ZXpand+ facilities?\n\n32K RAM Pack, RAM in 8K-16K Region, WRX High Resolution, ZON X Sound", L"ZXpand+ Configuration", MB_YESNO | MB_ICONQUESTION);

                        if (ret == IDYES)
                        {
                                HiResBox->ItemIndex = 1;        // WRX
                                RamPackBox->ItemIndex = 5;      // 32K RAM
                                SoundCardBox->ItemIndex = 1;    // ZON X
                                EnableLowRAM->Checked = true;   // 8K-16K RAM
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::BrowseRomCartridgeClick(TObject *Sender)
{
        AnsiString Path;

        Path = emulator.cwd;

        int romcartridgetype=DetermineRomCartridgeType(RomCartridgeBox->Text, NewMachine, NewSpec);

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

        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomCartridgeFileBox->Text;
        if (RomSelect->FileName.Length() == 0 || *(RomSelect->FileName.LastChar()) == '\\')
        {
                RomSelect->FileName = "";
        }
        if (!RomSelect->Execute()) return;

        AnsiString selectedRomPath = FileNameGetPath(RomSelect->FileName);

        Path=RomSelect->FileName;

        RomCartridgeFileBox->Text=Path;
        RomCartridgeFileBox->SelStart=0;

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

        ResetRequired=true;
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
        ResetRequired=true;
}
//---------------------------------------------------------------------------
void __fastcall THW::SinclairRomCartridgeFileBoxChange(TObject *Sender)
{
        if (SinclairRomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = SinclairRomCartridgeFileBox->Text;
                ResetRequired=true;
        }
}
//---------------------------------------------------------------------------
void __fastcall THW::TC2068RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TC2068RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TC2068RomCartridgeFileBox->Text;
                ResetRequired=true;
        }

}
//---------------------------------------------------------------------------
void __fastcall THW::TS2068RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TC2068RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TC2068RomCartridgeFileBox->Text;
                ResetRequired=true;
        }
}
//---------------------------------------------------------------------------
void __fastcall THW::TS1510RomCartridgeFileBoxChange(TObject *Sender)
{
        if (TS1510RomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = TS1510RomCartridgeFileBox->Text;
                ResetRequired=true;
        }
}//---------------------------------------------------------------------------

void __fastcall THW::ColourBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::RomBoxChange(TObject *Sender)
{
        if (ZX80Btn->Down)
        {
                bool zx81Ed1ROM = RomBox->Text == "zx81.edition1.rom";
                FloatingPointHardwareFix->Enabled = zx81Ed1ROM;
                if (!zx81Ed1ROM)
                        FloatingPointHardwareFix->Checked = false;
        }

        ResetRequired=true;

        SetZX80Icon();
        SetSpectrum128Icon();
}
//---------------------------------------------------------------------------

void __fastcall THW::ZXPrinterClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------
  
void __fastcall THW::FloatingPointHardwareFixClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------
                
extern char* zxpandSDCardFolderRoot;

void __fastcall THW::ButtonZXpandSDCardClick(TObject *Sender)
{
        AnsiString root(zxpandSDCardFolderRoot);
        AnsiString replaced = StringReplace(root, "/", "\\", TReplaceFlags()<<rfReplaceAll);
        ShellExecute(NULL, "explore", replaced.c_str(), NULL, NULL, SW_NORMAL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ButtonAdvancedMoreClick(TObject *Sender)
{
        ZX97Dialog->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall THW::ImprovedWaitClick(TObject *Sender)
{
        ResetRequired=true;
}
void __fastcall THW::uSourceClick(TObject *Sender)
{
        ResetRequired=true;
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
#if __CODEGEARC__ >= 0x0620
                AddRomCartridgeFile(TS1510RomCartridgeFileBox, &(*iter), ".rom");
#else
                AddRomCartridgeFile(TS1510RomCartridgeFileBox, iter, ".rom");
#endif
        }
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTS2068List()
{
        vector<RomCartridgeEntry>::iterator iter;

        TS2068RomCartridgeFileBox->Items->Clear();

        for (iter = ts2068RomCartridges.begin(); iter != ts2068RomCartridges.end(); iter++)
        {
#if __CODEGEARC__ >= 0x0620
                AddRomCartridgeFile(TS2068RomCartridgeFileBox, &(*iter), ".dck");
#else
                AddRomCartridgeFile(TS2068RomCartridgeFileBox, iter, ".dck");
#endif
        }
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTC2068List()
{
        vector<RomCartridgeEntry>::iterator iter;

        TC2068RomCartridgeFileBox->Items->Clear();

        for (iter = tc2068RomCartridges.begin(); iter != tc2068RomCartridges.end(); iter++)
        {
#if __CODEGEARC__ >= 0x0620
                AddRomCartridgeFile(TC2068RomCartridgeFileBox, &(*iter), ".dck");
#else
                AddRomCartridgeFile(TC2068RomCartridgeFileBox, iter, ".dck");
#endif
        }
}//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeSinclairList()
{
        vector<RomCartridgeEntry>::iterator iter;

        SinclairRomCartridgeFileBox->Items->Clear();

        for (iter = sinclairRomCartridges.begin(); iter != sinclairRomCartridges.end(); iter++)
        {
#if __CODEGEARC__ >= 0x0620
                AddRomCartridgeFile(SinclairRomCartridgeFileBox, &(*iter), ".rom");
#else
                AddRomCartridgeFile(SinclairRomCartridgeFileBox, iter, ".rom");
#endif
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
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::FormClose(TObject *Sender, TCloseAction &Action)
{
        ReloadFromInternalSettings();
}
//---------------------------------------------------------------------------

void __fastcall THW::CancelClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------

