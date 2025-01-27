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
 *
 * kbstatus.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include <stdio.h>
#include "kbstatus.h"
#include "zx81.h"
#include "zx81config.h"

int PCShift=1;
unsigned int PCALT=0;

struct kb
{
        BYTE Shift;
        WORD WinKey;
        BYTE Addr1, Data1, Addr2, Data2;
};

void SetKeyMap(const kb *source);
void AdjustLocalKeyboard();

struct kb KeyMap[100];

const struct kb KBZX81[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD4, 255,255 },
        { 0, 'C' , kbA8, kbD3, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD2, 255,255 },
        { 0, 'N' , kbA15, kbD3, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA8, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD2, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD1, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 0, VK_NUMPAD1 , kbA11, kbD0, 255,255 },
        { 0, VK_NUMPAD2 , kbA11, kbD1, 255,255 },
        { 0, VK_NUMPAD3 , kbA11, kbD2, 255,255 },
        { 0, VK_NUMPAD4 , kbA11, kbD3, 255,255 },
        { 0, VK_NUMPAD5 , kbA11, kbD4, 255,255 },
        { 0, VK_NUMPAD6 , kbA12, kbD4, 255,255 },
        { 0, VK_NUMPAD7 , kbA12, kbD3, 255,255 },
        { 0, VK_NUMPAD8 , kbA12, kbD2, 255,255 },
        { 0, VK_NUMPAD9 , kbA12, kbD1, 255,255 },
        { 0, VK_NUMPAD0 , kbA12, kbD0, 255,255 },

        { 0, VK_MULTIPLY , kbA15, kbD4, kbA8, kbD0 },
        { 0, VK_DIVIDE , kbA8, kbD4, kbA8, kbD0 },
        { 0, VK_SUBTRACT, kbA14, kbD3, kbA8, kbD0 },
        { 0, VK_ADD, kbA14, kbD2, kbA8, kbD0 },
        { 0, VK_DECIMAL , kbA15, kbD1, 255,255 },


        { 1, VK_OEM_1, kbA8, kbD2, kbA8, kbD0 },
        { 2, VK_OEM_1, kbA8, kbD1, kbA8, kbD0 },
        { 1, VK_OEM_MINUS, kbA14, kbD3, kbA8, kbD0 },

        { 1, VK_OEM_PLUS, kbA14, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_PLUS, kbA14, kbD2, kbA8, kbD0 },

        { 1, VK_OEM_COMMA, kbA15, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },

        { 1, VK_OEM_PERIOD , kbA15, kbD1, 255,255 },
        { 2, VK_OEM_PERIOD , kbA15, kbD2, kbA8, kbD0 },
        { 1, VK_OEM_2 , kbA8, kbD4, kbA8, kbD0 },
        { 2, VK_OEM_2 , kbA8, kbD3, kbA8, kbD0 },
        { 0, VK_OEM_4 , kbA13, kbD2, kbA8, kbD0 },
        { 0, VK_OEM_6 , kbA13, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_7 , kbA13, kbD0, kbA8, kbD0 },
        { 0, VK_OEM_3 , kbA13, kbD0, kbA8, kbD0 },

        //{ 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },
        //{ 2, VK_OEM_PERIOD, kbA15, kbD2, kbA8, kbD0 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },

        { 0, VK_CONTROL , kbA14, kbD0, kbA8 , kbD0 },

        {0, 0, 0, 0, 0, 0 }
};

const struct kb KBZX80[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD4, 255,255 },
        { 0, 'C' , kbA8, kbD3, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD2, 255,255 },
        { 0, 'N' , kbA15, kbD3, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA8, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD2, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD1, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 0, VK_NUMPAD1 , kbA11, kbD0, 255,255 },
        { 0, VK_NUMPAD2 , kbA11, kbD1, 255,255 },
        { 0, VK_NUMPAD3 , kbA11, kbD2, 255,255 },
        { 0, VK_NUMPAD4 , kbA11, kbD3, 255,255 },
        { 0, VK_NUMPAD5 , kbA11, kbD4, 255,255 },
        { 0, VK_NUMPAD6 , kbA12, kbD4, 255,255 },
        { 0, VK_NUMPAD7 , kbA12, kbD3, 255,255 },
        { 0, VK_NUMPAD8 , kbA12, kbD2, 255,255 },
        { 0, VK_NUMPAD9 , kbA12, kbD1, 255,255 },
        { 0, VK_NUMPAD0 , kbA12, kbD0, 255,255 },

        { 0, VK_MULTIPLY , kbA8, kbD4, 255,255 },
        { 0, VK_DIVIDE , kbA15, kbD4, 255,255 },

        { 1, VK_OEM_1, kbA8, kbD2, kbA8, kbD0 },
        { 2, VK_OEM_1, kbA8, kbD1, kbA8, kbD0 },
        { 1, VK_OEM_MINUS, kbA14, kbD3, kbA8, kbD0 },
        { 1, VK_SUBTRACT, kbA14, kbD3, kbA8, kbD0 },

        { 1, VK_OEM_PLUS, kbA14, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_PLUS, kbA14, kbD2, kbA8, kbD0 },
        { 2, VK_ADD, kbA14, kbD2, kbA8, kbD0 },

        { 1, VK_OEM_COMMA, kbA15, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },

        { 1, VK_OEM_PERIOD , kbA15, kbD1, 255,255 },
        { 1, VK_DECIMAL , kbA15, kbD1, 255,255 },
        { 2, VK_OEM_PERIOD , kbA15, kbD2, kbA8, kbD0 },
        { 1, VK_OEM_2 , kbA8, kbD4, kbA8, kbD0 },
        { 2, VK_OEM_2 , kbA8, kbD3, kbA8, kbD0 },
        { 0, VK_OEM_4 , kbA13, kbD2, kbA8, kbD0 },
        { 0, VK_OEM_6 , kbA13, kbD1, kbA8, kbD0 },
        { 2, VK_OEM_7 , kbA13, kbD4, kbA8, kbD0 },

        //{ 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },
        //{ 2, VK_OEM_PERIOD, kbA15, kbD2, kbA8, kbD0 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },

        { 0, VK_CONTROL , kbA14, kbD0, kbA8 , kbD0 },

        {0, 0, 0, 0, 0, 0 }
};

const struct kb KBACE[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_CONTROL , kbA8, kbD1, 255,255 },

        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD3, 255,255 },
        { 0, 'C' , kbA8, kbD4, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD1, 255,255 },
        { 0, 'N' , kbA15, kbD2, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA15, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD3, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD2, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 1, VK_OEM_1, kbA13, kbD1, kbA8, kbD1 },
        { 2, VK_OEM_1, kbA8, kbD2, kbA8, kbD1 },
        //{ 1, VK_OEM_3, kbA13, kbD1, kbA8, kbD1 },
        { 2, VK_OEM_3, kbA11, kbD1, kbA8, kbD1 },
        { 1, VK_OEM_7, kbA11, kbD2, kbA8, kbD1 },
        { 2, VK_OEM_7, kbA9, kbD0, kbA8, kbD1 },
        { 1, VK_OEM_4, kbA13, kbD4, kbA8, kbD1 },
        { 1, VK_OEM_6, kbA13, kbD3, kbA8, kbD1 },
        { 2, VK_OEM_4, kbA9, kbD3, kbA8, kbD1 },
        { 2, VK_OEM_6, kbA9, kbD4, kbA8, kbD1 },
        { 1, VK_OEM_MINUS, kbA14, kbD3, kbA8, kbD1 },
        { 2, VK_OEM_MINUS, kbA12, kbD0, kbA8, kbD1 },
        { 1, VK_OEM_PLUS, kbA14, kbD1, kbA8, kbD1 },
        { 2, VK_OEM_PLUS, kbA14, kbD2, kbA8, kbD1 },
        { 1, VK_OEM_MINUS, kbA14, kbD3, kbA8, kbD1 },
        { 2, VK_OEM_MINUS, kbA12, kbD0, kbA8, kbD1 },
        { 1, VK_OEM_COMMA, kbA15, kbD2, kbA8, kbD1 },
        { 1, VK_OEM_PERIOD, kbA15, kbD1, kbA8, kbD1 },
        { 2, VK_OEM_COMMA, kbA10, kbD3, kbA8, kbD1 },
        { 2, VK_OEM_PERIOD, kbA10, kbD4, kbA8, kbD1 },
        { 1, VK_OEM_2, kbA15, kbD4, kbA8, kbD1 },
        { 2, VK_OEM_2, kbA8, kbD4, kbA8, kbD1 },
        { 1, VK_OEM_5, kbA9, kbD2, kbA8, kbD1 },
        { 2, VK_OEM_5, kbA9, kbD1, kbA8, kbD1 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },
        {0, 0, 0, 0, 0, 0 }
};

const struct kb KBLAMBDA[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD4, 255,255 },
        { 0, 'C' , kbA8, kbD3, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD2, 255,255 },
        { 0, 'N' , kbA15, kbD3, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA8, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD2, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD1, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 0, VK_NUMPAD1 , kbA11, kbD0, 255,255 },
        { 0, VK_NUMPAD2 , kbA11, kbD1, 255,255 },
        { 0, VK_NUMPAD3 , kbA11, kbD2, 255,255 },
        { 0, VK_NUMPAD4 , kbA11, kbD3, 255,255 },
        { 0, VK_NUMPAD5 , kbA11, kbD4, 255,255 },
        { 0, VK_NUMPAD6 , kbA12, kbD4, 255,255 },
        { 0, VK_NUMPAD7 , kbA12, kbD3, 255,255 },
        { 0, VK_NUMPAD8 , kbA12, kbD2, 255,255 },
        { 0, VK_NUMPAD9 , kbA12, kbD1, 255,255 },
        { 0, VK_NUMPAD0 , kbA12, kbD0, 255,255 },

        { 0, VK_MULTIPLY , kbA8, kbD4, 255,255 },
        { 0, VK_DIVIDE , kbA15, kbD4, 255,255 },

        { 1, VK_OEM_1, kbA12, kbD4, kbA8, kbD0 },
        //{ 2, VK_OEM_1, kbA8, kbD1, kbA8, kbD0 }, // No : on the lambda
        { 1, VK_OEM_MINUS, kbA14, kbD2, kbA8, kbD0 },
        { 1, VK_SUBTRACT, kbA14, kbD2, kbA8, kbD0 },

        { 1, VK_OEM_PLUS, kbA12, kbD0, kbA8, kbD0 },
        { 2, VK_OEM_PLUS, kbA14, kbD1, kbA8, kbD0 },
        { 2, VK_ADD, kbA14, kbD2, kbA8, kbD0 },

        { 1, VK_OEM_COMMA, kbA12, kbD3, kbA8, kbD0 },
        { 2, VK_OEM_COMMA, kbA13, kbD1, kbA8, kbD0 },

        { 1, VK_OEM_PERIOD , kbA15, kbD1, 255,255 },
        { 1, VK_DECIMAL , kbA15, kbD1, 255,255 },
        { 2, VK_OEM_PERIOD , kbA13, kbD0, kbA8, kbD0 },
        { 1, VK_OEM_2 , kbA14, kbD4, kbA8, kbD0 },

        { 0, VK_OEM_4 , kbA12, kbD2, kbA8, kbD0 },
        { 0, VK_OEM_6 , kbA12, kbD1, kbA8, kbD0 },

        // { 2, VK_OEM_2 , kbA8, kbD3, kbA8, kbD0 }, // Lambda has no ? symbol!
        { 2, VK_OEM_7 , kbA11, kbD4, kbA8, kbD0 },

        //{ 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },
        //{ 2, VK_OEM_PERIOD, kbA15, kbD2, kbA8, kbD0 },

        { 0, VK_BACK , kbA15, kbD1, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },

        { 0, VK_CONTROL , kbA14, kbD0, kbA8 , kbD0 },

        {0, 0, 0, 0, 0, 0 }
};

const struct kb KBSPEC[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD4, 255,255 },
        { 0, 'C' , kbA8, kbD3, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD2, 255,255 },
        { 0, 'N' , kbA15, kbD3, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA8, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD2, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD1, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 0, VK_NUMPAD1 , kbA11, kbD0, 255,255 },
        { 0, VK_NUMPAD2 , kbA11, kbD1, 255,255 },
        { 0, VK_NUMPAD3 , kbA11, kbD2, 255,255 },
        { 0, VK_NUMPAD4 , kbA11, kbD3, 255,255 },
        { 0, VK_NUMPAD5 , kbA11, kbD4, 255,255 },
        { 0, VK_NUMPAD6 , kbA12, kbD4, 255,255 },
        { 0, VK_NUMPAD7 , kbA12, kbD3, 255,255 },
        { 0, VK_NUMPAD8 , kbA12, kbD2, 255,255 },
        { 0, VK_NUMPAD9 , kbA12, kbD1, 255,255 },
        { 0, VK_NUMPAD0 , kbA12, kbD0, 255,255 },

        { 0, VK_MULTIPLY , kbA8, kbD4, 255,255 },
        { 0, VK_DIVIDE , kbA15, kbD4, 255,255 },

        { 1, VK_OEM_1, kbA13, kbD1, kbA15, kbD1 },
        { 2, VK_OEM_1, kbA8, kbD1, kbA15, kbD1 },
        { 1, VK_OEM_MINUS, kbA14, kbD3, kbA15, kbD1 },
        { 2, VK_OEM_MINUS, kbA12, kbD0, kbA15, kbD1 },
        { 1, VK_SUBTRACT, kbA14, kbD3, kbA15, kbD1 },

        { 1, VK_OEM_PLUS, kbA14, kbD1, kbA15, kbD1 },
        { 2, VK_OEM_PLUS, kbA14, kbD2, kbA15, kbD1 },
        { 2, VK_ADD, kbA14, kbD2, kbA15, kbD1 },

        { 1, VK_OEM_COMMA, kbA15, kbD3, kbA15, kbD1 },
        { 2, VK_OEM_COMMA, kbA10, kbD3, kbA15, kbD1 },

        { 1, VK_OEM_PERIOD , kbA15, kbD2, kbA15, kbD1 },
        { 2, VK_OEM_PERIOD , kbA10, kbD4, kbA15, kbD1 },
        { 1, VK_DECIMAL , kbA15, kbD1, 255,255 },
        { 1, VK_OEM_2 , kbA8, kbD4, kbA15, kbD1 },
        { 2, VK_OEM_2 , kbA8, kbD3, kbA15, kbD1 },
        { 1, VK_OEM_3 , kbA12, kbD3, kbA15, kbD1 },
        { 2, VK_OEM_3 , kbA11, kbD1, kbA15, kbD1 },
        { 0, VK_OEM_4 , kbA12, kbD2, kbA15, kbD1 },
        { 0, VK_OEM_6 , kbA12, kbD1, kbA15, kbD1 },
        { 1, VK_OEM_7 , kbA11, kbD2, kbA15, kbD1 },
        { 2, VK_OEM_7 , 0, 0, kbA15, kbD1 },

        //{ 2, VK_OEM_COMMA, kbA15, kbD3, kbA8, kbD0 },
        //{ 2, VK_OEM_PERIOD, kbA15, kbD2, kbA8, kbD0 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },

        { 0, VK_CONTROL , kbA15, kbD1, 255,255 },
        { 0, VK_CAPITAL , kbA11, kbD1, kbA8, kbD0 },

        {0, 0, 0, 0, 0, 0 }
};
//---------------------------------------------------------------------------

#pragma package(smart_init)

void PCKbInit(void)
{
        PCAllKeysUp();

        switch(emulator.machine)
        {
        case MACHINEZX80:
                SetKeyMap(KBZX80);
                break;

        case MACHINESPECTRUM:
                SetKeyMap(KBSPEC);
                break;

        case MACHINEACE:
                SetKeyMap(KBACE);
                break;

        case MACHINELAMBDA:
                SetKeyMap(KBLAMBDA);
                break;

        default:
                SetKeyMap(KBZX81);
                break;
        }

        AdjustLocalKeyboard();
}

void SetKeyMap(const kb *source)
{
        int i=0;

        while (source[i].WinKey)
        {
                KeyMap[i]=source[i];
                i++;
        }
        KeyMap[i]=source[i];
}

int PCFindKey(Word key, bool shift)
{
        int i=0;

        while (KeyMap[i].WinKey)
        {
                if (KeyMap[i].WinKey == key)
                {
                        if ((KeyMap[i].Shift==2 && shift) || (KeyMap[i].Shift<2 && !shift))
                                return(i);
                }
                i++;
        }

        return(-1);
}

int PCFindKey(Word key)
{
        return PCFindKey(key,false);
}

void PCSetKey(WORD dest, int source, int shift)
{
        int d;

        d=PCFindKey(dest);

        if (d!=-1)
        {
                KeyMap[d].Addr1 = KeyMap[source].Addr1;
                KeyMap[d].Data1 = KeyMap[source].Data1;

                if (shift)
                {
                        KeyMap[d].Addr2 = kbA8;
                        KeyMap[d].Data2 = kbD0;
                }
                else
                {
                        KeyMap[d].Addr2 = 255;
                        KeyMap[d].Data2 = 255;
                }
        }
}

void PCKeySetCTRL(char key)
{
        int Kctrl;

        if (emulator.machine==MACHINESPECTRUM
                || emulator.machine==MACHINEACE) return;

        if (!key)
        {
                Kctrl=PCFindKey(VK_RETURN);
                PCSetKey(VK_CONTROL,Kctrl,1);
        }
        else
        {
                Kctrl=PCFindKey(key);
                PCSetKey(VK_CONTROL,Kctrl,0);
        }
}

void PCKeySetCursor(char left, char down, char up, char right, int shift)
{
        int Kleft,Kdown, Kright, Kup;
 
        Kleft=PCFindKey(left);
        Kdown=PCFindKey(down);
        Kup=PCFindKey(up);
        Kright=PCFindKey(right);

        PCSetKey(VK_LEFT,Kleft,shift);
        PCSetKey(VK_DOWN,Kdown,shift);
        PCSetKey(VK_UP,Kup,shift);
        PCSetKey(VK_RIGHT,Kright,shift);
}

void PCKeyDown(WORD key)
{
        int i=0;
        if (key==VK_SHIFT) PCShift=2;
        PCALT=(GetKeyState(VK_MENU)&128);

        if (PCALT) return;
        while (KeyMap[i].WinKey)
        {
                if ((KeyMap[i].WinKey == key) &&
                        ((KeyMap[i].Shift==PCShift) || (KeyMap[i].Shift==0)))
                {
                        if (key>=VK_OEM_1 && key<=VK_OEM_7 && emulator.machine==MACHINESPECTRUM)
                                ZXKeyboard[kbA8] &= ~kbD0;

                        ZXKeyboard[KeyMap[i].Addr1] |= KeyMap[i].Data1;
                        if (KeyMap[i].Addr2!=255)
                                ZXKeyboard[KeyMap[i].Addr2] |= KeyMap[i].Data2;
                        return;
                }
                i++;
        }
}

void PCKeyUp(WORD key)
{
        int i=0;

        if (key==VK_SHIFT) PCShift=1;

        while (KeyMap[i].WinKey)
        {
                if (KeyMap[i].WinKey == key) /* &&
                        ((KeyMap[i].Shift==PCShift) || (KeyMap[i].Shift==0))) */
                {
                        ZXKeyboard[KeyMap[i].Addr1] &= (BYTE)(~KeyMap[i].Data1);
                        if (KeyMap[i].Addr2!=255)
                                ZXKeyboard[KeyMap[i].Addr2] &= (BYTE)(~KeyMap[i].Data2);

                        //if (PCShift==2) ZXKeyboard[kbA8] |= kbD0;
                }
                i++;
        }
        if (PCShift==2) ZXKeyboard[kbA8] |= kbD0;
}

void PCAllKeysUp()
{
        int i;
        for(i=0; i<8; i++) ZXKeyboard[i]=0;
}

void AdjustLocalKeyboard()
{
        char keyboardName[KL_NAMELENGTH];
        if (GetKeyboardLayoutName(keyboardName))
        {
                unsigned int lcid=0;
                sscanf(keyboardName,"%x",&lcid);

                switch (lcid)
                {
                case 0x00000404: // Chinese (Traditional) - US
                case 0x00000409: // US
                case 0x00000804: // Chinese (Simplified) - US
                case 0x00000c04: // Chinese (Traditional, Hong Kong S.A.R.) - US
                case 0x00001004: // Chinese (Simplified, Singapore) - US
                case 0x00001404: // Chinese (Traditional, Macao S.A.R.) - US
                case 0x00010402: // Bulgarian (Latin)
                case 0x00020409: // United States-International
                        if (emulator.machine==MACHINESPECTRUM)
                        {
                                int source=PCFindKey(VK_OEM_3);
                                int dest=PCFindKey(VK_OEM_7);
                                KeyMap[dest].Addr1 = KeyMap[source].Addr1;
                                KeyMap[dest].Data1 = KeyMap[source].Data1;

                                source=PCFindKey('P');
                                dest=PCFindKey(VK_OEM_7,true);
                                KeyMap[dest].Addr1 = KeyMap[source].Addr1;
                                KeyMap[dest].Data1 = KeyMap[source].Data1;

                                dest=PCFindKey(VK_OEM_3,true);
                                KeyMap[dest].Addr1 = 0;
                                KeyMap[dest].Data1 = 0;
                        }
                        break;

                default:
                        break;
                }
        }
}
