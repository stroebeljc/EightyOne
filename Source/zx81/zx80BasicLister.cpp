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

#include "zx80BasicLister.h"
#include "zx81config.h"

zx80BasicLister::zx80BasicLister(bool zxpandEnabled)
{
        mZxpandEnabled = zxpandEnabled;
}

std::string zx80BasicLister::GetKeywords()
{
        std::string keywords;

        if (!mZxpandEnabled)
        {
                // Graphics encoded in accordance with ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
                //Non-printable characters encoded as '\' followed by a 2 digit hexadecimal value representing the character code.
                keywords = " ¦\"¦\\: ¦\\..¦\\' ¦\\ '¦\\. ¦\\ .¦\\.'¦\\##¦\\,,¦\\~~¦£¦$¦:¦?¦(¦)¦-¦+¦*¦/¦=¦>¦<¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦"
                           "\\40¦\\41¦\\42¦\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦%\"¦\\ :¦\\''¦\\.:¦\\:.¦\\':¦\\:'¦\\'.¦\\@@¦\\;;¦\\!!¦%£¦%$¦%:¦%?¦%(¦%)¦%-¦%+¦%*¦%/¦%=¦%>¦%<¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\\C0¦\\C1¦\\C2¦\\C3¦\\C4¦\\C5¦\\C6¦\\C7¦\\C8¦\\C9¦\\CA¦\\CB¦\\CC¦\\CD¦\\CE¦\\CF¦\\D0¦\\D1¦\\D2¦\\D3¦"
                           "\"¦ THEN ¦ TO ¦;¦,¦)¦(¦NOT ¦-¦+¦*¦/¦ AND ¦ OR ¦**¦=¦>¦<¦ LIST ¦ RETURN ¦ CLS ¦ DIM ¦ SAVE ¦ FOR ¦ GO TO ¦ POKE ¦ INPUT ¦ RANDOMISE ¦ LET ¦\\F1¦\\F2¦ NEXT ¦ PRINT ¦\\F5¦ NEW ¦ RUN ¦ STOP ¦ CONTINUE ¦ IF ¦ GO SUB ¦ LOAD ¦ CLEAR ¦ REM ¦\\FF¦";
        }
        else
        {
                keywords = " ¦\"¦\\: ¦\\..¦\\' ¦\\ '¦\\. ¦\\ .¦\\.'¦\\##¦\\,,¦\\~~¦£¦$¦:¦?¦(¦)¦-¦+¦*¦/¦=¦>¦<¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦"
                           "\\40¦\\41¦\\42¦\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦%\"¦\\ :¦\\''¦\\.:¦\\:.¦\\':¦\\:'¦\\'.¦\\@@¦\\;;¦\\!!¦%£¦%$¦%:¦%?¦%(¦%)¦%-¦%+¦%*¦%/¦%=¦%>¦%<¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\\C0¦\\C1¦\\C2¦\\C3¦\\C4¦\\C5¦\\C6¦\\C7¦\\C8¦\\C9¦\\CA¦\\CB¦\\CC¦\\CD¦\\CE¦\\CF¦\\D0¦\\D1¦\\D2¦\\D3¦"
                           "\"¦ THEN ¦ TO ¦;¦,¦)¦(¦NOT ¦-¦+¦*¦/¦ AND ¦ OR ¦**¦=¦>¦<¦ LIST ¦ RETURN ¦ CLS ¦ DIM ¦ SAVE ¦ FOR ¦ GO TO ¦ POKE ¦ INPUT ¦ RANDOMISE ¦ LET ¦ DELETE ¦\\F2¦ NEXT ¦ PRINT ¦ CONFIG ¦ NEW ¦ RUN ¦ STOP ¦ CONTINUE ¦ IF ¦ GO SUB ¦ LOAD ¦ CLEAR ¦ REM ¦ CAT ¦";
        }

        return keywords;
}

int zx80BasicLister::GetProgramStartAddress()
{
        return 16424;
}

int zx80BasicLister::GetProgramEndAddress()
{
        const int vars = 16392;
        return getbyte(vars) + (getbyte(vars + 1) << 8);
}

bool zx80BasicLister::ExtractLineDetails(int* address, LineInfo& lineInfo)
{
        int endAddress = GetProgramEndAddress();
        if ((*address) >= endAddress)
        {
                return false;
        }

        lineInfo.address = *address;
        const int lineHeaderLength = 2;
        lineInfo.addressContent = lineHeaderLength + lineInfo.address;

        const int EndOfBasicMarker = 0x40;
        int lineNumber = getbyte((*address)++);
        if (lineNumber >= EndOfBasicMarker)
        {
                return false;
        }

        lineNumber = (lineNumber << 8) + getbyte((*address)++);
        lineInfo.lineNumber = lineNumber;

        int length = 0;    
        bool lastKeywordEndedWithSpace = false;
        int b;
        bool endOfLine;

        lineInfo.displayLength = 4;     // Line number length

        do
        {
                b = getbyte((*address)++);
                
                endOfLine = (b == Newline);
                
                if (!endOfLine)
                {
                        int keywordLength = GetKeywordLength(b);
                        lineInfo.displayLength += keywordLength;

                        bool keywordStartsWithSpace = (keywordLength > 1) && (mKeyword[b][0] == ' ');
                        bool keywordEndsWithSpace = (keywordLength > 1) && (mKeyword[b].substr(keywordLength - 1, 1) == " ");

                        if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                        {
                                lineInfo.displayLength--;
                        }

                        bool characterIsSpace = (mKeyword[b] == " ");

                        if (!characterIsSpace)
                        {
                                lastKeywordEndedWithSpace = keywordEndsWithSpace;
                        }
                }
                
                length++;
        }
        while (!endOfLine);

        lineInfo.lineLength = lineHeaderLength + length;
        lineInfo.contentLength = length;
        lineInfo.displayRows = (lineInfo.displayLength + (GetDisplayColumns() - 1)) / GetDisplayColumns();

        return true;
}

unsigned char zx80BasicLister::ConvertToZXCode(unsigned char code)
{
        unsigned char c;

        switch ((char)code)
        {
                case ' ':
                        c = Space;
                        break;

                case '*':
                        c = Asterisk;
                        break;

                default:
                        if (code >= '0' && code <= '9')
                        {
                                c = Number0 + (code - '0');
                        }
                        else
                        {
                                c = LetterA + (code - 'A');
                        }
                        break;
        }

        return c;
}

bool zx80BasicLister::SupportFloatingPointNumbers()
{
        return false;
}

unsigned char zx80BasicLister::GetLineEndingCode()
{
        return Newline;
}

AnsiString zx80BasicLister::GetMachineName()
{
        return "ZX80";
}

AnsiString zx80BasicLister::GetBasicFileExtension()
{
        return "b80";
}

bool zx80BasicLister::RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes)
{
        bool containsMachineCode = false;

        bool endOfLine = (lengthRemaining <= 0);

        while (!endOfLine)
        {
                int c = getbyte(address);
                address++;
                lengthRemaining--;
                endOfLine = (lengthRemaining <= 0);

                if ((c >= 0x40 && c < 0x80 && !(c == 0x76 && endOfLine)) || (c >= 0xC0 && c <= 0xD3) ||
                     c == 0xF1 || c == 0xF2 || c == 0xF5 || c == 0xFF || (c >= 0x10 && c <= 0x1A))
                {
                        containsMachineCode = true;
                        break;
                }
                else if (outputRemTokensAsCharacterCodes && ( c == 0xD5 || c == 0xD6 || c == 0xDB || (c >= 0xE0 && c <= 0xE2) || c >= 0xE6))
                {
                        containsMachineCode = true;
                        break;
                }
        }

        return containsMachineCode;
}

