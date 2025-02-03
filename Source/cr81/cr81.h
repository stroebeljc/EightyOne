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
 * zx81.h
 *
 */

#ifndef zx81_h
#define zx81_h

#include "z80\config.h"
#include "zx81config.h"

#ifdef __cplusplus

extern "C" BYTE memory[];
extern "C" void zx81_initialise(void);
extern "C" int zx81_do_scanline(SCANLINE *CurScanLine);
extern "C" int zx80_do_scanline(SCANLINE *CurScanLine);
extern "C" void zx81_writebyte(int Address, int Data);
extern "C" BYTE zx81_readbyte(int Address);
extern "C" BYTE zx81_readoperandbyte(int Address);
extern "C" BYTE zx81_opcode_fetch(int Address);
extern "C" void zx81_writeport(int Address, int Data, int *tstates);
extern "C" BYTE zx81_readport(int Address, int *tstates);
extern "C" int zx81_contend(int Address, int states, int time);
extern "C" int tstates, event_next_event;
extern "C" int framepos, frametstates;
extern "C" BOOL nmiGeneratorEnabled, syncOutputWhite;
extern "C" BYTE get_i_reg(void);
extern "C" BYTE ZXKeyboard[];
extern "C" int emulation_stop;
#else

extern BYTE memory[];
extern BYTE acecolour[];
extern void zx81_initialise(void);
extern int zx81_do_scanline(SCANLINE *CurScanLine);
extern int zx80_do_scanline(SCANLINE *CurScanLine);
extern void zx81_writebyte(int Address, int Data);
extern void zx81_setbyte(int Address, int Data);
extern BYTE zx81_readbyte(int Address);
extern BYTE zx81_getbyte(int Address);
extern BYTE zx81_opcode_fetch(int Address);
extern void zx81_writeport(int Address, int Data, int *tstates);
extern BYTE zx81_readport(int Address, int *tstates);
extern int zx81_contend(int Address, int states, int time);
extern int tstates, event_next_event;
extern int framepos, frametstates;
extern BOOL nmiGeneratorEnabled, syncOutputWhite;
extern int emulation_stop;
extern BYTE ZXKeyboard[];
extern int ace_do_accurate();

#endif
#endif
