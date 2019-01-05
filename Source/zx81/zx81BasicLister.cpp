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

#include "zx81BasicLister.h"
#include "zx81config.h"

zx81BasicLister::zx81BasicLister()
{
}

std::string zx81BasicLister::GetKeywords()
{
        // Graphics encoded in accordance with ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
        std::string keywords = " ¦\\' ¦\\ '¦\\''¦\\. ¦\\: ¦\\.'¦\\:'¦\\##¦\\,,¦\\~~¦\"¦£¦$¦:¦?¦(¦)¦>¦<¦=¦+¦-¦*¦/¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦"
                               "RND¦INKEY$¦PI¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦?¦"
                               "% ¦\\.:¦\\:.¦\\..¦\\':¦\\ :¦\\'.¦\\ .¦\\@@¦\\;;¦\\!!¦\"¦%£¦%$¦%:¦%?¦%(¦%)¦%>¦%<¦%=¦%+¦%-¦%*¦%/¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                               "\"\"¦AT ¦TAB ¦?¦CODE ¦VAL ¦LEN ¦SIN ¦COS ¦TAN ¦ASN ¦ACS ¦ATN ¦LN ¦EXP ¦INT ¦SQR ¦SGN ¦ABS ¦PEEK ¦USR ¦STR$ ¦CHR$ ¦NOT ¦**¦ OR ¦ AND ¦<=¦>=¦<>¦ THEN ¦ TO ¦ STEP ¦ LPRINT ¦ LLIST ¦ STOP ¦ SLOW ¦ FAST ¦ NEW ¦ SCROLL ¦ CONT ¦ DIM ¦ REM ¦ FOR ¦ GOTO ¦ GOSUB ¦ INPUT ¦ LOAD ¦ LIST ¦ LET ¦ PAUSE ¦ NEXT ¦ POKE ¦ PRINT ¦ PLOT ¦ RUN ¦ SAVE ¦ RAND ¦ IF ¦ CLS ¦ UNPLOT ¦ CLEAR ¦ RETURN ¦ COPY ¦";

        return keywords;
}

int zx81BasicLister::GetProgramStartAddress()
{
        return 16509;
}

int zx81BasicLister::GetProgramEndAddress()
{
        const int vars = 16400;
        return getbyte(vars) + (getbyte(vars + 1) << 8);
}

int zx81BasicLister::ConvertToZXCode(int code)
{
        int c;

        switch (code)
        {
                case ' ':
                        c = Space;
                        break;

                case '*':
                        c = Asterisk;
                        break;

                case '>':
                        c = GreaterThan;
                        break;

                case '<':
                        c = LessThan;
                        break;

                case '=':
                        c = Equal;
                        break;

                case '\"':
                        c = Quote;
                        break;

                case '$':
                        c = Dollar;
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

int zx81BasicLister::GetFloatingPointNumberCode()
{
        return Number;
}

int zx81BasicLister::GetLineEndingCode()
{
        return Newline;
}

