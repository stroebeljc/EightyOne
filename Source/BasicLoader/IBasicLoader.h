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
#include <vector>

using namespace std;

class IBasicLoader
{
private:
        struct LineEntry
        {
                string line;
                string lineLabel;
                int lineNumber;
                int lineNumberLength;
        };

        unsigned char mEscapeChar;

        unsigned char ConvertFromHexChar(unsigned char chr);
        bool ReadLine(ifstream& basicFile, string& line);
        void ReadBasicListingFile(AnsiString filename);
        bool GetLineNumber(LineEntry& lineEntry);
        bool GetLineLabel(LineEntry& lineEntry);
        void ReplaceLabel();

public:
        void LoadBasicFile(AnsiString filename, bool tokeniseRemContents, bool tokeniseStrings, bool discardRedundantSpaces);
        unsigned char* ProgramData();
        int ProgramLength();

protected:
        static const unsigned char Blank = 0x01;
        static const maxProgramLength = 49152;
        static const maxLineLength = 16384 * 8;  // Allow 16384 of 8 character tokens

        int mProgramLength;
        vector<LineEntry> mLines;
        unsigned char mLineBuffer[maxLineLength];
        unsigned char mLineBufferTokenised[maxLineLength];
        unsigned char mLineBufferOutput[maxLineLength];
        unsigned char mLineBufferStrings[maxLineLength];
        bool mLineBufferPopulated[maxLineLength];
        unsigned char mProgramData[maxProgramLength];
        unsigned char mCharacterCodeEscape;
        void BlankLineStart(LineEntry lineEntry);

        void OutputByte(int& addressOffset, unsigned char byte);
        void OutputWord(int& addressOffset, int word);
        void ChangeWord(int addressOffset, int word);
        void ProcessLine(LineEntry lineEntry, int& addressOffset, bool tokeniseRemContents, bool tokeniseStrings, bool discardRedundantSpaces);
        void MaskOutRemContents(unsigned char* buffer);
        unsigned char* ExtractLineNumber(int& lineNumber);
        void DoTokenise(map<unsigned char, string> tokens);
        bool StartOfNumber(int index);
        unsigned char ConvertFromHexChars(unsigned char chr1, unsigned char chr2);
        void ExtractEscapeCharacters();
        void MaskOutStrings(unsigned char* buffer);
        void ExtractSingleCharacters(bool discardRedundantSpaces);
        void OutputEmbeddedNumber(int& index, int& addressOffset);
        unsigned char DecodeCharacter(unsigned char** ppPos);
        void HandleTokenLineNumber(unsigned char* pStartToken, unsigned char* pLabelSearch, int outputIndex);
        int FindLabelDetails(string& label);
        string ExtractLabel(unsigned char* pLabelSearch);

        virtual unsigned char DecodeGraphic(unsigned char chr1, unsigned char chr2) { return '\0'; }
        virtual unsigned char AsciiToZX(unsigned char ascii) { return '\0'; }
        virtual void ExtractTokens() {}
        virtual void OutputLine(int lineNumber, int& address) {}
        virtual void OutputStartOfProgramData(AnsiString filename, int& addressOffset) {}
        virtual void OutputEndOfProgramData(int& addressOffset) {}
        virtual bool SupportUppercaseOnly() { return false; }
        virtual void ExtractInverseCharacters() {}
        virtual bool SingleEscapeSequence(unsigned char chr, unsigned char& zxChr) { return false; }
        virtual unsigned char GetEscapeCharacter() { return '\0'; }
        virtual void ExtractDoubleQuoteCharacters() {}
        virtual bool SupportFloatingPointNumbers() { return false; }
        virtual unsigned char GetEmbbededNumberMark() { return '\0'; }
        virtual void OutputFloatingPointEncoding(double value, int& addressOffset) {}
        virtual bool TokenSupportsLineNumber(unsigned char chr) { return false; }
};

#endif
