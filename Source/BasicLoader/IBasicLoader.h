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

#ifndef IBASICLOADER_H
#define IBASICLOADER_H

#include <vcl.h>
#include <string>
#include <map>

using namespace std;

class IBasicLoader
{
public:
        void LoadBasicFile(AnsiString filename, bool tokeniseRemContents, bool tokeniseStrings);
        unsigned char* ProgramData();
        int ProgramLength();

protected:
        static const unsigned char Blank = 0x01;
        static const unsigned char PoundReplacement = 0xC3;

        int mProgramLength;
        unsigned char mLineBuffer[16384];
        unsigned char mLineBufferDestringed[16384];
        unsigned char mLineBufferTokenised[16384];
        unsigned char mProgramData[16384];

        void OutputWord(int& addressOffset, int word);
        void ChangeWord(int addressOffset, int word);
        void ProcessLine(string line, int& addressOffset, bool tokeniseRemContents, bool tokeniseStrings);
        void ReplaceRemContents();
        unsigned char* ExtractLineNumber(int& lineNumber);
        unsigned char DecodeCharacter(unsigned char** ppPos);
        void DoTokenise(map<unsigned char, string> tokens);

        virtual void ReplaceStrings();
        virtual unsigned char DecodeGraphic(unsigned char chr1, unsigned char chr2) { return '\0'; }
        virtual unsigned char AsciiToZX(unsigned char ascii) { return '\0'; }
        virtual void Tokenise() {}
        virtual void OutputLine(int lineNumber, int& address, unsigned char* pPos) {}
        virtual void OutputStartOfProgramData(int& addressOffset) {}
        virtual void OutputEndOfProgramData(int& addressOffset) {}

private:
        unsigned char ConvertFromHexChars(unsigned char chr1, unsigned char chr2);
        unsigned char ConvertFromHexChar(unsigned char chr);
};

#endif
