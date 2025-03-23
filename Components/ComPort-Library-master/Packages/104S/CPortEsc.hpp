// CodeGear C++Builder
// Copyright (c) 1995, 2024 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'CPortEsc.pas' rev: 36.00 (Windows)

#ifndef CportescHPP
#define CportescHPP

#pragma delphiheader begin
#pragma option push
#if defined(__BORLANDC__) && !defined(__clang__)
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#endif
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.Classes.hpp>

//-- user supplied -----------------------------------------------------------

namespace Cportesc
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TEscapeCodes;
class DELPHICLASS TEscapeCodesVT52;
class DELPHICLASS TEscapeCodesVT100;
//-- type declarations -------------------------------------------------------
enum DECLSPEC_DENUM TEscapeResult : unsigned char { erChar, erCode, erNothing };

enum DECLSPEC_DENUM TEscapeCode : unsigned char { ecUnknown, ecNotCompleted, ecCursorUp, ecCursorDown, ecCursorLeft, ecCursorRight, ecCursorHome, ecCursorMove, ecReverseLineFeed, ecAppCursorLeft, ecAppCursorRight, ecAppCursorUp, ecAppCursorDown, ecEraseLineFrom, ecEraseScreenFrom, ecEraseLine, ecEraseScreen, ecSetTab, ecClearTab, ecClearAllTabs, ecIdentify, ecIdentResponse, ecQueryDevice, ecReportDeviceOK, ecReportDeviceFailure, ecQueryCursorPos, ecReportCursorPos, ecAttributes, ecSetMode, ecResetMode, ecReset, ecSaveCaretAndAttr, ecRestoreCaretAndAttr, ecSaveCaret, ecRestoreCaret, ecTest };

#pragma pack(push,4)
class PASCALIMPLEMENTATION TEscapeCodes : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	System::WideChar Fcharacter;
	TEscapeCode FCode;
	System::UnicodeString FData;
	System::Classes::TStrings* FParams;
	
public:
	__fastcall TEscapeCodes();
	__fastcall virtual ~TEscapeCodes();
	virtual TEscapeResult __fastcall ProcessChar(System::WideChar Ch) = 0 ;
	virtual System::UnicodeString __fastcall EscCodeToStr(TEscapeCode Code, System::Classes::TStrings* AParams) = 0 ;
	int __fastcall GetParam(int Num, System::Classes::TStrings* AParams);
	__property System::UnicodeString Data = {read=FData};
	__property TEscapeCode Code = {read=FCode, nodefault};
	__property System::WideChar character = {read=Fcharacter, nodefault};
	__property System::Classes::TStrings* Params = {read=FParams};
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION TEscapeCodesVT52 : public TEscapeCodes
{
	typedef TEscapeCodes inherited;
	
private:
	bool FInSequence;
	TEscapeCode __fastcall DetectCode(System::UnicodeString Str);
	
public:
	virtual TEscapeResult __fastcall ProcessChar(System::WideChar Ch);
	virtual System::UnicodeString __fastcall EscCodeToStr(TEscapeCode Code, System::Classes::TStrings* AParams);
public:
	/* TEscapeCodes.Create */ inline __fastcall TEscapeCodesVT52() : TEscapeCodes() { }
	/* TEscapeCodes.Destroy */ inline __fastcall virtual ~TEscapeCodesVT52() { }
	
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION TEscapeCodesVT100 : public TEscapeCodes
{
	typedef TEscapeCodes inherited;
	
private:
	bool FInSequence;
	bool FInExtSequence;
	TEscapeCode __fastcall DetectCode(System::UnicodeString Str);
	TEscapeCode __fastcall DetectExtCode(System::UnicodeString Str);
	
public:
	virtual TEscapeResult __fastcall ProcessChar(System::WideChar Ch);
	virtual System::UnicodeString __fastcall EscCodeToStr(TEscapeCode Code, System::Classes::TStrings* AParams);
public:
	/* TEscapeCodes.Create */ inline __fastcall TEscapeCodesVT100() : TEscapeCodes() { }
	/* TEscapeCodes.Destroy */ inline __fastcall virtual ~TEscapeCodesVT100() { }
	
};

#pragma pack(pop)

//-- var, const, procedure ---------------------------------------------------
}	/* namespace Cportesc */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_CPORTESC)
using namespace Cportesc;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// CportescHPP
