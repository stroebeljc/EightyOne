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

#include <string>
#include <vector>
#include "BasicLister\IBasicLister.h"
#include <Classes.hpp>

class zx81BasicLister : public IBasicLister
{
public:
        zx81BasicLister();
        virtual AnsiString GetMachineName();
        virtual AnsiString GetBasicFileExtension();

private:
        static const int Space = 0;
        static const int Quote = 11;
        static const int Dollar = 13;
        static const int GreaterThan = 18;
        static const int LessThan = 19;
        static const int Equal = 20;
        static const int Asterisk = 23;
        static const int Number0 = 28;
        static const int LetterA = 38;
        static const int Newline = 118;
        static const int Number = 126;

        virtual int GetProgramStartAddress();
        virtual int GetProgramEndAddress();
        virtual int ConvertToZXCode(int code);
        virtual inline int GetFloatingPointNumberCode();
        virtual inline int GetLineEndingCode();
        virtual std::string GetKeywords();
        virtual bool RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes);
};

