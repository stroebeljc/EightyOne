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

#ifndef Spectra_h
#define Spectra_h

#include "zx81config.h"

extern void DisableSpectra();
extern void InitialiseSpectra();
extern void FetchSpectraAttributeFileBytes(int y, int column, int* attr1, int* attr2);
extern void DetermineSpectraInkPaper(int attr, int attr2, int flashSwap, int* inkLeft, int* inkRight, int* paperLeft, int* paperRight);
extern int DetermineSpectraBorderColour(int Data, int flashSwap);
extern void SpectraRAMWrite(int Address, BYTE Data);
extern void DetermineSpectraDisplayBank();
extern BYTE SpectraRAMRead(int bankOffset);

#endif
