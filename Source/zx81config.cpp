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

extern bool GetVersionNumber(int& versionNumberMajor, int& versionNumberMinor, int& versionNumberPart3, int& versionNumberPart4);

void load_config(void)
{
        int versionNumberMajor;
        int versionNumberMinor;
        int versionNumberPart3;
        int versionNumberPart4;
        GetVersionNumber(versionNumberMajor, versionNumberMinor, versionNumberPart3, versionNumberPart4);
        
        emulator.emuid=EMUID;
        emulator.major=versionNumberMajor;
        emulator.minor=versionNumberMinor;
        emulator.testver=versionNumberPart3;

        emulator.machine=MACHINEZX81;
        
        zx81.shadowROM=0;
        zx81.RAM816k=1;
        machine.protectROM=1;
        zx81.RAMTOP=32767;
        zx81.ROMTOP=8191;
        zx81.truehires=HIRESWRX;
        emulator.inverse=0;
        zx81.extfont=0;
        emulator.frameskip=0;
        machine.aysound=0;
        machine.aytype=0;
        zx81.vsyncsound=0;
        emulator.beepersound=0;
        machine.ts2050=0;
        emulator.TZXin=0;
        emulator.TZXout=0;
        emulator.audioout=0;
        emulator.audioin=0;
        emulator.romcrc=-1;
        emulator.autoload=0;
        zx81.wobble=0;
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
        spectrum.intposition=0;
        machine.zxprinter=1;

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

        strcpy(emulator.ROM80, "zx80.rom");
        strcpy(emulator.ROM81, "zx81.edition3.rom");
        strcpy(emulator.ROMSP16, "spectrum48.rom");
        strcpy(emulator.ROMSP48, "spectrum48.rom");
        strcpy(emulator.ROMSPP, "spectrum48.rom");
        strcpy(emulator.ROMSP128, "spectrum128.rom");
        strcpy(emulator.ROMQL, "ql.js.rom");

        strcpy(emulator.ROMSPP2, "spectrum+2.rom");
        strcpy(emulator.ROMSPP2A, "spectrum+3.version4-1.rom");
        strcpy(emulator.ROMSPP3, "spectrum+3.version4-1.rom");

        strcpy(emulator.ROMTS1000, "zx81.edition3.rom");
        strcpy(emulator.ROMTS1500, "ts1500.rom");
        strcpy(emulator.ROMTC2048, "tc2048.rom");
        strcpy(emulator.ROMTS2068, "ts2068.rom");

        strcpy(emulator.ROMLAMBDA, "lambda8300.rom");
        strcpy(emulator.ROMR470, "ringo470.rom");
        strcpy(emulator.ROMTK85, "tk85.rom");
        strcpy(emulator.ROMACE, "jupiterace.rom");

        strcpy(emulator.ROM97LE, "zx97.rom");

        strcpy(emulator.ROMSPP3E, "simple+3e8bit(sm8en3eE).rom");
        strcpy(emulator.ROMSPP3ECF, "zxcf+3e(zxcen3eE).rom");
        strcpy(emulator.ROMZXCF, "zxcflba.rom");
        strcpy(emulator.ROMZX8BIT, "zx8blbs.rom");
        strcpy(emulator.ROMZX16BIT, "zxidelbs.rom");
        strcpy(emulator.ROMPLUSD,"plusd.rom");
        strcpy(emulator.ROMDISCIPLE,"disciple.rom");
        strcpy(emulator.ROMOPUSD,"opusdiscovery.rom");
        strcpy(emulator.ROMBETADISC,"trdos.rom");
        strcpy(emulator.ROMUSPEECH,"uspeech.rom");
        strcpy(emulator.ROMUSOURCE,"usource.rom");

        strcpy(emulator.machinename, "EightyOne");

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

        spectrum.uspeech=0;
        spectrum.usource=0;
        spectrum.kbissue=SPECKBISS3;
        spectrum.driveatype=DRIVE3INCHSS;
        spectrum.drivebtype=DRIVE3INCHSS;
        spectrum.driveaimg[0]='\0';
        spectrum.drivebimg[0]='\0';
        spectrum.drivebusy=-1;
        spectrum.kmouse=0;
        spectrum.HDType=HDNONE;
        spectrum.WriteProtectJumper=0;
        spectrum.MFVersion=MFNONE;
        spectrum.spectraMode=0x00;
        spectrum.spectraColourSwitchOn = 0;

        mouse.x=0;
        mouse.y=0;
        mouse.buttons=0;
        mouse.lastx=0;
        mouse.lasty=0;
}

