// CodeGear C++Builder
// Copyright (c) 1995, 2022 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'OffBtn.pas' rev: 35.00 (Windows)

#ifndef OffbtnHPP
#define OffbtnHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <Winapi.Windows.hpp>
#include <System.SysUtils.hpp>
#include <Winapi.Messages.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>
#include <System.UITypes.hpp>

//-- user supplied -----------------------------------------------------------

namespace Offbtn
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TOffice97Button;
//-- type declarations -------------------------------------------------------
enum DECLSPEC_DENUM TOffBtnAbout : unsigned char { abNone, abAbout };

enum DECLSPEC_DENUM TOffBtnState : unsigned char { bsInactive, bsActive, bsDown, bsDownAndOut };

enum DECLSPEC_DENUM TOffBtnType : unsigned char { bsButton, bsRadioButton, bsUpButton, bsDownButton, bsHintButton };

enum DECLSPEC_DENUM TGlyphPosition : unsigned char { bsTop, bsBottom, bsLeft, bsRight };

class PASCALIMPLEMENTATION TOffice97Button : public Vcl::Controls::TCustomControl
{
	typedef Vcl::Controls::TCustomControl inherited;
	
private:
	bool fAutoHeight;
	bool fAutoTransparency;
	bool fBtnKey;
	bool fCancel;
	bool fClicksDisabled;
	bool fDefault;
	bool fFocused;
	bool fNoDots;
	bool fOffice2000Look;
	bool fShowGlyph;
	bool fUseCustomGlyphs;
	bool fWordWrap;
	Vcl::Controls::TCaption fCaption;
	Vcl::Graphics::TBitmap* fActive;
	bool fTransparent;
	Vcl::Graphics::TBitmap* fControl;
	Vcl::Graphics::TBitmap* fCustomActive;
	Vcl::Graphics::TBitmap* fCustomDisabled;
	Vcl::Graphics::TBitmap* fCustomDownActive;
	Vcl::Graphics::TBitmap* fCustomInactive;
	Vcl::Graphics::TBitmap* fDisabled;
	Vcl::Graphics::TBitmap* fDisabledWorking;
	Vcl::Graphics::TBitmap* fDownActive;
	Vcl::Graphics::TBitmap* fInactive;
	TOffBtnAbout fAbout;
	TOffBtnType fType;
	Vcl::Graphics::TFont* fHoverFont;
	System::Classes::TNotifyEvent fMouseExit;
	System::Classes::TNotifyEvent fMouseEnter;
	System::Uitypes::TColor fActiveColor;
	System::Uitypes::TColor fActiveOutlineColor;
	System::Uitypes::TColor fActiveOutlineColor2;
	System::Uitypes::TColor fInactiveColor;
	System::Uitypes::TColor fTransparentColor;
	TGlyphPosition fGlyphPosition;
	System::Uitypes::TModalResult fModalResult;
	bool fDown;
	bool fToggle;
	System::Classes::TStringList* capWrap;
	int capLines;
	int tX;
	void __fastcall DrawTransparentBitmap(Vcl::Graphics::TCanvas* Dest, const short X, const short Y, Vcl::Graphics::TBitmap* srcBmp, const System::Uitypes::TColor transpColor);
	void __fastcall CreateDisabledBitmap(Vcl::Graphics::TBitmap* FOriginal, System::Uitypes::TColor OutlineColor);
	void __fastcall DrawOfficeFocusRect(const System::Types::TRect &txtRect, System::Classes::TStringList* capWrap);
	void __fastcall GetWrapText(System::UnicodeString txt, int tX);
	void __fastcall DrawFrame();
	void __fastcall DrawOffice2000Frame();
	void __fastcall SetAutoHeight(bool Val);
	void __fastcall SetAutoTransparency(bool Val);
	void __fastcall SetCaption(Vcl::Controls::TCaption Val);
	void __fastcall SetDefault(bool Value);
	Vcl::Graphics::TBitmap* __fastcall CurrentGlyph();
	void __fastcall SetActiveColor(System::Uitypes::TColor Val);
	void __fastcall SetActiveOutlineColor(System::Uitypes::TColor Val);
	void __fastcall SetActiveOutlineColor2(System::Uitypes::TColor Val);
	void __fastcall SetControlType(TOffBtnType Val);
	void __fastcall SetCustomActiveGlyph(Vcl::Graphics::TBitmap* Val);
	void __fastcall SetCustomDisabledGlyph(Vcl::Graphics::TBitmap* Val);
	void __fastcall SetCustomDownActiveGlyph(Vcl::Graphics::TBitmap* Val);
	void __fastcall SetCustomInactiveGlyph(Vcl::Graphics::TBitmap* Val);
	void __fastcall SetGlyphPosition(TGlyphPosition Val);
	void __fastcall SetInactiveColor(System::Uitypes::TColor Val);
	void __fastcall SetOffice2000Look(bool Val);
	void __fastcall SetShowGlyph(bool Val);
	void __fastcall SetTransparent(bool Val);
	void __fastcall SetTransparentColor(System::Uitypes::TColor Val);
	void __fastcall SetUseCustomGlyphs(bool Val);
	void __fastcall SetWordWrap(bool Val);
	void __fastcall SetHoverFont(Vcl::Graphics::TFont* Val);
	void __fastcall SetDown(bool Val);
	void __fastcall HoverFontChanged(System::TObject* Sender);
	void __fastcall ShowAbout(TOffBtnAbout Val);
	void __fastcall Layout(System::Types::TRect &txtRect, System::Types::TRect &bitRect);
	void __fastcall CalculateTxt(System::Types::TRect &txtRect, Vcl::Graphics::TBitmap* Glyph);
	HIDESBASE MESSAGE void __fastcall WMEraseBkgnd(Winapi::Messages::TWMEraseBkgnd &Message);
	HIDESBASE MESSAGE void __fastcall WMLButtonDown(Winapi::Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMMouseMove(Winapi::Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMLButtonUp(Winapi::Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMRButtonDown(Winapi::Messages::TWMMouse &Message);
	MESSAGE void __fastcall CNCommand(Winapi::Messages::TWMCommand &Message);
	HIDESBASE MESSAGE void __fastcall CNKeyDown(Winapi::Messages::TWMKey &Message);
	HIDESBASE MESSAGE void __fastcall CMDialogChar(Winapi::Messages::TWMKey &Message);
	HIDESBASE MESSAGE void __fastcall CMDialogKey(Winapi::Messages::TWMKey &Message);
	HIDESBASE MESSAGE void __fastcall CMEnabledChanged(Winapi::Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMFocusChanged(Winapi::Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Winapi::Messages::TWMKillFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Winapi::Messages::TWMSetFocus &Message);
	void __fastcall PerformEraseBackground(Vcl::Controls::TControl* Control, HDC DC);
	
protected:
	TOffBtnState fState;
	virtual void __fastcall Paint();
	virtual void __fastcall WndProc(Winapi::Messages::TMessage &Message);
	virtual void __fastcall CreateWnd();
	DYNAMIC void __fastcall ActionChange(System::TObject* Sender, bool CheckDefaults);
	
public:
	__fastcall virtual TOffice97Button(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TOffice97Button();
	DYNAMIC void __fastcall Click();
	void __fastcall AdjustHeight();
	
__published:
	__property bool Office2000Look = {read=fOffice2000Look, write=SetOffice2000Look, default=1};
	__property TOffBtnAbout About = {read=fAbout, write=ShowAbout, default=0};
	__property Action;
	__property System::Uitypes::TColor ActiveColor = {read=fActiveColor, write=SetActiveColor, default=8421504};
	__property Vcl::Graphics::TBitmap* ActiveGlyph = {read=fCustomActive, write=SetCustomActiveGlyph};
	__property System::Uitypes::TColor ActiveOutlineColor = {read=fActiveOutlineColor, write=SetActiveOutlineColor, default=16777215};
	__property System::Uitypes::TColor ActiveOutlineColor2 = {read=fActiveOutlineColor2, write=SetActiveOutlineColor2, default=14084071};
	__property Align = {default=0};
	__property Anchors = {default=3};
	__property bool AutoHeight = {read=fAutoHeight, write=SetAutoHeight, default=0};
	__property bool AutoTransparency = {read=fAutoTransparency, write=SetAutoTransparency, default=1};
	__property BiDiMode;
	__property bool Cancel = {read=fCancel, write=fCancel, default=0};
	__property Vcl::Controls::TCaption Caption = {read=fCaption, write=SetCaption};
	__property Color = {default=-16777211};
	__property TOffBtnType ControlType = {read=fType, write=SetControlType, default=0};
	__property bool Default = {read=fDefault, write=SetDefault, default=0};
	__property Vcl::Graphics::TBitmap* DisabledGlyph = {read=fCustomDisabled, write=SetCustomDisabledGlyph};
	__property Vcl::Graphics::TBitmap* DownActiveGlyph = {read=fCustomDownActive, write=SetCustomDownActiveGlyph};
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Enabled = {default=1};
	__property Font;
	__property Vcl::Graphics::TFont* HoverFont = {read=fHoverFont, write=SetHoverFont};
	__property System::Uitypes::TColor InactiveColor = {read=fInactiveColor, write=SetInactiveColor, default=12632256};
	__property Vcl::Graphics::TBitmap* InactiveGlyph = {read=fCustomInactive, write=SetCustomInactiveGlyph};
	__property System::Uitypes::TModalResult ModalResult = {read=fModalResult, write=fModalResult, default=0};
	__property ParentBiDiMode = {default=1};
	__property ParentFont = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property TGlyphPosition Position = {read=fGlyphPosition, write=SetGlyphPosition, default=2};
	__property bool ShowGlyph = {read=fShowGlyph, write=SetShowGlyph, default=0};
	__property ShowHint;
	__property TabOrder = {default=-1};
	__property TabStop = {default=1};
	__property bool Transparent = {read=fTransparent, write=SetTransparent, default=0};
	__property System::Uitypes::TColor TransparentColor = {read=fTransparentColor, write=SetTransparentColor, default=0};
	__property bool UseCustomGlyphs = {read=fUseCustomGlyphs, write=SetUseCustomGlyphs, default=0};
	__property Visible = {default=1};
	__property bool WordWrap = {read=fWordWrap, write=SetWordWrap, default=1};
	__property bool Down = {read=fDown, write=SetDown, default=0};
	__property bool Toggle = {read=fToggle, write=fToggle, default=0};
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMouseDown;
	__property System::Classes::TNotifyEvent OnMouseExit = {read=fMouseExit, write=fMouseExit};
	__property System::Classes::TNotifyEvent OnMouseEnter = {read=fMouseEnter, write=fMouseEnter};
	__property OnMouseMove;
	__property OnMouseUp;
public:
	/* TWinControl.CreateParented */ inline __fastcall TOffice97Button(HWND ParentWindow) : Vcl::Controls::TCustomControl(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE void __fastcall Register(void);
}	/* namespace Offbtn */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_OFFBTN)
using namespace Offbtn;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// OffbtnHPP
