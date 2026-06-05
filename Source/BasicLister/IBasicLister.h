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

#include <vcl4.h>
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
        int overheadLength;
        int addressContent;
        int addressArray;
        int contentLength;
        int startDisplayRow;
};

class IBasicLister
{
protected:
        static const int DisplayColumns = 32;
        static const int VarDisplayColumns = 50;
        static const int EmbeddedNumberSize = 5;

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
        Graphics::TBitmap *mCset;
        COLORREF mInk;
        COLORREF mPaper;
        void* BpEnabledBitmap;
        void* BpDisabledBitmap;

        bool RenderLine(HDC hdc, HDC cshdc, int& y, LineInfo& lineInfo);
        void RenderVariable(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo& varInfo);
        void RenderBPStyle(HDC hdc, int& x, int& y, int breakStyle);
        bool RenderLineNumber(HDC hdc, HDC cshdc, int& x, int& y, int lineNumber);
        bool RenderToken(HDC hdc, HDC cshdc, int& address, int& x, int& y, int& lengthRemaining, bool& lastKeywordEndedWithSpace, bool isVariable = false);
        bool RenderCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        bool RenderTokenAsText(int& address, int& lengthRemaining, bool& lastKeywordEndedWithSpace, AnsiString& zxCharacter, bool& outputLineAsControlCodes, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool& withinQuotes, bool& withinRem);
        void RenderVariableName(HDC hdc, HDC cshdc, int xOffset, int& y, VariableInfo varInfo);
        bool RenderCharacterInternal(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        bool RenderVarCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        AnsiString FormatLineNumber(int lineNumber, bool outputFullWidthLineNumbers = false);
        COLORREF GetBackgroundColour();
        unsigned char GetEscapeCharacter() { return '\\'; }

        static const int UnsupportedType = 0;
        static const int SingleNumber = 1;
        static const int MultiNumber = 2;
        static const int NumberArray = 3;
        static const int ForNextControl = 4;
        static const int SimpleString = 5;
        static const int CharacterArray = 6;
        static const int ZX80String = 7;
        static const int ZX80Array = 8;

        virtual std::string GetKeywords() { return std::string(""); }
        virtual inline unsigned char ConvertToZXCode(unsigned char code) { return code; }
        virtual inline unsigned char ConvertVariableNameCode(unsigned char code, bool first) { return code; }
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
        virtual unsigned char ReadByte(int address) { return getbyte(address); }

        virtual int TranslateVariableType(unsigned char code) { return UnsupportedType; }
        virtual double ConvertZXNumberToDouble(int* address) { return 0; }

        int GetKeywordLength(unsigned char code);

        std::string mKeyword[256];

public:
        IBasicLister();
        virtual ~IBasicLister();
        void PopulateKeywords();
        void ExtractProgramDetails();
        void ExtractVariablesDetails();
        int GetVariablesRows();
        int GetProgramRows();
        void ClearRenderedListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds);
        bool RenderListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds, int scaling);
        void ClearRenderedVariablesList(HDC hdc, HBITMAP bitmap, RECT rect);
        void RenderVariables(HDC hdc, HBITMAP bitmap, RECT rect, int scaling);
        AnsiString RenderLineAsText(LineInfo& lineInfo, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool limitLineLengths, bool outputFullWidthLineNumbers);
        void SetLines(std::vector<LineInfo>* linesInfo);
        void CopyCsetImage();
        static void StopRefresh();
        static void GoRefresh();
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
        virtual bool BasicVariablesSupported() { return false; }
        virtual int GetVariablesStartAddress() { return 65535; }
        virtual int GetForVariableLength() { return 17; }
        virtual bool DetectLastLetter(unsigned char letter) { return (letter & 0xC0); }
};

#endif
