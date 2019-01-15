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

#ifndef SPECBASICLOADER_H
#define SPECBASICLOADER_H

#include "BasicLoader/IBasicLoader.h"
#include <vcl.h>
#include <string>

using namespace::std;

class specBasicLoader : public IBasicLoader
{
private:
        static const unsigned char Return = 0x0D;
        static const unsigned char Number = 0x0E;
        static const unsigned char Pound = 0x60;
        static const unsigned char Copyright = 0x7F;
        static const unsigned char Rem = 0xEA;
        static const unsigned char Escape = 0xAC;
        
        void OutputSystemVariables(int& addressOffset);

        virtual void OutputFloatingPointEncoding(double value, int& addressOffset);
        virtual unsigned char DecodeGraphic(unsigned char chr1, unsigned char chr2);
        virtual void ExtractTokens();
        virtual void OutputLine(int lineNumber, int& addressOffset);
        virtual void OutputStartOfProgramData(AnsiString filename, int& addressOffset);
        virtual void OutputEndOfProgramData(int& addressOffset);
        virtual bool SupportUppercaseOnly();
        virtual unsigned char GetEscapeCharacter();
        virtual unsigned char GetEmbbededNumberMark();
        virtual unsigned char AsciiToZX(unsigned char ascii);
};

#endif
