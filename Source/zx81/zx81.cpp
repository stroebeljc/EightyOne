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
 * zx81.c
 *
 */
#include <vcl.h>

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

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
#include "iecbus.h"
#include "ide.h"
#include "symbolstore.h"
#include "SymBrowse.h"
#include "zxpand\ZXpand_emu.h"
#include "RomCartridge\IF2RomCartridge.h"
#include "Chroma\Chroma.h"
#include "LiveMemoryWindow_.h"

#define VBLANKCOLOUR (0*16)

#define LASTINSTNONE  0
#define LASTINSTINFE  1
#define LASTINSTOUTFE 2
#define LASTINSTOUTFD 3
#define LASTINSTOUTFF 4

extern "C"
{
        int CRC32Block(char *memory, int romlen);
        void P3DriveMachineHasInitialised(void);
}

ZXpand* zxpand = NULL;

int memoryLoadToAddress(char *filename, void* destAddress, int length);

void add_blank(SCANLINE *line, int borrow, BYTE colour);

extern AnsiString getMachineRoot(AnsiString fullRomName);

extern void LogOutAccess(int address, BYTE data);
extern void LogInAccess(int address, BYTE data);
extern void ResetLastIOAccesses();
extern void DebugUpdate(void);
extern long noise;
extern int SelectAYReg;
extern int RasterY;
extern int VSYNC_TOLLERANCEMAX;
extern int VSYNC_TOLLERANCEMIN;

extern bool directMemoryAccess;
extern int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
extern int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
extern int lastMemoryReadValueLo, lastMemoryWriteValueLo;
extern int lastMemoryReadValueHi, lastMemoryWriteValueHi;

static BYTE ReadInputPort(int Address, int *tstates);

static BYTE idleDataBus = 0xFF;

const int colourBlack = 0;
const int colourBrightWhite = 15;

const BYTE idleDataBus80 = 0x40;
const BYTE idleDataBus81 = 0xFF;

int border=colourBrightWhite, ink=colourBlack, paper=colourBrightWhite;

int NMI_generator=0;
int HSYNC_generator=0;
int rowcounter=0;
int hsync_counter=207;
int borrow=0;
int tstates, frametstates;
int tStatesCount;
int configbyte=0;
int setborder=0;
int zx81_stop=0;
int LastInstruction;
int MemotechMode=0;
//int HWidthCounter=0;

BYTE zxpandROMOverlay[8192];
BYTE memory[1024 * 1024];
BYTE font[1024];                //Allows for both non-inverted and inverted for the QS Chars Board 
BYTE memhrg[1024];
BYTE ZXKeyboard[8];
BYTE ZX1541Mem[(8+32)*1024]; // ZX1541 has 8k EEPROM and 32k RAM
BYTE ZX1541PORT=0;

int shift_register=0, shift_reg_inv, shift_store=0;

int int_pending=0;

extern int shift_register, shift_reg_inv;
extern long noise;

extern int font_load(char*, char*,int);

BYTE get_i_reg(void)
{
        return(z80.i);
}

void zx81_initialise(void)
{
        int i, romlen;
        z80_init();
        tStatesCount = 0;

        directMemoryAccess = false;
        ResetLastIOAccesses();
        InitialiseRomCartridge();

        if (zx81.machine == MACHINEZX80)
        {
                idleDataBus = idleDataBus80;
        }
        else
        {
                idleDataBus = idleDataBus81;
        }

        InitialiseChroma();

        for(i=0;i<65536;i++) memory[i]=7;
        for(i=0;i<1024;i++) font[i]=0;
        for(i=0;i<1024;i++) memhrg[i]=0;

        AnsiString romname = machine.CurRom;

        // horrible cheesy hack
        //  to prevent zxpand from interfering with - say - aszmic
        if (romname.SubString(1,3).LowerCase() != "zx8")
                zx81.zxpand = 0;

        if (zx81.zxpand)
        {
                AnsiString romRoot = LowerCase(getMachineRoot(romname));
                AnsiString overlayName = romRoot + ".zxpand.ovl";
                if (romRoot == "zx81") {
                        memoryLoadToAddress(overlayName.c_str(), (void*)zxpandROMOverlay, 8192);
                } else if (romRoot == "zx80") {
                        romname = overlayName;
                }
        }
        romlen=memory_load(romname.c_str(), 0, 65536);
        zx81.romcrc=CRC32Block(memory,romlen);

        if (zx81.extfont) font_load("lmbfnt.rom",font,512);
        if ((zx81.chrgen==CHRGENDK) && ((zx81.colour != COLOURCHROMA) || (zx81.colour == COLOURCHROMA && !zx81.RAM816k)))
                romlen+=memory_load("dkchr.rom",8192,4096);

        zx81.ROMTOP=romlen-1;

        if (zx81.machine==MACHINEZX97LE)
        {
                for(i=0;i<8191;i++) memory[i+0xa000]=memory[i+0x2000];
                for(i=0;i<16384;i++) zx97.bankmem[i]=memory[i+0x4000];
                //for(i=8192;i<32768;i++) memory[i]=0x07;
                zx81.ROMTOP=8191;
        }

        if (zx81.truehires==HIRESMEMOTECH) memory_load("memohrg.rom", 8192, 2048);
        if (zx81.truehires==HIRESG007) memory_load("g007hrg.rom",10240,2048);

        if (zx81.machine==MACHINELAMBDA) { ink=7; paper=border=0; }
        else { ink=0; paper=border=15; }

        if (spectrum.floppytype==FLOPPYLARKEN81)
        {
                memory_load("larken81.rom", 14336, 2048);
                memory[0x38DB] = 0xCD;
                memory[0x38DC] = 0x2B;
                memory[0x38DD] = 0x0F;
                memory[0x38DE] = 0xC9;

                memory[0x3A0D] = 0xCD;
                memory[0x3A0E] = 0x2B;
                memory[0x3A0F] = 0x0F;
                memory[0x3A10] = 0xC9;

                memory[12300]=69;
                memory[12301]=0;
        }

        if (spectrum.HDType==HDPITERSCF)
        {
                ATA_Reset();
                ATA_SetMode(ATA_MODE_16BIT);
                memory_load("mwcfide.rom", 32768, 32768);

                //i=open("C:\\Documents and Settings\\Mike\\My Documents\\Programming\\ZXFat\\zxfat.bin",O_RDONLY);
                //read(i, memory+32768, 32768);
                //close(i);
        }


        ZX1541PORT=0;

        NMI_generator=0;
        HSYNC_generator=0;
        MemotechMode=0;

        z80_reset();
        d8255_reset();
        d8251reset();
        z80_reset();

        tStatesCount = 0;

        if (zxpand)
        {
                delete(zxpand);
                zxpand = NULL;
        }
        if (zx81.zxpand)
        {
                zxpand = new ZXpand();
        }

        P3DriveMachineHasInitialised();
}

// Stores the supplied byte in memory,
// taking into account memory mapped devices and RAM/ROM shadows.

void zx81_WriteByte(int Address, int Data)
{
        LiveMemoryWindow->Write(Address);

        noise = (noise<<8) | Data;

        // A ROM cartridge has highest precedence over the 0K-16K region, which it
        // ensures by masking out the MREQ line.
        // The ROM cartridge socket does not differentiate between a memory read or write,
        // so all accesses are treated as reads.
        if ((zx81.romCartridge != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
        {
                BYTE data;
                if (WriteRomCartridge(Address, (BYTE*)&data))
                {
                        LiveMemoryWindow->Write(Address);
                        return;
                }
        }

        // Quicksilva Sound Board uses a memory mapped AY8912 chip

        if (zx81.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) sound_ay_write(SelectAYReg,Data);
        }

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (zx81.colour==COLOURLAMBDA && Address>=8192 && Address<16384)
        {
                Address = (Address&1023)+8192;

                goto writeMem;
        }

        // ZX97 has various bank switched modes - check out the website for details

        if (zx81.machine==MACHINEZX97LE)
        {
                if (zx97.protect08 && Address<0x2000) return;
                if (zx97.protectab && Address>=0xa000 && Address<0xc000) return;

                if (Address>=0xC000)
                {
                        if (!(d8255_read(D8255PRTB)&16)) return;
                        if (zx97.protectb0 && ((d8255_read(D8255PRTB)&15)==0)) return;
                        if (zx97.protectb115 && ((d8255_read(D8255PRTB)&15)>0)) return;

                        zx97.bankmem[(Address&16383) +  16384*(d8255_read(D8255PRTB)&15)]=Data;
                        return;
                }

                if (zx97.bankswitch && Address>0x7fff && Address<=0x9fff) Address -=8000;
                if (zx97.bankswitch && Address<8192) Address += 0x8000;
        }

        // The RAM of the Chroma interface has precedence over devices connected behind it, e.g. ZXpand,
        // which it ensures by masking out the MREQ line
        if (ChromaRAMWrite(Address, Data, memory, font))
        {
                LiveMemoryWindow->Write(Address);
                return;
        }

        if ((zx81.chrgen == CHRGENQS) && (zx81.colour != COLOURCHROMA) && (Address >= 0x8400) && (Address < 0x8800))
        {
                font[Address-0x8400]=Data;
                goto writeMem;
        }

        // zx1541 floppy controller has 8k of EEPROM at 0x2000 and 32k RAM
        // in 2 banks at 0x8000

        if (spectrum.floppytype==FLOPPYZX1541 && !(ZX1541PORT&1))
        {
                if (Address>=0x2000 && Address<0x4000)
                {
                        ZX1541Mem[Address-0x2000]=Data;
                        return;
                }

                if (Address>=0x8000 && Address<0xc000)
                {
                        Address -= 0x8000;
                        if (ZX1541PORT&2) Address+=0x4000;
                        ZX1541Mem[Address+8192]=Data;
                        return;
                }
        }

        if (zx81.zxpand)
        {
                int configData;
                zxpand->GetConfig(configData);
                bool configLow = ((configData & (1<<CFG_BIT_LOW)) != 0);

                // ZXpand 8-16K RAM is not shadowed at 40-48K
                if ((configLow && (Address >= 0x2000 && Address < 0xA000)) ||
                    (!configLow && (Address >= 0x4000 && Address < 0xC000)))
                {
                goto writeMem;
                }
        }
        
        // Memotech Hi-res board has 1k of RAM available at address 8192
        // permanently and overlaid the ROM at address 0 when z80.i is odd
        // or data is written to address 0-1k

        if (Address<=zx81.ROMTOP && zx81.protectROM)
        {
                if ((zx81.truehires==HIRESMEMOTECH) && (Address<1024))
                        memhrg[Address]=Data;
                return;
        }

        // Take into account RAM Shadows when writing beyond RAMTOP
        if (Address > zx81.RAMTOP)
        {
                if ((Address & 0x7FFF) >= 0x4000)
                {
                        // Shadowing 16K RAM between 16-32K and 48-64K
                        // Shadowing 1K RAM at 17-18K, 18-19K, 19-20K ... 31-32K, and repeated within 48-64K region
                        Address = 0x4000 | (Address & zx81.RAMTOP & 0x3FFF);
                }
                else if (((Address & 0x7FFF) >= 0x2000) && zx81.RAM816k)
                {
                        // Shadow the RAM at 40-48K
                        Address = Address & 0x7FFF;
                }
                else
                {
                        // Shadow the ROM at 32-48K
                        Address = Address & zx81.ROMTOP;
                }
        }

        if (Address<10240 && zx81.truehires==HIRESMEMOTECH) return;
        if (Address>=10240 && Address<12288 && zx81.truehires==HIRESG007) return;
        if (Address<=zx81.ROMTOP && zx81.protectROM) return;
        if (Address>8191 && Address<16384 && zx81.shadowROM && zx81.protectROM) return;
        if (Address>8191 && Address<16384 && !zx81.RAM816k) return;

writeMem:
        memory[Address]=Data;
}

// Write to memory without accidentally invoking the ZXC ROM cartridge paging mechanism
void zx81_setbyte(int Address, int Data)
{
        directMemoryAccess = true;
        zx81_WriteByte(Address, Data);
        directMemoryAccess = false;
}

void zx81_writebyte(int Address, int Data)
{
        lastMemoryWriteAddrLo = lastMemoryWriteAddrHi;
        lastMemoryWriteAddrHi = Address;

        lastMemoryWriteValueLo = lastMemoryWriteValueHi;
        lastMemoryWriteValueHi = Data;

        zx81_WriteByte(Address, Data);
}

// BYTE zx81_readbyte(int Address)
//
// Given an Address, zx81_readbyte returns the byte which is stored at that address
// taking into account memory mapped devices and RAM/ROM shadows.
int video = 0;

BYTE zx81_ReadByte(int Address)
{
        LiveMemoryWindow->Read(Address);

        int data;

        // A ROM cartridge has highest precedence over the 0K-16K region, which it
        // ensures by masking out the MREQ line
        if ((zx81.romCartridge != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
        {
                BYTE data;
                if (ReadRomCartridge(Address, (BYTE*)&data))
                {
                        return data;
                }
        }

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (zx81.colour==COLOURLAMBDA && ((Address>=8192 && Address<16384)
                                          || (Address>=49152 && Address<57344)))
        {
                Address = (Address&1023)+8192;
                data=memory[Address];
                return(data);
        }

        // ZX97 has various bank switched modes - check out the website for details

        if (zx81.machine==MACHINEZX97LE)
        {
                if (zx97.bankswitch && Address<8192) Address+=0x8000;
                else if (zx97.bankswitch && (Address>=0x8000 && Address<=0x9fff)) Address-=0x8000;

                if (Address>=49152)
                {
                        data=zx97.bankmem[(Address&16383) + (d8255_read(D8255PRTB)&15)*16384];
                        noise = (noise<<8) | data;
                        return(data);
                }
        }

        // zx1541 floppy controller has 8k of EEPROM at 0x2000 and 32k RAM
        // in 2 banks at 0x8000

        if (spectrum.floppytype==FLOPPYZX1541 && !(ZX1541PORT&1))
        {
                if (Address>=0x2000 && Address<0x4000)
                {
                        data=ZX1541Mem[Address-0x2000];
                        noise = (noise<<8) | data;
                        return(data);
                }

                if (Address>=0x8000 && Address<0xc000)
                {
                        Address -= 0x8000;
                        if (ZX1541PORT&2) Address+=0x4000;
                        data=ZX1541Mem[Address+8192];
                        noise = (noise<<8) | data;
                        return(data);
                }
        }

        bool zxpandRamAccess = false;

        if (zx81.zxpand)
        {
                int zxpConfigData;
                zxpand->GetConfig(zxpConfigData);

                bool configLow = ((zxpConfigData & (1<<CFG_BIT_LOW)) != 0);

                // ZXpand 8-16K RAM is not shadowed at 40-48K
                zxpandRamAccess = (configLow && Address >= 0x2000 && Address < 0xA000) ||
                                  (!configLow && Address >= 0x4000 && Address < 0xC000);
        }

        // The Chroma interface has precedence over devices containing RAM connected behind it
        if (ChromaRAMRead(Address, (BYTE*)&data, memory))
        {
        }
        else if (zxpandRamAccess)
        {
                data=memory[Address];
        }
        else if (zx81.zxpand && (zx81.machine==MACHINEZX81 || zx81.machine==MACHINETS1000 || zx81.machine==MACHINEZX80) && video && Address>=0x1E00 && Address<0x2000)
        {
                // CR  zxpand enables the ROM for character access
                data=memory[Address];
        }
        else if ((zx81.chrgen == CHRGENQS) && (zx81.colour != COLOURCHROMA) && (Address >= 0x8400) && (Address < 0x8800))
        {
                data=memory[Address];
        }
        else if (zx81.RAM816k && Address >= 0x2000 && Address < 0x4000)
        {
                data=memory[Address];
        }
        else if (Address >= 0x4000 && Address <= zx81.RAMTOP)
        {
                data=memory[Address];
        }
        else if (Address <= zx81.ROMTOP)
        {
                // CR  reads from ROM whilst zxpand is disabled will return
                // normal ROM content, else overlay ROM

                if (zx81.zxpand && zx81.machine==MACHINEZX81)
                {
                        int zxpConfigData;
                        zxpand->GetConfig(zxpConfigData);
                        bool zxpandDisabled = zxpConfigData & (1<<CFG_BIT_DISABLED);

                        if (!zxpandDisabled)
                                data = zxpandROMOverlay[Address];
                        else
                                data=memory[Address];
                }
                else
                        data=memory[Address];
        }
        else if ((Address & 0x7FFF) >= 0x4000)
        {
                // Shadow 16K RAM between 16-32K and 48-64K
                // Shadow 1K RAM at 17-18K, 18-19K, 19-20K ... 31-32K, and repeat within 48-64K region
                data=memory[0x4000 | (Address & zx81.RAMTOP & 0x3FFF)];
        }
        else
        {
                if (zx81.RAM816k)
                {
                        // Shadow 8-16K RAM at 40-48K
                        data=memory[Address & 0x7FFF];
                }
                else if ((zx81.machine == MACHINEZX80) && !strcmp(machine.CurRom, "zx80.rom") && !zx81.zxpand && (Address & 0x1000))
                {
                        data = idleDataBus80;
                }
                else
                {
                        // Shadow the ROM at 32-48K
                        data=memory[Address & zx81.ROMTOP];
                }
        }

        // Memotech Hi-res board has 1k of RAM available at address 8192
        // permanently and overlaid the ROM at address 0 when z80.i is odd
        if ((Address<1024 && (zx81.truehires==HIRESMEMOTECH)) && (z80.i&1))
        {
                data=memhrg[Address];
        }

        // G007 similarly overlays the ROM but a smaller range
        if ((Address>=0x0c00 && Address<=0x0cff) && (zx81.truehires==HIRESG007))
        {
                data=memory[Address+8192];
        }

        if ((Address<256 || (Address>=512 && Address<768))
                && (z80.i&1) && (zx81.truehires==HIRESG007))
                        data=memory[Address+8192];

        noise = (noise<<8) | data;
        return(data);
}

// Read from memory without accidentally invoking the ZXC ROM cartridge paging mechanism
// Called by debugger routines
BYTE zx81_getbyte(int Address)
{
        directMemoryAccess = true;
        BYTE b = zx81_ReadByte(Address);
        directMemoryAccess = false;

        return b;
}

// Called by Z80 instruction operand fetches
BYTE zx81_readoperandbyte(int Address)
{
        return zx81_ReadByte(Address);
}

// Called by emulated program
BYTE zx81_readbyte(int Address)
{
        lastMemoryReadAddrLo = lastMemoryReadAddrHi;
        lastMemoryReadAddrHi = Address;

        BYTE byte = zx81_ReadByte(Address);

        lastMemoryReadValueLo = lastMemoryReadValueHi;
        lastMemoryReadValueHi = byte;

        return byte;
}

// BYTE opcode_fetch(int Address)
//
// Given an address, opcode fetch returns the byte at that memory address,
// modified depending on certain circumstances.
// It also loads the video shift register and generates video noise.
//
// If Address is less than M1NOT, all code is executed,
// the shift register is cleared and video noise is set to what is on
// the data bus.
//
// If Address >= M1NOT, and bit 6 of the fetched opcode is not set
// a NOP is returned and we load the shift register accordingly,
// depending on which video system is in use (WRX/Memotech/etc.)
//
// The ZX81 has effectively two busses.  The ROM is on the first bus
// while (usually) RAM is on the second.  In video generation, the ROM
// bus is used to get character bitmap data while the second bus
// is used to get the display file.  This is important because depending
// on which bus RAM is placed, it can either be used for extended
// Fonts OR WRX style hi-res graphics, but never both.

// Called by Z80 instruction opcode fetches
BYTE zx81_opcode_fetch(int Address)
{
        static int calls = 0;
        int inv;
        int opcode, bit6, update=0;
        BYTE data;

        // very rough timing here;
        // assuming a 1mhz call rate it will be 1ms every 1000 calls.
        ++calls;
        if (calls == 1000)
        {
                calls = 0;
                if (zxpand) zxpand->Update(1);
        }

        if (Address < zx81.m1not)
        {
                // This is not video related, so just return the opcode
                // and generate some video noise.
                data = zx81_ReadByte(Address);

                // The floating point hardware fix intercepts instruction opcode fetches from addresses
                // matching %-0xx0x1100110101 and forces bit 6 of the instruction opcode to 0.
                // The fix affects addresses $0335, $0735, $1335, $1735, $2335, $2735, $3335 and $3735,
                // and upper locations $8335, $8735, $9335, $9735, $A335, $A735, $B335 and $B735 which
                // can affect programs utilising the M1Not modification that allows code to run from the
                // 32K-48K region.
                if (((zx81.machine == MACHINEZX80) || (zx81.machine == MACHINEZX81)) && zx81.FloatingPointHardwareFix)
                {
                        if ((Address & 0x4BFF) == 0x0335)
                        {
                                data &= 0xBF;
                        }
                }

                noise |= data;
                return(data);
        }

        // We can only execute code below M1NOT.  If an opcode fetch occurs
        // above M1NOT, we actually fetch (address&32767).  This is important
        // because it makes it impossible to place the display file in the
        // 48-64k region if a 64k RAM Pack is used.  How does the real
        // Hardware work?
        data = zx81_ReadByte((Address>=49152)?Address&32767:Address);
        opcode=data;
        bit6=opcode&64;

        // Since we got here, we're generating video (ouch!)
        // Bit six of the opcode is important.  If set, the opcode
        // gets executed and nothing appears onscreen.  If unset
        // the Z80 executes a NOP and the code is used to somehow
        // generate the TV picture (exactly how depends on which
        // display method is used)

        if (!bit6) opcode=0;
        inv = data&128;

        bool zx80 = (zx81.machine == MACHINEZX80);
        bool chromaColour = (zx81.colour == COLOURCHROMA);
        bool chrgenChr128 = (zx81.chrgen == CHRGENCHR128);
        bool upper16KAccess = (z80.i >= 0xC0);
        bool region8KAccess = (z80.i >= 0x20) && (z80.i < 0x40);
        bool wrxAccess = (zx81.truehires == HIRESWRX) && !bit6;

        bool chroma80 = zx80 && chromaColour;
        bool chroma80Chr128 = chroma80 && chrgenChr128 && upper16KAccess;
        bool notChr128mode  = !chroma80Chr128 && (z80.i > zx81.maxireg);
        bool chr128mode8kRam = !chroma80Chr128 && region8KAccess && zx81.RAM816k;

        // First check for WRX graphics.  This is easy, we just create a
        // 16 bit Address from the IR Register pair and fetch that byte
        // loading it into the video shift register.
        if ((notChr128mode || chr128mode8kRam) && wrxAccess && !chroma80Chr128)
        {
                FetchChromaColour(Address, data, rowcounter, memory);

                data=zx81_ReadByte((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
                update=1;
        }
        else if ((z80.i&1) && MemotechMode)
        {
                // Next Check Memotech Hi-res.  Memotech is only enabled
                // When the I register is odd.

                extern int RasterY;
                extern SCANLINE *BuildLine;

                if ((opcode!=118 || BuildLine->scanline_len<66) && RasterY>=56 && RasterY<=(56+192))
                {
                        inv=(MemotechMode==3);
                        update=1;
                }
        }
        else if ((z80.i&1) && (zx81.truehires==HIRESG007))
        {
                // Like Memotech, G007 is enabled when I is odd.
                // However, it is much simpler, in that it disables
                // the bit 6 detection entirely and relies on the R
                // register to generate an interupt at the right time.

                inv=0;
                update=1;
        }
        else if (!bit6)
        {
                // If we get here, we're generating normal Characters
                // (or pseudo Hi-Res), but we still need to figure out
                // where to get the bitmap for the character from
                FetchChromaColour(Address, data, rowcounter, memory);

                // First try to figure out which character set we're going
                // to use if CHR$x16 is in use.  Else, standard ZX81
                // character sets are only 64 characters in size.

                bool chrgenQS = (zx81.chrgen == CHRGENQS);  
                bool lower16KAccess = (z80.i < 0x40);

                bool chr128 = chrgenChr128 && (z80.i & 1) && !chroma80;
                bool qsChars = chrgenQS && zx81.enableQSchrgen;

                if (chr128 || qsChars || chroma80Chr128)
                {
                        data = ((data&128)>>1)|(data&63);
                }
                else
                {
                    data = data&63;
                }

                // If I points to ROM, OR I points anywhere else for
                // CHR$128, we'll fetch the bitmap from there.
                // Lambda and the QS Character board have external memory
                // where the character set is stored, so if one of those
                // is enabled we better fetch it from the dedicated
                // external memory.
                // Otherwise, we can't get a bitmap from anywhere, so
                // display 11111111.

                if (lower16KAccess || chrgenChr128)
                {
                        if (zx81.extfont || qsChars)
                        {
                                data= font[(data<<3) | rowcounter];
                        }
                        else
                        {
                                video = 1;
                                data=readoperandbyte(((z80.i & 254) << 8) + (data << 3) | rowcounter);
                                video = 0;
                        }
                }
                else
                {
                        data=255;
                }

                update=1;
        }

        if (update)
        {
                // Update gets set to true if we managed to fetch a bitmap from
                // somewhere.  The only time this doesn't happen is if we encountered
                // an opcode with bit 6 set above M1NOT.

                if (zx81.colour == COLOURLAMBDA)
                {
                        int c;

                        // If Lambda colour is enabled, we had better fetch
                        // the ink and paper colour from memory too.
                        //
                        // 0=Black, 1=Blue, 2=Green, 3=Cyan, 4=Red, 5=Magenta, 6=Yellow, 7=White
                        // Ink = bits 0-2, Paper = bits 4-6

                        c=zx81_ReadByte((Address&1023)+8192);

                        ink = (c & 0x01) | ((c & 0x02) << 1) | ((c & 0x04) >> 1);
                        c = (c >> 4);
                        paper = (c & 0x01) | ((c & 0x02) << 1) | ((c & 0x04) >> 1);

                        if (setborder)
                        {
                                border=paper;
                                setborder=0;
                        }
                }
                else if (zx81.colour == COLOURCHROMA)
                {
                        if (setborder)
                        {
                                border = GetChromaBorderColour();
                                setborder = 0;
                        }
                }
                else if (zx81.machine != MACHINELAMBDA)
                {
                        border = colourBrightWhite;
                }

                // Finally load the bitmap we retrieved into the video shift
                // register, remembering to make some video noise too.

                shift_register |= data;
                shift_reg_inv |= inv? 255:0;
                if (zx81.machine==MACHINELAMBDA) noise |= (Address>>8);
                else noise |= z80.i;
                return(0);
        }
        else
        {
                // This is the fallthrough for when we found an opcode with
                // bit 6 set in the display file.  We actually execute these
                // opcodes, and generate the noise.

                SetChromaColours();

                noise |= data;
                return(opcode);
        }
}

void zx81_writeport(int Address, int Data, int *tstates)
{
        LogOutAccess(Address, Data);

        // The Chroma IO port is fully decoded
        if (ChromaIOWrite(Address, Data))
        {
                if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
                if (zx81.vsyncsound) sound_beeper(1);
                return;
        }

        if ((spectrum.HDType==HDPITERSCF) && ((Address&0x3b)==0x2b))
                ATA_WriteRegister(((Address>>2)&1) | ((Address>>5)&6), Data);

        switch(Address&255)
        {
        case 0x01:
                configbyte=Data;
                break;

        case 0x07:
                if (zxpand) zxpand->IO_Write(Address>>8, Data);
                break;

        case 0x0f:
        case 0x1f:
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data);
                break;

        case 0x3f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        SelectAYReg=Data&15;

        case 0x5f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        sound_ay_write(SelectAYReg, Data);
                break;

        case 0x73:
                if (zx81.ts2050) d8251writeDATA(Data);
                break;

        case 0x77:
                if (zx81.ts2050) d8251writeCTRL(Data);
                break;

        case 0xbf:
                if (spectrum.floppytype==FLOPPYZX1541)
                        ZX1541PORT=Data;

                        Data>>=2;

                        if (Data&1) IECAssertReset(0); else IECReleaseReset(0);
                        if (Data&2) IECAssertATN(0); else IECReleaseATN(0);
                        if (Data&4) IECAssertClock(0); else IECReleaseClock(0);
                        if (Data&8) IECAssertData(0); else IECReleaseData(0);
                break;

        case 0xc7:
                d8255_write(D8255PRTA,Data);
                break;

        case 0xcf:
                if (zx81.aytype==AY_TYPE_ZONX) SelectAYReg=Data&15;
                else d8255_write(D8255PRTB,Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC,Data);
                break;

        case 0xdf:
                if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data);
                if (zx81.aytype==AY_TYPE_ZONX) SelectAYReg=Data&15;
                break;

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) SelectAYReg=Data;
                break;

        case 0xfb:
                if (zx81.zxprinter) ZXPrinterWritePort(Data);
                break;

        case 0xfd:
                if (zx81.machine==MACHINEZX80) break;
                LastInstruction = LASTINSTOUTFD;
                break;

        //case 0xfe:
        default:
                if (!(Address&1))
                {
                        if (zx81.machine==MACHINEZX80) break;
                        LastInstruction = LASTINSTOUTFE;
                        break;
                }
                break;
        }

        if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
        if ((zx81.machine != MACHINELAMBDA) && zx81.vsyncsound)
                sound_beeper(1);
}

BYTE zx81_readport(int Address, int *tstates)
{
        BYTE data = ReadInputPort(Address, tstates);
        LogInAccess(Address, data);

        return data;
}

BYTE ReadInputPort(int Address, int *tstates)
{
        static int beeper;
        BYTE data = 0;

        setborder=1;

        // The Chroma IO port is fully decoded
        if (ChromaIORead(Address, &data))
        {
                return data;
        }

        if (!(Address&1))
        {
                BYTE keyb;
                int i;
                if ((zx81.machine!=MACHINELAMBDA) && zx81.vsyncsound)
                        sound_beeper(0);
                if (zx81.NTSC) data|=64;
                if (!GetEarState()) data |= 128;

                LastInstruction=LASTINSTINFE;
                keyb=Address/256;
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }

                return(~data);
        }   
        else
        {
                if ((spectrum.HDType==HDPITERSCF || spectrum.HDType==HDPITERS8B) && ((Address&0x3b)==0x2b))
                        return(ATA_ReadRegister(((Address>>2)&1) | ((Address>>5)&6)));

                switch(Address&255)
                {
                case 0x01:
                {
                        char *config;

                        config=(char *)(&zx81);
                        return(config[configbyte]);
                }

                case 0x7:
                        if (zxpand) return zxpand->IO_Read(Address>>8);
                        return 0xfd;

                case 0x17:
                        if (zxpand) return zxpand->IO_ReadStatus();
                        return 0;

                case 0x41:
                        if (spectrum.floppytype==FLOPPYLARKEN81) return(0xfe);
                        break;

                case 0x43:
                        if (spectrum.floppytype==FLOPPYLARKEN81) return(0x1e);
                        break;

                case 0x45:
                        if (spectrum.floppytype==FLOPPYLARKEN81) return(0x26);
                        break;

                case 0x5f:
                        if (zx81.truehires==HIRESMEMOTECH) MemotechMode=(Address>>8);
                        return(255);

                case 0x73:
                        if (zx81.ts2050) return(d8251readDATA());

                case 0x77:
                        if (zx81.ts2050) return(d8251readCTRL());

                case 0xbf:
                        if (spectrum.floppytype==FLOPPYZX1541)
                        {
                                int a = ZX1541PORT & 3;

                                if (!IECIsReset()) a |= 16;
                                if (!IECIsATN()) a |= 32;
                                if (!IECIsClock()) a |= 64;
                                if (!IECIsData()) a |= 128;
                                return(a);
                        }

                case 0xdd:
                        if (zx81.aytype==AY_TYPE_ACE)
                                return(sound_ay_read(SelectAYReg));

                case 0xf5:
                        beeper = 1-beeper;
                        if ((zx81.machine==MACHINELAMBDA) && zx81.vsyncsound)
                                sound_beeper(beeper);
                        return(255);
                case 0xfb:
                        if (zx81.zxprinter) return(ZXPrinterReadPort(idleDataBus));

                default:
                        break;
                }
        }
        
        return idleDataBus;
}

int zx81_contend(int Address, int states, int time)
{
        return(time);
}

void ramwobble(int now)
{
        int start, length, data;
        int i;

        start=zx81.ROMTOP+1;
        length=zx81.RAMTOP-start;
        data=random(256);

        if (now || !random(64))
                for(i=0;i<length;i++) memory[start+i] ^= data;
}

int zx81_do_scanline(SCANLINE *CurScanLine)
{
        int ts,i;
        int MaxScanLen;
        int PrevRev=0, PrevBit=0, PrevGhost=0;
        int tstotal=0;
        int pixels;
        static int nonHaltedWaitStates=0;
        static int haltedWaitStates=0;
        static int nmiOnRasterY=0;
        static int frameSynchronisedCounter = 0;
        static int prevRasterY = 0;
        static int prevPrevRasterY = 0;

        const int HSyncDuration = 16;
        const int BackPorchDuration = 10;

        CurScanLine->scanline_len=0;

        MaxScanLen = (zx81.single_step? 1:420);

        // If the end of a bottom border line occurs midway through an instruction then the instruction is completed
        // and a corresponding number of pixels drawn at the start of the next line to make up for it. However, it is
        // not desirable to do this when running in SLOW mode since the overspill from the bottom border line would
        // change the length of the VSync pulse. So instead the overspill is deferred for inclusion into the first
        // line of the top border.

        nonHaltedWaitStates &= 0x001F;
        haltedWaitStates &= 0x001F;

        // A crude mechanism to determine whether frame synchronisation has been achieved, which is required to suppress
        // showing HSync pulses when showing a saving or loading pattern. Synchronisation is assumed when 3 consecutive
        // frames is found. A counter provides hysteresis so that if a save or load pattern appears to form a valid frame
        // then it will not cause momentary display of HSync pulses.

        if (RasterY == 0)
        {
                bool validScanlineCount = (prevRasterY >= VSYNC_TOLLERANCEMIN) && (prevRasterY <= VSYNC_TOLLERANCEMAX);

                if ((abs(prevRasterY - prevPrevRasterY) <= 1) && validScanlineCount)
                {
                        if (frameSynchronisedCounter < 18)
                                frameSynchronisedCounter++;
                }
                else
                {
                        if (frameSynchronisedCounter > 0)
                                frameSynchronisedCounter--;
                }

                prevPrevRasterY = prevRasterY;
        }

        prevRasterY = RasterY;

        bool frameSynchronised = (frameSynchronisedCounter > 3);

        bool startSlowModeTopBorder = (CurScanLine->sync_valid == SYNCTYPEV) && (RasterY == 0) && (nmiOnRasterY > 0);
        bool withinSlowModeBottomBorder = (RasterY > nmiOnRasterY) && (nmiOnRasterY > 0) && NMI_generator;
        bool startSlowModeBottomBorder = (RasterY == nmiOnRasterY+1) && withinSlowModeBottomBorder;

        // A fudge to get the Back Porch signal to align on the top line
        // but allowing mistimed flicker-free programs to show distortion
        if ((zx81.machine == MACHINEZX80) && (RasterY == 0))
        {
                borrow -= 4;
        }

        if (startSlowModeTopBorder)
        {
                nmiOnRasterY = 0;
                nonHaltedWaitStates += borrow;
                borrow = 0;
        }

        if (CurScanLine->sync_valid)
        {
                int backPorchBorrow = HSYNC_generator ? ((borrow < BackPorchDuration) ? borrow : BackPorchDuration) : 0;
                int displayBorrow = (borrow - backPorchBorrow);

                add_blank(CurScanLine, backPorchBorrow, HSYNC_generator && !zx81.HideHardwareHSyncs && frameSynchronised ? VBLANKCOLOUR : (16*paper));
                add_blank(CurScanLine, displayBorrow, HSYNC_generator ? (16*paper) : VBLANKCOLOUR);

                if (CurScanLine->sync_len>machine.tperscanline && machine.tperscanline==208)
                        hsync_counter=machine.tperscanline-10;
                borrow=0;
                CurScanLine->sync_valid=0;
                CurScanLine->sync_len=0;
        }

        do
        {
                LastInstruction=LASTINSTNONE;

                // The wait states should be incorporated into the NMI response but doing so breaks the approximation used
                // to align the main picture area, causing it to appear 36 pixels to the right and various programs using
                // advanced custom display drivers to misbehave, even crash. The last line of the top border is generated
                // using a HALT in the ROM display routine and the wait states from this can only be included after the main
                // picture area has been displayed, i.e. within the bottom border where they will not be noticeable.

                bool waitStatesWithinBorderArea = (nonHaltedWaitStates > 0);
                bool haltedWaitStatesAtStartBottomBorder = (haltedWaitStates > 0) && startSlowModeBottomBorder;
                
                if (!waitStatesWithinBorderArea || haltedWaitStatesAtStartBottomBorder)
                {
                        z80.pc.w=PatchTest(z80.pc.w);
                        ts = z80_do_opcode();

                        if (haltedWaitStatesAtStartBottomBorder)
                        {
                                ts += haltedWaitStates;
                                haltedWaitStates = 0;
                        }
                }
                else
                {
                        ts = nonHaltedWaitStates;
                        nonHaltedWaitStates = 0;
                }

                if (int_pending)
                {
                        int tsint = z80_interrupt(idleDataBus);

                        if (tsint)
                        {
                                ts += tsint;
                                //if (HWidthCounter>200 && HWidthCounter<216)
                                //{
                                //        machine.tperscanline=HWidthCounter;
                                //}
                                //HWidthCounter=0;
                        }
                        int_pending=0;
                }

                //HWidthCounter+=ts;

                frametstates += ts;
                tStatesCount += ts;
                WavClockTick(ts, !HSYNC_generator);
                if (zx81.zxprinter) ZXPrinterClockTick(ts);
                if (spectrum.floppytype==FLOPPYZX1541) IECClockTick(ts);

                shift_store=shift_register;
                pixels=ts<<1;

                for (i=0; i<pixels; i++)
                {
                        int colour, bit;

                        bit=((shift_register^shift_reg_inv)&32768);

                        bool HSyncPeriod = (hsync_counter-(i/2) < HSyncDuration);
                        bool BackPorchPeriod = (hsync_counter-(i/2)) > (machine.tperscanline - BackPorchDuration);
                        bool BlankingPeriod = !zx81.HideHardwareHSyncs && (HSyncPeriod || BackPorchPeriod);
                        if (HSYNC_generator && (!BlankingPeriod || (BlankingPeriod && !frameSynchronised)))
                            colour = (bit ? ink:paper)<<4;
                        else
                            colour = VBLANKCOLOUR;

                        if (zx81.dirtydisplay)
                        {
                                if (PrevGhost) colour|=4;
                                PrevGhost=0;

                                if (PrevBit && (PrevRev || zx81.simpleghost))
                                        { colour|=2; PrevGhost=1; }

                                if (noise&1) colour|=1;
                                noise>>=1;
                                PrevRev=shift_reg_inv&32768;
                                PrevBit= bit;
                        }

                        CurScanLine->scanline[CurScanLine->scanline_len++] = colour;

                        shift_register<<=1;
                        shift_reg_inv<<=1;

                        if ((zx81.colour == COLOURCHROMA) && ((i & 7) == 7))
                        {
                                if (frameSynchronised)
                                {
                                        GetChromaColours(&ink, &paper);
                                }
                                else
                                {
                                        ink=colourBlack;
                                        paper=colourBrightWhite;
                                }
                        }
                }

                switch(LastInstruction)
                {
                case LASTINSTOUTFD:
                        NMI_generator=0;
                        if (!HSYNC_generator) rowcounter=0;
                        if (CurScanLine->sync_len) CurScanLine->sync_valid=SYNCTYPEV;
                        HSYNC_generator=1;
                        break;
                case LASTINSTOUTFE:
                        NMI_generator=1;
                        if (RasterY > 0)
                                nmiOnRasterY = RasterY;
                        if (!HSYNC_generator) rowcounter=0;
                        if (CurScanLine->sync_len) CurScanLine->sync_valid=SYNCTYPEV;
                        HSYNC_generator=1;
                        break;
                case LASTINSTINFE:
                        if (!NMI_generator)
                        {
                                HSYNC_generator=0;
                                if (CurScanLine->sync_len==0) CurScanLine->sync_valid=0;
                        }
                        break;
                case LASTINSTOUTFF:
                        if (!HSYNC_generator) rowcounter=0;
                        if (CurScanLine->sync_len == 11 || CurScanLine->sync_len > 20)
                                CurScanLine->sync_valid=SYNCTYPEV;
                        else
                        {
                                if (CurScanLine->sync_len > 0)
                                {
                                        if ((NMI_generator == 0) || (zx81.machine == MACHINEZX80))
                                        {
                                                CurScanLine->sync_valid=SYNCTYPEH;
                                        }
                                        if (zx81.machine == MACHINEZX80)
                                        {
                                                if (CurScanLine->scanline_len>(machine.tperscanline*2))
                                                        CurScanLine->scanline_len=machine.tperscanline*2;
                                                add_blank(CurScanLine, machine.tperscanline-CurScanLine->scanline_len, 16*paper);
                                                hsync_counter += machine.tperscanline;
                                        }
                                }
                        }
                        HSYNC_generator=1;
                        break;
                default:
                        break;
                }

                hsync_counter -= ts;

                if (!(z80.r & 64))
                        int_pending=1;

                if (!HSYNC_generator)
                    CurScanLine->sync_len += ts;

                if (hsync_counter <= 1)
                {
                        if (NMI_generator)
                        {
                                int nmilen;
                                nmilen = z80_nmi(CurScanLine->scanline_len, &nonHaltedWaitStates, &haltedWaitStates);
                                hsync_counter -= nmilen;
                                ts += nmilen;

                                if (zx81.HideHardwareHSyncs)
                                        add_blank(CurScanLine, 1, 16*paper);
                                else
                                        add_blank(CurScanLine, 1, VBLANKCOLOUR);
                        }

                        borrow = -hsync_counter;
                }
                if (hsync_counter <= 0)
                {
                        if (HSYNC_generator && CurScanLine->sync_len==0)
                        {
                                CurScanLine->sync_len=HSyncDuration;
                                CurScanLine->sync_valid=SYNCTYPEH;
                                if (CurScanLine->scanline_len>(machine.tperscanline*2))
                                        CurScanLine->scanline_len=machine.tperscanline*2;
                                rowcounter = (++rowcounter)&7;
                        }
                        hsync_counter += machine.tperscanline;
                }

                tstotal += ts;

                DebugUpdate();

        } while(CurScanLine->scanline_len<MaxScanLen && !CurScanLine->sync_valid && !zx81_stop);

        if (CurScanLine->sync_valid==SYNCTYPEV)
        {
                hsync_counter=machine.tperscanline;
        }

        return(tstotal);
}

