/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2019 Michael D Wynne
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

#ifndef ZX81BASICLOADER_H
#define ZX81BASICLOADER_H

#include "BasicLoader/IBasicLoader.h"
#include <vcl.h>
#include <string>

using namespace::std;

class zx81BasicLoader : public IBasicLoader
{
private:
        static const unsigned char Space = 0x00;
        static const unsigned char Newline = 0x76;
        static const unsigned char Number = 0x7E;
        static const unsigned char Rnd = 0x40;
        static const unsigned char Inkey = 0x41;
        static const unsigned char Pi = 0x42;
        static const unsigned char Rem = 0xEA;

        void OutputSystemVariables(int& addressOffset);
        void OutputEmbeddedNumber(unsigned char** ppPos, int& addressOffset);
        bool StartOfNumber(unsigned char* pPos);
        void OutputFloatingPointEncoding(double value, int& addressOffset);        
        virtual unsigned char DecodeGraphic(unsigned char chr1, unsigned char chr2);
        virtual unsigned char AsciiToZX(unsigned char ascii);
        virtual void Tokenise();
        virtual void OutputLine(int lineNumber, int& address, unsigned char* pPos);
        virtual void OutputStartOfProgramData(int& addressOffset);
        virtual void OutputEndOfProgramData(int& addressOffset);
        virtual void ReplaceStrings();
};

#endif
