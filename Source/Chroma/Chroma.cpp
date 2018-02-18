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
 */

#include "Chroma.h"

/*
Chroma 81 is a multi-function interface for the ZX81, with the primary purpose
of providing a clear, sharp picture on most modern TV sets via RGB SCART. The
interface plugs into the ZX81 expansion bus and requires no modifications to
the ZX81.

The interface provides the following facilities:
- 16K RAM at $4000
- 8K RAM at $2000 (suitable for UDGs)
- 16K RAM at $C000
- WRX graphic support
- QS Character Board emulation
- Two colour modes (attributes file and character colourisation)
- Cursor joystick socket
- RS232 socket
- ROM cartridge socket (allows complete overriding of the ZX81 ROM)
- Loading/saving sounds through the TV speaker(s)
- Reset button

Full details on the interface can be found at www.fruitcake.plus.com
*/

const BYTE idleDataBus80 = 0x40;
const BYTE idleDataBus81 = 0xFF;

const int chromaIoPort = 0x7FEF;
const BYTE colourModeMask = 0x10;
const BYTE colourEnabledMask = 0x20;
const BYTE colourPresentMask81 = 0x20;
const BYTE colourPresentMask80 = 0x22;
const BYTE colourPresentValue81 = 0x00;
const BYTE colourPresentValue80 = 0x02;
const BYTE colourInkMask = 0x0F;
const BYTE colourPaperMask = 0xF0;

const int colourBlack = 0;
const int colourBrightWhite = 15;

int chromaInk = colourBlack;
int chromaPaper = colourBrightWhite;

static BYTE idleDataBus;
static BYTE colourPresentMask;
static BYTE colourPresentValue;

void DisableChroma()
{
        zx81.chromaMode = colourBrightWhite;
}

void InitialiseChroma()
{
        if (zx81.machine==MACHINEZX80)
        {
                idleDataBus = idleDataBus80;
                colourPresentMask = colourPresentMask80;
                colourPresentValue = colourPresentValue80;
        }
        else
        {
                idleDataBus = idleDataBus81;
                colourPresentMask = colourPresentMask81;
                colourPresentValue = colourPresentValue81;
        }

        DisableChroma();
}

bool ChromaRAMWrite(int Address, BYTE Data, BYTE* memory, BYTE* font)
{
        // The Chroma interface's additional 16K RAM is shared by the colour RAM,
        // 8K UDG RAM and QS Character RAM. The following approximates it by writing to
        // all three areas.

        // The QS Character Board provides a programmable character set, with the
        // character RAM stored in 1K RAM located at address 0x8400-0x87FF.
        // Frist are the bit patterns for the normal characters, and then the
        // patterns for the inverted characters. The ZX81 display hardware inverts
        // the bit patterns for the inverted characters and so in these are
        // stored in the QS Character Board RAM in non-inverted form. The RAM was
        // always enabled and a switch was used to select between displaying the
        // standard ZX81 character set in the ROM and the programmable character
        // set in the QS RAM.
        // The Chroma interface emulates the QS Character Board but deviates with
        // respect to the switch functionality. Chroma's switch both enables the RAM
        // and selects it for display.

        bool writeHandled = false;

        if (zx81.colour == COLOURCHROMA)
        {
                // Check for a write to the Chroma's QS Character RAM
                if ((zx81.chrgen == CHRGENQS) && zx81.enableQSchrgen && (Address >= 0x8400) && (Address < 0x8800))
                {
                        font[Address - 0x8400]=Data;
                        memory[Address] = Data;
                        memory[Address + 0x4000] = Data;
                        memory[Address - 0x6000] = Data;
                        writeHandled = true;
                }
                // Check for a write to the Chroma's 8K-16K RAM
                else if (zx81.RAM816k && (Address >= 0x2000) && (Address < 0x4000))
                {
                        if (zx81.chrgen != CHRGENDK)
                        {
                                memory[Address] = Data;
                                memory[Address + 0x8000] = Data;
                                memory[Address + 0xA000] = Data;
                                if ((Address >= 0x2400) && (Address < 0x2800)) memory[Address + 0x6000] = Data;
                                writeHandled = true;
                        }
                }
                // Check for a write to Chroma's 48K-64K colour RAM
                else if (zx81.chromaColourSwitchOn && (Address >= 0xC000))
                {
                        memory[Address] = Data;
                        if ((Address < 0xE000) && (zx81.chrgen != CHRGENDK))
                        {
                                memory[Address - 0xA000] = Data;
                        }
                        if ((Address >= 0xC400) && (Address < 0x8800)) memory[Address - 0x4000] = Data;
                        writeHandled = true;
                }
                // Chroma's 8-16K RAM is mirrored at 40K-48K
                else if (zx81.RAM816k && (Address >= 0xA000) && (Address < 0xC000))
                {
                        memory[Address] = Data;
                        memory[Address - 0x8000] = Data;
                        memory[Address + 0x4000] = Data;
                        if ((Address >= 0xA400) && (Address < 0xA800)) memory[Address - 0x4000] = Data;
                        writeHandled = true;
                }
        }

        return writeHandled;
}

bool ChromaRAMRead(int Address, BYTE* pData, BYTE* memory)
{
        bool readHandled = false;

        if (zx81.colour == COLOURCHROMA)
        {
                if (zx81.RAM816k && (Address >= 0x2000) && (Address < 0x4000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                // QS Character mode must be enabled if Chroma implementation,
                // or is always enabled if just a QS Character Board
                else if ((zx81.chrgen == CHRGENQS) && zx81.enableQSchrgen && (Address >= 0x8400) && (Address < 0x8800))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                // The Chroma interface colour switch pages in 16K RAM at 0xC000
                else if (zx81.chromaColourSwitchOn && (Address >= 0xC000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                else if (zx81.RAM816k && (Address >= 0xA000) && (Address < 0xC000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
        }

        return readHandled;
}

bool ChromaIOWrite(int Address, BYTE Data)
{
        bool writeHandled = false;

        if (zx81.colour == COLOURCHROMA)
        {
                if ((Address == chromaIoPort) && zx81.chromaColourSwitchOn)
                {
                        zx81.chromaMode = Data;
                        if (!(zx81.chromaMode & colourEnabledMask))
                        {
                                DisableChroma();
                        }
                        writeHandled = true;
                }
        }

        return writeHandled;
}

bool ChromaIORead(int Address, BYTE* pData)
{
        bool readHandled = false;

        if (zx81.colour == COLOURCHROMA)
        {
                if ((Address == chromaIoPort) && zx81.chromaColourSwitchOn)
                {
                        *pData = (idleDataBus & ~colourPresentMask) | colourPresentValue;
                        readHandled = true;
                }
        }

        return readHandled;
}

void FetchChromaColour(const int Address, const BYTE data, int rowcounter, BYTE* memory)
{
    if ((zx81.colour == COLOURCHROMA) && zx81.chromaColourSwitchOn && (zx81.chromaMode & colourEnabledMask))
    {
        int colourAddress;

        if (zx81.chromaMode & colourModeMask)
        {
            // Attribute file colour mode
            // Fetch from display file address + 0x8000
            colourAddress = (Address | 0x8000);
        }
        else
        {
            // Character code colour mode
            BYTE entry = ((data & 0x80) >> 1) | (data & 0x3F);
            colourAddress = 0xC000 + (entry << 3) + rowcounter;
        }

        int c = memory[colourAddress];
        chromaInk = (c & colourInkMask);
        chromaPaper = (c & colourPaperMask) >> 4;
    }
    else
    {
        chromaInk = colourBlack;
        chromaPaper = colourBrightWhite;
    }
}

void SetChromaColours()
{
        if (zx81.colour == COLOURCHROMA)
        {
                chromaPaper = GetChromaBorderColour();
                chromaInk = colourBlack;
        }
        else if (zx81.colour == COLOURDISABLED)
        {
                chromaPaper = colourBrightWhite;
                chromaInk = colourBlack;
        }
}

int GetChromaBorderColour()
{
        return (zx81.chromaMode & colourInkMask);
}

void GetChromaColours(int* pInk, int* pPaper)
{
        *pInk = chromaInk;
        *pPaper = chromaPaper;
}

