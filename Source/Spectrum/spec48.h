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

#ifndef spec48_h
#define spec48_h

#include "configuration.h"

#define RAMRead(Bank, Address) (SpectrumMem[(Bank<<14) | ((Address)&16383)])
#define RAMWrite(Bank, Address, Data) ((SpectrumMem[(Bank<<14) | ((Address)&16383)])=Data)

extern BYTE SpectrumMem[];  //enough memory for 64k ROM + 128k RAM
extern BYTE TimexMem[];  // Timex has two more blocks of 64k each
extern BYTE SpectraMem[]; // Spectra has two banks of 16K RAM
extern BYTE SPECLast7ffd;
extern BYTE divIDEMem[];       // divIDE has 8k of FlashRAM and 32k of RAM
extern BYTE ZXCFMem[];  // ZXCF has 1024k arranged as 64 x 16k pages

extern int SPECNextBorder;
extern BYTE memory[];
extern void spec48_initialise();
extern int spec48_do_scanline(SCANLINE *CurScanLine);
extern void spec48_writebyte(int Address, int Data);
extern void spec48_setbyte(int Address, int Data);
extern BYTE spec48_readbyte(int Address);
extern BYTE spec48_readoperandbyte(int Address);
extern BYTE spec48_getbyte(int Address);
extern BYTE spec48_opcode_fetch(int Address);
extern void spec48_writeport(int Address, int Data, int *tstates);
extern BYTE spec48_readport(int Address, int *tstates);
extern int tstates, event_next_event;
extern int framepos, frametstates;
extern BOOL nmiGeneratorEnabled, syncOutputWhite;
extern int tStatesCount;
extern int emulation_stop;
extern BYTE ZXKeyboard[];
extern int spec48_contend(int Address, int states, int time);
extern int spec48_contendio(int Address, int states, int time);
extern void spec48_reset(void);
extern void spec48_exit(void);

#endif
