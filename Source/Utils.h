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

//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------

#include <IniFiles.hpp>
#include "zx81config.h"

#define IsAsyncKeyPressed(key) (GetAsyncKeyState(key)<0)
#define IsKeyPressed(key) (GetKeyState(key)<0)

enum IniFileAccessType
{
        Read,
        Write
};


#ifdef __cplusplus
extern "C" {
#endif

int CRC32Block(BYTE *block, int len);
void ASCIIZX81(BYTE *in, BYTE *Output);
int ZX81Strlen(BYTE *str);

#ifdef __cplusplus
}
#endif

AnsiString FileNameGetPath(AnsiString Fname);
AnsiString FileNameGetFname(AnsiString Fname);
AnsiString FileNameGetExt(AnsiString Fname);
AnsiString ConvertZX81ASCII(BYTE *str);
void ConvertASCIIZX81(AnsiString Str, BYTE *Output);
AnsiString RemoveExt(AnsiString Fname);
AnsiString GetExt(AnsiString Fname);
AnsiString RemovePath(AnsiString Str);
int EnumeratePorts(TStrings *List, AnsiString Type);
void AccessIniFileBoolean(TIniFile* ini, IniFileAccessType accessType, AnsiString section, AnsiString entryName, bool& entryValue);
void AccessIniFileInteger(TIniFile* ini, IniFileAccessType accessType, AnsiString section, AnsiString entryName, int& entryValue);
void AccessIniFileString(TIniFile* ini, IniFileAccessType accessType, AnsiString section, AnsiString entryName, AnsiString& entryValue);
void AccessIniFileString(TIniFile* ini, IniFileAccessType accessType, AnsiString section, AnsiString entryName, char* entryValue);

#endif

