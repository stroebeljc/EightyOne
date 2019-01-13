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

#include "BasicLoader/IBasicLoader.h"
#include <fstream>
#include <sstream>
#include <cctype>

void IBasicLoader::LoadBasicFile(AnsiString filename, bool tokeniseRemContents, bool tokeniseStrings)
{
        mProgramLength = 0;

        ifstream basicFile(filename.c_str());
        if (!basicFile)
        {
                Application->MessageBox("File not found.", "Load BASIC Listing", MB_OK);
                return;
        }

        memset(mProgramData, 0, sizeof(mProgramData));

        int addressOffset = 0x0000;
        OutputStartOfProgramData(filename, addressOffset);

        mCharacterCodeEscape = GetEscapeCharacter();
        
        int lineCount = 1;
        string line;

        while (ReadLine(basicFile, line))
        {
                line = line.substr(0, line.length()-1);
                
                try
                {
                        size_t i = line.find_first_not_of(" \t");
                        if ((i != string::npos) && (line[i] != '\0') && (line[i] != '#'))
                        {
                                ProcessLine(line, addressOffset, tokeniseRemContents, tokeniseStrings);
                                lineCount++;
                        }
                }
                catch (exception& ex)
                {
                        stringstream msg;
                        msg << "Unable to parse line " << lineCount << " - " << ex.what() << endl;
                        msg << endl;
                        msg << line;
                        Application->MessageBox(msg.str().c_str(), "Load BASIC Listing", MB_OK);
                        return;
                }
        }

        OutputEndOfProgramData(addressOffset);

        mProgramLength = addressOffset;
}

bool IBasicLoader::ReadLine(ifstream& basicFile, string& line)
{
        bool lineAvailable = (getline(basicFile, line) != NULL);
        if (!lineAvailable)
        {
                return lineAvailable;
        }

        if (SupportLineContinuations())
        {
                while (line[line.length()-2] == '\\')
                {
                        string contLine;
                        lineAvailable = (getline(basicFile, contLine) != NULL);
                        if (!lineAvailable)
                        {
                                break;
                        }

                        line = line.substr(0, line.length()-2) + contLine;
                }
        }
        
        return lineAvailable;
}

void IBasicLoader::OutputByte(int& addressOffset, unsigned char byte)
{
        if (addressOffset >= maxProgramLength)
        {
                throw out_of_range("Program size too large");
        }

        mProgramData[addressOffset++] = byte;
}

void IBasicLoader::OutputWord(int& addressOffset, int word)
{
        OutputByte(addressOffset, word & 0xFF);
        OutputByte(addressOffset, word >> 8);
}

void IBasicLoader::ChangeWord(int addressOffset, int word)
{
        OutputWord(addressOffset, word);
}

unsigned char* IBasicLoader::ProgramData()
{
        return mProgramData;
}

int IBasicLoader::ProgramLength()
{
        return mProgramLength;
}

void IBasicLoader::ProcessLine(string line, int& addressOffset, bool tokeniseRemContents, bool tokeniseStrings)
{
        if (SupportUppercaseOnly())
        {
                for (string::iterator c = line.begin(); c != line.end(); ++c)
                {
                        *c = toupper(*c);
                }
        }

        memset(mLineBuffer, 0, sizeof(mLineBuffer));
        strcpy((char*)mLineBuffer, line.c_str());

        memset(mLineBufferOutput, 0, sizeof(mLineBufferOutput));
        int i = strlen((char*)mLineBuffer);
        memset(mLineBufferOutput, Blank, i);

        memset(mLineBufferPopulated, false, sizeof(mLineBufferPopulated));

        int lineNumber;
        char* pPos = ExtractLineNumber(lineNumber);
        memset(mLineBuffer, Blank, pPos - mLineBuffer);

        ExtractInverseCharacters();
        ExtractEscapeCharacters();
        ExtractDoubleQuoteCharacters();

        memset(mLineBufferDestringed, 0, sizeof(mLineBufferDestringed));
        strcpy((char*)mLineBufferDestringed, (char*)mLineBuffer);

        // Append a space at the end of the line to ensure a token without a trailing space is detected
        i = strlen((char*)mLineBufferDestringed);
        mLineBufferDestringed[i] = ' ';

        if (!tokeniseStrings)
        {
                MaskOutStrings();
        }

        if (!tokeniseRemContents)
        {
                MaskOutRemContents();
        }

        ExtractTokens();

        ExtractSingleCharacters();

        OutputLine(lineNumber, addressOffset);
}

void IBasicLoader::ExtractSingleCharacters()
{
        int i = 0;

        while (mLineBuffer[i] != '\0')
        {
                if (mLineBuffer[i] != Blank)
                {
                        mLineBufferOutput[i] = AsciiToZX(mLineBuffer[i]);
                        mLineBufferPopulated[i] = true;
                }

                i++;
        }
}

void IBasicLoader::MaskOutStrings()
{
        const int Quote = '\"';

        char* pPos = mLineBufferDestringed;
        bool withinQuote = false;

        while (*pPos != '\0')
        {
                if (*pPos == Quote)
                {
                        withinQuote = !withinQuote;
                }
                else if (withinQuote)
                {
                        *pPos = ' '; 
                }

                pPos++;
        }
}

void IBasicLoader::MaskOutRemContents()
{
        const char* const remToken = " REM ";

        unsigned char* pMatch = strstr((char*)mLineBufferDestringed, remToken);
        if (pMatch != NULL)
        {
                char* pContents = pMatch + strlen(remToken);
                memset(pContents, ' ', strlen(pContents));
        }
}

unsigned char* IBasicLoader::ExtractLineNumber(int& lineNumber)
{
        unsigned char* pCommand;

        lineNumber = strtol((char*)mLineBuffer, &(char*)pCommand, 10);

        return pCommand;
}

unsigned char IBasicLoader::DecodeCharacter(unsigned char** ppPos)
{
        unsigned char zxChr;

        unsigned char* pPos = *ppPos;
        
        unsigned char chr1 = *(++pPos);
        unsigned char chr2 = *(++pPos);

        if (isxdigit(chr1) && isxdigit(chr2))
        {
                zxChr = ConvertFromHexChars(chr1, chr2);
        }
        else
        {
                zxChr = DecodeGraphic(chr1, chr2);
        }

        *ppPos = pPos;
        
        return zxChr;
}

bool IBasicLoader::StartOfNumber(int index)
{
        char chrPrev = mLineBuffer[index-1];
        char chr1 = mLineBuffer[index];
        char chr2 = mLineBuffer[index+1];
        char chr3 = mLineBuffer[index+2];

        bool partOfVariableName = isalpha(chrPrev);
        bool beginsWithDigit = isdigit(chr1);
        bool plusMinusOrDecimal = (chr1 == '-' || chr1 == '+' || chr1 == '.') && isdigit(chr2);
        bool plusMinusAndDecimal = (chr1 == '-' || chr1 == '+') && chr2 == '.' && isdigit(chr3);

        return !partOfVariableName && (beginsWithDigit || plusMinusOrDecimal || plusMinusAndDecimal);
}

void IBasicLoader::OutputEmbeddedNumber(int& index, int& addressOffset)
{
        char* pEnd;
        double value = strtod((char*)(mLineBuffer + index), &pEnd);

        while ((mLineBuffer + index) < pEnd)
        {
                unsigned char chr = mLineBufferOutput[index];
                OutputByte(addressOffset, chr);
                index++;
        }

        while ((mLineBuffer[index] != '\0') && (mLineBuffer[index] == ' '))
        {
                OutputByte(addressOffset, AsciiToZX(' '));
                index++;
        }

        index--;

        OutputByte(addressOffset, GetEmbbededNumberMark());

        OutputFloatingPointEncoding(value, addressOffset);
}

void IBasicLoader::ExtractEscapeCharacters()
{
        unsigned char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                if (*pPos == mCharacterCodeEscape)
                {
                        *pPos = Blank;

                        pPos++;
                        unsigned char chr1 = *pPos;
                        if (chr1 == '\0')
                        {
                                throw out_of_range("Escape sequence incomplete before line ending");
                        }

                        *pPos = Blank;

                        unsigned char zxChr;

                        if (!SingleEscapeSequence(chr1, zxChr))
                        {
                                pPos++;
                                unsigned char chr2 = *pPos;
                                if (chr2 == '\0')
                                {
                                        throw out_of_range("Escape sequence incomplete before line ending");
                                }

                                *pPos = Blank;

                                if (isxdigit(chr1))
                                {
                                        zxChr = ConvertFromHexChars(chr1, chr2);
                                }
                                else
                                {
                                        zxChr = DecodeGraphic(chr1, chr2);
                                }
                        }
                        
                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = zxChr;
                        mLineBufferPopulated[index] = true;
                }

                pPos++;
        }
}

unsigned char IBasicLoader::ConvertFromHexChars(unsigned char chr1, unsigned char chr2)
{
        if (!isxdigit(chr1) || !isxdigit(chr2))
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid character Code: " + code.str();
                throw out_of_range(msg.c_str());
        }
        
        int d1 = ConvertFromHexChar(chr1);
        int d2 = ConvertFromHexChar(chr2);
        return (d1 << 4) + d2;
}

unsigned char IBasicLoader::ConvertFromHexChar(unsigned char chr)
{
        unsigned char val = isdigit(chr) ? chr - '0' : (chr & 0xDF) - 'A' + 10;
        return val;
}

void IBasicLoader::DoTokenise(map<unsigned char, string> tokens)
{
        map<unsigned char, string>::reverse_iterator it;

        // Iterate through in reverse numerical order
        for (it = tokens.rbegin(); it != tokens.rend(); it++)
        {
                unsigned char tokenCode = it->first;
                const unsigned char* pToken = (it->second).c_str();
                int lenToken = strlen((char*)pToken);

                unsigned char* pMatch;
                bool tokenFound;

                do
                {
                        bool tokenBeginsWithSpace = (pToken[0] == ' ');
                        bool tokenEndsWithSpace = (pToken[lenToken-1] == ' ');
                        bool tokenBeginsWithAlpha = isalpha(pToken[0]);
                        bool tokenEndsWithAlpha = isalpha(pToken[lenToken-1]);
                        
                        pMatch = strstr((char*)mLineBufferDestringed, (char*)pToken);
                        bool matchFound = (pMatch != NULL);
                        
                        bool startOk = matchFound && (tokenBeginsWithSpace || !tokenBeginsWithAlpha || (!tokenBeginsWithSpace && !isalpha(pMatch[-1])));
                        bool endOk = matchFound && (tokenEndsWithSpace || !tokenEndsWithAlpha || (!tokenEndsWithSpace && !isalpha(pMatch[lenToken])));

                        tokenFound = matchFound && startOk && endOk;

                        if (tokenFound)
                        {
                                int start = tokenBeginsWithSpace? 1 : 0;
                                int end = tokenEndsWithSpace ? lenToken - 1 : lenToken;

                                // Blank the token apart from the leading and/or trailing spaces, which allows
                                // supporting suppressed spaces between tokens
                                for (int b = start; b < end; b++)
                                {
                                        *(pMatch + b) = Blank;
                                }

                                int offset = pMatch - mLineBufferDestringed;

                                for (int b = 0; b < lenToken; b++)
                                {
                                        mLineBuffer[offset + b] = Blank;
                                }

                                mLineBufferOutput[offset + start] = tokenCode;
                                mLineBufferPopulated[offset + start] = true;
                        }
                }
                while (tokenFound);
        }
 /*
        char* pBuffer = mLineBufferTokenised;

        while (*pBuffer != '\0')
        {
                int offset = pBuffer - mLineBufferTokenised;

                if (*pBuffer == Blank)
                {
                        mLineBufferDestringed[offset] = Blank;
                }
                else if (mLineBufferDestringed[offset] == ' ')
                {
                        mLineBufferDestringed[offset] = mLineBuffer[offset];
                }

                pBuffer++;
        }
        */           
}


