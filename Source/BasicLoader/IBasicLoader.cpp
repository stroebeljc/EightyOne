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
        OutputStartOfProgramData(addressOffset);

        string line;
        int lineCount = 1;

        while (getline(basicFile, line))
        {
                try
                {
                        size_t i = line.find_first_not_of(" \t");
                        if ((i != string::npos) && (line[i] != '\0') && (line[i] != '#'))
                        {
                                ProcessLine(line, addressOffset, tokeniseRemContents, tokeniseStrings);
                                lineCount++;
                        }
                }
                catch (out_of_range& ex)
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

void IBasicLoader::OutputWord(int& addressOffset, int word)
{
        mProgramData[addressOffset++] = word & 0xFF;
        mProgramData[addressOffset++] = word >> 8;
}

void IBasicLoader::ChangeWord(int addressOffset, int word)
{
        mProgramData[addressOffset] = word & 0xFF;
        mProgramData[addressOffset+1] = word >> 8;
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

        int lineNumber;
        char* pPos = ExtractLineNumber(lineNumber);
        memset(mLineBuffer, 0x01, pPos - mLineBuffer);

        memset(mLineBufferDestringed, 0, sizeof(mLineBufferDestringed));
        strcpy((char*)mLineBufferDestringed, (char*)mLineBuffer);

        if (!tokeniseStrings)
        {
                ReplaceStrings();
        }

        memset(mLineBufferTokenised, 0, sizeof(mLineBufferTokenised));
        strcpy((char*)mLineBufferTokenised, (char*)mLineBufferDestringed);

        // Append a space at the end of the line to ensure a token without a trailing space is detected
        int e = strlen((char*)mLineBufferTokenised);
        mLineBufferDestringed[e] = ' ';
        mLineBufferTokenised[e] = Blank;

        if (!tokeniseRemContents)
        {
                ReplaceRemContents();
        }

        Tokenise();          

        OutputLine(lineNumber, addressOffset, mLineBufferDestringed);
}

void IBasicLoader::ReplaceStrings()
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

void IBasicLoader::ReplaceRemContents()
{
        char* remToken = " REM ";

        unsigned char* pMatch = strstr((char*)mLineBufferDestringed, remToken);
        if (pMatch == NULL)
        {
                return;
        }

        char* pContents = pMatch + strlen(remToken);
        memset(pContents, ' ', strlen(pContents));
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

        if (*pPos == '%')
        {
                pPos++;

                if (*pPos == PoundReplacement)
                {
                        zxChr = 0x80 | AsciiToZX('£');
                }
                else
                {
                        zxChr = 0x80 | AsciiToZX(*pPos);
                }
        }
        else if (*pPos == '\\')
        {
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
        }

        *ppPos = pPos;
        
        return zxChr;
}

unsigned char IBasicLoader::ConvertFromHexChars(unsigned char chr1, unsigned char chr2)
{
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
                                        mLineBufferTokenised[offset + b] = Blank;
                                }

                                mLineBufferTokenised[offset + start] = tokenCode;
                                mLineBufferDestringed[offset + start] = tokenCode;
                        }
                }
                while (tokenFound);
        }

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
}


