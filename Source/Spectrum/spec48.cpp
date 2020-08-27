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
 * spec48.c
 *
 */
#include <vcl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>

#include "zx81.h"
#include "z80\z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "serialport.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "tzxman.h"
#include "interface1.h"
#include "spec48.h"
#include "floppy.h"
#include "wd1770.h"
#include "ide.h"
#include "parallel.h"
#include "sp0256.h"
#include "rzx.h"
#include "RomCartridge\IF2RomCartridge.h"
#include "Spectra\Spectra.h"
#include "LiveMemoryWindow_.h"

#define VBLANKCOLOUR (0*16)

extern "C"
{
        int CRC32Block(char *memory, int romlen);
        void P3DriveMachineHasInitialised(void);
        void MidiClockTick(int);
}

extern void InitialiseSpectra();
extern void FetchSpectraAttributeFileBytes(int y, int column, int* attr1, int* attr2);
extern void DetermineSpectraInkPaper(int attr, int attr2, int flashSwap, int* inkLeft, int* inkRight, int* paperLeft, int* paperRight);
extern int DetermineSpectraBorderColour(int Data, int flashSwap);
extern void SpectraRAMWrite(int Address, BYTE Data);
extern void DetermineSpectraDisplayBank();
extern BYTE SpectraRAMRead(int bankOffset);

extern bool directMemoryAccess;
extern int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
extern int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
extern int lastMemoryReadValueLo, lastMemoryWriteValueLo;
extern int lastMemoryReadValueHi, lastMemoryWriteValueHi;

extern void add_blank(SCANLINE *line, int borrow, BYTE colour);

extern void LogOutAccess(int address, BYTE data);
extern void LogInAccess(int address, BYTE data);
extern void ResetLastIOAccesses();
extern void DebugUpdate(void);
extern void add_blank(SCANLINE *line, int tstates, BYTE colour);
extern void LoadDock(char *filename);

extern long noise;
extern int SelectAYReg;
extern int zx81_stop;
extern BYTE ZXKeyboard[8];

static BYTE ReadPort(int Address, int *tstates);

const BYTE idleDataBus = 0xFF;

BYTE SpecMem[(128+64+16)*1024];  //enough memory for 64k ROM + 128k RAM + extra 16k on SE
BYTE TimexMem[(64+64)*1024];  // Timex has two more blocks of 64k each
BYTE TimexWritable[16];
BYTE divIDEMem[5*8192];       // divIDE has 8k of FlashRAM and 32k of RAM
BYTE ZXCFMem[64*16384];  // ZXCF has 1024k arranged as 64 x 16k pages
BYTE MFMem[16*1024]; // Multiface - 1 x 16k page
BYTE PlusDMem[16*1024]; //Disciple/PlusD RAM - 8k RAM, 8k ROM
BYTE uSpeechMem[16*1024];

int divIDEPaged, divIDEPage0, divIDEPage1, divIDEPort, divIDEMapRam;
int divIDEPage0WP, divIDEPage1WP;

int MFActive=0, MFLockout=0;
int ZXCFPort=0;

int uSpeechPaged=0;

int SPECMICState, SPECTopBorder, SPECLeftBorder, SPECBorder=7, FloatingBus;
int SPECBlk[4], SPECVideoBank, SPECBankEnable, ContendCounter;
int SPECKb, SPECNextBorder=7;
int SPECVSync=0;
BYTE SPECLast7ffd, SPECLast1ffd;
extern void ZXPrinterWritePort(unsigned char Data);
extern unsigned char ZXPrinterReadPort(void);
extern void TZXWriteByte(unsigned char Byte);
extern int TZXEventCounter;
int InteruptPosition;
int SPECFlashLoading=0;
int fts=0;
int flash=0;

extern unsigned short RZXCounter;
extern RZX_INFO rzx;

int TIMEXByte, TIMEXMode, TIMEXColour;
int TIMEXPage, TIMEXBank;

int PlusDPaged, PlusDMemSwap;
extern wd1770_drive PlusDDrives[], *PlusDCur;

int USEFDC765DLL;
extern "C" void LoadFDC765DLL(void);

BYTE ContendArray[80000];

RZX_EMULINFO  RZXemulinfo =
{
   "EightyOne",
   0,
   0,
   NULL, 0, 0
} ;

int RZXError;

void spec48_LoadRZX(char *FileName)
{
        rzx_playback(FileName);
        RZXCounter=0;
}

extern void spec_load_z80(char *fname);

rzx_u32 RZXcallback(int Msg, void *data)
{
        int a;
        //int b,c,d;

        switch(Msg)
        {
        case RZXMSG_CREATOR:
                break;
        case RZXMSG_LOADSNAP:
                spec_load_z80( ((RZX_SNAPINFO *) data)->filename);
                break;
        case RZXMSG_IRBNOTIFY:
                a=((RZX_IRBINFO *) data)->framecount;
                //b=((RZX_IRBINFO *) data)->tstates;
                //c=((RZX_IRBINFO *) data)->options;
                //d=0;

                fts=a;
                RZXCounter=0;
                break;
        default:
                break;
        }

        return(0);
}

extern bool GetVersionNumber(int& versionNumberMajor, int& versionNumberMinor, int& versionNumberPart3, int& versionNumberPart4);

void SpecStartUp(void)
{
        memset(&PlusDDrives[0], 0, sizeof(wd1770_drive));
        memset(&PlusDDrives[1], 0, sizeof(wd1770_drive));
        LoadFDC765DLL();

        int versionNumberMajor;
        int versionNumberMinor;
        int versionNumberPart3;
        int versionNumberPart4;
        GetVersionNumber(versionNumberMajor, versionNumberMinor, versionNumberPart3, versionNumberPart4);

        RZXemulinfo.ver_major = versionNumberMajor;
        RZXemulinfo.ver_minor = versionNumberMinor;
        RZXError=rzx_init(&RZXemulinfo, RZXcallback);
}

int SPECShrink(int b)
{
        int a=0,i;

        for(i=0;i<8;i++)
        {
                //a = (a<<1) | (b&49152)?1:0;
                a=(a<<1) | (((b&32768)|((b<<1)&32768))>>15);
                b<<=2;
        }
        return(a);
}

static void divIDEPage(void)
{
        if (divIDEPaged & 128)
        {
                divIDEPage0=0;
                divIDEPage1=1+(divIDEPort&3);
                divIDEPage0WP=spectrum.WriteProtectJumper;
                divIDEPage1WP=0;
        }
        else if (divIDEPaged & 1)
        {
                if (divIDEMapRam)
                {
                        divIDEPage0=4;
                        divIDEPage1=1+(divIDEPort&3);
                        divIDEPage0WP=1;
                        divIDEPage1WP=0;
                        if (divIDEPage1==4) divIDEPage1WP=1;
                }
                else
                {
                        divIDEPage0=0;
                        divIDEPage1=1+(divIDEPort&3);
                        divIDEPage0WP=1;
                        divIDEPage1WP=0;
                }
        }
}

extern "C"
{
void spec48_nmi(void)
{
        uSpeechPaged=0;

        if (spectrum.MFVersion)
        {
                MFActive=1;
                MFLockout=0;
        }
        else if (spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYBETA)
        {
                PlusDPaged=1;
        }

        int nonHaltedWaitStates;
        int haltedWaitStates;
        z80_nmi(0, &nonHaltedWaitStates, &haltedWaitStates);
}
}

void spec48_exit(void)
{
        floppy_shutdown();
}

void spec48_reset(void)
{
        SPECBlk[0]=0;
        SPECBlk[1]=5+4;
        SPECBlk[2]=2+4;
        SPECBlk[3]=0+4;

        SPECVideoBank=9;
        SPECLast7ffd=0;
        SPECLast1ffd=0;
        if (spectrum.divIDEVersion==2) SPECLast7ffd=16;

        if (spectrum.machine==SPECCYSE) SPECBlk[2]=8+4;
        if (spectrum.machine==SPECCYTS2068) SPECBankEnable=0;
        else if (spectrum.machine>=SPECCY128) SPECBankEnable=1;
        else SPECBankEnable=0;

        MFActive=0;
        MFLockout=0;

        TIMEXByte=0;
        TIMEXMode=0;
        TIMEXColour=0;
        TIMEXBank=0;
        TIMEXPage=0;

        uSpeechPaged=0;

        ZXCFPort=128;
        if (spectrum.HDType==HDZXCF)
        {
                if (spectrum.UploadJumper) ZXCFPort=192;
                else ZXCFPort=0;
        }

        divIDEPaged=0;
        divIDEPage0=0;
        divIDEPage1=1;
        divIDEPort=0;
        divIDEMapRam=0;
        divIDEPage0WP=0;
        divIDEPage1WP=0;

        MFActive=0;
        MFLockout=0;

        PlusDPaged=PlusDMemSwap=0;
        PlusDCur= &PlusDDrives[0];
        PlusDCur->side=0;
        if (spectrum.floppytype==FLOPPYBETA && spectrum.autoboot) PlusDPaged=1;

        z80_reset();
        d8255_reset();
        d8251reset();
        z80_reset();
        floppy_init();
        ATA_Reset();
        if (spectrum.HDType==HDPLUS3E) ATA_SetMode(ATA_MODE_8BIT);
        if (spectrum.HDType==HDDIVIDE) ATA_SetMode(ATA_MODE_16BIT);
        if (spectrum.HDType==HDZXCF) ATA_SetMode(ATA_MODE_16BIT);
        if (spectrum.HDType==HDPITERSCF) ATA_SetMode(ATA_MODE_16BIT);
        if (spectrum.HDType==HDPITERS16B) ATA_SetMode(ATA_MODE_16BIT_WRSWAP);
        if (spectrum.HDType==HDPITERS8B) ATA_SetMode(ATA_MODE_8BIT);
        mouse.buttons=255;
}

void spec48_initialise(void)
{
        int i, j, romlen, pos, delay;
        z80_init();
        tStatesCount = 0;
        
        directMemoryAccess = false;
        ResetLastIOAccesses();
        InitialiseRomCartridge();
        InitialiseSpectra();

        for(i=0;i<sizeof(SpecMem);i++) SpecMem[i]=random(256);
        for(i=0;i<sizeof(TimexMem);i++) TimexMem[i]=255;
        for(i=0;i<sizeof(PlusDMem);i++) PlusDMem[i]=255;

        for(i=0;i<sizeof(TimexWritable);i++)
                TimexWritable[i]=(spectrum.machine==SPECCYSE) ? 1:0;

        if (spectrum.floppytype==FLOPPYDISCIPLE)
        {
                romlen=memory_load(zx81.ROMDISCIPLE,0,16384);
                memcpy(PlusDMem, memory, romlen);
        }

        if (spectrum.floppytype==FLOPPYPLUSD)
        {
                romlen=memory_load(zx81.ROMPLUSD,0,16384);
                memcpy(PlusDMem, memory, romlen);
        }

        if (spectrum.floppytype==FLOPPYOPUSD)
        {
                romlen=memory_load(zx81.ROMOPUSD,0,16384);
                memcpy(PlusDMem, memory, romlen);
        }

        if (spectrum.floppytype==FLOPPYBETA)
        {
                romlen=memory_load(zx81.ROMBETADISC,0,16384);
                memcpy(PlusDMem, memory, romlen);
        }

        if (spectrum.uspeech)
        {
                romlen=memory_load(zx81.ROMUSPEECH,0,16384);
                memcpy(uSpeechMem, memory, romlen);
        }

        if (spectrum.floppytype==FLOPPYIF1)
        {
                if (IF1->RomEdition->Text == "Edition 2")
                        romlen=memory_load("specif1.edition2.rom",0,65536);
                else
                        romlen=memory_load("specif1.edition1.rom",0,65536);

                memcpy(SpecMem+32768,memory,romlen);
                if (romlen<=8192) memcpy(SpecMem+32768+8192,memory,romlen);
        }

        romlen=memory_load(machine.CurRom, 0, 65536);
        zx81.romcrc=CRC32Block(memory,romlen);

        memcpy(SpecMem, memory, romlen);

        if (spectrum.machine==SPECCYTS2068)
                memcpy(TimexMem+65536, memory+16384, 8192);

        if (spectrum.MFVersion == MF128)
        {
                romlen=memory_load("mf128.rom",0,65536);
                memcpy(MFMem,memory,romlen);
        }

        if (spectrum.MFVersion == MFPLUS3)
        {
                romlen=memory_load("mfp3.rom",0,65536);
                memcpy(MFMem,memory,romlen);
        }


        if (strlen(zx81.ROMDock)) LoadDock(zx81.ROMDock);

        SPECTopBorder= (zx81.NTSC) ? 32:56;
        SPECLeftBorder=1+37*2;

        InteruptPosition=((SPECLeftBorder/2)+SPECTopBorder*machine.tperscanline)-machine.intposition;
        if (InteruptPosition<0) InteruptPosition+=machine.tperframe;

        for(i=0;i<79999;i++) ContendArray[i]=0;

        pos=machine.intposition-1;//((SPECLeftBorder/2)+SPECTopBorder*zx81.tperscanline)-8;

        for(i=0;i<192;i++)
        {
                delay=6;
                for(j=0;j<128;j++)
                {
                        if (delay>0) ContendArray[pos+j]= delay;
                        if (--delay==-2) delay=6;
                }

                pos += machine.tperscanline;
        }

        spec48_reset();
        P3DriveMachineHasInitialised();
}

void SPECLoadCheck(void)
{
        static int OldTStates=0, LoopCount=0, FailCount=0;
        static BYTE *OldReg;
        static processor PrevZ80;
        static BYTE *Reg;
        static int Diff, TDiff;
        int DiffA, DiffB, DiffC, DiffD, DiffE, DiffH, DiffL, DiffCount;

        // First check to see if we're already flash loading, if not
        // establish whether this is a loader

        // Calculate Number of T States since we were last called.
        // if we've started a new frame, this could be negative
        // so take that into account.

        TDiff=frametstates-OldTStates;
        if (TDiff<0) TDiff += machine.tperframe;

        // if it was more than 96 T States, or PC isn't the same
        // as before we're not loading

        if (TDiff>96 || z80.pc.w!=PrevZ80.pc.w || z80.iff1)
        {
                LoopCount=0;
                FailCount++;
                PrevZ80=z80;
                OldTStates=frametstates;
        }
        else
        {

	        // Find out exactly which register has changes and if it has
                // changed by exactly 1, make the pointer Reg point to it.

        	DiffA = z80.af.b.h - PrevZ80.af.b.h;
        	DiffB = z80.bc.b.h - PrevZ80.bc.b.h;
        	DiffC = z80.bc.b.l - PrevZ80.bc.b.l;
        	DiffD = z80.de.b.h - PrevZ80.de.b.h;
        	DiffE = z80.de.b.l - PrevZ80.de.b.l;
        	DiffH = z80.hl.b.h - PrevZ80.hl.b.h;
        	DiffL = z80.hl.b.l - PrevZ80.hl.b.l;

                Diff=abs(DiffA)+abs(DiffB)+abs(DiffC)+
                        abs(DiffD)+abs(DiffE)+abs(DiffH)+abs(DiffL);

                if (Diff!=1)
                {
                	OldReg=Reg;
                	PrevZ80=z80;
                	OldTStates=frametstates;
                	LoopCount=0;
                        FailCount++;
        	}
                else
                {
                        DiffCount=0;

	                if (abs(DiffA)==1) { Reg=&(z80.af.b.h); Diff=DiffA; DiffCount++; }
        	        if (abs(DiffB)==1) { Reg=&(z80.bc.b.h); Diff=DiffB; DiffCount++; }
        	        if (abs(DiffC)==1) { Reg=&(z80.bc.b.l); Diff=DiffC; DiffCount++; }
        	        if (abs(DiffD)==1) { Reg=&(z80.de.b.h); Diff=DiffD; DiffCount++; }
        	        if (abs(DiffE)==1) { Reg=&(z80.de.b.l); Diff=DiffE; DiffCount++; }
        	        if (abs(DiffH)==1) { Reg=&(z80.hl.b.h); Diff=DiffH; DiffCount++; }
        	        if (abs(DiffL)==1) { Reg=&(z80.hl.b.l); Diff=DiffL; DiffCount++; }

	                // If the register which has changed is different to
        	        // last time, we're not loading

	                if (Reg!=OldReg || DiffCount!=1)
        	        {
                	        OldReg=Reg;
                	        PrevZ80=z80;
                	        OldTStates=frametstates;
                	        LoopCount=0;
                                FailCount++;
        	        }
                }
        }

        // If we got here 8 time in a row, we can start loading
        if (++LoopCount >=8)
        {
                WavStart();
                if (WavFlashLoad()) SPECFlashLoading=1;
                FailCount=0;
        }

        OldReg=Reg;
       	PrevZ80=z80;
        OldTStates=frametstates;
        return;
}

void spec48_WriteByte(int Address, int Data)
{
        register int SpecSETemp;

        LiveMemoryWindow->Write(Address);

        if (Address>=32768 && spectrum.machine==SPECCY16) return;

        if (Address<16384)
        {
                if ((Address==0x1000) && spectrum.uspeech) SP0256_Write(Data);

                if (PlusDPaged)
                {
                        Address ^= PlusDMemSwap;
                        if (spectrum.floppytype == FLOPPYBETA) return;
                        if (Address<8192) return;
                        PlusDMem[Address]=Data;

                        if (spectrum.floppytype==FLOPPYOPUSD)
                        {
                                if (Address>=14336) ;           // Undefined on Opus
                                else if (Address>=12288)
                                        OpusD6821Access(Address, Data, 1);              // 6821 PIA
                                else if (Address>=10240)        // WD1770 FDC
                                        switch(Address&3)
                                        {
                                        case 0: floppy_write_cmdreg(Data); break;
                                        case 1: floppy_write_trackreg(Data); break;
                                        case 2: floppy_write_secreg(Data); break;
                                        case 3: floppy_write_datareg(Data); break;
                                        }
                        }

                        return;
                }

                if (MFActive && (Address>8191))
                {
                        MFMem[Address]=Data;
                        return;
                }

                if (!(ZXCFPort&128))
                {
                        if (!(ZXCFPort&64)) return;
                        ZXCFMem[(((ZXCFPort&63)&(spectrum.ZXCFRAMSize-1))<<14)+Address]=Data;
                        return;
                }

                if (divIDEPaged)
                {
                        if (Address<8192)
                        {
                                if (!divIDEPage0WP)
                                        divIDEMem[Address+(divIDEPage0*8192)]=Data;
                        }
                        else
                        {
                                if (!divIDEPage1WP)
                                        divIDEMem[(Address&8191)+(divIDEPage1*8192)]=Data;
                        }
                        return;
                }  
                
                // The ROM cartridge socket does not differentiate between a memory read or write,
                // so all accesses are treated as reads
                if ((zx81.romCartridge != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
                {
                        BYTE data;
                        if (ReadRomCartridge(Address, (BYTE*)&data))
                        {
                                return;
                        }
                }

                if ((SPECBlk[0]<4) && (zx81.protectROM) && (!TIMEXPage) ) return;
        }

        SpecSETemp=TIMEXPage;
        if (spectrum.machine==SPECCYSE)
        {
                if ((SPECBlk[Address>>14]&1) && (Address>=49152)) TIMEXPage=0;
        }


        if ((1<<(Address>>13)) & TIMEXPage)
        {
                if (TimexWritable[(1<<(Address>>13))+8*TIMEXBank])
                        TimexMem[65536*TIMEXBank+Address]=Data;
        }
        else
        {
                RAMWrite(SPECBlk[Address>>14], Address, Data);
                if (zx81.colour == COLOURSPECTRA)
                {
                        SpectraRAMWrite(Address, Data);
                }
        }

        TIMEXPage=SpecSETemp;
        noise = (noise<<8) | Data;
}

// Write to memory without accidentally invoking the ZXC ROM cartridge paging mechanism
void spec48_setbyte(int Address, int Data)
{
        directMemoryAccess = true;
        spec48_WriteByte(Address, Data);
        directMemoryAccess = false;
}

void spec48_writebyte(int Address, int Data)
{

        lastMemoryWriteAddrLo = lastMemoryWriteAddrHi;
        lastMemoryWriteAddrHi = Address;

        lastMemoryWriteValueLo = lastMemoryWriteValueHi;
        lastMemoryWriteValueHi = Data;

        spec48_WriteByte(Address, Data);
}

BYTE spec48_ReadByte(int Address)
{
        int data;
        register int SpecSETemp;

        LiveMemoryWindow->Read(Address);

        if (Address<16384)
        {
                if ((Address==0x1000) && spectrum.uspeech)
                {
                        data=SP0256_Busy();
                        noise = (noise<<8) | data;
                        return(data);
                }

                if (PlusDPaged)
                {
                        Address ^= PlusDMemSwap;
                        data=PlusDMem[Address];

                        if (spectrum.floppytype==FLOPPYOPUSD)
                        {
                                if (Address>=14336) data=255;   // Undefined on Opus
                                else if (Address>=12288)        // 6821 PIA
                                        data=OpusD6821Access(Address, 0, 0);
                                else if (Address>=10240)        // WD1770 FDC
                                        switch(Address&3)
                                        {
                                        case 0: data=floppy_read_statusreg(); break;
                                        case 1: data=floppy_read_trackreg(); break;
                                        case 2: data=floppy_read_secreg(); break;
                                        case 3: data=floppy_read_datareg(); break;
                                        }
                        }

                        noise = (noise<<8) | data;
                        return(data);
                }

                if (MFActive)
                {
                        data=MFMem[Address];
                        noise = (noise<<8) | data;
                        return(data);
                }

                if (!(ZXCFPort&128))
                {
                        data=ZXCFMem[(((ZXCFPort&63)&(spectrum.ZXCFRAMSize-1))<<14)+Address];
                        noise = (noise<<8) | data;
                        return(data);
                }

                if (divIDEPaged)
                {
                        if (Address<8192) data=divIDEMem[Address+(divIDEPage0*8192)];
                        else data=divIDEMem[(Address&8191)+(divIDEPage1*8192)];
                        noise = (noise<<8) | data;
                        return(data);
                }

                if (uSpeechPaged)
                {
                        data=uSpeechMem[Address];
                        noise = (noise<<8) | data;
                        return(data);
                }

                if ((zx81.romCartridge != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
                {
                        if (ReadRomCartridge(Address, (BYTE*)&data))
                        {
                                return data;
                        }
                }
        }

        SpecSETemp=TIMEXPage;
        if (spectrum.machine==SPECCYSE)
        {
                if ((SPECBlk[Address>>14]&1) && (Address>=49152)) TIMEXPage=0;
        }

        if ((1<<(Address>>13)) & TIMEXPage)
                data=TimexMem[65536*TIMEXBank+Address];
        else
                data=RAMRead(SPECBlk[Address>>14], Address);

        TIMEXPage=SpecSETemp;

        noise = (noise<<8) | data;
        return(data);
}

// Read from memory without accidentally invoking the ZXC ROM cartridge paging mechanism
// Called by debugger routines
BYTE spec48_getbyte(int Address)
{
        directMemoryAccess = true;
        BYTE b = spec48_ReadByte(Address);
        directMemoryAccess = false;

        return b;
}

// Called by emulated program
BYTE spec48_readbyte(int Address)
{
        lastMemoryReadAddrLo = lastMemoryReadAddrHi;
        lastMemoryReadAddrHi = Address;

        BYTE byte = spec48_ReadByte(Address);

        lastMemoryReadValueLo = lastMemoryReadValueHi;
        lastMemoryReadValueHi = byte;

        return byte;
}

// Called by Z80 instruction operand fetches
BYTE spec48_readoperandbyte(int Address)
{
        return spec48_ReadByte(Address);
}

// Called by Z80 instruction opcode fetches
BYTE spec48_opcode_fetch(int Address)
{
        return(spec48_ReadByte(Address));
}

void spec48_writeport(int Address, int Data, int *tstates)
{
        LogOutAccess(Address, Data);

        if (spectrum.HDType==HDDIVIDE && ((Address&0xa3)==0xa3))
        {
                if (Address&64) // Control Register
                {
                        divIDEPort=Data;
                        divIDEPaged=(Data&128) | (divIDEPaged&127);
                        if (Data&64) divIDEMapRam=1;
                        divIDEPage();
                }
                else    // IDE Interface
                        ATA_WriteRegister((Address>>2)&7,Data);
        }

        if ((spectrum.HDType==HDPITERSCF || spectrum.HDType==HDPITERS8B) && ((Address&0x3b)==0x2b))
                ATA_WriteRegister(((Address>>2)&1) | ((Address>>5)&6), Data);

        if ((spectrum.HDType==HDPITERS16B) && ((Address&0xe9)==0x69))
                ATA_WriteRegister(((Address>>4)&1) | (Address&6), Data);

        switch(Address&255)
        {
        case 0x1b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) floppy_write_cmdreg(Data);
                break;

        case 0x1f:
                if (spectrum.floppytype==FLOPPYDISCIPLE) floppy_set_motor(Data);
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) floppy_write_cmdreg(Data);
                break;

        case 0x3f:
                if (spectrum.MFVersion==MF128) MFLockout=1;
                if (zx81.aytype==AY_TYPE_FULLER) SelectAYReg=Data&15;
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) floppy_write_trackreg(Data);
        case 0x5b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) floppy_write_trackreg(Data);
                break;

        case 0x5f:
                if (zx81.aytype==AY_TYPE_FULLER) sound_ay_write(SelectAYReg, Data);
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) floppy_write_secreg(Data);
                break;

        case 0x73:
                if (zx81.ts2050) d8251writeDATA(Data);
                break;
        case 0x77:
                if (zx81.ts2050) d8251writeCTRL(Data);
                break;

        case 0x7b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) PlusDMemSwap=0x2000;
                break;

        case 0x7f:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) floppy_write_datareg(Data);
                break;

        case 0x9b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) floppy_write_secreg(Data);
                break;

        case 0xbb:
                if (spectrum.floppytype==FLOPPYDISCIPLE) PlusDPaged=0;
                break;

        case 0xbf:
                if (spectrum.HDType==HDZXCF)
                {
                        int a=(Address>>8)&255;

                        switch(a)
                        {
                        case 0x00:
                        case 0x01:
                        case 0x02:
                        case 0x03:
                        case 0x04:
                        case 0x05:
                        case 0x06:
                        case 0x07:
                                ATA_WriteRegister(a,Data);
                                break;

                        case 0x08:
                                ATA_WriteRegister(0,Data);
                                break;
                        case 0x09:
                                ATA_WriteRegister(8,Data);
                                break;

                        case 0x10:
                                ZXCFPort=Data;
                                break;

                        default:
                                break;
                        }
                }

                break;

        case 0xc7:
                d8255_write(D8255PRTA,Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB,Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC,Data);
                break;

        case 0xdb:
                if (spectrum.floppytype==FLOPPYDISCIPLE) floppy_write_datareg(Data);
                break;

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) SelectAYReg=Data;
                break;

        case 0xdf:
                switch(Address>>8)
                {
                case 0x7f:
                        // The SPECTRA IO port has priority over devices connected behind it, which it
                        // ensures by masking out the IORQ line
                        if ((zx81.colour == COLOURSPECTRA) && zx81.spectraColourSwitchOn)
                        {
                                zx81.spectraMode = Data;
                                DetermineSpectraDisplayBank();
                                break;
                        }
                default:
                        if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data);
                        break;
                }
        case 0xe3:
                if (spectrum.floppytype==FLOPPYPLUSD) floppy_write_cmdreg(Data);
                break;

        case 0xe7:
                if (spectrum.floppytype==FLOPPYPLUSD) PlusDPaged=0;
                if (spectrum.floppytype==FLOPPYIF1) IF1PortE7Write(Data,tstates);
                break;

        case 0xeb:
                if (spectrum.floppytype==FLOPPYPLUSD) floppy_write_trackreg(Data);
                break;

        case 0xef:
                if (spectrum.HDType==HDPLUS3E)
                {
                        int Addr=0;
                        if (Address&256) Addr |=1;
                        if (Address&4096) Addr |= 2;
                        if (Address&8192) Addr |= 4;

                        ATA_WriteRegister(Addr,Data);
                }
                if (spectrum.floppytype==FLOPPYIF1) IF1PortEFWrite(Data);
                if (spectrum.floppytype==FLOPPYPLUSD) floppy_set_motor(Data);
                break;

        case 0xf3:
                if (spectrum.floppytype==FLOPPYPLUSD) floppy_write_secreg(Data);
                break;

        case 0xf4:
                if (spectrum.machine==SPECCYTC2048
                        || spectrum.machine==SPECCYTS2068
                        || spectrum.machine==SPECCYSE)
                        TIMEXPage=Data;
                break;

        case 0xf5:
                if (zx81.aytype==AY_TYPE_TIMEX || spectrum.machine==SPECCYSE) SelectAYReg=Data;
                break;

        case 0xf6:
                if (zx81.aytype==AY_TYPE_TIMEX || spectrum.machine==SPECCYSE) sound_ay_write(SelectAYReg, Data);
                break;

        case 0xf7:
                if (spectrum.floppytype==FLOPPYIF1) IF1PortF7Write(Data);
                if (spectrum.floppytype==FLOPPYPLUSD) PrinterWriteData(Data);
                break;

        case 0xfb:
                if (spectrum.floppytype==FLOPPYPLUSD) floppy_write_datareg(Data);
                if (spectrum.floppytype==FLOPPYDISCIPLE) PrinterWriteData(Data);
                if (zx81.zxprinter) ZXPrinterWritePort(Data);
                break;
        case 0xfd:
                switch(Address>>8)
                {
                case 0x0f:
                        if ((zx81.machine == MACHINESPEC48) && (spectrum.machine >= SPECCYPLUS2A))
                        {
                                PrinterWriteData(Data);
                        }
                        break;

                case 0x3f:
                        if ((zx81.machine == MACHINESPEC48) && (spectrum.machine >= SPECCYPLUS2A))
                        {
                                floppy_write_datareg(Data);
                        }
                        break;

                case 0x7f:
                        if (!SPECBankEnable) break;
                        SPECLast7ffd=Data;
                        SPECBlk[0]= ((SPECLast1ffd>>1)&2) | ((Data>>4)&1);
                        SPECBlk[1]=9;
                        SPECBlk[2]=6;
                        SPECBlk[3]=4+(Data&7);
                        SPECVideoBank=(Data>>3)&1 ? 11:9;
                        if (spectrum.machine!=SPECCYSE) SPECBankEnable=!((Data>>5)&1);
                        if (zx81.colour == COLOURSPECTRA)
                        {
                                DetermineSpectraDisplayBank();
                        }
                        break;

                case 0x1f:
                        if ((zx81.machine == MACHINESPEC48) && (spectrum.machine >= SPECCYPLUS2A))
                        {
                                spectrum.drivebusy = (Data&8) ? 1:0;
                                floppy_set_motor(Data);
                                PrinterSetStrobe(Data&16);

                                if ((!SPECBankEnable) || spectrum.machine<=SPECCY128)
                                        break;
                                SPECLast1ffd=Data;
                                if (Data&1)
                                switch((Data>>1)&3)
                                {
                                case 0: SPECBlk[0]=4; SPECBlk[1]=5; SPECBlk[2]=6; SPECBlk[3]=7; break;
                                case 1: SPECBlk[0]=8; SPECBlk[1]=9; SPECBlk[2]=10; SPECBlk[3]=11; break;
                                case 2: SPECBlk[0]=8; SPECBlk[1]=9; SPECBlk[2]=10; SPECBlk[3]=7; break;
                                case 3: SPECBlk[0]=8; SPECBlk[1]=11; SPECBlk[2]=10; SPECBlk[3]=7; break;
                                }
                                else
                                {
                                        SPECBlk[0]= ((SPECLast7ffd>>4)&1) | ((Data>>1)&2);
                                        SPECBlk[3]=4+(SPECLast7ffd&7);
                                        SPECVideoBank=(SPECLast7ffd>>3)&1 ? 11:9;
                                }
                        }
                        break;
                case 0xff:
                        SelectAYReg=Data;
                        break;

                case 0xbf:
                        sound_ay_write(SelectAYReg, Data);
                        break;
                }
                break;

        case 0xff:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) floppy_set_motor(Data);

                if (spectrum.machine==SPECCYTC2048
                        || spectrum.machine==SPECCYTS2068
                        || spectrum.machine==SPECCYSE)
                {
                        TIMEXByte=Data;
                        TIMEXMode=Data&7;
                        TIMEXColour=(Data>>3)&7;
                        TIMEXBank=(Data>>7)&1;
                }
                break;

        default:
                if (!(Address&1))
                {
                        SPECMICState = Data&8;
                        if (zx81.vsyncsound) sound_beeper(Data&16);

                        if (zx81.colour == COLOURSPECTRA)
                        {
                                SPECNextBorder = DetermineSpectraBorderColour(Data, flash & 0x10);
                        }
                        else
                        {
                                SPECNextBorder = (Data & 7);
                        }

                        SPECKb = Data;
                }
        }
}

int spec48_contend(int Address, int states, int time)
{
        if (Address>=16384 && Address<=32768) time += ContendArray[ContendCounter+states+time];
        return(time);
}

int spec48_contendio(int Address, int states, int time)
{
        if (!(Address&1) || (Address>=16384 && Address<=32768))
                time += ContendArray[ContendCounter+states+time];
        return(time);

        time=0;
        if (Address&1)
        {
                if (Address>16384 && Address<32768)
                {
                        time++;
                        time += 1+ContendArray[ContendCounter+states+time];
                        time +=2;

                }
                else
                {
                        time+=4;
                }
        }
        else
        {
                if (Address>16384 && Address<32768)
                {
                        time += 1+ContendArray[ContendCounter+states+time];
                        time += 1+ContendArray[ContendCounter+states+time+time];
                        time += 2;
                }
                else
                {
                        time += 1+ContendArray[ContendCounter+states+time];
                        time += 1+ContendArray[ContendCounter+states+time];
                        time += 1+ContendArray[ContendCounter+states+time];
                        time += 1+ContendArray[ContendCounter+states+time];
                }
        }

        return(time);
}


BYTE spec48_readport(int Address, int *tstates)
{
        BYTE data = ReadPort(Address, tstates);
        LogInAccess(Address, data);

        return data;
}

BYTE ReadPort(int Address, int *tstates)
{
        int RZXPortVal;
        int data;
        //static int LastT=0;
        //int CurT;

        //CurT=frametstates-LastT;
        //if (CurT<0) CurT+=frametstates;
        //LastT=CurT;
        //if (CurT>1000) TZXStopPlaying();

        if (rzx.mode==RZX_PLAYBACK)
        {
                RZXPortVal = rzx_get_input();
                if (RZXPortVal>=0) return(RZXPortVal);
                //rzx_close();
        }

        if (spectrum.HDType==HDDIVIDE && ((Address&0xe3)==0xa3))
                return(ATA_ReadRegister((Address>>2)&7));

        if ((spectrum.HDType==HDPITERSCF || spectrum.HDType==HDPITERS8B) && ((Address&0x3b)==0x2b))
                return(ATA_ReadRegister(((Address>>2)&1) | ((Address>>5)&6)));

        if ((spectrum.HDType==HDPITERS16B) && ((Address&0xe9)==0x69))
                return(ATA_ReadRegister(((Address>>4)&1) | (Address&6)));

        switch(Address&255)
        {
        case 0x1b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) return(floppy_read_statusreg());
                break;

        case 0x1f:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) return(floppy_read_statusreg());
                if (spectrum.floppytype==FLOPPYDISCIPLE)
                        return(PrinterBusy()<<6);
                break;

        case 0x3f:
                switch(spectrum.MFVersion)
                {
                case MF128:
                        MFActive=0;
                        return(0xff);

                case MFPLUS3:
                        MFActive=1;
                        switch((Address>>8)&255)
                        {
                        case 0x7f: return(SPECLast7ffd);
                        case 0x1f: return(SPECLast1ffd);
                        default: return(0xff);
                        }
                default:
                        break;
                }
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) return(floppy_read_trackreg());
                break;

        case 0x5b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) return(floppy_read_trackreg());
                break;

        case 0x5f:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) return(floppy_read_secreg());
                break;

        case 0x73:
                if (zx81.ts2050) return(d8251readDATA());
                break;

        case 0x77:
                if (zx81.ts2050) return(d8251readCTRL());
                break;

        case 0x7b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) PlusDMemSwap=0;
                break;

        case 0x7f:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged) return(floppy_read_datareg());
                break;

        case 0x9b:
                if (spectrum.floppytype==FLOPPYDISCIPLE) return(floppy_read_secreg());
                break;

        case 0xbb:
                if (spectrum.floppytype==FLOPPYDISCIPLE) PlusDPaged=1;
                break;

        case 0xbf:
                if (spectrum.MFVersion!=MFNONE)
                {
                        switch(spectrum.MFVersion)
                        {
                        case MF128:
                                if (!MFLockout)
                                {
                                        MFActive=1;
                                        return((SPECLast7ffd&8)<<4);
                                }
                                return(0xff);

                        case MFPLUS3:
                                MFActive=0;
                                return(0xff);

                        default:
                                break;
                        }
                }

                if (spectrum.HDType==HDZXCF)
                {
                        int a=(Address>>8)&255;

                        switch(a)
                        {
                        case 0x00:
                        case 0x01:
                        case 0x02:
                        case 0x03:
                        case 0x04:
                        case 0x05:
                        case 0x06:
                        case 0x07: return(ATA_ReadRegister(a));

                        case 0x08: return(ATA_ReadRegister(0));
                        case 0x09: return(ATA_ReadRegister(8));
                        case 0x10: return(ZXCFPort);
                        default: break;
                        }
                }

                break;

        case 0xdb:
                if (spectrum.floppytype==FLOPPYDISCIPLE) return(floppy_read_datareg());
                break;

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE)
                        return(sound_ay_read(SelectAYReg));
                break;

        case 0xdf:
                switch(Address>>8)
                {
                case 0x7f:
                        // The SPECTRA IO port has priority over devices connected behind it, which it
                        // ensures by masking out the IORQ line
                        if ((zx81.colour == COLOURSPECTRA) && zx81.spectraColourSwitchOn)
                        {
                                return zx81.spectraMode;
                        }
                default:
                        if (spectrum.kmouse)
                        {
                                switch((Address>>8)&255)
                                {
                                case 0xfb: return(mouse.x & 255);
                                case 0xff: return(mouse.y & 255);
                                case 0xfa: return(mouse.buttons);
                                }
                        }
                        break;
                }
                break;
                
        case 0xe3:
                if (spectrum.floppytype==FLOPPYPLUSD) return(floppy_read_statusreg());
                break;

        case 0xe7:
                if (spectrum.floppytype==FLOPPYIF1) return(IF1PortE7Read(tstates));
                if (spectrum.floppytype==FLOPPYPLUSD) PlusDPaged=1;
                break;

        case 0xeb:
                if (spectrum.floppytype==FLOPPYPLUSD) return(floppy_read_trackreg());
                break;

        case 0xef:
                if (spectrum.floppytype==FLOPPYIF1) return(IF1PortEFRead());
                if (spectrum.HDType==HDPLUS3E)
                {
                        int Addr=0;
                        if (Address&256) Addr |=1;
                        if (Address&4096) Addr |= 2;
                        if (Address&8192) Addr |= 4;

                        return(ATA_ReadRegister(Addr));
                }
                break;

        case 0xf3:
                if (spectrum.floppytype==FLOPPYPLUSD) return(floppy_read_secreg());
                break;

        case 0xf4:
                if (spectrum.machine==SPECCYTC2048
                        || spectrum.machine==SPECCYTS2068
                        || spectrum.machine==SPECCYSE)
                        return(TIMEXPage);
                break;

        case 0xf6:
                if (zx81.aytype==AY_TYPE_TIMEX || spectrum.machine==SPECCYSE)
                        return(sound_ay_read(SelectAYReg));
                break;

        case 0xf7:
                if (spectrum.floppytype==FLOPPYIF1) return(IF1PortF7Read());
                if (spectrum.floppytype==FLOPPYPLUSD)
                        return(PrinterBusy()<<7);
                break;

        case 0xfb:
                if (spectrum.floppytype==FLOPPYPLUSD) return(floppy_read_datareg());
                if (zx81.zxprinter) return(ZXPrinterReadPort());
                break;

        case 0xfd:
                switch((Address>>8)&255)
                {
                case 0x0f:      return(PrinterBusy());
                case 0xff:      return(sound_ay_read(SelectAYReg));
                case 0x3f:
                                return(floppy_read_datareg());
                case 0x2f:
                                return(floppy_read_statusreg());
                }
                break;

        case 0xff:
                if (spectrum.floppytype==FLOPPYBETA && PlusDPaged)
                        return(floppy_get_state());
                if (spectrum.machine==SPECCYTC2048
                        || spectrum.machine==SPECCYTS2068
                        || spectrum.machine==SPECCYSE)
                        return(TIMEXByte);
                break;

        default:
                if (!(Address&1))
                {
                        BYTE keyb, data=0;
                        int i;

                        if ( !(((SPECKb&16) && spectrum.kbissue==SPECKBISS2)
                                || ((SPECKb&8) && spectrum.kbissue==SPECKBISS3))) data=64;

                        if ((spectrum.machine==SPECCYTC2048)
                                || (spectrum.machine==SPECCYTS2068)
                                || (spectrum.machine==SPECCYSE)) data=160;



                        SPECLoadCheck();
                        if (WavPlaying()) data = (data&~64) | (GetEarState() ? 64:0);

                        keyb=Address/256;
                        for(i=0; i<8; i++) if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];

                        //if (z80.pc.w==0x05f3)
                        //{
                        //        TZXStartPlaying();
                        //        if (TZXFlashLoad()) SPECFlashLoading=1;
                        //}

                        return(~data);
                }
                break;
        }

        if (spectrum.machine<=SPECCY128) return(FloatingBus);

        return(idleDataBus);
}

int spec48_do_scanline(SCANLINE *CurScanLine)
{
        int ts,i;
        static int ink, paper, ink2, paper2;
        static int Sy=0, loop=207;
        static int borrow=0;
        static int sts=0, chars=0, delay=0, IntDue=0;
        static int DrawingBorder=1, DCCount=0;
        static int BaseColour, PBaseColour;
        static int shift_register;
        static int clean_exit=1;
        int attr, attr2, b1, b2;
        int MaxScanLen;
        int PrevBit=0, PrevGhost=0;
        int scale= (tv.AdvancedEffects ? 2:1);
        int LastPC;
        int SpeedUp, SpeedUpCount;
        int InteruptTime;
        int shiftCount;

        int HSyncDuration = spectrum.machine >= SPECCY128 ? 28 : 24;

        SpeedUpCount=0;
        SpeedUp=(zx81.speedup*machine.tperscanline)/100;

        CurScanLine->scanline_len=0;

        if (clean_exit)
        {
                int bpaper = (zx81.colour != COLOURSPECTRA) ? paper*16 : paper;
                if (fts >= (machine.scanlines-4)*machine.tperscanline)
                {
                        bpaper=VBLANKCOLOUR;
                }

                add_blank(CurScanLine, borrow*scale,bpaper);
                PBaseColour=paper;
                sts=0;
                delay=SPECLeftBorder - borrow*2;
                chars=0;
        }

        if (fts<=0) IntDue=1;

        MaxScanLen = scale * zx81.single_step? 1:500;
        do
        {
                LastPC=z80.pc.w;

                if (!(TIMEXPage&1)
                        && (ZXCFPort&128)
                        && spectrum.HDType!=HDPITERSCF
                        && spectrum.HDType!=HDPITERS8B
                        && spectrum.HDType!=HDPITERS16B
                        && (    (SPECBlk[0]==0 && spectrum.machine<SPECCYTS2068)
                             || (SPECBlk[0]==1 && spectrum.machine>SPECCYTS2068 && spectrum.machine<SPECCYPLUS2A)
                             || (SPECBlk[0]==3 && spectrum.machine>SPECCYPLUS2) ))
                {
                        if (FlashSaveable() && !PlusDPaged && !RomCartridgePagedIn())
                        {
                                if (LastPC==0x04D0) z80.hl.w=0x0102;
                                if (LastPC==0x0525)
                                {
                                        WavRecordByte(z80.hl.b.l);
                                        z80.pc.w=0x052a;
                                }
                        }

                        if (spectrum.floppytype==FLOPPYDISCIPLE
                                && (LastPC==0x0008 || LastPC==0x0066 || LastPC==0x28E))
                                        PlusDPaged=1;

                        if (spectrum.floppytype==FLOPPYPLUSD
                                && (LastPC==0x0008  || LastPC==0x003A  || LastPC==0x0066))
                                        PlusDPaged=1;

                        if (spectrum.floppytype==FLOPPYBETA)
                        {
                                if (LastPC>=0x3d00  && LastPC<=0x3dff) PlusDPaged=1;
                                if (LastPC>=0x4000) PlusDPaged=0;
                        }

                        if (spectrum.uspeech && LastPC==56) uSpeechPaged = !uSpeechPaged;
                }


                ts=z80_do_opcode();

                WavClockTick(ts, SPECMICState);
                if (zx81.zxprinter) ZXPrinterClockTick(ts);
                PrinterClockTick(ts);
                MidiClockTick(ts);
                if (spectrum.floppytype==FLOPPYIF1) IF1ClockTick(ts);
                else if (spectrum.floppytype!=FLOPPYNONE) floppy_ClockTick(ts);

                if (LastPC==0x0) WavStop();

                i=70;
                while(SPECFlashLoading && FlashLoadable() && i)
                {
                        ts=z80_do_opcode();
                        WavClockTick(ts,0);
                        i--;
                }
                if (!WavPlaying()) SPECFlashLoading=0;

                if (spectrum.floppytype==FLOPPYIF1) 
                {
                        if ((LastPC==0x0008 || LastPC==0x1708)) SPECBlk[0]=2;
                        if (LastPC==0x0700)
                                SPECBlk[0]=(SPECLast7ffd>>4)&1;
                }

                if (spectrum.floppytype==FLOPPYOPUSD)
                {
                        if (LastPC==0x0008  || LastPC==0x0048  || LastPC==0x1708) PlusDPaged=1;
                        if (LastPC==0x1748) PlusDPaged=0;
                }


                if (spectrum.HDType==HDDIVIDE && !spectrum.UploadJumper)
                {
                        if (LastPC==0x00
                                || LastPC==0x08
                                || LastPC==0x38
                                || LastPC==0x66
                                || LastPC==0x04c6
                                || LastPC==0x0562
                                || (z80.pc.w>=0x3d00 && z80.pc.w<=0x3dff))
                        {
                                if ((spectrum.divIDEVersion==1)
                                        || ((spectrum.divIDEVersion==2)
                                                && (SPECLast7ffd&16)))
                                        divIDEPaged |= 1;
                                        divIDEPage();
                        }

                        if (LastPC>=0x1ff8 && LastPC<=0x1fff)
                        {
                                if ((spectrum.divIDEVersion==1)
                                        || ((spectrum.divIDEVersion==2)
                                                && (SPECLast7ffd&16)))
                                        divIDEPaged &= 254;
                                        divIDEPage();
                        }
                }

                if (!SpeedUpCount)
                {
                        if (fts>InteruptPosition && IntDue)
                        {
                                InteruptTime=(TIMEXByte&64)?0:z80_interrupt(0);
                                if (rzx.mode==RZX_PLAYBACK)
                                {
                                        //if (RZXCounter<=0) Sy=machine.scanlines;
                                        rzx_update(&RZXCounter);
                                }

                                if (InteruptTime && !WavInGroup()) WavStop();
                                ts+=InteruptTime;
                                if (++flash >32) flash=0;
                                DrawingBorder=1;
                                DCCount = (++DCCount)&3;
                                IntDue=0;
                                ContendCounter=(fts-InteruptPosition);
                                ContendCounter= (ContendCounter+1)&~3;
                        }

                        loop-=ts;
                        fts+=ts;
                        sts+=ts;
                        frametstates+=ts;
                        tStatesCount += ts;
                        ContendCounter+=ts;

                        ts*=2;
                        while(ts--)
                        {
                                int colour, altcolour;
                                delay--;

                                if (TIMEXMode&4) SPECBorder=8+((~TIMEXColour)&7);
                                else if (((CurScanLine->scanline_len-10)%16)==0)
                                        SPECBorder=SPECNextBorder;

                                if (!(Sy<SPECTopBorder || Sy>SPECTopBorder+191 || delay))
                                {
                                        if (chars>31)
                                        {
                                                delay=258;
                                                DrawingBorder=1;
                                                FloatingBus=255;
                                        }
                                        else
                                        {
                                                DrawingBorder=0;
                                                int y = Sy-SPECTopBorder;
                                                int area = (y & 0xC0);
                                                int line = ((y & 0x7) << 3);
                                                int row = ((y >> 3) & 0x7);
                                                int lineOffset = ((area | line | row) << 5);
                                                int cellOffset = lineOffset + chars;
                                                
                                                switch(TIMEXMode)
                                                {
                                                case 0:
                                                case 1:
                                                        if (zx81.colour != COLOURSPECTRA)
                                                        {
                                                                shift_register=RAMRead(SPECVideoBank, (TIMEXMode<<13)+cellOffset);
                                                                attr=RAMRead(SPECVideoBank, (TIMEXMode<<13)+6144+chars+((y>>3)<<5));
                                                        }
                                                        else
                                                        {
                                                                shift_register = SpectraRAMRead(cellOffset);
                                                                FetchSpectraAttributeFileBytes(y, chars, &attr, &attr2);
                                                                shiftCount = 0;
                                                        }
                                                        break;
                                                case 2:
                                                case 3:
                                                        attr=RAMRead(SPECVideoBank, 8192+cellOffset);
                                                        shift_register=RAMRead(SPECVideoBank, cellOffset);
                                                        break;
                                                case 4:
                                                case 5:
                                                case 6:
                                                case 7:
                                                        attr=(((~TIMEXColour)&7)<<3) | TIMEXColour | 64;
                                                        b1=RAMRead(SPECVideoBank, cellOffset);
                                                        b2=RAMRead(SPECVideoBank, 8192+cellOffset);

                                                        if (tv.AdvancedEffects)
                                                                shift_register=(b1<<8)|b2;
                                                        else
                                                                shift_register=SPECShrink((b1<<8)|b2);

                                                        break;
                                                }

                                                FloatingBus=attr;

                                                int flashSwap = (flash & 0x10);

                                                if (zx81.colour == COLOURSPECTRA)
                                                {
                                                        DetermineSpectraInkPaper(attr, attr2, flashSwap, &ink, &ink2, &paper, &paper2);
                                                        SPECNextBorder = DetermineSpectraBorderColour(SPECKb, flashSwap);
                                                }
                                                else
                                                {
                                                        int inkMask = 0x07;
                                                        int paperMask = 0x38;
                                                        int brightMask = 0x40;
                                                        int flashMask = 0x80;
                                                        int brightColour = 0x08;
                                                        
                                                        if ((attr &  flashMask) && flashSwap) shift_register = ~shift_register;
                                                        ink = (attr & inkMask);
                                                        paper = ((attr & paperMask) >> 3);
                                                        if (attr & brightMask) { ink += brightColour; paper += brightColour; }
                                                }

                                                chars++;
                                                //noise=0;
                                                noise=(noise<<8) | attr;
                                                delay=8;
                                        }
                                }

                                if (DrawingBorder)
                                    paper=paper2=SPECBorder;

                                i=(tv.AdvancedEffects && (TIMEXMode&4)) ? 2:1;

                                while(i--)
                                {

                                        if (tv.AdvancedEffects && (TIMEXMode&4))
                                                colour = ((shift_register&32768)?ink:paper) << 4;
                                        else if (zx81.colour != COLOURSPECTRA)
                                                colour = ((shift_register&128)?ink:paper) << 4;
                                        else
                                        {
                                                // SPECTRA
                                                if (shiftCount < 4)
                                                        colour = ((shift_register&128)?ink:paper);
                                                else
                                                        colour = ((shift_register&128)?ink2:paper2);
                                        }
                                        
                                        if (fts >= (machine.scanlines-4)*machine.tperscanline)
                                                colour=VBLANKCOLOUR;

                                        altcolour=colour;
                                        BaseColour=colour>>4;

                                        if (zx81.dirtydisplay)
                                        {
                                                if (PrevGhost) { colour|=4; PrevGhost=0; }
                                                if (BaseColour!=PrevBit &&
                                                        !( (BaseColour==0 && PrevBit==8)
                                                        ||(BaseColour==8 && PrevBit==0)))
                                                                { colour|=2; PrevGhost=1; }

                                                if (noise&1) colour|=1;
                                                noise>>=1;
                                                PrevBit= BaseColour;
                                        }
                                        if (tv.DotCrawl)
                                        {
                                                if ((BaseColour&7) == (PBaseColour&7)) PBaseColour=BaseColour;

                                                if (BaseColour!=PBaseColour)
                                                {
                                                        if (((Sy&3)==DCCount) || ((Sy&3)==((DCCount+1)&3)))
                                                                altcolour=(PBaseColour+1)<<4;
                                                        else    altcolour=(BaseColour+1)<<4;
                                                        //if (altcolour==128) altcolour--;
                                                }
                                        }

                                        if (CurScanLine->scanline_len >= ((machine.tperscanline-HSyncDuration)*2*scale))
                                        {
                                                if (tv.AdvancedEffects && !(TIMEXMode&4))
                                                        CurScanLine->scanline[CurScanLine->scanline_len++]=VBLANKCOLOUR;
                                                CurScanLine->scanline[CurScanLine->scanline_len++]=VBLANKCOLOUR;
                                        }
                                        else
                                        {
                                                if (tv.AdvancedEffects && !(TIMEXMode&4))
                                                        CurScanLine->scanline[CurScanLine->scanline_len++]=altcolour;
                                                CurScanLine->scanline[CurScanLine->scanline_len++]=colour; //(SPECVSync>0)? 0:colour;
                                        }
                                        PBaseColour=BaseColour;
                                        shift_register <<= 1;
                                        ++shiftCount;
                                }
                        }
                        if (loop<0) SpeedUpCount=SpeedUp;
                }
                else    SpeedUpCount -=ts;
                DebugUpdate();


        } while((loop>0 || SpeedUpCount>0) && !zx81_stop && sts<MaxScanLen);


        if (loop<=0)
        {
                CurScanLine->sync_len=HSyncDuration;
                CurScanLine->sync_valid = SYNCTYPEH;
                if (CurScanLine->scanline_len > (machine.tperscanline*scale))
                        CurScanLine->scanline_len=(machine.tperscanline*2*scale);

                borrow = -loop;
                loop += machine.tperscanline;

                Sy++;
                if (Sy>=machine.scanlines)
                {
                        fts =0; //-= machine.tperframe;
                        CurScanLine->sync_len=414;
                        CurScanLine->sync_valid = SYNCTYPEV;
                        Sy=0;
                        //borrow=0;
                        loop=machine.tperscanline;
                }
                clean_exit=1;
        }
        else    clean_exit=0;

        return(sts);
}

