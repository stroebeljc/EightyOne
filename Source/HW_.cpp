//--------------------------------------------------------------------------

#include <vcl.h>
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
#include "debug68.h"
#include "ql.h"
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

//extern "C" void sound_ay_init(void);
extern "C" BYTE ZX1541Mem[];

extern void HWSetMachine(int machine, int speccy);
extern int zx81_do_scanline(SCANLINE *CurScanLine);
extern int zx80_do_scanline(SCANLINE *CurScanLine);
extern void InitPatches(int machineType);
extern bool LoadRomCartridgeFile(char *filename);
extern int RomCartridgeCapacity;

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
                case SPECCYTS2068: HW->TS2068BtnClick(NULL);break;
                case SPECCY128: HW->Spec128BtnClick(NULL);break;
                case SPECCYPLUS2: HW->SpecP2BtnClick(NULL);break;
                case SPECCYPLUS2A: HW->SpecP2aBtnClick(NULL);break;
                }
                break;
        }

        HW->OKClick(NULL);
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
        QLCPU->ItemIndex=0;
        QLMem->ItemIndex=0;
        ZXCFRAM->ItemIndex=1;
        ZX81BtnClick(NULL);

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        ResetRequired=true;
        OKClick(NULL);
}
//---------------------------------------------------------------------------

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

void ql_writebyteProxy(int address, int data)
{
        ql_writebyte(address, data);
        dirtyBird.insert(address);
}

AnsiString getMachineRoot(AnsiString fullRomName)
{
        // return the first part of the rom name up to but excluding the first '.'
        char* p1 = fullRomName.c_str();
        char* p2 = strchr(p1, '.');
        if (p2 == NULL) return fullRomName;
        int len = p2 - p1;
        return fullRomName.SubString(1,len);
}

void __fastcall THW::OKClick(TObject *Sender)
{
        RomBox->Text = RomBox->Text.LowerCase();

        AnsiString Name=NewMachineName;
        emulator.newMachine = NewMachine;

        if (AceBtn->Down)
        {
                int i=3;
                AnsiString Mem;

                if (RamPackBox->ItemIndex!=0)
                        i+=atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());

                Name=i;
                Name += "K Jupiter Ace";
        }
        else if (QLBtn->Down)
        {
                Name = QLMem->Items->Strings[QLMem->ItemIndex] + " " + NewMachineName;
        }
        else if (RamPackBox->Visible)
        {
                SetUpRamSettings();

                int totalRam = machine.baseRamSize;
                if (RamPackBox->ItemIndex!=0)
                {
                        int ramPack = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                        totalRam = machine.ramPackSupplementsInternalRam ? totalRam + ramPack : ramPack;
                }
                AnsiString ramSize = totalRam;
                Name = ramSize + "K " + Name;
        }

        zx81.zxpand = 0;

        bool machineChanged = (NewMachine != emulator.machine);
        if (machineChanged && LiveMemoryWindow && LiveMemoryWindow->Visible)
                LiveMemoryWindow->Close();

        strcpy(emulator.machinename, Name.c_str());
        Form1->StatusBar1->Panels->Items[0]->Text = Name;

        strcpy(machine.CurRom, RomBox->Text.c_str());

        emulator.machine=NewMachine;
        spectrum.model=NewSpec;

        Form1->RamPackWobble1->Enabled=true;
        Form1->RamPackWobble1->Visible=true;
        Form1->InsertDockCart1->Visible=false;
        Form1->RemoveDockCart1->Visible=false;
        Form1->DockSpacer->Visible=false;

        if (NewMachine == MACHINEQL)
        {
                if (Dbg && Dbg->Visible) Dbg->Close();
        }
        else
        {
                if (Debug68k && Debug68k->Visible) Debug68k->Close();
        }

        bool prevChromaColourSwitchOn = (machine.colour == COLOURCHROMA);
        bool prevSpectraColourSwitchOn = (machine.colour == COLOURSPECTRA);

        switch(NewMachine)
        {
        case MACHINEQL:
                machine.colour = COLOURDISABLED;
                break;
        case MACHINEACE:
                machine.colour = (ColourBox->ItemIndex > 0) ? COLOURACE : COLOURDISABLED;
                break;
        case MACHINESPECTRUM:
                machine.colour = (ColourBox->ItemIndex > 0) ? COLOURSPECTRA : COLOURSINCLAIR;
                break;
        default:
                machine.colour = ColourBox->ItemIndex;
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

        zx81.chromaColourSwitchOn = (machine.colour == COLOURCHROMA);
        if (!prevChromaColourSwitchOn && zx81.chromaColourSwitchOn)
        {
                InitialiseChroma();
        }
        Form1->ChromaColourEnable->Enabled = zx81.chromaColourSwitchOn;
        Form1->ChromaColourEnable->Checked = zx81.chromaColourSwitchOn;
        Form1->ChromaColourEnable->Visible = (NewMachine == MACHINEZX80) || (NewMachine == MACHINEZX81) ||
                                             (NewMachine == MACHINETS1000) || (NewMachine == MACHINETS1500);

        spectrum.spectraColourSwitchOn = (machine.colour == COLOURSPECTRA);
        if (!prevSpectraColourSwitchOn && spectrum.spectraColourSwitchOn)
        {
                InitialiseSpectra();
        }
        Form1->SpectraColourEnable->Enabled = spectrum.spectraColourSwitchOn;
        Form1->SpectraColourEnable->Checked = spectrum.spectraColourSwitchOn;
        Form1->SpectraColourEnable->Visible = (NewMachine == MACHINESPECTRUM) && ((NewSpec != SPECCYTC2048) && (NewSpec != SPECCYTS2068));

        Form1->BasicListerOption->Enabled = false;
        if (BasicLister->ListerAvailable())
        {
                BasicLister->Clear();
                BasicLister->SetBasicLister(NULL);
        }

        spectrum.MFVersion=MFNONE;
        Multiface->Enabled = false;

        switch(NewMachine)
        {
        case MACHINEZX80:
                strcpy(emulator.ROM80, machine.CurRom);
                zx81.zxpand = (ZXpand->Checked == true);
                CreateBasicLister();
                break;

        case MACHINEZX81:
                strcpy(emulator.ROM81, machine.CurRom);
                CreateBasicLister();

                zx81.zxpand = (ZXpand->Checked == true);
                ZXpand->Caption = "ZXpand+";
                break;

        case MACHINER470:
                strcpy(emulator.ROMR470, machine.CurRom);
                break;

        case MACHINETK85:
                strcpy(emulator.ROMTK85, machine.CurRom);
                CreateBasicLister();
                break;

        case MACHINETS1000:
                strcpy(emulator.ROM81, machine.CurRom);
                CreateBasicLister();

                zx81.zxpand = (ZXpand->Checked == true);
                ZXpand->Caption = "ZXpand+";
                break;

        case MACHINEACE:
                strcpy(emulator.ROMACE, machine.CurRom);
                break;

        case MACHINETS1500:
                strcpy(emulator.ROMTS1500, machine.CurRom);
                CreateBasicLister();

                zx81.zxpand = (ZXpand->Checked == true);
                ZXpand->Caption = "ZXpand+";
                break;

        case MACHINELAMBDA:
                strcpy(emulator.ROMLAMBDA, machine.CurRom);
                break;

        case MACHINEZX97LE:
                strcpy(emulator.ROM97LE, machine.CurRom);
                break;

        case MACHINEQL:
                Form1->RamPackWobble1->Enabled=false;
                Form1->RamPackWobble1->Visible=false;
                strcpy(emulator.ROMQL, machine.CurRom);
                break;

        case MACHINESPECTRUM:
                Form1->RamPackWobble1->Enabled=false;
                Form1->RamPackWobble1->Visible=false;

                switch(NewSpec)
                {
                case SPECCY16:
                        spectrum.RAMBanks=1;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP48, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;

                case SPECCY48:
                case SPECCYPLUS:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP48, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;

                case SPECCYTC2048:
                        Form1->InsertDockCart1->Visible=true;
                        Form1->RemoveDockCart1->Visible=true;
                        Form1->DockSpacer->Visible=true;
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMTC2048, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        break;

                case SPECCYTS2068:
                        Form1->InsertDockCart1->Visible=true;
                        Form1->RemoveDockCart1->Visible=true;
                        Form1->DockSpacer->Visible=true;
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMTS2068, machine.CurRom);
                        spectrum.MFVersion=MFNONE;
                        Multiface->Enabled = false;
                        break;

                case SPECCY128:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(emulator.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(emulator.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(emulator.ROMZX16BIT, machine.CurRom);
                        else strcpy(emulator.ROMSP128, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;

                case SPECCYPLUS2:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        strcpy(emulator.ROMSPP2, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        Multiface->Caption = "Multiface 128";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;

                case SPECCYPLUS2A:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
                                strcpy(emulator.ROMSPP3ECF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E")
                                strcpy(emulator.ROMSPP3E, machine.CurRom);
                        else strcpy(emulator.ROMSPP3, machine.CurRom);
                        spectrum.MFVersion=MFPLUS3;
                        Multiface->Caption = "Multiface 3";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;

                case SPECCYPLUS3:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
                                strcpy(emulator.ROMSPP3ECF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E")
                                strcpy(emulator.ROMSPP3E, machine.CurRom);
                        spectrum.MFVersion=MFPLUS3;
                        Multiface->Caption = "Multiface 3";
                        Multiface->Enabled = true;
                        CreateBasicLister();
                        break;
                }
                break;
        }

        if (!BasicLister->ListerAvailable() && BasicLister->Visible)
        {
                BasicLister->Close();
        }

        romcartridge.type = RomCartridgeBox->ItemIndex;
        if (RomCartridgeBox->Text == "Timex")
        {
                romcartridge.type++;
        }
        romcartridge.zxc1Configuration = (ZXC1TYPE)ZXC1ConfigurationBox->ItemIndex;

        if ((romcartridge.type == ROMCARTRIDGENONE) || (RomCartridgeFileBox->Text.Trim() == ""))
        {
                RomCartridgeCapacity = 0;
                RomCartridgeFileBox->Enabled = false;
                ComboBoxRomCartridgeFileBox->Enabled = false;
                BrowseRomCartridge->Enabled = false;
                RomCartridgeBox->ItemIndex = ROMCARTRIDGENONE;
                ZXC1ConfigurationBox->Visible = false;
                RomCartridgeFileBox->Left = 184;
                RomCartridgeFileBox->Width = 181;
        }
        else
        {
                SelectRomCartridge();
                LoadRomCartridgeFile(RomCartridgeFileBox->Text.c_str());
                RomCartridgeFileBox->Enabled = true;
                ComboBoxRomCartridgeFileBox->Enabled = true;
                BrowseRomCartridge->Enabled = true;
                if (romcartridge.type == ROMCARTRIDGEZXC1)
                {
                        ZXC1ConfigurationBox->Visible = true;
                        RomCartridgeFileBox->Left = 277;
                        RomCartridgeFileBox->Width = 88;
                }
                else
                {
                        ZXC1ConfigurationBox->Visible = false;
                        RomCartridgeFileBox->Left = 184;
                        RomCartridgeFileBox->Width = 181;
                }
                ResetRequired=true;
        }

        if (!Multiface->Checked) spectrum.MFVersion=MFNONE;

        spectrum.uspeech = uSpeech->Checked;
        spectrum.usource = uSource->Checked;
        
        Form1->InWaveLoader->Enabled=true;
        Form1->OutWaveLoader->Enabled=true;

        zx81.shadowROM = !EnableLowRAM->Checked;
        zx81.RAM816k = EnableLowRAM->Checked;
        machine.protectROM = ProtectROM->Checked;

        if ((zx81.chrgen != CHRGENQS) && (ChrGenBox->ItemIndex == CHRGENQS))
        {
                zx81.enableQSchrgen = true;
        }
        else if (ChrGenBox->ItemIndex != CHRGENQS)
        {
                zx81.enableQSchrgen = false;
        }
        zx81.chrgen = ChrGenBox->ItemIndex;
        Form1->QSChrEnable->Checked = zx81.enableQSchrgen;
        Form1->QSChrEnable->Enabled = (zx81.chrgen == CHRGENQS);
        Form1->QSChrEnable->Visible = (NewMachine != MACHINEACE) && (NewMachine != MACHINEQL) && (NewMachine != MACHINESPECTRUM);

        zx81.FloatingPointHardwareFix = FloatingPointHardwareFix->Checked;

        machine.zxprinter = ZXPrinter->Checked;
        zx81.extfont=0;
        if ((zx81.chrgen==CHRGENDK) || (zx81.chrgen==CHRGENCHR128))
                zx81.maxireg = 0x3F;
        else
                zx81.maxireg = 0x1F;
        if (zx81.chrgen == CHRGENLAMBDA) zx81.extfont=1;

        machine.NTSC = NTSC->Checked;

        Form1->MemotechReset->Enabled = false;
        Form1->MemotechReset->Visible = (NewMachine != MACHINEACE) && (NewMachine != MACHINEQL) && (NewMachine != MACHINESPECTRUM);

        switch(HiResBox->ItemIndex)
        {
        case 1: zx81.truehires = HIRESWRX; break;
        case 2: zx81.truehires = HIRESG007; break;
        case 3: zx81.truehires = HIRESMEMOTECH;
                Form1->MemotechReset->Enabled = true;
                break;
        default:
        case 0: zx81.truehires = HIRESDISABLED; break;
        }

        switch(SoundCardBox->ItemIndex)
        {
        case 1: machine.aysound=1; machine.aytype=AY_TYPE_ACE; break;
        case 2: machine.aysound=1; machine.aytype=AY_TYPE_BOLDFIELD; break;
        case 3: machine.aysound=1; machine.aytype=AY_TYPE_FULLER; break;
        case 4: machine.aysound=1; machine.aytype=AY_TYPE_SINCLAIR; break;
        case 5: machine.aysound=1; machine.aytype=AY_TYPE_TIMEX; break;
        case 6: machine.aysound=1; machine.aytype=AY_TYPE_QUICKSILVA; break;
        case 7: machine.aysound=1; machine.aytype=AY_TYPE_ZONX; break;
        case 0:
        default: machine.aysound=0; machine.aytype=0; break;
        }

        spectrum.HDType=HDNONE;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF") spectrum.HDType=HDZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE V1") { spectrum.HDType=HDDIVIDE; spectrum.divIDEVersion=1; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE V2") { spectrum.HDType=HDDIVIDE; spectrum.divIDEVersion=2; }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E") spectrum.HDType=HDPLUS3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="AceCF") spectrum.HDType=HDACECF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") spectrum.HDType=HDPITERSCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") spectrum.HDType=HDPITERS8B;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") spectrum.HDType=HDPITERS16B;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="MWCFIde") spectrum.HDType=HDPITERSCF;
        spectrum.WriteProtectJumper=WriteProtect->Checked;
        spectrum.UploadJumper=Upload->Checked;

        switch(ZXCFRAM->ItemIndex)
        {
        case 0: spectrum.ZXCFRAMSize=128/16; break;
        case 1: spectrum.ZXCFRAMSize=512/16; break;
        case 2: spectrum.ZXCFRAMSize=1024/16; break;
        }

        //spectrum.interface1=false;
        spectrum.floppytype=FLOPPYNONE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="MGT Disciple") spectrum.floppytype=FLOPPYDISCIPLE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="MGT PlusD") spectrum.floppytype=FLOPPYPLUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Plus 3 FDC") spectrum.floppytype=FLOPPYPLUS3;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Opus Discovery") spectrum.floppytype=FLOPPYOPUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="BetaDisc") spectrum.floppytype=FLOPPYBETA;
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
        zx81.m1not = M1Not->Checked? 49152 : 32768;
        zx81.improvedWait = ImprovedWait->Checked;

        if (emulator.machine==MACHINEZX97LE)
        {
                zx81.RAMTOP=65535;
                zx81.m1not=49152;
        }

        if (TS2050->Checked==1) { machine.ts2050=1; Form1->TS20501->Enabled=true; }
        else { machine.ts2050=0; Form1->TS20501->Enabled=false; }

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
                machine.nmi = NULL;
                machine.exit = NULL;
                break;

        case MACHINEQL:
                machine.initialise = ql_initialise;
                machine.do_scanline = ql_do_scanline;
                machine.writebyte = ql_writebyteProxy;
                machine.setbyte = ql_writebyte;
                machine.readbyte = ql_readbyte;
                machine.readoperandbyte = ql_readbyte;
                machine.getbyte = ql_readbyte;
                machine.opcode_fetch = ql_opcode_fetch;
                machine.readport = ql_readport;
                machine.writeport = ql_writeport;
                machine.contendmem = ql_contend;
                machine.contendio = ql_contend;
                machine.reset = NULL;
                machine.nmi = NULL;
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
                machine.nmi = spec48_nmi;
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
                //else if (spectrum.model==SPECCYTC2048)
                //{
                //        machine.clockspeed=3528000;
                //        machine.tperscanline=226;
                //        spectrum.intposition=14336;
                //        machine.scanlines=312;
                //        machine.tperframe= machine.tperscanline * machine.scanlines;
                //}
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
                machine.reset = NULL;
                machine.nmi = NULL;
                machine.exit = NULL;
                break;
        }

        if ((NewMachine == MACHINESPECTRUM) || (NewMachine == MACHINEQL))
        {
                zx81.RAMTOP = 65535;
                machine.ace96k=0;
        }
        else if (RamPackBox->Items->Strings[RamPackBox->ItemIndex]=="96k")
        {
                zx81.RAMTOP = 65535;
                machine.ace96k=1;
        }
        else
        {
                int kp = machine.baseRamSize;
                if (RamPackBox->ItemIndex!=0)
                {
                        int rp = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                        kp = machine.ramPackSupplementsInternalRam ? kp + rp : rp;
                }
                zx81.RAMTOP = (1 << (kp + 10)) + 16383;
                if (RamPackBox->ItemIndex == 6) zx81.RAMTOP = 65535;
                if (emulator.machine==MACHINEACE && RamPackBox->ItemIndex==0) zx81.RAMTOP=16383;
                machine.ace96k=0;
        }

        SetUpRamSettings();

        spectrum.kbissue = Issue2->Checked;
        spectrum.kmouse = KMouse->Checked;

        Form1->DiskDrives1->Visible=true;
        P3Drive->FormShow(NULL);

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
                        Form1->PrinterPort1->Caption="Disciple Printer Port";
                if (spectrum.floppytype==FLOPPYPLUSD)
                        Form1->PrinterPort1->Caption="PlusD Printer Port";
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

        symbolstore::reset();

        AnsiString romBase = emulator.cwd;
        romBase += "ROM\\";

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

        AnsiString sym = romBase;
        sym += ChangeFileExt(machine.CurRom, ".sym");
        symbolstore::loadROMSymbols(sym.c_str());
        SymbolBrowser->RefreshContent();

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
                        bmp = romBase + "ace.bmp";
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
                                bmp = romBase + "spec128.bmp";
                                break;
                        case SPECCYTC2048:
                                bmp = romBase + "tc2048.bmp";
                                break;
                        case SPECCYTS2068:
                                bmp = romBase + "ts2068.bmp";
                                break;
                        case SPECCY48:
                        case SPECCY16:
                        case SPECCYPLUS:
                                bmp = romBase + "spec48.bmp";
                                break;
                        case SPECCYPLUS2:
                                bmp = romBase + "specp2.bmp";
                                break;
                        case SPECCYPLUS2A:
                        case SPECCYPLUS3:
                                bmp = romBase + "specp3.version4_1.bmp";
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

        AccurateInit(false);
        Speed->Recalc(NULL);
        PCKbInit();
        Kb->OKClick(NULL);
        Artifacts->TrackBarChange(NULL);
        Artifacts->ConfigureDotCrawlOption();

        bool spec48 = (emulator.machine==MACHINESPECTRUM && spectrum.model<SPECCY128);
        if (!spec48)
        {
                Artifacts->DotCrawl1->Checked = false;
        }

        if (ResetRequired) machine.initialise();
        Sound.AYInit();
        Keyboard->KbChange();

        Form1->RZX1->Enabled=false;
        if (emulator.machine==MACHINESPECTRUM) Form1->RZX1->Enabled=true;

        if (machineChanged)
        {
                if ( ((emulator.machine==MACHINESPECTRUM || emulator.machine==MACHINEACE || emulator.machine==MACHINELAMBDA) && !Form1->Sound1->Checked)
                || ( (emulator.machine!=MACHINESPECTRUM && emulator.machine!=MACHINEACE && emulator.machine!=MACHINELAMBDA) && (machine.colour != COLOURCHROMA) && Form1->Sound1->Checked) )
                {
                        Form1->Sound1Click(NULL);
                }
        }

        Form1->EnableColourisationOptions();

        spectrum.drivebusy = -1;

        InitPatches(NewMachine);

        ResetDisplaySize();

        Dbg->ResetBreakpointHits();
        Dbg->RefreshBreakpointList();

        if (Sender) Close();

        if (Dbg->Visible) Dbg->UpdateVals();
        int r = Sound.ReInitialise(NULL, machine.fps,0,0,0);
        if (r)
        {
                AnsiString err = "EightyOne is unable to run. DirectSound creation failed, reporting error " + DirectSoundError(r);
                MessageBox(NULL, err.c_str(), "Error", 0);
                Application->Terminate();
        }
}

AnsiString THW::DirectSoundError(int errorCode)
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

void THW::CreateBasicLister()
{
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
        else if (!strcmp(machine.CurRom, "spec48.rom"))
        {
                BasicLister->SetBasicLister(new spec48BasicLister());
                Form1->BasicListerOption->Enabled = true;
        }
        else if (!strcmp(machine.CurRom, "spec128.rom") ||
                 !strcmp(machine.CurRom, "spec128.spanish.rom") ||
                 !strcmp(machine.CurRom, "specp2.rom") ||
                 !strcmp(machine.CurRom, "specp3.version4_0.rom") ||
                 !strcmp(machine.CurRom, "specp3.version4_1.rom") ||
                 !strcmp(machine.CurRom, "specp2.french.rom") ||
                 !strcmp(machine.CurRom, "specp2.spanish.rom") ||
                 !strcmp(machine.CurRom, "specp3.spanish.rom"))
        {
                BasicLister->SetBasicLister(new spec128BasicLister());
                Form1->BasicListerOption->Enabled = true;
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
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        QLBtn->Down=false;

        FloatingPointHardwareFix->Enabled = false;

        ButtonAdvancedMore->Visible = false;

        EnableRomCartridgeOption(true);
        RomCartridgeLabel->Enabled=true;

        ZXpand->Caption = "ZXpand+";
        Multiface->Caption = "Multiface 128";

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

        QLSettings->TabVisible=false;
        ResetRequired=true;

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count-1] == "96k")
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count-1] != "48k")
        {
                RamPackBox->Items->Add("48k");
        }

        if (NewMachine == MACHINETS1500 || NewMachine == MACHINER470 || NewMachine == MACHINETK85)
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        RamPackLbl->Enabled=true; RamPackBox->Enabled=true;
        RamPackBox->ItemIndex = machine.defaultRamPackIndex;
        DisplayTotalRam();
        SoundCardLbl->Enabled=true; SoundCardBox->Enabled=true;
        SoundCardBox->ItemIndex=0;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenLbl->Enabled=true; ChrGenBox->Enabled=true;
        if (ChrGenBox->ItemIndex==-1 || ChrGenBox->ItemIndex==4)
                ChrGenBox->ItemIndex=0;

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

        uSpeech->Enabled=false;
        uSpeech->Checked=false;

        uSource->Enabled=false;
        uSource->Checked=false;

        if (RomCartridgeBox->Items->Strings[RomCartridgeBox->Items->Count-1] == "ZXC1")
        {
                RomCartridgeBox->Items->Delete(RomCartridgeBox->Items->Count-1);
        }

        if (RomCartridgeBox->Items->Strings[RomCartridgeBox->Items->Count-1] == "Timex")
        {
                if (NewMachine != MACHINETS1500 && NewMachine != MACHINETS1000 && NewMachine != MACHINEZX81)
                {
                        RomCartridgeBox->Items->Delete(RomCartridgeBox->Items->Count-1);
                }
        }
        else
        {
                if (NewMachine == MACHINETS1500 || NewMachine == MACHINETS1000 || NewMachine == MACHINEZX81)
                {
                        RomCartridgeBox->Items->Add("Timex");
                }
        }

        bool timexSelected = (RomCartridgeBox->Text == "Timex");
        RomCartridgeFileBox->Visible = !timexSelected;
        ComboBoxRomCartridgeFileBox->Visible = timexSelected;

        SetZX80Icon();
}

void THW::SetZXpandState(bool checked, bool enabled)
{
        ZXpand->Checked=checked;
        ZXpand->Enabled=enabled;
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
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        QLBtn->Down=false;

        FloatingPointHardwareFix->Enabled = false;
        FloatingPointHardwareFix->Checked = false;

        ButtonAdvancedMore->Visible = false;

        SetZXpandState(false, false);
        ZXpand->Caption = "ZXpand+";
        Multiface->Caption = "Multiface 128";

        ResetRequired=true;

        EnableRomCartridgeOption(true);
        RomCartridgeLabel->Enabled=true;

        OldFDC=FDC->Items->Strings[FDC->ItemIndex];
        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->Items->Add("ZX Interface 1");
        FDC->Items->Add("BetaDisc");
        FDC->Items->Add("Opus Discovery");
        FDC->Items->Add("MGT Disciple");
        FDC->Items->Add("MGT PlusD");

        FDC->ItemIndex=0;
        for(i=0;i<FDC->Items->Count;i++)
        {
                if (FDC->Items->Strings[i]==OldFDC) FDC->ItemIndex=i;
        }

        FDC->Enabled=true;
        FDCChange(NULL);

        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        uSource->Checked=false;
        uSource->Enabled=false;

        QLSettings->TabVisible=false;

        RamPackLbl->Enabled=false; RamPackBox->Enabled=false;
        RamPackBox->ItemIndex=-1;

        SoundCardLbl->Enabled=true; SoundCardBox->Enabled=true;
        SoundCardBox->ItemIndex=0;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenBox->ItemIndex=0;
        ChrGenLbl->Enabled=false; ChrGenBox->Enabled=false;

        HiResBox->Items->Strings[0]="Sinclair";
        HiResBox->ItemIndex=0;
        HiResLbl->Enabled=false; HiResBox->Enabled=false;

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
        IDEBox->Items->Add("Plus 2/3E");
        IDEBox->Items->Add("divIDE V1");
        IDEBox->Items->Add("divIDE V2");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Piters CF");
        IDEBox->Items->Add("Piters 8Bit");
        IDEBox->Items->Add("Piters 16Bit");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=true;
        LabelIDE->Enabled=true;
        LabelFDC->Enabled=true;

        for(i=0;i<IDEBox->Items->Count;i++)
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;

        uSpeech->Enabled=true;
        uSource->Enabled=true;

        if (RomCartridgeBox->Items->Strings[RomCartridgeBox->Items->Count-1] == "Timex")
        {
                RomCartridgeBox->Items->Delete(RomCartridgeBox->Items->Count-1);
        }

        if (RomCartridgeBox->Items->Strings[RomCartridgeBox->Items->Count-1] != "ZXC1")
        {
                RomCartridgeBox->Items->Add("ZXC1");
        }
        bool sinclairSelected = (RomCartridgeBox->Text == "Sinclair");
        RomCartridgeFileBox->Visible = !sinclairSelected;
        ComboBoxRomCartridgeFileBox->Visible = sinclairSelected;

        SetZX80Icon();
}

void THW::SetupForQL(void)
{
        AnsiString OldIDE;
        int i;

        FloatingPointHardwareFix->Enabled = false;

        SetZXpandState(false,false);
        ZXpand->Caption = "ZXpand+";
        Multiface->Caption = "Multiface";

        EnableRomCartridgeOption(false);
        RomCartridgeLabel->Enabled=false;

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
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        QLBtn->Down=false;

        ButtonAdvancedMore->Visible = false;

        ResetRequired=true;

        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        uSource->Checked=false;
        uSource->Enabled=false;

        QLSettings->TabVisible=true;

        RamPackLbl->Enabled=false; RamPackBox->Enabled=false;
        RamPackBox->ItemIndex=-1;

        SoundCardLbl->Enabled=false; SoundCardBox->Enabled=false;
        if (SoundCardBox->ItemIndex==-1) SoundCardBox->ItemIndex=5;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenBox->ItemIndex=0;
        ChrGenLbl->Enabled=false; ChrGenBox->Enabled=false;

        HiResBox->Items->Strings[0]="Sinclair";
        HiResBox->ItemIndex=0;
        HiResLbl->Enabled=false; HiResBox->Enabled=false;

        while(ColourBox->Items->Count>1) ColourBox->Items->Delete(ColourBox->Items->Count-1);
        ColourBox->Items->Strings[0]="Sinclair";
        ColourBox->ItemIndex=0;
        ColourBox->Enabled=false;
        ColourLabel->Enabled=false;

        ProtectROM->Enabled=true;
        NTSC->Enabled=false;
        NTSC->Checked=false;
        EnableLowRAM->Enabled=false;
        M1Not->Enabled=false;
        ImprovedWait->Enabled=false;
        TS2050->Enabled=false;
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
        Multiface->Enabled=false;

        OldIDE=IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("Plus 2/3E");
        IDEBox->Items->Add("divIDE V1");
        IDEBox->Items->Add("divIDE V2");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Piters CF");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=false;
        LabelIDE->Enabled=false;

        for(i=0;i<IDEBox->Items->Count;i++)
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;

        AnsiString OldFDC=FDC->Items->Strings[FDC->ItemIndex];
        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->Items->Add("ZX Interface 1");
        FDC->Items->Add("BetaDisc");
        FDC->Items->Add("Opus Discovery");
        FDC->Items->Add("MGT Disciple");
        FDC->Items->Add("MGT PlusD");

        FDC->ItemIndex=0;
        for(i=0;i<FDC->Items->Count;i++)
                if (FDC->Items->Strings[i]==OldFDC) FDC->ItemIndex=i;

        FDC->Enabled=true;
        FDCChange(NULL);
}
//---------------------------------------------------------------------------
void THW::SetUpRamSettings()
{
        switch (NewMachine)
        {
        case MACHINEACE:
                machine.baseRamSize = 3;
                machine.ramPackSupplementsInternalRam = true;
                machine.defaultRamPackIndex = 4;
                break;

        case MACHINEQL:
                machine.baseRamSize = 128;
                machine.ramPackSupplementsInternalRam = true;
                break;

        case MACHINESPECTRUM:
                if (spectrum.model==SPECCY16 || spectrum.model==SPECCY48 || spectrum.model==SPECCYPLUS || spectrum.model==SPECCYTC2048)
                {
                        machine.baseRamSize = (spectrum.model==SPECCY16) ? 16 : 48;
                        machine.ramPackSupplementsInternalRam = true;
                }
                else if (spectrum.model==SPECCYTS2068)
                {
                        machine.baseRamSize = 48;
                        machine.ramPackSupplementsInternalRam = false;
                }
                else
                {
                        machine.baseRamSize = 128;
                        machine.ramPackSupplementsInternalRam = false;
                }
                break;
        default:
                if (NewMachine == MACHINETS1000 || NewMachine == MACHINELAMBDA)
                {
                        machine.baseRamSize = 2;
                        machine.ramPackSupplementsInternalRam = false;
                        machine.defaultRamPackIndex = 4;
                }
                else if (NewMachine == MACHINETS1500 || NewMachine == MACHINETK85 || NewMachine == MACHINER470)
                {
                        machine.baseRamSize = 16;
                        machine.ramPackSupplementsInternalRam = true;
                        machine.defaultRamPackIndex = 0;
                }
                else
                {
                        machine.baseRamSize = 1;
                        machine.ramPackSupplementsInternalRam = false;
                        machine.defaultRamPackIndex = 4;
                }
                break;
        }

        DisplayTotalRam();
}
void THW::DisplayTotalRam()
{
        int totalRam = machine.baseRamSize;
        if (RamPackBox->ItemIndex!=0)
        {
                int ramPack = atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());
                totalRam = machine.ramPackSupplementsInternalRam ? totalRam + ramPack : ramPack;
        }
        AnsiString ramSize = totalRam;
        LabelTotalRAM->Caption = "Total RAM: " + ramSize + "K";
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX80BtnClick(TObject *Sender)
{
        if (ZX80Btn->Down) return;

        NewMachine=MACHINEZX80;
        SetUpRamSettings();
        SetupForZX81();
        ZXpand->Enabled=true;
        ZXpand->Caption = "ZXpand";
        ZX80Btn->Down=true;
        NewMachineName=ZX80Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("zx80.rom");
        RomBox->Items->Add("zx81.edition1.rom");
        RomBox->Items->Add("zx81.edition2.rom");
        RomBox->Items->Add("zx81.edition3.rom");
        RomBox->Text = emulator.ROM80;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        FloatingPointHardwareFix->Enabled = true;
        ImprovedWait->Enabled = false;
        ImprovedWait->Checked = false;
        Form1->EnableColourisationOptions();

        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX81BtnClick(TObject *Sender)
{
        NewMachine=MACHINEZX81;
        SetUpRamSettings();
        SetupForZX81();
        ZXpand->Enabled=true;
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
        Form1->EnableColourisationOptions();

        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec48BtnClick(TObject *Sender)
{
        if (Spec48Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCY48;
        SetUpRamSettings();
        SetupForSpectrum();
        Spec48Btn->Down=true;

        uSpeech->Enabled=true;
        uSource->Enabled=true;

        Issue2->Enabled=true;
        NewMachineName=Spec48Btn->Caption;
        Form1->EnableColourisationOptions();
        RomBox->Clear();
        RomBox->Items->Add("spec48.rom");
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
        SetUpRamSettings();
        SetupForSpectrum();
        Spec128Btn->Down=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        NewMachineName=Spec128Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("spec128.rom");
        RomBox->Items->Add("spec128.spanish.rom");
        RomBox->Text = emulator.ROMSP128;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        Form1->EnableColourisationOptions();

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
        SetUpRamSettings();
        SetupForSpectrum();
        SpecPlusBtn->Down=true;

        uSpeech->Enabled=true;
        uSource->Enabled=true;

        Issue2->Enabled=true;
        NewMachineName=SpecPlusBtn->Caption;
        Form1->EnableColourisationOptions();
        RomBox->Clear();
        RomBox->Items->Add("spec48.rom");
        RomBox->Text = emulator.ROMSP48;
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
        SetUpRamSettings();
        SetupForSpectrum();
        Spec16Btn->Down=true;

        uSpeech->Enabled=true;
        uSource->Enabled=true;

        Issue2->Enabled=true;
        Issue2->Checked=true;
        NewMachineName=Spec16Btn->Caption;
        Form1->EnableColourisationOptions();
        RomBox->Clear();
        RomBox->Items->Add("spec48.rom");
        RomBox->Text = emulator.ROMSP48;
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
        SetUpRamSettings();
        SetupForSpectrum();
        SpecP2Btn->Down=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Form1->EnableColourisationOptions();
        NewMachineName=SpecP2Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("specp2.rom");
        RomBox->Items->Add("specp2.french.rom");
        RomBox->Items->Add("specp2.spanish.rom");
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
        SetUpRamSettings();
        SetupForSpectrum();
        SpecP2aBtn->Down=true;
        Multiface->Caption = "Multiface 3";

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Form1->EnableColourisationOptions();
        NewMachineName=SpecP2aBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("specp3.version4_0.rom");
        RomBox->Items->Add("specp3.version4_1.rom");
        RomBox->Items->Add("specp3.spanish.rom");
        RomBox->Text = emulator.ROMSPP3;
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
        SetUpRamSettings();
        SetupForSpectrum();
        SpecP3Btn->Down=true;
        Multiface->Caption = "Multiface 3";

        FloppyDrives->TabVisible=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Form1->EnableColourisationOptions();
        NewMachineName=SpecP3Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("specp3.version4_0.rom");
        RomBox->Items->Add("specp3.version4_1.rom");
        RomBox->Items->Add("specp3.spanish.rom");
        RomBox->Text = emulator.ROMSPP3;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        IDEBoxChange(NULL);

        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="Plus 3 FDC";
        FDC->ItemIndex=0;

        FDC->Enabled=false;
        FDCChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1000BtnClick(TObject *Sender)
{
        if (TS1000Btn->Down) return;

        NewMachine=MACHINETS1000;
        SetUpRamSettings();
        SetupForZX81();
        ZXpand->Enabled=true;
        TS1000Btn->Down=true;
        NewMachineName=TS1000Btn->Caption;
        Form1->EnableColourisationOptions();
        FloatingPointHardwareFix->Checked = false;
        RomBox->Clear();
        RomBox->Items->Add("zx81.edition1.rom");
        RomBox->Items->Add("zx81.edition2.rom");
        RomBox->Items->Add("zx81.edition3.rom");
        RomBox->Text = emulator.ROMTS1000;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1500BtnClick(TObject *Sender)
{
        if (TS1500Btn->Down) return;

        NewMachine=MACHINETS1500;
        SetUpRamSettings();
        SetupForZX81();
        ZXpand->Enabled=true;
        TS1500Btn->Down=true;
        NewMachineName=TS1500Btn->Caption;
        Form1->EnableColourisationOptions();
        FloatingPointHardwareFix->Checked = false;
        RomBox->Clear();
        RomBox->Items->Add("ts1500.rom");
        RomBox->Text = emulator.ROMTS1500;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaBtnClick(TObject *Sender)
{
        if (LambdaBtn->Down) return;

        NewMachine=MACHINELAMBDA;
        SetUpRamSettings();
        SetupForZX81();
        SetZXpandState(false,false);
        LambdaBtn->Down=true;
        NewMachineName=LambdaBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("lambda.rom");
        RomBox->Text = emulator.ROMLAMBDA;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=true;
        ChrGenBox->ItemIndex=4;
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
        ComboBoxRomCartridgeFileBox->Enabled = false;
        BrowseRomCartridge->Enabled = false;
        EnableRomCartridgeOption(false);
        RomCartridgeLabel->Enabled = false;
        FloatingPointHardwareFix->Checked = false;
        Form1->EnableColourisationOptions();
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::R470BtnClick(TObject *Sender)
{
        if (R470Btn->Down) return;

        NewMachine=MACHINER470;
        SetUpRamSettings();
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
        RomCartridgeLabel->Enabled = false;
        FloatingPointHardwareFix->Checked = false;
        Form1->EnableColourisationOptions();
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TK85BtnClick(TObject *Sender)
{
        if (TK85Btn->Down) return;

        NewMachine=MACHINETK85;
        SetUpRamSettings();
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
        RomCartridgeLabel->Enabled = false;
        FloatingPointHardwareFix->Checked = false;
        Form1->EnableColourisationOptions();
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::AceBtnClick(TObject *Sender)
{
        if (AceBtn->Down) return;

        NewMachine=MACHINEACE;
        SetUpRamSettings();
        SetupForZX81();
        IDEBox->Enabled=true;
        LabelIDE->Enabled=true;

        AceBtn->Down=true;
        NewMachineName=AceBtn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("ace.rom");
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
        IDEBox->Items->Add("AceCF");
        RamPackBox->Items->Add("96k");
        EnableRomCartridgeOption(false);
        RomCartridgeLabel->Enabled = false;
        SetZXpandState(false,false);
        Form1->EnableColourisationOptions();
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2048BtnClick(TObject *Sender)
{
        if (TC2048Btn->Down) return;

        NewMachine=MACHINESPECTRUM;
        NewSpec=SPECCYTC2048;
        SetUpRamSettings();
        SetupForSpectrum();
        TC2048Btn->Down=true;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        EnableRomCartridgeOption(false);
        RomCartridgeLabel->Enabled = false;

        NewMachineName=TC2048Btn->Caption;
        Form1->EnableColourisationOptions();
        RomBox->Clear();
        RomBox->Items->Add("tc2048.rom");
        RomBox->Text = emulator.ROMTC2048;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
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
        SetUpRamSettings();
        SetupForSpectrum();
        TS2068Btn->Down=true;
        Multiface->Enabled = false;

        uSpeech->Checked = false;
        uSpeech->Enabled = false;

        uSource->Checked = false;
        uSource->Enabled = false;
        
        SoundCardBox->ItemIndex = AY_TYPE_SINCLAIR;

        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NTSC->Checked=true;

        EnableRomCartridgeOption(false);
        RomCartridgeLabel->Enabled = false;

        Form1->EnableColourisationOptions();
        NewMachineName=TS2068Btn->Caption;
        RomBox->Clear();
        RomBox->Items->Add("ts2068.rom");
        RomBox->Text = emulator.ROMTS2068;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
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
        SetUpRamSettings();
        SetupForZX81();
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
        RamPackBox->ItemIndex=0;
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
        Form1->EnableColourisationOptions();
}
//---------------------------------------------------------------------------

void __fastcall THW::QLBtnClick(TObject *Sender)
{
        if (QLBtn->Down) return;

        NewMachine=MACHINEQL;
        SetUpRamSettings();
        SetupForQL();
        QLBtn->Down=true;

        NewMachineName=QLBtn->Caption;
        Form1->EnableColourisationOptions();
        RomBox->Text = emulator.ROMQL;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
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
        NewMachine = emulator.machine;
        FloatingPointHardwareFix->Enabled = false;

        if (ZX80Btn->Down || ZX81Btn->Down || Spec16Btn->Down || Spec48Btn->Down || SpecPlusBtn->Down || Spec128Btn->Down || QLBtn->Down)
        {
                Machine->ActivePage=Sinclair;
                FloatingPointHardwareFix->Enabled = ZX81Btn->Down;
        }
        else if (SpecP2Btn->Down || SpecP2aBtn->Down || SpecP3Btn->Down)
        {
                Machine->ActivePage=Amstrad;
        }
        else if (TS1000Btn->Down || TS1500Btn->Down  || TC2048Btn->Down || TS2068Btn->Down)
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
        AnsiString Rom;
        FILE *f;
        char FileName[256];

        ini->WriteInteger("HWARE","Top",Top);
        ini->WriteInteger("HWARE","Left",Left);
        ini->WriteBool("HWARE","ZX80",ZX80Btn->Down);
        ini->WriteBool("HWARE","ZX81",ZX81Btn->Down);
        ini->WriteBool("HWARE","Spec16",Spec16Btn->Down);
        ini->WriteBool("HWARE","Spec48",Spec48Btn->Down);
        ini->WriteBool("HWARE","SpecPlus",SpecPlusBtn->Down);
        ini->WriteBool("HWARE","Spec128",Spec128Btn->Down);
        ini->WriteBool("HWARE","SpecP2",SpecP2Btn->Down);
        ini->WriteBool("HWARE","SpecP2A",SpecP2aBtn->Down);
        ini->WriteBool("HWARE","SpecP3",SpecP3Btn->Down);
        ini->WriteBool("HWARE","TS1000",TS1000Btn->Down);
        ini->WriteBool("HWARE","TS1500",TS1500Btn->Down);
        ini->WriteBool("HWARE","TC2048",TC2048Btn->Down);
        ini->WriteBool("HWARE","TS2068",TS2068Btn->Down);
        ini->WriteBool("HWARE","Lambda",LambdaBtn->Down);
        ini->WriteBool("HWARE","R470",R470Btn->Down);
        ini->WriteBool("HWARE","TK85",TK85Btn->Down);
        ini->WriteBool("HWARE","ZX97LE",ZX97LEBtn->Down);
        ini->WriteBool("HWARE","QL",QLBtn->Down);
        ini->WriteBool("HWARE","ACE",AceBtn->Down);

        ini->WriteInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        ini->WriteInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        ini->WriteInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        ini->WriteInteger("HWARE","HiRes",HiResBox->ItemIndex);
        ini->WriteInteger("HWARE","QLCPU",QLCPU->ItemIndex);
        ini->WriteInteger("HWARE","QLMEM",QLMem->ItemIndex);
        ini->WriteInteger("HWARE","Colour",ColourBox->ItemIndex);
        ini->WriteInteger("HWARE","RomCartridge",RomCartridgeBox->ItemIndex);
        ini->WriteString("HWARE","RomCartridgeFile",RomCartridgeFileBox->Text);
        ini->WriteInteger("HWARE","ZXC1Configuration",ZXC1ConfigurationBox->ItemIndex);

        ini->WriteInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        ini->WriteInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        ini->WriteString("DRIVES","DriveA", spectrum.driveaimg);
        ini->WriteString("DRIVES","DriveB", spectrum.drivebimg);
        ini->WriteInteger("HWARE","FDCType",FDC->ItemIndex);
        ini->WriteBool("HWARE","Autoboot",Autoboot->Checked);
        ini->WriteBool("HWARE","uSpeech",uSpeech->Checked);
        ini->WriteBool("HWARE","uSource",uSource->Checked);
        ini->WriteBool("HWARE","ZXpand",ZXpand->Checked);

        if (ATA_GetHDF(0)) Rom=ATA_GetHDF(0); else Rom="NULL";
        ini->WriteString("DRIVES","HD0", Rom);
        if (ATA_GetHDF(1)) Rom=ATA_GetHDF(1); else Rom="NULL";
        ini->WriteString("DRIVES","HD1", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(0)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV0", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(1)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV1", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(2)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV2", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(3)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV3", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(4)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV4", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(5)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV5", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(6)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV6", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(7)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV7", Rom);

        ini->WriteBool("HWARE","ProtectRom",ProtectROM->Checked);
        ini->WriteBool("HWARE","NTSC",NTSC->Checked);
        ini->WriteBool("HWARE","LowRAM",EnableLowRAM->Checked);
        ini->WriteBool("HWARE","M1NOT",M1Not->Checked);
        ini->WriteBool("HWARE","IMPROVED_WAIT",ImprovedWait->Checked);
        ini->WriteBool("HWARE","TS2050",TS2050->Checked);
        ini->WriteBool("HWARE","Iss2Kb",Issue2->Checked);
        ini->WriteBool("HWARE","KMouse",KMouse->Checked);
        ini->WriteBool("HWARE","divIDEWP",WriteProtect->Checked);
        ini->WriteBool("HWARE","ZXCFWP",Upload->Checked);
        ini->WriteBool("HWARE","MFace",Multiface->Checked);
        ini->WriteBool("HWARE","ZXPrinter",ZXPrinter->Checked);
        ini->WriteInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        ini->WriteInteger("HWARE","HDRIVE",IDEBox->ItemIndex);
        ini->WriteBool("HWARE","FloatingPointHardwareFix",FloatingPointHardwareFix->Checked);

        Rom=emulator.ROM80; ini->WriteString("HWARE","ROM80",Rom);
        Rom=emulator.ROM81; ini->WriteString("HWARE","ROM81",Rom);
        Rom=emulator.ROMACE; ini->WriteString("HWARE","ROMACE",Rom);
        Rom=emulator.ROMTS1000; ini->WriteString("HWARE","ROMTS1000",Rom);
        Rom=emulator.ROMTS1500; ini->WriteString("HWARE","ROMTS1500",Rom);
        Rom=emulator.ROMLAMBDA; ini->WriteString("HWARE","ROMLAMBDA",Rom);
        Rom=emulator.ROMPC8300; ini->WriteString("HWARE","ROMPC8300",Rom);
        Rom=emulator.ROMTK85; ini->WriteString("HWARE","ROMTK85",Rom);
        Rom=emulator.ROM97LE; ini->WriteString("HWARE","ROM97LE",Rom);
        Rom=emulator.ROMR470; ini->WriteString("HWARE","ROMR470",Rom);
        Rom=emulator.ROMSP48; ini->WriteString("HWARE","ROMSP48",Rom);
        Rom=emulator.ROMSP128; ini->WriteString("HWARE","ROMSP128",Rom);
        Rom=emulator.ROMSPP2; ini->WriteString("HWARE","ROMSPP2",Rom);
        Rom=emulator.ROMSPP3; ini->WriteString("HWARE","ROMSPP3",Rom);
        Rom=emulator.ROMSPP3E; ini->WriteString("HWARE","ROMSPP3E",Rom);
        Rom=emulator.ROMSPP3ECF; ini->WriteString("HWARE","ROMSPP3ECF",Rom);
        Rom=emulator.ROMDock; ini->WriteString("HWARE","Dock",Rom);
        Rom=emulator.ROMZX8BIT; ini->WriteString("HWARE","ZX8BIT",Rom);
        Rom=emulator.ROMZX16BIT; ini->WriteString("HWARE","ZX16BIT",Rom);
        Rom=emulator.ROMZXCF; ini->WriteString("HWARE","ZXCF",Rom);
        Rom=emulator.ROMQL; ini->WriteString("HWARE","ROMQL",Rom);

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\divide.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(divIDEMem, 8192, 1, f);
                fclose(f);
        }

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\zxcf.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(ZXCFMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\zx1541.nv");

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
        Rom=emulator.ROMLAMBDA; Rom=ini->ReadString("HWARE","ROMLAMBDA",Rom).LowerCase(); strcpy(emulator.ROMLAMBDA, Rom.c_str());
        Rom=emulator.ROMPC8300; Rom=ini->ReadString("HWARE","ROMPC8300",Rom).LowerCase(); strcpy(emulator.ROMPC8300, Rom.c_str());
        Rom=emulator.ROMTK85; Rom=ini->ReadString("HWARE","ROMTK85",Rom).LowerCase(); strcpy(emulator.ROMTK85, Rom.c_str());
        Rom=emulator.ROM97LE; Rom=ini->ReadString("HWARE","ROM97LE",Rom).LowerCase(); strcpy(emulator.ROM97LE, Rom.c_str());
        Rom=emulator.ROMR470; Rom=ini->ReadString("HWARE","ROMR470",Rom).LowerCase(); strcpy(emulator.ROMR470, Rom.c_str());
        Rom=emulator.ROMSP48; Rom=ini->ReadString("HWARE","ROMSP48",Rom).LowerCase(); strcpy(emulator.ROMSP48, Rom.c_str());
        Rom=emulator.ROMSP128; Rom=ini->ReadString("HWARE","ROMSP128",Rom).LowerCase(); strcpy(emulator.ROMSP128, Rom.c_str());
        Rom=emulator.ROMSPP2; Rom=ini->ReadString("HWARE","ROMSPP2",Rom).LowerCase(); strcpy(emulator.ROMSPP2, Rom.c_str());
        Rom=emulator.ROMSPP3; Rom=ini->ReadString("HWARE","ROMSPP3",Rom).LowerCase(); strcpy(emulator.ROMSPP3, Rom.c_str());
        Rom=emulator.ROMSPP3E; Rom=ini->ReadString("HWARE","ROMSPP3E",Rom).LowerCase(); strcpy(emulator.ROMSPP3E, Rom.c_str());
        Rom=emulator.ROMSPP3ECF; Rom=ini->ReadString("HWARE","ROMSPP3ECF",Rom).LowerCase(); strcpy(emulator.ROMSPP3ECF, Rom.c_str());
        Rom=emulator.ROMDock; Rom=ini->ReadString("HWARE","Dock",Rom).LowerCase(); strcpy(emulator.ROMDock, Rom.c_str());
        Rom=emulator.ROMZX8BIT; Rom=ini->ReadString("HWARE","ZX8BIT",Rom).LowerCase(); strcpy(emulator.ROMZX8BIT, Rom.c_str());
        Rom=emulator.ROMZX16BIT; Rom=ini->ReadString("HWARE","ZX16BIT",Rom).LowerCase(); strcpy(emulator.ROMZX16BIT, Rom.c_str());
        Rom=emulator.ROMZXCF; Rom=ini->ReadString("HWARE","ZXCF",Rom).LowerCase(); strcpy(emulator.ROMZXCF, Rom.c_str());
        Rom=emulator.ROMQL; Rom=ini->ReadString("HWARE","ROMQL",Rom).LowerCase(); strcpy(emulator.ROMQL, Rom.c_str());

        if (ini->ReadBool("HWARE","ZX80",ZX80Btn->Down)) ZX80BtnClick(NULL);
        if (ini->ReadBool("HWARE","ZX81",ZX81Btn->Down)) ZX81BtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec16",Spec16Btn->Down)) Spec16BtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec48",Spec48Btn->Down)) Spec48BtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecPlus",SpecPlusBtn->Down)) SpecPlusBtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec128",Spec128Btn->Down)) Spec128BtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP2",SpecP2Btn->Down)) SpecP2BtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP2A",SpecP2aBtn->Down)) SpecP2aBtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP3",SpecP3Btn->Down)) SpecP3BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS1000",TS1000Btn->Down)) TS1000BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS1500",TS1500Btn->Down)) TS1500BtnClick(NULL);
        if (ini->ReadBool("HWARE","TC2048",TC2048Btn->Down)) TC2048BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS2068",TS2068Btn->Down)) TS2068BtnClick(NULL);
        if (ini->ReadBool("HWARE","Lambda",LambdaBtn->Down)) LambdaBtnClick(NULL);
        if (ini->ReadBool("HWARE","R470",R470Btn->Down)) R470BtnClick(NULL);
        if (ini->ReadBool("HWARE","TK85",TK85Btn->Down)) TK85BtnClick(NULL);
        if (ini->ReadBool("HWARE","ZX97LE",ZX97LEBtn->Down)) ZX97LEBtnClick(NULL);
        if (ini->ReadBool("HWARE","ACE",AceBtn->Down)) AceBtnClick(NULL);
        if (ini->ReadBool("HWARE","QL",QLBtn->Down)) QLBtnClick(NULL);

        RamPackBox->ItemIndex=ini->ReadInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        SoundCardBox->ItemIndex=ini->ReadInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        ChrGenBox->ItemIndex=ini->ReadInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        HiResBox->ItemIndex=ini->ReadInteger("HWARE","HiRes",HiResBox->ItemIndex);
        QLMem->ItemIndex=ini->ReadInteger("HWARE","QLMEM",QLMem->ItemIndex);
        QLCPU->ItemIndex=ini->ReadInteger("HWARE","QLCPU",QLCPU->ItemIndex);
        ColourBox->ItemIndex=ini->ReadInteger("HWARE","Colour",ColourBox->ItemIndex);
        RomCartridgeBox->ItemIndex=ini->ReadInteger("HWARE","RomCartridge",RomCartridgeBox->ItemIndex);
        RomCartridgeFileBox->Text=ini->ReadString("HWARE","RomCartridgeFile","");
        ZXC1ConfigurationBox->ItemIndex=ini->ReadInteger("HWARE","ZXC1Configuration",ZXC1ConfigurationBox->ItemIndex);
        if (RomCartridgeFileBox->Text.Length() > 0)
        {
                RomCartridgeFileBox->SelStart=RomCartridgeFileBox->Text.Length()-1;
                RomCartridgeFileBox->SelLength=0;
        }
        
        DriveAType->ItemIndex=ini->ReadInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        DriveBType->ItemIndex=ini->ReadInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        FDC->ItemIndex=ini->ReadInteger("HWARE","FDCType",FDC->ItemIndex);
        Autoboot->Checked=ini->ReadBool("HWARE","Autoboot",Autoboot->Checked);

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

        Rom=ini->ReadString("DRIVES","MDV0", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(0,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV1", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(1,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV2", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(2,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV3", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(3,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV4", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(4,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV5", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(5,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV6", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(6,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV7", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(7,Rom.c_str());

        SetZXpandState(ini->ReadBool("HWARE","ZXpand",ZXpand->Checked),
                        ini->ReadBool("HWARE","ZX81",ZX81Btn->Down)||ini->ReadBool("HWARE","ZX80",ZX81Btn->Down)||ini->ReadBool("HWARE","TS1500",TS1500Btn->Down)); 
        ProtectROM->Checked=ini->ReadBool("HWARE","ProtectRom",ProtectROM->Checked);
        NTSC->Checked=ini->ReadBool("HWARE","NTSC",NTSC->Checked);
        EnableLowRAM->Checked=ini->ReadBool("HWARE","LowRAM",EnableLowRAM->Checked);
        M1Not->Checked=ini->ReadBool("HWARE","M1NOT",M1Not->Checked);
        ImprovedWait->Checked=ini->ReadBool("HWARE","IMPROVED_WAIT",ImprovedWait->Checked);
        TS2050->Checked=ini->ReadBool("HWARE","TS2050",TS2050->Checked);
        Issue2->Checked=ini->ReadBool("HWARE","Iss2Kb",Issue2->Checked);
        KMouse->Checked=ini->ReadBool("HWARE","KMouse",KMouse->Checked);
        WriteProtect->Checked=ini->ReadBool("HWARE","divIDEWP",WriteProtect->Checked);
        Multiface->Checked=ini->ReadBool("HWARE","MFace",Multiface->Checked);
        ZXPrinter->Checked=ini->ReadBool("HWARE","ZXPrinter",ZXPrinter->Checked);
        FloatingPointHardwareFix->Checked=ini->ReadBool("HWARE","FloatingPointHardwareFix",FloatingPointHardwareFix->Checked);

        Upload->Checked=ini->ReadBool("HWARE","ZXCFWP",Upload->Checked);
        ZXCFRAM->ItemIndex=ini->ReadInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        IDEBox->ItemIndex=ini->ReadInteger("HWARE","HDRIVE",IDEBox->ItemIndex);
        IDEBoxChange(NULL);
        FDCChange(NULL);

        uSpeech->Checked=ini->ReadBool("HWARE","uSpeech",uSpeech->Checked);
        uSource->Checked=ini->ReadBool("HWARE","uSource",uSource->Checked);

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\zxcf.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(ZXCFMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\divide.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(divIDEMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName,emulator.cwd);
        strcat(FileName,"NV_Memory\\zx1541.nv");

        f=fopen(FileName,"rb");
        if (f)
        {
                fread(ZX1541Mem, 1, 8192, f);
                fclose(f);
        }
}

void __fastcall THW::BrowseROMClick(TObject *Sender)
{
        AnsiString Path;
        char cPath[512];

        Path = emulator.cwd;
        Path += "ROM";

        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomBox->Text;

        if (!RomSelect->Execute()) return;

        strcpy(cPath,(RomSelect->FileName).c_str());
        if (!strncmp(cPath,Path.c_str(),strlen(Path.c_str())))
        {
                Path=RomSelect->FileName;
                Path=RemovePath(Path);
        }
        else    Path=cPath;

        RomBox->Text=Path;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ResetRequired=true;

        SetZX80Icon();
}
//---------------------------------------------------------------------------

void THW::EnableRomCartridgeOption(bool enable)
{
        RomCartridgeBox->ItemIndex = 0;
        RomCartridgeFileBox->Text = "";
        RomCartridgeBox->Enabled = enable;
        RomCartridgeFileBox->Enabled = enable;
        ComboBoxRomCartridgeFileBox->Enabled = enable;
        BrowseRomCartridge->Enabled = enable;
        ZXC1ConfigurationBox->Visible = false;
        RomCartridgeFileBox->Left = 184;
        RomCartridgeFileBox->Width = 181;
}

//---------------------------------------------------------------------------

void __fastcall THW::RamPackBoxChange(TObject *Sender)
{
        SetUpRamSettings();

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
        if (HiResBox->Items->Strings[HiResBox->ItemIndex]=="G007") EnableLowRAM->Checked=true;
        if (HiResBox->Items->Strings[HiResBox->ItemIndex]=="Memotech") ProtectROM->Checked=true;
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
        WriteProtect->Visible=false;

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

                if (SpecP2aBtn->Down || SpecP3Btn->Down) RomBox->Text = emulator.ROMSPP3ECF;
        }

        if ((IDEBox->Items->Strings[IDEBox->ItemIndex]).Pos("divIDE"))
        {
                WriteProtect->Visible=true;
                Upload->Visible=true;
        }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E") RomBox->Text = emulator.ROMSPP3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") RomBox->Text = emulator.ROMZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") RomBox->Text = emulator.ROMZX8BIT;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") RomBox->Text = emulator.ROMZX16BIT;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="None")
        {
                if (SpecP2aBtn->Down || SpecP3Btn->Down) RomBox->Text = emulator.ROMSPP3;
                if (Spec16Btn->Down || Spec48Btn->Down || SpecPlusBtn->Down) RomBox->Text = emulator.ROMSP48;
                if (Spec128Btn->Down) RomBox->Text = emulator.ROMSP128;
                if (SpecP2Btn->Down) RomBox->Text = emulator.ROMSPP2;
                if (TC2048Btn->Down) RomBox->Text = emulator.ROMTC2048;
                if (TS2068Btn->Down) RomBox->Text = emulator.ROMTS2068;
        }

        ResetRequired=true;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
}
//---------------------------------------------------------------------------

void __fastcall THW::FDCChange(TObject *Sender)
{
        if (FDC->Items->Strings[FDC->ItemIndex]=="Larken")
                EnableLowRAM->Checked=true;

        if (FDC->Items->Strings[FDC->ItemIndex]=="Plus 3 FDC")
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

        if (FDC->Items->Strings[FDC->ItemIndex]=="BetaDisc")
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
                                             (SoundCardBox->ItemIndex == 7) &&
                                             EnableLowRAM->Checked;

                if (!allFacilitiesSelected)
                {
                        int ret = Application->MessageBox("Automatically select the following ZXpand+ facilities?\n\n32k RAM Pack, RAM in 8k-16k Region, WRX High Resolution, ZonX Sound", "ZXpand+ Configuration", MB_YESNO | MB_ICONQUESTION);

                        if (ret == IDYES)
                        {
                                HiResBox->ItemIndex = 1;        // WRX
                                RamPackBox->ItemIndex = 5;      // 32K RAM
                                SoundCardBox->ItemIndex = 7;    // ZONX
                                EnableLowRAM->Checked = true;   // 8K-16K RAM
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::BrowseRomCartridgeClick(TObject *Sender)
{
        AnsiString Path;
        char cPath[512];

        Path = emulator.cwd;
        Path += "ROM";

        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomCartridgeFileBox->Text;

        if (!RomSelect->Execute()) return;

        strcpy(cPath,(RomSelect->FileName).c_str());
        if (!strncmp(cPath,Path.c_str(),strlen(Path.c_str())))
        {
                Path=RomSelect->FileName;
                Path=RemovePath(Path);
        }
        else    Path=cPath;

        RomCartridgeFileBox->Text=Path;
        RomCartridgeFileBox->SelStart=RomCartridgeFileBox->Text.Length()-1;
        RomCartridgeFileBox->SelLength=0;

        ComboBoxRomCartridgeFileBox->Text = RomCartridgeFileBox->Text;
        ComboBoxRomCartridgeFileBox->SelStart = RomCartridgeFileBox->SelStart;
        ComboBoxRomCartridgeFileBox->SelLength = RomCartridgeFileBox->SelLength;

        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::RomCartridgeBoxChange(TObject *Sender)
{
        RomCartridgeFileBox->Text = "";
        ComboBoxRomCartridgeFileBox->Text = "";

        bool romCartridgeSelected = (RomCartridgeBox->Text != "None");
        RomCartridgeFileBox->Enabled = romCartridgeSelected;
        ComboBoxRomCartridgeFileBox->Enabled = romCartridgeSelected;
        BrowseRomCartridge->Enabled = romCartridgeSelected;

        bool zxc1Selected = (RomCartridgeBox->Text == "ZXC1");
        ZXC1ConfigurationBox->Visible = zxc1Selected;
        if (zxc1Selected)
        {
                RomCartridgeFileBox->Left = 277;
                RomCartridgeFileBox->Width = 88;

                if (ZXC1ConfigurationBox->ItemIndex == -1)
                {
                        ZXC1ConfigurationBox->ItemIndex = 0;
                }
        }
        else
        {
                RomCartridgeFileBox->Left = 184;
                RomCartridgeFileBox->Width = 181;
        }

        bool spectrumSinclairSelected = (RomCartridgeBox->Text == "Sinclair") && (NewMachine == MACHINESPECTRUM);
        bool timexSelected = (RomCartridgeBox->Text == "Timex");
        RomCartridgeFileBox->Visible = !spectrumSinclairSelected && !timexSelected;
        ComboBoxRomCartridgeFileBox->Visible = spectrumSinclairSelected || timexSelected;

        if (timexSelected)
        {
                PopulateRomCartridgeTimexList();
        }
        else if (spectrumSinclairSelected)
        {
                PopulateRomCartridgeSinclairList();
        }

        ResetRequired=true;
}

void THW::SelectRomCartridge()
{
        AnsiString romCartridgePath;

        if (ComboBoxRomCartridgeFileBox->Visible)
        {
                romCartridgePath = ComboBoxRomCartridgeFileBox->Text;
        }
        else
        {
                romCartridgePath = RomCartridgeFileBox->Text;
        }

        RomCartridgeFileBox->Text = romCartridgePath;
        ComboBoxRomCartridgeFileBox->Text = romCartridgePath;

        bool spectrumSinclairSelected = (RomCartridgeBox->Text == "Sinclair") && (emulator.machine == MACHINESPECTRUM);
        bool timexSelected = (RomCartridgeBox->Text == "Timex");
        RomCartridgeFileBox->Visible = !spectrumSinclairSelected && !timexSelected;
        ComboBoxRomCartridgeFileBox->Visible = spectrumSinclairSelected || timexSelected;
}
//---------------------------------------------------------------------------
void __fastcall THW::ComboBoxRomCartridgeFileBoxChange(TObject *Sender)
{
        if (ComboBoxRomCartridgeFileBox->Visible)
        {
                RomCartridgeFileBox->Text = ComboBoxRomCartridgeFileBox->Text;
                ResetRequired=true;
        }
}
//---------------------------------------------------------------------------

void __fastcall THW::ColourBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::RomBoxChange(TObject *Sender)
{
        ResetRequired=true;

        SetZX80Icon();
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
        ShellExecute(Application->Handle,"OPEN","EXPLORER.EXE", replaced.c_str(), NULL, 1);
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
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeTimexList()
{
        ComboBoxRomCartridgeFileBox->Items->Clear();

        AddRomCartridgeFile("07-9001 Supermath.rom");
        AddRomCartridgeFile("07-9002 States & Capitals.rom");
        AddRomCartridgeFile("07-9003 Chess.rom");
}
//---------------------------------------------------------------------------
void THW::PopulateRomCartridgeSinclairList()
{
        ComboBoxRomCartridgeFileBox->Items->Clear();

        AddRomCartridgeFile("G9R Space Raiders.rom");
        AddRomCartridgeFile("G10R Chess.rom");
        AddRomCartridgeFile("G12R Planetoids.rom");
        AddRomCartridgeFile("G13R Hungry Horace.rom");
        AddRomCartridgeFile("G22R Backgammon.rom");
        AddRomCartridgeFile("G24R Horace & The Spiders.rom");
        AddRomCartridgeFile("G27R Jet Pac.rom");
        AddRomCartridgeFile("G28R Pssst.rom");
        AddRomCartridgeFile("G29R Tranz Am.rom");
        AddRomCartridgeFile("G30R Cookie.rom");
        AddRomCartridgeFile("Gyruss.rom");
        AddRomCartridgeFile("Loco Motion.rom");
        AddRomCartridgeFile("Montezuma's Revenge #1.rom");
        AddRomCartridgeFile("Montezuma's Revenge #2.rom");
        AddRomCartridgeFile("Popeye.rom");
        AddRomCartridgeFile("Q-Bert.rom");
        AddRomCartridgeFile("Return Of The Jedi.rom");
        AddRomCartridgeFile("Star Wars.rom");
}
//---------------------------------------------------------------------------
void THW::AddRomCartridgeFile(AnsiString fileName)
{
        AnsiString romBase = emulator.cwd;
        romBase += "ROM\\";

        if (FileExists(romBase + fileName))
        {
                ComboBoxRomCartridgeFileBox->Items->Add(fileName);
        }
}
void __fastcall THW::uSourceClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

