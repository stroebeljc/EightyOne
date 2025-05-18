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

#include "lambdaBasicLoader.h"
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <sstream>

using namespace std;

lambdaBasicLoader::lambdaBasicLoader(bool zxpandEnabled)
{
        mZxpandEnabled = zxpandEnabled;
}

void lambdaBasicLoader::OutputStartOfProgramData(AnsiString filename, int& addressOffset)
{
        // Output the system variables
        OutputByte(addressOffset, 0xFF);        //VERSN
        OutputWord(addressOffset, 0x407D);      //E_PPC
        OutputWord(addressOffset, 0x4396);      //D_FILE
        OutputWord(addressOffset, 0x407E);      //DF_CC
        OutputWord(addressOffset, 0x0000);      //VARS
        OutputWord(addressOffset, 0x0000);      //DEST
        OutputWord(addressOffset, 0x0000);      //E_LINE
        OutputWord(addressOffset, 0x0000);      //CH_ADD
        OutputWord(addressOffset, 0x0000);      //X_PTR
        OutputWord(addressOffset, 0x0000);      //STKBOT
        OutputWord(addressOffset, 0x0000);      //STKEND
        OutputByte(addressOffset, 0x00);        //BERG
        OutputWord(addressOffset, 0x405D);      //MEM
        OutputByte(addressOffset, 0x00);        //SPARE1
        OutputByte(addressOffset, 0x02);        //DF_SZ
        OutputWord(addressOffset, 0x0000);      //S_TOP
        OutputWord(addressOffset, 0xFFFF);      //LAST_K
        OutputByte(addressOffset, 0x00);        //DBOUNC
        OutputByte(addressOffset, 0x37);        //MARGIN
        OutputWord(addressOffset, 0x0000);      //NXTLIN
        OutputWord(addressOffset, 0x0000);      //OLDPPC
        OutputByte(addressOffset, 0x00);        //FLAGX
        OutputWord(addressOffset, 0x0000);      //STRLEN
        OutputWord(addressOffset, 0x0C8D);      //T_ADDR
        OutputWord(addressOffset, 0x4321);      //SEED
        OutputWord(addressOffset, 0xE6E0);      //FRAMES
        OutputWord(addressOffset, 0x0000);      //COORDS
        OutputByte(addressOffset, 0xBC);        //PR_CC
        OutputWord(addressOffset, 0x1821);      //S_POSN
        OutputByte(addressOffset, 0x40);        //CDFLAG
        addressOffset += 32;                    //PRBUFF
        OutputByte(addressOffset, 0x76);        //PRBUFF
        addressOffset += 30;                    //MEMBOT
        OutputWord(addressOffset, 0x0000);      //SPARE

        // Insert a display file
        for (int d = 0; d < 24; d++)
        {
                OutputByte(addressOffset, Newline);
                for (int j = 0; j < 32; j++)
                {
                        OutputByte(addressOffset, 0x00);
                }
        }
        OutputByte(addressOffset, Newline);
}

void lambdaBasicLoader::OutputEndOfProgramData(int& addressOffset)
{
        const int startOfRam = 16393;

        // Insert the variables area
        int varsAddress = startOfRam + addressOffset;

        OutputWord(addressOffset, 0x80FF);

        int elineAddress = startOfRam + addressOffset;

        // Update system variables
        int varsOffset = 7;
        ChangeWord(varsOffset, varsAddress);
        int elineOffset = 11;
        ChangeWord(elineOffset, varsAddress+1);
        int chaddOffset = 13;
        ChangeWord(chaddOffset, varsAddress+5);
        int stkbotOffset = 17;
        ChangeWord(stkbotOffset, elineAddress+5);
        int stkendOffset = 19;
        ChangeWord(stkendOffset, elineAddress+5);
        int nxtlinOffset = 32;
        ChangeWord(nxtlinOffset, varsAddress);
}

void lambdaBasicLoader::ExtractTokens(bool acceptAlternateKeywordSpelling)
{
        map<unsigned char, string> tokens;

        tokens[64] = " THEN ";
        tokens[65] = " TO ";
        tokens[66] = " STEP ";
        tokens[67] = "RND";
        tokens[68] = "INKEY$";
        tokens[69] = "PI";
        tokens[70] = " INK ";
        tokens[71] = " PAPER ";
        tokens[72] = " BORDER ";
        tokens[192] = "CODE ";
        tokens[193] = "VAL ";
        tokens[194] = "LEN ";
        tokens[195] = "SIN ";
        tokens[196] = "COS ";
        tokens[197] = "TAN ";
        tokens[198] = "ASN ";
        tokens[199] = "ACS ";
        tokens[200] = "ATN ";
        tokens[201] = "LOG ";
        tokens[202] = "EXP ";
        tokens[203] = "INT ";
        tokens[204] = "SQR ";
        tokens[205] = "SGN ";
        tokens[206] = "ABS ";
        tokens[207] = "PEEK ";
        tokens[208] = "USR ";
        tokens[209] = "STR$ ";
        tokens[210] = "CHR$ ";
        tokens[211] = "NOT ";
        tokens[212] = "AT ";
        tokens[213] = "TAB ";
        tokens[214] = "**";
        tokens[215] = " OR ";
        tokens[216] = " AND ";
        tokens[217] = "<=";
        tokens[218] = ">=";
        tokens[219] = "<>";
        tokens[220] = " TEMPO ";
        tokens[221] = " MUSIC ";
        tokens[222] = " SOUND ";
        tokens[223] = " BEEP ";
        tokens[224] = " NOBEEP ";
        tokens[225] = " LPRINT ";
        tokens[226] = " LLIST ";
        tokens[227] = " STOP ";
        tokens[228] = " SLOW ";
        tokens[229] = " FAST ";
        tokens[230] = " NEW ";
        tokens[231] = " SCROLL ";
        tokens[232] = " CONT ";
        tokens[233] = " DIM ";
        tokens[234] = " REM ";
        tokens[235] = " FOR ";
        tokens[236] = " GOTO ";
        tokens[237] = " GOSUB ";
        tokens[238] = " INPUT ";
        tokens[239] = " LOAD ";
        tokens[240] = " LIST ";
        tokens[241] = " LET ";
        tokens[242] = " PAUSE ";
        tokens[243] = " NEXT ";
        tokens[244] = " POKE ";
        tokens[245] = " PRINT ";
        tokens[246] = " PLOT ";
        tokens[247] = " RUN ";
        tokens[248] = " SAVE ";
        tokens[249] = " RAND ";
        tokens[250] = " IF ";
        tokens[251] = " CLS ";
        tokens[252] = " UNPLOT ";
        tokens[253] = " CLEAR ";
        tokens[254] = " RETURN ";
        tokens[255] = " COPY ";
      
        DoTokenise(tokens);

        if (mZxpandEnabled)
        {
                tokens.clear();

                tokens[225] = " ZXPAND ";
                tokens[226] = " CONFIG ";
                tokens[255] = " CAT ";

                DoTokenise(tokens);
        }

        if (acceptAlternateKeywordSpelling)
        {
                ReplaceTokenEndCharacters(tokens, ' ', '(');
                DoTokenise(tokens);

                ReplaceTokenEndCharacters(tokens, '(', '\"');
                DoTokenise(tokens);

                tokens.clear();

                tokens[215] = "OR ";
                tokens[216] = "AND ";
                tokens[65] = "TO ";

                tokens[232] = " CONTINUE ";
                tokens[236] = " GO TO ";
                tokens[237] = " GO SUB ";
                tokens[249] = " RANDOMISE ";

                DoTokenise(tokens);

                tokens.clear();

                tokens[223] = " TO)";
                tokens[245] = " PRINT,";
                tokens[249] = " RANDOMIZE ";

                DoTokenise(tokens);

                tokens.clear();

                tokens[215] = "OR(";
                tokens[216] = "AND(";
                tokens[65] = "TO(";
                tokens[245] = " PRINT;";

                DoTokenise(tokens);
        }
}

void lambdaBasicLoader::OutputLine(int lineNumber, int& addressOffset)
{
        int i = 0;

        OutputByte(addressOffset, (unsigned char)(lineNumber >> 8));
        OutputByte(addressOffset, (unsigned char)(lineNumber & 0xFF));

        int lineLengthOffset = addressOffset;
        addressOffset += 2;
        int commandOffset = addressOffset;

        bool withinQuotes = false;
        bool withinRem = false;

        while (mLineBuffer[i] != '\0')
        {
                if (mLineBufferPopulated[i])
                {
                        unsigned char chr = mLineBufferOutput[i];

                        if (!withinRem && chr == Quote)
                        {
                                withinQuotes = !withinQuotes;
                        }
                        else if (!withinQuotes && chr == Rem)
                        {
                                withinRem = true;
                        }

                        if (!withinQuotes && !withinRem && StartOfNumber(i))
                        {
                                const bool binaryFormatFlag = false;
                                OutputEmbeddedNumber(i, addressOffset, binaryFormatFlag);
                        }
                        else
                        {
                                OutputByte(addressOffset, chr);
                        }
                }

                i++;
        }

        OutputByte(addressOffset, Newline);

        int lineLength = addressOffset - commandOffset;
        ChangeWord(lineLengthOffset, lineLength);
}

void lambdaBasicLoader::OutputFloatingPointEncoding(double value, int& addressOffset)
{                             
        int exponent = 0;
        unsigned long mantissa  = 0;

        if (value != 0)
        {
                if (value < 0)
                {
                        value = -value;
                }

                exponent = (int)floor(DBL_EPSILON + (log(value) / log(2.0)));
                if (exponent < -129 || exponent > 126)
                {
                        throw out_of_range("Number out of range");
                }

                double mantissaVal = (value / pow(2.0, exponent)) - 1;
                mantissaVal *= 0x80000000;
                mantissa = (unsigned long)floor(mantissaVal);

                exponent += 129;
        }

        // Numbers in BASIC are always positive (a leading negative sign is an operator and
        // not part of the actual number) so set the msb to 0 to signify a positive value

        OutputByte(addressOffset, (unsigned char)exponent);
        OutputByte(addressOffset, (unsigned char)(mantissa >> 24));
        OutputByte(addressOffset, (unsigned char)(mantissa >> 16));
        OutputByte(addressOffset, (unsigned char)(mantissa >> 8));
        OutputByte(addressOffset, (unsigned char)(mantissa & 0xFF));
}

bool lambdaBasicLoader::SingleEscapeSequence(unsigned char chr, unsigned char& zxChr)
{
        if (chr == '\"')
        {
                zxChr = DoubleQuote;
                return true;
        }

        return false;
}

unsigned char lambdaBasicLoader::DecodeGraphic(unsigned char chr1, unsigned char chr2)
{
        map<string, unsigned char> graphics;

        graphics["  "] = 0;
        graphics["' "] = 1;
        graphics[" '"] = 2;
        graphics["''"] = 3;
        graphics[". "] = 4;
        graphics[": "] = 5;
        graphics[".'"] = 6;
        graphics[":'"] = 7;
        graphics["##"] = 8;
        graphics[",,"] = 9;
        graphics["~~"] = 10;    
        graphics["::"] = 128;
        graphics[".:"] = 129;
        graphics[":."] = 130;
        graphics[".."] = 131;
        graphics["':"] = 132;
        graphics[" :"] = 133;
        graphics["'."] = 134;
        graphics[" ."] = 135;
        graphics["@@"] = 136;
        graphics[";;"] = 137;
        graphics["!!"] = 138;

        string search;
        search += (char)chr1;
        search += (char)chr2;

        map<string, unsigned char>::const_iterator it = graphics.find(search);
        if (it == graphics.end())
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid graphic Code: " + code.str();
                throw invalid_argument(msg.c_str());
        }

        return it->second;
}

unsigned char lambdaBasicLoader::AsciiToZX(unsigned char ascii)
{
        const unsigned char Number0 = 28;
        const unsigned char LetterA = 38;

        if (isalpha(ascii))
        {
                return (unsigned char)((ascii - 'A') + LetterA);
        }
        else if (isdigit(ascii))
        {
                return (unsigned char)((ascii - '0') + Number0);
        }

        unsigned char zxChr;

        map<string, unsigned char> chrs;

        switch ((char)ascii)
        {
                case ' ':
                        zxChr = 0;
                        break;
                case '\"':
                        zxChr = 11;
                        break;
                case '£':
                case '#':
                        zxChr = 12;
                        break;
                case '$':
                        zxChr = 13;
                        break;
                case ':':
                        zxChr = 14;
                        break;
                case '?':
                        zxChr = 15;
                        break;
                case '(':
                        zxChr = 16;
                        break;
                case ')':
                        zxChr = 17;
                        break;
                case '-':
                        zxChr = 22;
                        break;
                case '+':
                        zxChr = 21;
                        break;
                case '*':
                        zxChr = 23;
                        break;
                case '/':
                        zxChr = 24;
                        break;
                case '=':
                        zxChr = 20;
                        break;
                case '>':
                        zxChr = 18;
                        break;
                case '<':
                        zxChr = 19;
                        break;
                case ';':
                        zxChr = 25;
                        break;
                case ',':
                        zxChr = 26;
                        break;
                case '.':
                        zxChr = 27;
                        break;
                default:
                        ostringstream code;
                        code << ascii;
                        string msg = "Invalid character: " + code.str();
                        throw invalid_argument(msg.c_str());
        }

        return zxChr;
}
                                      
void lambdaBasicLoader::ExtractInverseCharacters()
{
        unsigned char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                if (*pPos == '%')
                {
                        *pPos = Blank;
                        pPos++;

                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = (unsigned char)(0x80 | AsciiToZX(*pPos));
                        mLineBufferPopulated[index] = true;
                        
                        *pPos = Blank;
                }

                pPos++;
        }
}

void lambdaBasicLoader::ExtractDoubleQuoteCharacters(bool tokeniseRemContents, bool acceptAlternateKeywordSpelling)
{
        int i;
        bool withinQuote = false;    

        unsigned char* pRemBuffer = new unsigned char[sizeof(mLineBuffer)];

        strcpy((char*)pRemBuffer, (char*)mLineBuffer);

        if (acceptAlternateKeywordSpelling)
        {
                i = 0;
                while (pRemBuffer[i] != '\0')
                {
                        pRemBuffer[i] = (unsigned char)toupper(pRemBuffer[i]);
                        i++;
                }
        }

        i = 0;

        while (pRemBuffer[i] == Blank || pRemBuffer[i] == ' ')
        {
                i++;
        }

        const char* const rem = "REM ";
        char* pRem = strstr((char*)&pRemBuffer[i], rem);
        bool withinRem = (pRem == (char*)&pRemBuffer[i]);
        if (withinRem && !tokeniseRemContents)
        {
                return;
        }

        i = 0;

        while (mLineBuffer[i] != '\0')
        {
                if (!withinQuote)
                {
                        if (mLineBuffer[i] == '\"')
                        {
                                withinQuote = true;
                        }
                }
                else
                {
                        char chr1 = mLineBuffer[i];
                        if (chr1 == '\0')
                        {
                                throw runtime_error("End quote missing");
                        }

                        if (chr1 == '\"')
                        {
                                if (mLineBuffer[i + 1] == '\"')
                                {
                                        mLineBuffer[i] = Blank;
                                        mLineBufferOutput[i] = DoubleQuote;
                                        mLineBufferPopulated[i] = true;

                                        i++;
                                        mLineBuffer[i] = Blank;
                                }
                                else
                                {
                                        withinQuote = false;
                                        if (withinRem && tokeniseRemContents && mLineBuffer[i-1] == '\"')
                                        {
                                                mLineBuffer[i-1] = Blank;
                                                mLineBufferOutput[i-1] = DoubleQuote;
                                                mLineBufferPopulated[i-1] = true;

                                                mLineBuffer[i] = Blank;
                                        }
                                }
                        }
                }

                i++;
        }
}

unsigned char lambdaBasicLoader::GetEmbbededNumberMark()
{
        return Number;
}

bool lambdaBasicLoader::SupportUppercaseOnly()
{
        return true;
}

bool lambdaBasicLoader::TokenSupportsLineNumber(unsigned char chr)
{
        return (chr == Goto || chr == Gosub || chr == Run || chr == List || chr == LList);
}

void lambdaBasicLoader::ExtractZxTokenCharacterCodes()
{
        map<unsigned char, unsigned char> zxTokenChars;

        //        ZxToken    ZX81
        //           Code    Code
        zxTokenChars[0xB6] = 1;         // ¶
        zxTokenChars[0xB7] = 2;         // ·
        zxTokenChars[0xB8] = 3;         // ¸
        zxTokenChars[0xB9] = 4;         // ¹
        zxTokenChars[0xBA] = 5;         // º
        zxTokenChars[0xBB] = 6;         // »
        zxTokenChars[0xBC] = 7;         // ¼
        zxTokenChars[0xBD] = 8;         // ½
        zxTokenChars[0xBE] = 9;         // ¾
        zxTokenChars[0xBF] = 10;        // ¿
        zxTokenChars[0xAB] = 128;       // «
        zxTokenChars[0xAC] = 129;       // ¬
        zxTokenChars[0xAA] = 130;       // ª
        zxTokenChars[0xAE] = 131;       // ®
        zxTokenChars[0xAF] = 132;       // ¯
        zxTokenChars[0xB0] = 133;       // °
        zxTokenChars[0xB1] = 134;       // ±
        zxTokenChars[0xB2] = 135;       // ²
        zxTokenChars[0xB3] = 136;       // ³
        zxTokenChars[0xB4] = 137;       // ´
        zxTokenChars[0xB5] = 138;       // µ
        zxTokenChars[0x86] = 139;       // †
        zxTokenChars[0x87] = 140;       // ‡
        zxTokenChars[0x88] = 141;       // ˆ
        zxTokenChars[0x89] = 142;       // ‰
        zxTokenChars[0x8A] = 143;       // Š
        zxTokenChars[0x8B] = 144;       // ‹
        zxTokenChars[0x8C] = 145;       // Œ
        zxTokenChars[0x8D] = 146;
        zxTokenChars[0x8E] = 147;       // Ž
        zxTokenChars[0x8F] = 148;
        zxTokenChars[0x90] = 149;
        zxTokenChars[0x91] = 150;       // ‘
        zxTokenChars[0x92] = 151;       // ’
        zxTokenChars[0x93] = 152;       // “
        zxTokenChars[0x94] = 153;       // ”
        zxTokenChars[0x95] = 154;       // •
        zxTokenChars[0x96] = 155;       // –
        zxTokenChars[0x97] = 156;       // —
        zxTokenChars[0x98] = 157;       // ˜
        zxTokenChars[0x99] = 158;       // ™
        zxTokenChars[0x9A] = 159;       // š
        zxTokenChars[0x9B] = 160;       // ›
        zxTokenChars[0x9C] = 161;       // œ
        zxTokenChars[0x9D] = 162;
        zxTokenChars[0x9E] = 163;       // ž
        zxTokenChars[0x9F] = 164;       // Ÿ
        zxTokenChars[0xA0] = 165;
        zxTokenChars[0xC0] = 192;       // À
                                        
        for (unsigned char c = 'a'; c <= 'z'; c++)
        {
                zxTokenChars[c] = (unsigned char)(c + 0x45);
        }

        unsigned char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                map<unsigned char, unsigned char>::const_iterator it = zxTokenChars.find(*pPos);

                if (it != zxTokenChars.end())
                {
                        *pPos = Blank;

                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = it->second;
                        mLineBufferPopulated[index] = true;
                }

                pPos++;
        }
}
