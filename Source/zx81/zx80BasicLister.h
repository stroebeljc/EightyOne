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

#include <string>
#include <vector>
#include <map>
#include "BasicLister\IBasicLister.h"
#include <Classes.hpp>

class zx80BasicLister : public IBasicLister
{
public:
        zx80BasicLister(bool zxpandEnabled);
        virtual AnsiString GetMachineName();
        virtual AnsiString GetBasicFileExtension();
        virtual bool ZxTokenSupported();

private:
        static const unsigned char Space = 0;
        static const unsigned char Asterisk = 20;
        static const unsigned char Number0 = 28;
        static const unsigned char LetterA = 38;
        static const unsigned char Newline = 118;
        static const unsigned char Escape = '\\';

        bool mZxpandEnabled;
        std::map<AnsiString, AnsiString> mZxTokenChars;

        virtual int GetProgramStartAddress();
        virtual int GetProgramEndAddress();
        virtual unsigned char ConvertToZXCode(unsigned char code);
        virtual inline bool SupportFloatingPointNumbers();
        virtual inline unsigned char GetLineEndingCode();
        virtual std::string GetKeywords();
        virtual bool ExtractLineDetails(int* address, LineInfo& lineInfo);
        virtual bool RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes);
        virtual AnsiString TranslateToZxToken(AnsiString chr);
};

