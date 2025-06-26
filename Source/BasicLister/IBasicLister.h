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

#ifndef IBASICLISTER
#define IBASICLISTER

#include <Classes.hpp>
#include <Graphics.hpp>
#include <vector>
#include <string>
#include "zx81config.h"

enum BPStyle
{
        BPNONE,
        BPENABLED,
        BPDISABLED
};

const int PixelsPerCharacterWidth = 8;
const int PixelsPerCharacterHeight = 8;

struct LineInfo
{
        int lineNumber;
        int address;
        int addressContent;
        int lineLength;
        int contentLength;
        int displayLength;
        int displayRows;
        int startDisplayRow;
        int breakStyle;
};

struct VariableInfo
{
        int type;
        int address;
        int nameSize;
        int addressContent;
        int addressArray;
        int contentLength;
        int startDisplayRow;
};

class IBasicLister
{
private:
        static const int DisplayColumns = 32;
        static const int VarDisplayColumns = 50;
        static const int EmbeddedNumberSize = 5;

        static const int SingleNumber = 0x60;
        static const int MultiNumber = 0xA0;
        static const int NumberArray = 0x80;
        static const int ForNextControl = 0xE0;
        static const int SimpleString = 0x40;
        static const int CharacterArray = 0xC0;

        std::vector<LineInfo>* mLines;
        std::vector<VariableInfo>* mVariables;
        int mProgramDisplayRows;
        int mVariablesDisplayRows;
        int mEmbeddedNumberSize;
        int mFloatingPointNumberCode;
        int mLineEndingCode;
        bool mSupportsFloatingPointNumbers;
        bool mSupportEmbeddedControlCodes;
        AnsiString mEscapeCharacter;
        int mScaling;
        void* BpEnabledBitmap;
        void* BpDisabledBitmap;

        void RenderLine(HDC hdc, HDC cshdc, int& y, LineInfo& lineInfo);
        void RenderVariable(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo& varInfo);
        void RenderBPStyle(HDC hdc, int& x, int& y, int breakStyle);
        void RenderLineNumber(HDC hdc, HDC cshdc, int& x, int& y, int lineNumber);
        void RenderToken(HDC hdc, HDC cshdc, int& address, int& x, int& y, int& lengthRemaining, bool& lastKeywordEndedWithSpace);
        void RenderCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        bool RenderTokenAsText(int& address, int& lengthRemaining, bool& lastKeywordEndedWithSpace, AnsiString& zxCharacter, bool& outputLineAsControlCodes, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool& withinQuotes, bool& withinRem);
        void RenderVariableName(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo varInfo);
        void RenderCharacterInternal(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        bool RenderVarCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        AnsiString FormatLineNumber(int lineNumber, bool outputFullWidthLineNumbers = false);
        COLORREF GetBackgroundColour();
        unsigned char GetEscapeCharacter() { return '\\'; }

public:
        IBasicLister();
        virtual ~IBasicLister();
        void PopulateKeywords();
        void ExtractProgramDetails();
        void ExtractVariablesDetails();
        int GetProgramRows();
        int GetVariablesRows();
        void ClearRenderedListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds);
        void ClearRenderedVariablesList(HDC hdc, HBITMAP bitmap, RECT rect);
        void RenderListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds, int scaling);
        void RenderVariables(HDC hdc, HBITMAP bitmap, RECT rect, int scaling);
        AnsiString RenderLineAsText(LineInfo& lineInfo, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool limitLineLengths, bool outputFullWidthLineNumbers);
        void SetLines(std::vector<LineInfo>* linesInfo);
        void SetVariables(std::vector<VariableInfo>* variablesInfo);
        void SetBpEnabledBitmap(Graphics::TBitmap* bitmap);
        void SetBpDisabledBitmap(Graphics::TBitmap* bitmap);

        virtual int GetDisplayColumns() { return DisplayColumns; };
        virtual int GetVarDisplayColumns() { return VarDisplayColumns; };
        virtual COLORREF GetInkColour() { return RGB(0, 0, 0); }
        virtual COLORREF GetPaperColour() { return RGB(255, 255, 255); }
        virtual AnsiString GetMachineName() { return ""; }
        virtual AnsiString GetBasicFileExtension() { return "txt"; }
        virtual bool ZxTokenSupported() { return false; }
        virtual int GetProgramStartAddress() { return 65535; }
        virtual int GetProgramEndAddress() { return 65535; }
        virtual int GetBasicLineExecuteStartAddress() { return 65535; }
        virtual int GetNextBasicLineNumber() { return 65535; }
        virtual bool BasicDebugSupported() { return false; }
        virtual int GetVariablesStartAddress() { return 65535; }
        virtual int GetForVariableLength() { return 17; }

protected:
        COLORREF mInk;
        COLORREF mPaper;

        virtual std::string GetKeywords() { return std::string(""); }
        virtual inline unsigned char ConvertToZXCode(unsigned char code) { return code; }
        virtual inline unsigned char ConvertVariableNameCode(unsigned char code) { return code; }
        virtual inline bool SupportFloatingPointNumbers() { return true; }
        virtual unsigned char GetFloatingPointNumberCode() { return 0; }
        virtual unsigned char GetLineEndingCode() { return 0; }
        virtual int GetEmbeddedNumberSize() { return EmbeddedNumberSize; }
        virtual bool ExtractLineDetails(int* address, LineInfo& lineInfo);
        virtual bool ExtractEachVariable(int* address, VariableInfo& varInfo);
        virtual inline bool SupportEmbeddedControlCodes() { return false; }
        virtual bool IsEmbeddedControlCode(unsigned char code) { return false; }
        virtual int GetEmbeddedControlCodeSize(unsigned char code) { return 0; }
        virtual void ProcessControlCode(unsigned char code, unsigned char arg1, unsigned char arg2) {}
        virtual void InitialiseColours() { }
        virtual inline bool CustomColoursSupported() { return false; }
        virtual COLORREF GetDefaultPaperColour() { return RGB(255, 255, 255); }
        virtual bool RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes) { return false; }
        virtual bool RequiresInitialSpace() { return true; }
        virtual AnsiString TranslateToZxToken(AnsiString chr) { return chr; }
        virtual double ConvertZXFloatToDouble(int* address);
        
        int GetKeywordLength(unsigned char code);

        std::string mKeyword[256];
};

#endif
