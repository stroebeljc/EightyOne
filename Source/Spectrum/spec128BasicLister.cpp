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

#include "spec48BasicLister.h"
#include "spec128BasicLister.h"
#include "zx81config.h"
#include "spec48.h"

// The following embedded codes are not supported:
// AT
// TAB
// OVER
// LEFT
// COMMA

spec128BasicLister::spec128BasicLister()
{
}

std::string spec128BasicLister::GetKeywords()
{
        // Graphics encoding based upon the scheme used by ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
        // UDGs are prefixed with '\_' followed by the letter. Embedded control codes are prefixed by '\' followed by
        // the value of control code as a two digit hexadecimal number.
        std::string keywords = "\\00¦\\01¦\\02¦\\03¦\\04¦\\05¦\\06¦\\07¦\\08¦\\09¦\\0A¦\\0B¦\\0C¦\\0D¦\\0E¦\\0F¦\\10¦\\11¦\\12¦\\13¦\\14¦\\15¦\\16¦\\17¦\\18¦\\19¦\\1A¦\\1B¦\\1C¦\\1D¦\\1E¦\\1F¦"
                               " ¦!¦\"¦#¦$¦%¦&¦'¦(¦)¦*¦+¦,¦-¦.¦/¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦:¦;¦<¦=¦>¦?¦"
                               "@¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦[¦\\¦]¦^¦_¦"
                               "£¦a¦b¦c¦d¦e¦f¦g¦h¦i¦j¦k¦l¦m¦n¦o¦p¦q¦r¦s¦t¦u¦v¦w¦x¦y¦z¦{¦|¦}¦~¦\xA9¦"
                               "\\  ¦\\ '¦\\' ¦\\''¦\\ .¦\\ :¦\\'.¦\\':¦\\. ¦\\.'¦\\: ¦\\:'¦\\..¦\\.:¦\\:.¦\\::¦\\_A¦\\_B¦\\_C¦\\_D¦\\_E¦\\_F¦\\_G¦\\_H¦\\_I¦\\_J¦\\_K¦\\_L¦\\_M¦\\_N¦\\_O¦\\_P¦\\_Q¦\\_R¦\\_S¦"
                               " SPECTRUM ¦ PLAY ¦RND¦INKEY$¦PI¦FN ¦POINT ¦SCREEN$ ¦ATTR ¦AT ¦TAB ¦VAL$ ¦CODE ¦VAL ¦LEN ¦SIN ¦COS ¦TAN ¦ASN ¦ACS ¦ATN ¦LN ¦EXP ¦INT ¦"
                               "SQR ¦SGN ¦ABS ¦PEEK ¦IN ¦USR ¦STR$ ¦CHR$ ¦NOT ¦BIN ¦ OR ¦ AND ¦<=¦>=¦<>¦ LINE ¦ THEN ¦ TO ¦ STEP ¦ DEF FN ¦ CAT ¦"
                               " FORMAT ¦ MOVE ¦ ERASE ¦ OPEN #¦ CLOSE #¦ MERGE ¦ VERIFY ¦ BEEP ¦ CIRCLE ¦ INK ¦ PAPER ¦ FLASH ¦ BRIGHT ¦ INVERSE ¦ OVER ¦ OUT ¦"
                               " LPRINT ¦ LLIST ¦ STOP ¦ READ ¦ DATA ¦ RESTORE ¦ NEW ¦ BORDER ¦ CONTINUE ¦ DIM ¦ REM ¦ FOR ¦ GO TO ¦ GO SUB ¦ INPUT ¦ LOAD ¦"
                               " LIST ¦ LET ¦ PAUSE ¦ NEXT ¦ POKE ¦ PRINT ¦ PLOT ¦ RUN ¦ SAVE ¦ RANDOMIZE ¦ IF ¦ CLS ¦ DRAW ¦ CLEAR ¦ RETURN ¦ COPY ¦";

        return keywords;
}

unsigned char spec128BasicLister::ReadByte(int address)
{
        int bank;
        if (address>=0x4000 && address<0x8000)
                bank = 5+4;
        else if (address>=0x8000 && address<0xC000)
                bank = 2+4;
        else
                bank = 0+4;
                
        return RAMRead(bank,address&0x3FFF);
}

