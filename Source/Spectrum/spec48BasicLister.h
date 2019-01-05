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

#include <string>
#include <vector>
#include "BasicLister\IBasicLister.h"
#include <Classes.hpp>

class spec48BasicLister : public IBasicLister
{
public:
        spec48BasicLister();
        virtual COLORREF GetPaperColour();
        virtual COLORREF GetInkColour();
        virtual AnsiString GetMachineName();
        virtual AnsiString GetBasicFileExtension();

private:
        static const int Return = 13;
        static const int Number = 14;
        static const int Ink = 16;
        static const int Paper = 17;
        static const int Flash = 18;
        static const int Bright = 19;
        static const int Inverse = 20;
        static const int Over = 21;
        static const int At = 22;
        static const int Tab = 23;

        int mInkValue;
        int mPaperValue;
        int mInverseValue;
        int mBrightValue;
        COLORREF mColours[16];

        virtual int GetProgramStartAddress();
        virtual int GetProgramEndAddress();
        virtual int GetFloatingPointNumberCode();
        virtual inline int GetLineEndingCode();
        virtual inline bool SupportEmbeddedControlCodes();
        virtual inline bool IsEmbeddedControlCode(int code);
        virtual inline int GetEmbeddedControlCodeSize(int code);
        virtual std::string GetKeywords();
        virtual void ProcessControlCode(int code, int arg1, int arg2);
        virtual void InitialiseColours();
        virtual bool CustomColoursSupported();
        virtual COLORREF GetDefaultPaperColour();
};

