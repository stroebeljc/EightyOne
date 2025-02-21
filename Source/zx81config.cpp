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
 * zx81config.c
 *
 */

#include <string.h>
#include <dir.h>
#include <vcl4.h>

#include "zx81config.h"

ZX81 zx81;
ZX97 zx97;
SPECTRUM spectrum;
TV tv;
MACHINE machine;
MOUSE mouse;
ROMCARTRIDGE romcartridge;
EMULATOR emulator;

int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
int lastMemoryReadValueLo, lastMemoryWriteValueLo;
int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
int lastMemoryReadValueHi, lastMemoryWriteValueHi;
bool directMemoryAccess;

const char* iniFolder                     = "EightyOne\\";
const char* temporaryFolder               = "EightyOne\\";

const char* nvMemoryFolder                = "NV_Memory\\";
const char* documentationFolder           = "Documentation\\";
const char* exampleZX81ProgramsFolder     = "Examples\\ZX81 Programs\\";
const char* exampleSpectrumProgramsFolder = "Examples\\Spectrum Programs\\";

const char* romsFolder                    = "ROMs\\";
const char* fdcRomsFolder                 = "FDCs\\";
const char* ideRomsFolder                 = "IDEs\\";
const char* interfaceRomsFolder           = "Interfaces\\";
const char* graphicRomsFolder             = "Graphics\\";
const char* replacementRomsFolder         = "Replacement ROMs\\";
const char* speechRomsFolder              = "Speech\\";

const char* romCartridgeFolder            = "ROM Cartridges\\";
const char* if2RomsFolder                 = "ROM Cartridges\\ZX Interface 2\\";
const char* ts1510RomsFolder              = "ROM Cartridges\\TS1510\\";
const char* ts2068RomsFolder              = "ROM Cartridges\\TS2068\\";
const char* tc2068RomsFolder              = "ROM Cartridges\\TC2068\\";
const char* spectrumPlus2RomsFolder       = "ROM Cartridges\\Spectrum+2\\";;

extern bool GetVersionNumber(int& versionNumberMajor, int& versionNumberMinor, int& versionNumberPart3, int& versionNumberPart4);

void LoadMachineRoms();

void load_config(void)
{
        int versionNumberMajor;
        int versionNumberMinor;
        int versionNumberPart3;
        int versionNumberPart4;
        GetVersionNumber(versionNumberMajor, versionNumberMinor, versionNumberPart3, versionNumberPart4);
        
        emulator.emuid=EMUID;
        emulator.major=(CFGBYTE)versionNumberMajor;
        emulator.minor=(CFGBYTE)versionNumberMinor;
        emulator.testver=(CFGBYTE)versionNumberPart3;

        emulator.machine=MACHINEZX81;
        
        zx81.shadowROM=0;
        zx81.RAM816k=0;
        zx81.RAM816kWriteProtected=0;
        machine.protectROM=1;
        zx81.RAMTOP=32767;
        zx81.ROMTOP=8191;
        zx81.truehires=HIRESWRX;
        emulator.inverse=0;
        zx81.extfont=0;
        emulator.frameskip=0;
        machine.aytype=0;
        machine.speech=0;
        machine.joystickInterfaceType=0;
        zx81.vsyncsound=0;
        emulator.beepersound=0;
        machine.ts2050=0;
        machine.plus3arabicPagedOut=0;
        emulator.TZXin=0;
        emulator.TZXout=0;
        emulator.audioout=0;
        emulator.audioin=0;
        emulator.romcrc=-1;
        emulator.autoload=0;
        zx81.chrgen=CHRGENSINCLAIR;
        zx81.enableQSchrgen=0;
        emulator.simpleghost=1;
        zx81.maxireg=0x1F;
        zx81.zxpand=0;
        emulator.speedup=0;
        emulator.UseRShift=0;
        zx81.chromaMode=0x0F;
        zx81.chromaColourSwitchOn = 0;
        zx81.FloatingPointHardwareFix = 0;
        machine.NTSC=0;
        machine.clockspeed=3250000;
        machine.tperscanline=207;
        machine.tperframe=312*207;
        machine.fps=50;
        spectrum.interruptPosition=0;
        machine.zxprinter=1;
        machine.joystick1Controller = -1;
        machine.joystick2Controller = -1;

        emulator.ColouriseHorizontalSyncPulse = false;
        emulator.ColouriseVerticalSyncPulse = false;
        emulator.ColouriseRomDisplayDriver = false;
        emulator.ColouriseBackPorch = false;
        emulator.ColouriseNonMaskableInterruptResponse = false;
        emulator.ColouriseNonMaskableInterruptResponseWaitStates = false;
        emulator.ColouriseNonMaskableInterruptServiceRoutine = false;
        emulator.ColouriseNonMaskableInterruptServiceRoutineRecursion = false;
        emulator.ColouriseMaskableInterruptResponse = false;
        emulator.ColouriseMaskableInterruptServiceRoutine = false;
        emulator.ColouriseInstructionStraddlingNMI = false;
        emulator.ColouriseInstructionStraddlingNMIWaitStates = false;
        emulator.ColouriseZ80Halted = false;

        machine.colour=COLOURDISABLED;

        romcartridge.type = ROMCARTRIDGENONE;
        romcartridge.zxcPaging = 0;
        romcartridge.zxc1PageOut = false;
        romcartridge.zxc1ActiveBank = 0;
        romcartridge.zxc1Configuration = ZXC1_32K;
        romcartridge.zxcLowerControlAccessSelected = 0;
        romcartridge.zxcInterface1BankPagedIn = 0;
        romcartridge.zxcCassetteBankPagedIn = 0;

        emulator.bordersize=BORDERNORMAL;

        emulator.single_step=0;

        LoadMachineRoms();

        AnsiString plusdRom = AnsiString("+d.g+dos.v1-a.rom");
        strcpy(emulator.ROMPLUSD, plusdRom.c_str());
        AnsiString discipleRom = AnsiString("disciple.gdos.v3.rom");
        strcpy(emulator.ROMDISCIPLE, discipleRom.c_str());
        AnsiString opusdiscoveryRom = AnsiString("discovery.v2-22.rom");
        strcpy(emulator.ROMDISCOVERY, opusdiscoveryRom.c_str());
        AnsiString trdosRom = AnsiString("beta128.trdos.v5-03.rom");
        strcpy(emulator.ROMBETADISK, trdosRom.c_str());
        AnsiString larken81Rom = AnsiString("larken81.rom");
        strcpy(emulator.ROMLARKEN81, larken81Rom.c_str());

        AnsiString interface1Ed1Rom = AnsiString("interface1.edition2.rom");
        strcpy(emulator.ROMINTERFACE1, interface1Ed1Rom.c_str());

        AnsiString simpleIde8BitRom = AnsiString("zx8blbs.rom");
        strcpy(emulator.ROMSIMPLE8BIT, simpleIde8BitRom.c_str());
        AnsiString simpleIde16BitRom = AnsiString("zxidelbs.rom");
        strcpy(emulator.ROMSIMPLE16BIT, simpleIde16BitRom.c_str());
        AnsiString simpleIdeCFRom = AnsiString("zxcflba.rom");
        strcpy(emulator.ROMSIMPLECF, simpleIdeCFRom.c_str());
        AnsiString mwcfideRom = AnsiString("mwcfide.rom");
        strcpy(emulator.ROMMWCFIDE, mwcfideRom.c_str());

        AnsiString uSpeechRom = interfaceRomsFolder + AnsiString("uspeech.rom");
        strcpy(emulator.ROMUSPEECH, uSpeechRom.c_str());
        AnsiString uSourceRom = interfaceRomsFolder + AnsiString("usource.rom");
        strcpy(emulator.ROMUSOURCE, uSourceRom.c_str());
        AnsiString multiface128Rom = interfaceRomsFolder + AnsiString("multiface128.rom");
        strcpy(emulator.ROMMULTIFACE128, multiface128Rom.c_str());
        AnsiString multiface3Rom = interfaceRomsFolder + AnsiString("multiface3.rom");
        strcpy(emulator.ROMMULTIFACE3, multiface3Rom.c_str());

        AnsiString dktronicsRom = graphicRomsFolder + AnsiString("dktronicsgraphics.rom");
        strcpy(emulator.ROMDKTRONICS, dktronicsRom.c_str());
        AnsiString kaydeRom = graphicRomsFolder + AnsiString("kaydegraphics.rom");
        strcpy(emulator.ROMKAYDE, kaydeRom.c_str());
        AnsiString g007Rom = graphicRomsFolder + AnsiString("g007hrg.rom");
        strcpy(emulator.ROMG007, g007Rom.c_str());
        AnsiString memotechRom = graphicRomsFolder + AnsiString("memotechhrg.rom");
        strcpy(emulator.ROMMEMOTECH, memotechRom.c_str());
        AnsiString QuicksilvaHiResRom = graphicRomsFolder + AnsiString("quicksilvahires.rom");
        strcpy(emulator.ROMQUICKSILVAHIRES, QuicksilvaHiResRom.c_str());

        AnsiString speechRomsPath = AnsiString(emulator.cwd) + AnsiString(romsFolder)
                + AnsiString(speechRomsFolder);
        strcpy(emulator.ROMSPEECHPATH, speechRomsPath.c_str());

        strcpy(emulator.machinename, "EightyOne");
        *(emulator.romcartridgefilepath) = '\0';

        zx97.bankswitch=0;
        zx97.protect08=1;
        zx97.protectab=0;
        zx97.protectb0=1;
        zx97.protectb115=0;
        zx97.saveram=0;

        tv.AdvancedEffects=0;
        tv.DotCrawl=0;
        tv.Interlaced=0;
        tv.DisableAdvanced=0;

        spectrum.kbissue=SPECKBISS3;
        machine.driveatype=DRIVE3INCHSS;
        machine.drivebtype=DRIVE3INCHSS;
        machine.driveaimg[0]='\0';
        machine.drivebimg[0]='\0';
        machine.drivebusy=-1;
        machine.HDType=HDNONE;
        machine.divIDEJumperEClosed=0;
        machine.zxcfUploadJumperClosed=192;
        machine.simpleIdeRomEnabled=0;

        spectrum.usource=0;
        spectrum.kmouse=0;
        spectrum.MFVersion=MFNONE;
        spectrum.spectraMode=0x00;
        spectrum.spectraColourSwitchOn = 0;
        spectrum.specdrum=0;
        spectrum.spectrum128Keypad=0;

        mouse.x=0;
        mouse.y=0;
        mouse.buttons=0;
        mouse.lastx=0;
        mouse.lasty=0;
}

void LoadMachineRoms()
{
        strcpy(emulator.ROM80, "zx80.rom");
        strcpy(emulator.ROM81, "zx81.edition3.rom");
        strcpy(emulator.ROMSP16, "spectrum48.rom");
        strcpy(emulator.ROMSP48, "spectrum48.rom");
        strcpy(emulator.ROMSPP, "spectrum48.rom");
        strcpy(emulator.ROMSP128, "spectrum128.rom");

        strcpy(emulator.ROMSPP2, "spectrum+2.rom");
        strcpy(emulator.ROMSPP2A, "spectrum+3.version4-1.rom");
        strcpy(emulator.ROMSPP3, "spectrum+3.version4-1.rom");

        strcpy(emulator.ROMTS1000, "zx81.edition3.rom");
        strcpy(emulator.ROMTS1500, "ts1500.rom");
        strcpy(emulator.ROMTC2048, "tc2048.rom");
        strcpy(emulator.ROMTC2068, "ts2068.rom");
        strcpy(emulator.ROMTS2068, "ts2068.rom");

        strcpy(emulator.ROMLAMBDA, "lambda8300colour.rom");
        strcpy(emulator.ROMR470, "ringo470.rom");
        strcpy(emulator.ROMTK85, "tk85.rom");
        strcpy(emulator.ROMACE, "jupiterace.rom");

        strcpy(emulator.ROM97LE, "zx97.rom");
}

AnsiString PrependFolder(AnsiString folder, char* romFile)
{
        AnsiString path = romFile;

        if (path.Pos("\\") == 0)
        {
                path = folder + path;
        }

        return path;
}

