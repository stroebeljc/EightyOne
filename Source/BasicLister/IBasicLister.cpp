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

#include <vcl4.h>
#pragma hdrstop

#include "IBasicLister.h"
#include "zx81config.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "Debug.h"

using namespace std;

extern Graphics::TPicture *listerBPpicture;

IBasicLister::IBasicLister() :
        mProgramDisplayRows(0),
        mVariablesDisplayRows(0),
        BpEnabledBitmap(NULL),
        BpDisabledBitmap(NULL)
{
}

void IBasicLister::PopulateKeywords()
{
        std::string keywords = GetKeywords();

        int keywordIndex = 0;
        int currPos = 0;
        int prevPos = 0;
        int sepPos;

        do
        {
                sepPos = keywords.find("¦", currPos);
                if (sepPos >= 0)
                {
                        currPos = sepPos;
                        mKeyword[keywordIndex] = keywords.substr(prevPos, currPos - prevPos);
                        currPos++;
                        prevPos = currPos;
                        keywordIndex++;
                }
        }
        while (sepPos >= 0);
}

IBasicLister::~IBasicLister()
{
        BpEnabledBitmap = NULL;
        BpDisabledBitmap = NULL;
}

int IBasicLister::GetKeywordLength(unsigned char code)
{
        int len;

        if (((mKeyword[code][0] == '%') && (mKeyword[code].length() > 1)) ||
            (mKeyword[code][0] == '\\'))
        {
                len = 1;
        }
        else
        {
                len = mKeyword[code].length();
        }

        return len;
}

void IBasicLister::SetLines(std::vector<LineInfo>* linesInfo)
{
        mLines = linesInfo;
}

void IBasicLister::SetVariables(std::vector<VariableInfo>* variablesInfo)
{
        mVariables = variablesInfo;
}

void IBasicLister::SetBpEnabledBitmap(Graphics::TBitmap* bitmap)
{
        BpEnabledBitmap = bitmap;
}

void IBasicLister::SetBpDisabledBitmap(Graphics::TBitmap* bitmap)
{
        BpDisabledBitmap = bitmap;
}

void IBasicLister::ExtractProgramDetails()
{
        const int startAddressOfProgram = GetProgramStartAddress();

        mLines->clear();

        mProgramDisplayRows = 0;
        bool lineAvailable;
        int displayRow = 0;

        int address = startAddressOfProgram;

        mEmbeddedNumberSize = GetEmbeddedNumberSize();
        mFloatingPointNumberCode = GetFloatingPointNumberCode();
        mLineEndingCode = GetLineEndingCode();
        mSupportsFloatingPointNumbers = SupportFloatingPointNumbers();
        mSupportEmbeddedControlCodes = SupportEmbeddedControlCodes();

        do
        {
                LineInfo lineInfo;

                lineAvailable = ExtractLineDetails(&address, lineInfo);
                if (lineAvailable)
                {
                        lineInfo.startDisplayRow = displayRow;
                        mLines->push_back(lineInfo);

                        mProgramDisplayRows += lineInfo.displayRows;
                        displayRow += lineInfo.displayRows;
                }
        }
        while (lineAvailable);
}

bool IBasicLister::ExtractLineDetails(int* address, LineInfo& lineInfo)
{
        int endAddress = GetProgramEndAddress();
        if ((*address) >= endAddress)
        {
                return false;
        }

        lineInfo.address = *address;
        const int lineHeaderLength = 4;
        lineInfo.addressContent = lineHeaderLength + lineInfo.address;

        const int EndOfBasicMarker = 0x40;
        int lineNumber = getbyte((*address)++);
        if (lineNumber >= EndOfBasicMarker)
        {
                return false;
        }

        lineNumber = (lineNumber << 8) + getbyte((*address)++);
        lineInfo.lineNumber = lineNumber;

        int length = getbyte((*address)++) + (getbyte((*address)++) << 8);
        if (length < 1)
        {
                return false;
        }

        lineInfo.contentLength = length;
        lineInfo.lineLength = lineHeaderLength + length;

        const int LineNumberLength = 4;
        lineInfo.displayLength = LineNumberLength;

        bool lastKeywordEndedWithSpace = false;

        for (int c = 0; c < length; c++)
        {
                bool endOfLine = (c == (length - 1));

                unsigned char b = (unsigned char)getbyte((*address)++);

                if (mSupportsFloatingPointNumbers && (b == mFloatingPointNumberCode))
                {
                        if ((length - (c + 1)) >= mEmbeddedNumberSize)
                        {
                                c += mEmbeddedNumberSize;
                                (*address) += mEmbeddedNumberSize;
                        }
                        else
                        {
                                int leftover = length - (c + 1);
                                c += leftover;
                                (*address) += leftover;
                        }

                        lastKeywordEndedWithSpace = false;
                        continue;
                }

                if (mSupportEmbeddedControlCodes && IsEmbeddedControlCode(b) && ((length - c) >= GetEmbeddedControlCodeSize(b)))
                {
                        int size = GetEmbeddedControlCodeSize(b);
                        c += size;
                        (*address) += size;

                        lastKeywordEndedWithSpace = false;
                        continue;
                }

                if (!endOfLine)
                {
                        int keywordLength = GetKeywordLength(b);
                        lineInfo.displayLength += keywordLength;

                        bool keywordStartsWithSpace = (keywordLength > 1) && (mKeyword[b][0] == ' ');
                        bool keywordEndsWithSpace = (keywordLength > 1) && (mKeyword[b][keywordLength-1] == ' ');

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
        }

        lineInfo.displayRows = (lineInfo.displayLength + (GetDisplayColumns() - 1)) / GetDisplayColumns();
        breakpoint bp(lineInfo.lineNumber, BP_BASIC);
        int index = Dbg->FindBreakPointEntry(0, bp, false);
        if (index < 0)
        {
                lineInfo.breakStyle = BPNONE;
        }
        else if (Dbg->BreakpointIsEnabled(index))
        {
                lineInfo.breakStyle = BPENABLED;
        }
        else
        {
                lineInfo.breakStyle = BPDISABLED;
        }

        return true;
}

void IBasicLister::ExtractVariablesDetails()
{
        const int startAddressOfVariables = GetVariablesStartAddress();

        mVariables->clear();

        mVariablesDisplayRows = 0;
        bool varAvailable;
        int displayRow = 0;

        int address = startAddressOfVariables;

        do
        {
                VariableInfo varInfo;
                varInfo.highLighted = false;

                varAvailable = ExtractEachVariable(&address, varInfo);
                if (varAvailable)
                {
                        varInfo.startDisplayRow = displayRow;
                        mVariables->push_back(varInfo);

                        mVariablesDisplayRows++;
                        displayRow++;
                }
        }
        while (varAvailable);
}

bool IBasicLister::ExtractEachVariable(int* address, VariableInfo& varInfo)
{
        varInfo.address = *address;

        const int EndOfVariablesMarker = 0x80;
        int typeByte = getbyte((*address)++);
        if (typeByte == EndOfVariablesMarker)
        {
                return false;
        }

        varInfo.type = TranslateVariableType((unsigned char)(typeByte & 0xE0));
        unsigned char letter;
        int size;
        switch (varInfo.type)
        {
        case SingleNumber: // Single letter number
                varInfo.nameSize = 1;
                varInfo.addressContent = *address;
                varInfo.overheadLength = 0;
                varInfo.contentLength = mEmbeddedNumberSize;
                *address += varInfo.contentLength;
                break;

        case MultiNumber: // Multi-letter number
                varInfo.nameSize = 1;
                do
                {
                        letter = getbyte((*address)++);
                        varInfo.nameSize++;
                } while (!(letter & 0xC0));
                varInfo.addressContent = *address;
                varInfo.overheadLength = 0;
                varInfo.contentLength = mEmbeddedNumberSize;
                *address += varInfo.contentLength;
                break;

        case NumberArray: // Array of numbers
                {
                varInfo.nameSize = 1;
                size = getbyte((*address)++) + 256*getbyte((*address)++);
                varInfo.addressArray = (*address)++;
                int dimensions = getbyte(varInfo.addressArray);
                for (int i = 0; i < dimensions; i++)
                {
                        int currentDim = getbyte((*address)++) + 256*getbyte((*address)++);
                        varInfo.nameSize += std::log10(currentDim) + 2;
                }
                varInfo.nameSize++; // final paren
                varInfo.addressContent = *address;
                varInfo.overheadLength = 2 + (1 + 2*dimensions);
                varInfo.contentLength = size - (1 + 2*dimensions);
                *address += varInfo.contentLength;
                }
                break;

        case ForNextControl: // FOR-NEXT control variable
                varInfo.nameSize = 1;
                varInfo.addressContent = *address;
                varInfo.overheadLength = 0;
                varInfo.contentLength = GetForVariableLength();
                *address += varInfo.contentLength;
                break;

        case SimpleString: // String
                varInfo.nameSize = 2; // single letter and '$'
                size = getbyte((*address)++) + 256*getbyte((*address)++);
                varInfo.addressContent = *address;
                varInfo.overheadLength = 2;
                varInfo.contentLength = size;
                *address += varInfo.contentLength;
                break;

        case ZX80String: // ZX80 String
                varInfo.nameSize = 2; // single letter and '$'
                varInfo.addressContent = *address;
                varInfo.overheadLength = 0;
                size = 1;
                while (getbyte((*address)++) != 0x01)
                {
                        size++;
                }
                varInfo.contentLength = size;
                break;

        case ZX80Array: // Array of numbers
                {
                varInfo.nameSize = 1;
                size = getbyte(*address) + 1;
                varInfo.nameSize += std::log10(size) + 2;
                varInfo.nameSize++; // final paren
                varInfo.addressArray = (*address)++;
                varInfo.addressContent = *address;
                varInfo.overheadLength = 1;
                varInfo.contentLength = 2*size;
                *address += varInfo.contentLength;
                }
                break;

        case CharacterArray: // Array of characters
                {
                varInfo.nameSize = 2;
                size = getbyte((*address)++) + 256*getbyte((*address)++);
                varInfo.addressArray = (*address)++;
                int dimensions = getbyte(varInfo.addressArray);
                for (int i = 0; i < dimensions; i++)
                {
                        int currentDim = getbyte((*address)++) + 256 * getbyte((*address)++);
                        varInfo.nameSize += std::log10(currentDim) + 2;
                }
                varInfo.nameSize++; // final paren
                varInfo.addressContent = *address;
                varInfo.overheadLength = 2 + (1 + 2*dimensions);
                varInfo.contentLength = size - (1 + 2*dimensions);
                *address += varInfo.contentLength;
                }
                break;

        default:
                return false;
        }

        return true;
}


int IBasicLister::GetProgramRows()
{
        return mProgramDisplayRows;
}

int IBasicLister::GetVariablesRows()
{
        return mVariablesDisplayRows;
}

void IBasicLister::ClearRenderedListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds)
{
        COLORREF canvasColour = GetSysColor(COLOR_BTNFACE);
        if (mLines->size() > 0)
        {
                canvasColour = showLineEnds ? GetBackgroundColour() : GetDefaultPaperColour();
        }
        HBRUSH hBrush = CreateSolidBrush(canvasColour);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
}

void IBasicLister::ClearRenderedVariablesList(HDC hdc, HBITMAP bitmap, RECT rect)
{
        COLORREF canvasColour = GetSysColor(COLOR_BTNFACE);
        if (mLines->size() > 0)
        {
                canvasColour = GetDefaultPaperColour();
        }
        HBRUSH hBrush = CreateSolidBrush(canvasColour);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
}

void IBasicLister::RenderListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds, int scaling)
{
        mScaling = scaling;

        int yOffset = 0;

        InitialiseColours();

        HDC cshdc = CreateCompatibleDC(hdc);

        HGDIOBJ oldBitmap = SelectObject(cshdc, (HGDIOBJ)((Graphics::TBitmap*)machine.cset)->Handle);

        ClearRenderedListing(hdc, bitmap, rect, showLineEnds);              

        for (std::vector<LineInfo>::iterator it = mLines->begin(); it != mLines->end(); it++)
        {
                LineInfo lineInfo = *it;
                RenderLine(hdc, cshdc, yOffset, lineInfo);
        }

        SelectObject(cshdc, oldBitmap);
        DeleteDC(cshdc);
}

void IBasicLister::RenderVariables(HDC hdc, HBITMAP bitmap, RECT rect, int scaling)
{
        mScaling = scaling;

        int yOffset = 0;

        InitialiseColours();

        HDC cshdc = CreateCompatibleDC(hdc);

        HGDIOBJ oldBitmap = SelectObject(cshdc, (HGDIOBJ)((Graphics::TBitmap*)machine.cset)->Handle);

        ClearRenderedVariablesList(hdc, bitmap, rect);

        int maxNameSize = 0;
        for (std::vector<VariableInfo>::iterator it = mVariables->begin(); it != mVariables->end(); it++)
        {
                VariableInfo varInfo = *it;
                if (varInfo.nameSize > maxNameSize)
                        maxNameSize = varInfo.nameSize;
                const int NameSizeLimit = 15;
                if (maxNameSize > NameSizeLimit)
                        maxNameSize = NameSizeLimit;
        }

        for (std::vector<VariableInfo>::iterator it = mVariables->begin(); it != mVariables->end(); it++)
        {
                VariableInfo varInfo = *it;
                RenderVariable(hdc, cshdc, maxNameSize, yOffset, varInfo);
        }

        SelectObject(cshdc, oldBitmap);
        DeleteDC(cshdc);
}

COLORREF IBasicLister::GetBackgroundColour()
{
        return RGB(132, 130, 132);
}

void IBasicLister::RenderLine(HDC hdc, HDC cshdc, int& y, LineInfo& lineInfo)
{
        int x = 0;

        int breakStyle = lineInfo.breakStyle;
        RenderBPStyle(hdc, x, y, breakStyle);

        int lineNumber = lineInfo.lineNumber;
        RenderLineNumber(hdc, cshdc, x, y, lineNumber);
        
        bool requiresInitialSpace = RequiresInitialSpace();
        if (requiresInitialSpace)
        {
                RenderCharacter(hdc, cshdc, x, y, ConvertToZXCode(' '));
        }

        int address = lineInfo.addressContent;
        int lengthRemaining = lineInfo.contentLength;
        bool lastKeywordEndedWithSpace = requiresInitialSpace;

        do
        {
                RenderToken(hdc, cshdc, address, x, y, lengthRemaining, lastKeywordEndedWithSpace);
        }
        while (lengthRemaining > 0);

        if (x > 1)
        {
                x = 1;
                y++;
        }
}

void IBasicLister::RenderVariable(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo& varInfo)
{
        int x = xOffset;
        bool lastKeywordEndedWithSpace = false;

        RenderVariableName(hdc, cshdc, xOffset, y, varInfo);

        RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode('='));

        int address = varInfo.addressContent;
        int lengthRemaining = varInfo.contentLength;
        bool notDone = true;

        switch (varInfo.type)
        {
        case ZX80String:
        case SimpleString:
        case CharacterArray:
                RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode('\"'));
                while (notDone && lengthRemaining > 0)
                {
                        notDone = RenderToken(hdc, cshdc, address, x, y, lengthRemaining, lastKeywordEndedWithSpace, true);
                }
                if (varInfo.type != ZX80String) RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode('\"'));
                break;

        case SingleNumber:
        case MultiNumber:
        case ForNextControl:
                {
                AnsiString numStr = AnsiString(ConvertZXNumberToDouble(&address));
                for (int i = 1; i <= numStr.Length(); i++)
                {
                        RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode(numStr[i]));
                }
                }
                break;

        case ZX80Array:
        case NumberArray:
                RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode('('));
                while (notDone && lengthRemaining > 0)
                {
                        lengthRemaining -= mEmbeddedNumberSize;
                        AnsiString numStr = AnsiString(ConvertZXNumberToDouble(&address));
                        for (int i = 1; i <= numStr.Length() && notDone; i++)
                        {
                                notDone = RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode(numStr[i]));
                        }
                        if (notDone && lengthRemaining > 0)
                                RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode(' '));
                }
                RenderVarCharacter(hdc, cshdc, x, y, ConvertToZXCode(')'));
                break;
        }

        y++;
}

void IBasicLister::RenderBPStyle(HDC hdc, int& x, int& y, int breakStyle)
{
        int xpos = (x << 3) * mScaling;
        int ypos = (y << 3) * mScaling;
        const int srcW = 16;
        const int srcH = 16;
        int destW = 8 * mScaling;
        int destH = 8 * mScaling;
        x++;

        Graphics::TBitmap* bitmap = NULL;

        if (breakStyle == BPENABLED) bitmap = (Graphics::TBitmap*)BpEnabledBitmap;
        else if (breakStyle == BPDISABLED) bitmap = (Graphics::TBitmap*)BpDisabledBitmap;
        
        if (bitmap)
        {
                HDC cshdc = CreateCompatibleDC(hdc);
                HGDIOBJ oldBitmap = SelectObject(cshdc, (HGDIOBJ)(bitmap)->Handle);

                StretchBlt(hdc, xpos, ypos, destW, destH, cshdc, 0, 0, srcW, srcH, SRCCOPY);

                SelectObject(cshdc, oldBitmap);
                DeleteDC(cshdc);
        }
}

void IBasicLister::RenderLineNumber(HDC hdc, HDC cshdc, int& x, int& y, int lineNumber)
{
        AnsiString formattedLineNumber = FormatLineNumber(lineNumber);

        for (signed int i = 2; i <= formattedLineNumber.Length(); i++)
        {
                unsigned char c = ConvertToZXCode(formattedLineNumber[i]);
                RenderCharacter(hdc, cshdc, x, y, c);
        }
}

void IBasicLister::RenderVariableName(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo varInfo)
{
        int tempX = 0;
        for (int i = varInfo.nameSize; i < xOffset; i++)
        {
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode(' '));
        }

        // All variable types start with a letter
        unsigned char c = ConvertVariableNameCode(getbyte(varInfo.address), true);
        RenderVarCharacter(hdc, cshdc, tempX, y, c);

        switch (varInfo.type)
        {
        case MultiNumber: // Multi-letter number
                {
                int usedSize = (varInfo.nameSize < xOffset) ? varInfo.nameSize : xOffset;
                for (int i = 1; i < usedSize; i++)
                {
                        unsigned char c = ConvertVariableNameCode(getbyte(varInfo.address + i), false);
                        RenderVarCharacter(hdc, cshdc, tempX, y, c);
                }
                }
                break;

        case CharacterArray: // Array of characters
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode('$'));
        case NumberArray: // Array of numbers
                {
                int address = varInfo.addressArray;
                int numDim = getbyte(address++);
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode('('));
                for (int i = numDim; i > 0; i--)
                {
                        int dimension = getbyte(address++) + 256*getbyte(address++);
                        AnsiString dimStr = AnsiString(dimension);
                        for (int j = 1; j <= dimStr.Length(); j++)
                        {
                                unsigned char c = ConvertToZXCode(dimStr[j]);
                                RenderVarCharacter(hdc, cshdc, tempX, y, c);
                        }
                        if (i > 1)
                        {
                                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode(','));
                        }
                }
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode(')'));
                }
                break;

        case ZX80Array: // Array of numbers
                {
                int address = varInfo.addressArray;
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode('('));
                int size = getbyte(address++);
                AnsiString dimStr = AnsiString(size);
                for (int j = 1; j <= dimStr.Length(); j++)
                {
                        unsigned char c = ConvertToZXCode(dimStr[j]);
                        RenderVarCharacter(hdc, cshdc, tempX, y, c);
                }
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode(')'));
                }
                break;

        case ZX80String:
        case SimpleString: // String
                RenderVarCharacter(hdc, cshdc, tempX, y, ConvertToZXCode('$'));
                break;

        case SingleNumber: // Single letter number
        case ForNextControl: // FOR-NEXT control variable
        default:
                break;
        }
}

bool IBasicLister::RenderToken(HDC hdc, HDC cshdc, int& address, int& x, int& y, int& lengthRemaining, bool& lastKeywordEndedWithSpace, bool isVariable)
{
        unsigned char c = (unsigned char)getbyte(address);
        address++;
        lengthRemaining--;
        bool endOfLine = (lengthRemaining <= 0);

        if (endOfLine && (c == mLineEndingCode))
        {
                return true;
        }

        if (mSupportsFloatingPointNumbers && (c == mFloatingPointNumberCode))
        {
                if (lengthRemaining >= mEmbeddedNumberSize)
                {
                        address += mEmbeddedNumberSize;
                        lengthRemaining -= mEmbeddedNumberSize;
                }
                else
                {
                        address += lengthRemaining;
                        lengthRemaining = 0;
                }
                
                return true;
        }

        if (mSupportEmbeddedControlCodes && IsEmbeddedControlCode(c))
        {
                unsigned char arg1 = (unsigned char)getbyte(address);
                address++;
                lengthRemaining--;

                unsigned char arg2;
                if (GetEmbeddedControlCodeSize(c) == 2)
                {
                        arg2 = (unsigned char)getbyte(address);
                        address++;
                        lengthRemaining--;
                }

                ProcessControlCode(c, arg1, arg2);

                return true;
        }

        bool retVal = true;
        int length = GetKeywordLength(c);
        if (length > 1)
        {
                std::string kw = mKeyword[c];

                bool keywordStartsWithSpace = (mKeyword[c][0] == ' ');
                bool keywordEndsWithSpace = (mKeyword[c][length-1] == ' ');
                int startIndex = 0;

                if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                {
                        startIndex = 1;
                }

                for (int i = startIndex; i < length && retVal; i++)
                {
                        unsigned char code = mKeyword[c].at(i);
                        unsigned char zxCode = ConvertToZXCode(code);
                        if (isVariable)
                                retVal = RenderVarCharacter(hdc, cshdc, x, y, zxCode);
                        else
                                RenderCharacter(hdc, cshdc, x, y, zxCode);
                }

                lastKeywordEndedWithSpace = keywordEndsWithSpace;
        }
        else
        {
                if (isVariable)
                        retVal = RenderVarCharacter(hdc, cshdc, x, y, c);
                else
                        RenderCharacter(hdc, cshdc, x, y, c);

                bool characterIsSpace = (mKeyword[c] == " ");

                if (!characterIsSpace)
                {
                        lastKeywordEndedWithSpace = false;
                }
        }

        return retVal;
}

void IBasicLister::RenderCharacterInternal(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c)
{
        int charX = (c % 32) << 3;
        int charY = (c / 32) << 3;
        int xpos = (x << 3) * mScaling;
        int ypos = (y << 3) * mScaling;
        const int srcW = 8;
        const int srcH = 8;
        int destW = 8 * mScaling;
        int destH = 8 * mScaling;
        StretchBlt(hdc, xpos, ypos, destW, destH, cshdc, charX, charY, srcW, srcH, SRCCOPY);

        if (CustomColoursSupported())
        {
                COLORREF backgroundColour = RGB(255, 255, 255);
                COLORREF foregroundColour = RGB(0, 0, 0);
                COLORREF paperColour = GetPaperColour();
                COLORREF inkColour = GetInkColour();
                bool differentPaperColour = (paperColour != backgroundColour);
                bool differentInkColour = (inkColour != foregroundColour);

                for (int y = ypos; y < ypos + (8 * mScaling); y += mScaling)
                {
                        for (int x = xpos; x < xpos + (8 * mScaling); x += mScaling)
                        {
                                COLORREF pixelColor = GetPixel(hdc, x, y);
                                if (pixelColor == backgroundColour)
                                {
                                        if (differentPaperColour)
                                        {
                                                SetPixelV(hdc, x, y, paperColour);
                                                if (mScaling == 2)
                                                {
                                                        SetPixelV(hdc, x+1, y, paperColour);
                                                        SetPixelV(hdc, x, y+1, paperColour);
                                                        SetPixelV(hdc, x+1, y+1, paperColour);
                                                }
                                        }
                                }
                                else
                                {
                                        if (differentInkColour)
                                        {
                                                SetPixelV(hdc, x, y, inkColour);
                                                if (mScaling == 2)
                                                {
                                                        SetPixelV(hdc, x+1, y, inkColour);
                                                        SetPixelV(hdc, x, y+1, inkColour);
                                                        SetPixelV(hdc, x+1, y+1, inkColour);
                                                }
                                        }
                                }
                        }
                }
        }
}

void IBasicLister::RenderCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c)
{
        RenderCharacterInternal(hdc, cshdc, x, y, c);

        x++;
        if (x == 1 + GetDisplayColumns())
        {
                x = 1;
                y++;
        }
}

bool IBasicLister::RenderVarCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c)
{
        if (x < GetVarDisplayColumns() - 2)
        {
                RenderCharacterInternal(hdc, cshdc, x, y, c);
                x++;
                return true;
        }

        while (x <= GetVarDisplayColumns())
        {
                RenderCharacterInternal(hdc, cshdc, x, y, ConvertToZXCode('.'));
                x++;
        }

        return false;
}

AnsiString IBasicLister::RenderLineAsText(LineInfo& lineInfo, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool limitLineLengths, bool outputFullWidthLineNumbers)
{
        AnsiString lineText = "";

        AnsiString lineNumber = FormatLineNumber(lineInfo.lineNumber, outputFullWidthLineNumbers);
        int startPos = (lineNumber[1] != ' ') ? 1 : 2;
        int length = strlen(lineNumber.c_str()) - (startPos - 1);
        lineText += lineNumber.SubString(startPos, length);
        bool requiresInitialSpace = RequiresInitialSpace();
        if (requiresInitialSpace)
        {
                lineText += " ";
        }

        int address = lineInfo.addressContent;
        int lengthRemaining = lineInfo.contentLength;
        bool lastKeywordEndedWithSpace = requiresInitialSpace;
        AnsiString zxCharacter;

        mEscapeCharacter = AnsiChar(GetEscapeCharacter());

        bool outputLineAsControlCodes = false;
        bool withinRem = false;
        bool withinQuotes = false;
        int lineLength = lineText.Length();

        const int WrapLineThreshold = 120;
        
        do
        {
                if (RenderTokenAsText(address, lengthRemaining, lastKeywordEndedWithSpace, zxCharacter, outputLineAsControlCodes, outputRemTokensAsCharacterCodes, outputStringTokensAsCharacterCodes, outputNonAsciiAsCharacterCodes, outputVariableNamesInLowercase, outputInZxTokenFormat, withinQuotes, withinRem))
                {
                        lineText += zxCharacter;
                        lineLength += zxCharacter.Length();

                        if (limitLineLengths && (lineLength > WrapLineThreshold))
                        {
                                lineText += mEscapeCharacter + '\n';
                                lineLength = 0;
                        }
                }
        }
        while (lengthRemaining > 0);

        if (outputInZxTokenFormat)
        {
                TReplaceFlags flags = TReplaceFlags() << rfReplaceAll;
                lineText = StringReplace(lineText, "][HEX:", ",", flags);
                lineText = StringReplace(lineText, "]\\\n[HEX:", ",\\\n", flags);
        }

        return lineText;
}

bool IBasicLister::RenderTokenAsText(int& address, int& lengthRemaining, bool& lastKeywordEndedWithSpace, AnsiString& zxCharacter, bool& outputLineAsControlCodes, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool& withinQuotes, bool& withinRem)
{
        unsigned char c = (unsigned char)getbyte(address);
        address++;
        lengthRemaining--;
        bool endOfLine = (lengthRemaining <= 0);

        if (endOfLine && (c == mLineEndingCode))
        {
                return false;
        }

        bool remToken = (mKeyword[c] == " REM ");

        if (!withinRem && mKeyword[c] == "\"")
        {
                withinQuotes = !withinQuotes;
        }
        else if (!withinQuotes && remToken)
        {
                withinRem = true;
        }

        if (!withinRem && mSupportsFloatingPointNumbers && (c == mFloatingPointNumberCode))
        {
                address += mEmbeddedNumberSize;
                lengthRemaining -= mEmbeddedNumberSize;

                lastKeywordEndedWithSpace = false;
                return false;
        }

        int length = GetKeywordLength(c);
        bool token = (length > 1);

        if ((outputStringTokensAsCharacterCodes && withinQuotes && token) || outputLineAsControlCodes || (((mKeyword[c] == "£") || (mKeyword[c] == "©")) && outputNonAsciiAsCharacterCodes))
        {
                if (!outputInZxTokenFormat)
                {
                        zxCharacter = mEscapeCharacter;
                        zxCharacter += UpperCase(IntToHex(c, 2));
                }
                else
                {
                        zxCharacter = "[HEX:";
                        zxCharacter += UpperCase(IntToHex(c, 2));
                        zxCharacter += "]";
                }

                lastKeywordEndedWithSpace = false;
        }
        else if (length > 1)
        {
                if (!withinQuotes && withinRem)
                {
                        outputLineAsControlCodes = RemContainsMachineCode(address, lengthRemaining, outputRemTokensAsCharacterCodes);
                }

                bool keywordStartsWithSpace = (mKeyword[c][0] == ' ');
                bool keywordEndsWithSpace = (mKeyword[c][length-1] == ' ');
                int startIndex = 0;

                if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                {
                        startIndex = 1;
                }

                zxCharacter = "";

                for (int i = startIndex; i < length; i++)
                {
                        zxCharacter += mKeyword[c].at(i);
                }

                lastKeywordEndedWithSpace = keywordEndsWithSpace;
        }
        else
        {
                if (!withinQuotes && !withinRem && outputVariableNamesInLowercase && (mKeyword[c].length() == 1))
                {
                        char chr = (char)tolower(mKeyword[c][0]);
                        zxCharacter = chr;
                }
                else
                {
                        zxCharacter = mKeyword[c].c_str();

                        if (outputInZxTokenFormat)
                        {
                                zxCharacter = TranslateToZxToken(zxCharacter);
                        }
                }

                lastKeywordEndedWithSpace = false;

                if (zxCharacter == mEscapeCharacter)
                {
                        zxCharacter += mEscapeCharacter;
/*
                        unsigned char nc = (unsigned char)getbyte(address);
                        string nextChr = mKeyword[nc].substr(0, 1);

                        string validEscapeChars;
                        validEscapeChars = "\\_1234567890ABCDEFabcdef\n '.#~@!;:";

                        size_t pos = nextChr.find_first_of(validEscapeChars);
                        if (pos != string::npos)
                        {
                                zxCharacter += mEscapeCharacter;
                        }
*/
                }
        }

        return true;
}

AnsiString IBasicLister::FormatLineNumber(int lineNumber, bool outputFullWidthLineNumbers)
{
        std::ostringstream ss;

        ss << std::setfill(' ');
        ss << std::setw(5);
        ss << lineNumber;
        AnsiString lineNum = ss.str().c_str();

        if (!outputFullWidthLineNumbers && (lineNum[1] != ' '))
        {
                lineNum[2] = (char)('A' + (lineNum[2] - '0'));
                lineNum[1] = ' ';
        }

        return lineNum;
}

