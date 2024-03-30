// CodeGear C++Builder
// Copyright (c) 1995, 2022 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'CPortCtl.pas' rev: 35.00 (Windows)

#ifndef CportctlHPP
#define CportctlHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Winapi.Messages.hpp>
#include <Vcl.Graphics.hpp>
#include <Winapi.Windows.hpp>
#include <CPort.hpp>
#include <CPortEsc.hpp>
#include <System.UITypes.hpp>
#include <Vcl.Menus.hpp>
#include <System.Types.hpp>

//-- user supplied -----------------------------------------------------------

namespace Cportctl
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TComSelect;
class DELPHICLASS TComComboBox;
class DELPHICLASS TComRadioGroup;
class DELPHICLASS TComLed;
struct TComTermChar;
class DELPHICLASS TComTermBuffer;
struct TTermAttributes;
struct TTermMode;
class DELPHICLASS TCustomComTerminal;
class DELPHICLASS TComTerminal;
//-- type declarations -------------------------------------------------------
enum DECLSPEC_DENUM TComProperty : unsigned char { cpNone, cpPort, cpBaudRate, cpDataBits, cpStopBits, cpParity, cpFlowControl };

#pragma pack(push,4)
class PASCALIMPLEMENTATION TComSelect : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	System::UnicodeString FPort;
	Cport::TBaudRate FBaudRate;
	Cport::TDataBits FDataBits;
	Cport::TStopBits FStopBits;
	Cport::TParityBits FParity;
	Cport::TFlowControl FFlowControl;
	System::Classes::TStrings* FItems;
	TComProperty FComProperty;
	Cport::TCustomComPort* FComPort;
	bool FAutoApply;
	void __fastcall SetComProperty(const TComProperty Value);
	
public:
	void __fastcall SelectPort();
	void __fastcall SelectBaudRate();
	void __fastcall SelectParity();
	void __fastcall SelectStopBits();
	void __fastcall SelectDataBits();
	void __fastcall SelectFlowControl();
	void __fastcall Change(const System::UnicodeString Text);
	void __fastcall UpdateSettings(int &ItemIndex);
	void __fastcall ApplySettings();
	__property System::Classes::TStrings* Items = {read=FItems, write=FItems};
	__property TComProperty ComProperty = {read=FComProperty, write=SetComProperty, nodefault};
	__property Cport::TCustomComPort* ComPort = {read=FComPort, write=FComPort};
	__property bool AutoApply = {read=FAutoApply, write=FAutoApply, nodefault};
public:
	/* TObject.Create */ inline __fastcall TComSelect() : System::TObject() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TComSelect() { }
	
};

#pragma pack(pop)

class PASCALIMPLEMENTATION TComComboBox : public Vcl::Stdctrls::TCustomComboBox
{
	typedef Vcl::Stdctrls::TCustomComboBox inherited;
	
private:
	TComSelect* FComSelect;
	bool __fastcall GetAutoApply();
	Cport::TCustomComPort* __fastcall GetComPort();
	TComProperty __fastcall GetComProperty();
	HIDESBASE System::UnicodeString __fastcall GetText();
	void __fastcall SetAutoApply(const bool Value);
	void __fastcall SetComPort(Cport::TCustomComPort* const Value);
	void __fastcall SetComProperty(const TComProperty Value);
	HIDESBASE void __fastcall SetText(const System::UnicodeString Value);
	
protected:
	virtual void __fastcall Notification(System::Classes::TComponent* AComponent, System::Classes::TOperation Operation);
	DYNAMIC void __fastcall Change();
	
public:
	__fastcall virtual TComComboBox(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TComComboBox();
	void __fastcall ApplySettings();
	void __fastcall UpdateSettings();
	
__published:
	__property Cport::TCustomComPort* ComPort = {read=GetComPort, write=SetComPort};
	__property TComProperty ComProperty = {read=GetComProperty, write=SetComProperty, default=0};
	__property bool AutoApply = {read=GetAutoApply, write=SetAutoApply, default=0};
	__property System::UnicodeString Text = {read=GetText, write=SetText};
	__property Style = {default=0};
	__property Color = {default=-16777211};
	__property Ctl3D;
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property DropDownCount = {default=8};
	__property Enabled = {default=1};
	__property Font;
	__property ImeMode = {default=3};
	__property ImeName = {default=0};
	__property ItemHeight;
	__property ItemIndex;
	__property ParentColor = {default=0};
	__property ParentCtl3D = {default=1};
	__property ParentFont = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ShowHint;
	__property TabOrder = {default=-1};
	__property TabStop = {default=1};
	__property Visible = {default=1};
	__property Anchors = {default=3};
	__property BiDiMode;
	__property CharCase = {default=0};
	__property Constraints;
	__property DragKind = {default=0};
	__property ParentBiDiMode = {default=1};
	__property OnChange;
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnDrawItem;
	__property OnDropDown;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMeasureItem;
	__property OnStartDrag;
	__property OnEndDock;
	__property OnStartDock;
	__property OnContextPopup;
public:
	/* TWinControl.CreateParented */ inline __fastcall TComComboBox(HWND ParentWindow) : Vcl::Stdctrls::TCustomComboBox(ParentWindow) { }
	
};


class PASCALIMPLEMENTATION TComRadioGroup : public Vcl::Extctrls::TCustomRadioGroup
{
	typedef Vcl::Extctrls::TCustomRadioGroup inherited;
	
private:
	TComSelect* FComSelect;
	int FOldIndex;
	bool __fastcall GetAutoApply();
	Cport::TCustomComPort* __fastcall GetComPort();
	TComProperty __fastcall GetComProperty();
	void __fastcall SetAutoApply(const bool Value);
	void __fastcall SetComPort(Cport::TCustomComPort* const Value);
	void __fastcall SetComProperty(const TComProperty Value);
	
protected:
	virtual void __fastcall Notification(System::Classes::TComponent* AComponent, System::Classes::TOperation Operation);
	DYNAMIC void __fastcall Click();
	
public:
	__fastcall virtual TComRadioGroup(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TComRadioGroup();
	void __fastcall ApplySettings();
	void __fastcall UpdateSettings();
	
__published:
	__property Cport::TCustomComPort* ComPort = {read=GetComPort, write=SetComPort};
	__property TComProperty ComProperty = {read=GetComProperty, write=SetComProperty, default=0};
	__property bool AutoApply = {read=GetAutoApply, write=SetAutoApply, default=0};
	__property Align = {default=0};
	__property Caption = {default=0};
	__property Color = {default=-16777211};
	__property Ctl3D;
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Enabled = {default=1};
	__property Font;
	__property ItemIndex = {default=-1};
	__property ParentColor = {default=1};
	__property ParentCtl3D = {default=1};
	__property ParentFont = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ShowHint;
	__property TabOrder = {default=-1};
	__property TabStop = {default=0};
	__property Visible = {default=1};
	__property Anchors = {default=3};
	__property BiDiMode;
	__property Constraints;
	__property DockSite = {default=0};
	__property DragKind = {default=0};
	__property ParentBiDiMode = {default=1};
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnStartDrag;
	__property OnEnter;
	__property OnExit;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnMouseDown;
	__property OnEndDock;
	__property OnStartDock;
	__property OnGetSiteInfo;
	__property OnDockDrop;
	__property OnDockOver;
	__property OnUnDock;
	__property OnContextPopup;
public:
	/* TWinControl.CreateParented */ inline __fastcall TComRadioGroup(HWND ParentWindow) : Vcl::Extctrls::TCustomRadioGroup(ParentWindow) { }
	
};


typedef Vcl::Graphics::TBitmap TLedBitmap;

enum DECLSPEC_DENUM TLedKind : unsigned char { lkRedLight, lkGreenLight, lkBlueLight, lkYellowLight, lkPurpleLight, lkBulb, lkCustom };

enum DECLSPEC_DENUM TComLedSignal : unsigned char { lsConn, lsCTS, lsDSR, lsRLSD, lsRing, lsRx, lsTx };

enum DECLSPEC_DENUM TLedState : unsigned char { lsOff, lsOn };

typedef System::StaticArray<Vcl::Graphics::TBitmap*, 2> TComLedGlyphs;

typedef void __fastcall (__closure *TLedStateEvent)(System::TObject* Sender, TLedState AState);

class PASCALIMPLEMENTATION TComLed : public Vcl::Controls::TGraphicControl
{
	typedef Vcl::Controls::TGraphicControl inherited;
	
private:
	Cport::TComPort* FComPort;
	TComLedSignal FLedSignal;
	TLedKind FKind;
	TLedState FState;
	TLedStateEvent FOnChange;
	TComLedGlyphs FGlyphs;
	Cport::TComLink* FComLink;
	Vcl::Extctrls::TTimer* FRingTimer;
	Vcl::Graphics::TBitmap* __fastcall GetGlyph(const int Index);
	int __fastcall GetRingDuration();
	void __fastcall SetComPort(Cport::TComPort* const Value);
	void __fastcall SetKind(const TLedKind Value);
	void __fastcall SetState(const TLedState Value);
	void __fastcall SetLedSignal(const TComLedSignal Value);
	void __fastcall SetGlyph(const int Index, Vcl::Graphics::TBitmap* const Value);
	void __fastcall SetRingDuration(const int Value);
	bool __fastcall StoredGlyph(const int Index);
	void __fastcall SelectLedBitmap(const TLedKind LedKind);
	void __fastcall SetStateInternal(const TLedState Value);
	System::Types::TPoint __fastcall CalcBitmapPos();
	Vcl::Graphics::TBitmap* __fastcall BitmapToDraw();
	void __fastcall BitmapNeeded();
	void __fastcall SignalChange(System::TObject* Sender, bool OnOff);
	void __fastcall RingDetect(System::TObject* Sender);
	void __fastcall DoTimer(System::TObject* Sender);
	bool __fastcall IsStateOn();
	
protected:
	virtual void __fastcall Paint();
	virtual void __fastcall Notification(System::Classes::TComponent* AComponent, System::Classes::TOperation Operation);
	DYNAMIC void __fastcall DoChange(TLedState AState);
	HIDESBASE MESSAGE void __fastcall CMEnabledChanged(Winapi::Messages::TMessage &Message);
	
public:
	__fastcall virtual TComLed(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TComLed();
	
__published:
	__property Cport::TComPort* ComPort = {read=FComPort, write=SetComPort};
	__property TComLedSignal LedSignal = {read=FLedSignal, write=SetLedSignal, nodefault};
	__property TLedKind Kind = {read=FKind, write=SetKind, nodefault};
	__property Vcl::Graphics::TBitmap* GlyphOn = {read=GetGlyph, write=SetGlyph, stored=StoredGlyph, index=0};
	__property Vcl::Graphics::TBitmap* GlyphOff = {read=GetGlyph, write=SetGlyph, stored=StoredGlyph, index=1};
	__property TLedState State = {read=FState, write=SetState, default=0};
	__property int RingDuration = {read=GetRingDuration, write=SetRingDuration, default=1000};
	__property Align = {default=0};
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Enabled = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ShowHint;
	__property Visible = {default=1};
	__property Anchors = {default=3};
	__property Constraints;
	__property DragKind = {default=0};
	__property ParentBiDiMode = {default=1};
	__property TLedStateEvent OnChange = {read=FOnChange, write=FOnChange};
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnStartDrag;
	__property OnEndDock;
	__property OnResize;
	__property OnStartDock;
	__property OnContextPopup;
};


struct DECLSPEC_DRECORD TComTermChar
{
public:
	System::WideChar Ch;
	System::Uitypes::TColor FrontColor;
	System::Uitypes::TColor BackColor;
	bool Underline;
};


#pragma pack(push,4)
class PASCALIMPLEMENTATION TComTermBuffer : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	void *FBuffer;
	void *FTabs;
	TCustomComTerminal* FOwner;
	
public:
	__fastcall TComTermBuffer(TCustomComTerminal* AOwner);
	__fastcall virtual ~TComTermBuffer();
	void __fastcall Init();
	void __fastcall SetChar(int Column, int Row, const TComTermChar &TermChar);
	TComTermChar __fastcall GetChar(int Column, int Row);
	void __fastcall SetTab(int Column, bool Put);
	bool __fastcall GetTab(int Column);
	int __fastcall NextTab(int Column);
	void __fastcall ClearAllTabs();
	void __fastcall ScrollDown();
	void __fastcall ScrollUp();
	void __fastcall EraseScreen(int Column, int Row);
	void __fastcall EraseLine(int Column, int Row);
	int __fastcall GetLineLength(int Line);
	int __fastcall GetLastLine();
};

#pragma pack(pop)

enum DECLSPEC_DENUM TTermEmulation : unsigned char { teVT100orANSI, teVT52, teNone };

enum DECLSPEC_DENUM TTermCaret : unsigned char { tcBlock, tcUnderline, tcNone };

enum DECLSPEC_DENUM TAdvanceCaret : unsigned char { acChar, acReturn, acLineFeed, acReverseLineFeed, acTab, acBackspace, acPage };

enum DECLSPEC_DENUM TArrowKeys : unsigned char { akTerminal, akWindows };

struct DECLSPEC_DRECORD TTermAttributes
{
public:
	System::Uitypes::TColor FrontColor;
	System::Uitypes::TColor BackColor;
	bool Invert;
	bool AltIntensity;
	bool Underline;
};


struct DECLSPEC_DRECORD TTermMode
{
public:
	TArrowKeys Keys;
};


typedef void __fastcall (__closure *TEscapeEvent)(System::TObject* Sender, Cportesc::TEscapeCodes* &EscapeCodes);

typedef void __fastcall (__closure *TUnhandledEvent)(System::TObject* Sender, Cportesc::TEscapeCode Code, System::UnicodeString Data);

typedef void __fastcall (__closure *TStrRecvEvent)(System::TObject* Sender, System::UnicodeString &Str);

typedef void __fastcall (__closure *TChScreenEvent)(System::TObject* Sender, System::WideChar Ch);

class PASCALIMPLEMENTATION TCustomComTerminal : public Vcl::Controls::TCustomControl
{
	typedef Vcl::Controls::TCustomControl inherited;
	
private:
	Cport::TComLink* FComLink;
	Cport::TCustomComPort* FComPort;
	Vcl::Forms::TFormBorderStyle FBorderStyle;
	System::Uitypes::TScrollStyle FScrollBars;
	TArrowKeys FArrowKeys;
	bool FWantTab;
	int FColumns;
	int FRows;
	System::Uitypes::TColor FAltColor;
	bool FLocalEcho;
	bool FSendLF;
	bool FAppendLF;
	bool FForce7Bit;
	bool FWrapLines;
	bool FSmoothScroll;
	int FFontHeight;
	int FFontWidth;
	TTermEmulation FEmulation;
	TTermCaret FCaret;
	System::Types::TPoint FCaretPos;
	System::Types::TPoint FSaveCaret;
	bool FCaretCreated;
	System::Types::TPoint FTopLeft;
	int FCaretHeight;
	TTermAttributes FSaveAttr;
	TComTermBuffer* FBuffer;
	Cportesc::TEscapeCodes* FEscapeCodes;
	TTermAttributes FTermAttr;
	TTermMode FTermMode;
	TChScreenEvent FOnChar;
	TEscapeEvent FOnGetEscapeCodes;
	TUnhandledEvent FOnUnhandledCode;
	TStrRecvEvent FOnStrRecieved;
	void __fastcall AdvanceCaret(TAdvanceCaret Kind);
	bool __fastcall CalculateMetrics();
	void __fastcall CreateEscapeCodes();
	void __fastcall CreateTerminalCaret();
	void __fastcall DrawChar(int AColumn, int ARow, const TComTermChar &Ch);
	TComTermChar __fastcall GetCharAttr();
	bool __fastcall GetConnected();
	void __fastcall HideCaret();
	void __fastcall InitCaret();
	void __fastcall InvalidatePortion(const System::Types::TRect &ARect);
	void __fastcall ModifyScrollBar(int ScrollBar, int ScrollCode, int Pos);
	void __fastcall SetAltColor(const System::Uitypes::TColor Value);
	void __fastcall SetBorderStyle(const Vcl::Forms::TBorderStyle Value);
	void __fastcall SetColumns(const int Value);
	void __fastcall SetComPort(Cport::TCustomComPort* const Value);
	void __fastcall SetConnected(const bool Value);
	void __fastcall SetEmulation(const TTermEmulation Value);
	void __fastcall SetRows(const int Value);
	void __fastcall SetScrollBars(const System::Uitypes::TScrollStyle Value);
	void __fastcall SetCaret(const TTermCaret Value);
	void __fastcall SetAttributes(System::Classes::TStrings* AParams);
	void __fastcall SetMode(System::Classes::TStrings* AParams, bool OnOff);
	void __fastcall ShowCaret();
	void __fastcall StringReceived(System::UnicodeString Str);
	void __fastcall PaintTerminal(const System::Types::TRect &Rect);
	void __fastcall PaintDesign();
	void __fastcall PutChar(System::WideChar Ch);
	bool __fastcall PutEscapeCode(Cportesc::TEscapeCode ACode, System::Classes::TStrings* AParams);
	void __fastcall RestoreAttr();
	void __fastcall RestoreCaretPos();
	void __fastcall RxBuf(System::TObject* Sender, const void *Buffer, int Count);
	void __fastcall SaveAttr();
	void __fastcall SaveCaretPos();
	void __fastcall SendChar(System::WideChar Ch);
	void __fastcall SendCode(Cportesc::TEscapeCode Code, System::Classes::TStrings* AParams);
	void __fastcall SendCodeNoEcho(Cportesc::TEscapeCode Code, System::Classes::TStrings* AParams);
	void __fastcall PerformTest(System::WideChar ACh);
	void __fastcall UpdateScrollPos();
	void __fastcall UpdateScrollRange();
	
protected:
	HIDESBASE MESSAGE void __fastcall CMCtl3DChanged(Winapi::Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Winapi::Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMColorChanged(Winapi::Messages::TMessage &Message);
	MESSAGE void __fastcall WMGetDlgCode(Winapi::Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall WMHScroll(Winapi::Messages::TWMScroll &Message);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Winapi::Messages::TWMSetFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMLButtonDown(Winapi::Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Winapi::Messages::TWMSetFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Winapi::Messages::TWMSize &Msg);
	HIDESBASE MESSAGE void __fastcall WMVScroll(Winapi::Messages::TWMScroll &Message);
	virtual bool __fastcall CanAutoSize(int &NewWidth, int &NewHeight);
	virtual void __fastcall CreateParams(Vcl::Controls::TCreateParams &Params);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, System::Classes::TShiftState Shift);
	DYNAMIC void __fastcall KeyPress(System::WideChar &Key);
	virtual void __fastcall Loaded();
	virtual void __fastcall Notification(System::Classes::TComponent* AComponent, System::Classes::TOperation Operation);
	virtual void __fastcall Paint();
	DYNAMIC void __fastcall DoChar(System::WideChar Ch);
	DYNAMIC void __fastcall DoGetEscapeCodes(Cportesc::TEscapeCodes* &EscapeCodes);
	DYNAMIC void __fastcall DoStrRecieved(System::UnicodeString &Str);
	DYNAMIC void __fastcall DoUnhandledCode(Cportesc::TEscapeCode Code, System::UnicodeString Data);
	
public:
	__fastcall virtual TCustomComTerminal(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TCustomComTerminal();
	void __fastcall ClearScreen();
	void __fastcall MoveCaret(int AColumn, int ARow);
	void __fastcall Write(const System::UnicodeString Buffer, int Size);
	void __fastcall WriteStr(const System::UnicodeString Str);
	void __fastcall WriteEscCode(Cportesc::TEscapeCode ACode, System::Classes::TStrings* AParams);
	void __fastcall LoadFromStream(System::Classes::TStream* Stream);
	void __fastcall SaveToStream(System::Classes::TStream* Stream);
	void __fastcall SelectFont();
	void __fastcall ShowSetupDialog();
	__property System::Uitypes::TColor AltColor = {read=FAltColor, write=SetAltColor, default=10921638};
	__property bool AppendLF = {read=FAppendLF, write=FAppendLF, default=0};
	__property TArrowKeys ArrowKeys = {read=FArrowKeys, write=FArrowKeys, default=0};
	__property Vcl::Forms::TBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, default=1};
	__property TTermCaret Caret = {read=FCaret, write=SetCaret, default=0};
	__property bool Connected = {read=GetConnected, write=SetConnected, stored=false, nodefault};
	__property Cport::TCustomComPort* ComPort = {read=FComPort, write=SetComPort};
	__property int Columns = {read=FColumns, write=SetColumns, default=80};
	__property TTermEmulation Emulation = {read=FEmulation, write=SetEmulation, nodefault};
	__property Cportesc::TEscapeCodes* EscapeCodes = {read=FEscapeCodes};
	__property bool Force7Bit = {read=FForce7Bit, write=FForce7Bit, default=0};
	__property bool LocalEcho = {read=FLocalEcho, write=FLocalEcho, default=0};
	__property bool SendLF = {read=FSendLF, write=FSendLF, default=0};
	__property System::Uitypes::TScrollStyle ScrollBars = {read=FScrollBars, write=SetScrollBars, nodefault};
	__property bool SmoothScroll = {read=FSmoothScroll, write=FSmoothScroll, default=0};
	__property int Rows = {read=FRows, write=SetRows, default=24};
	__property bool WantTab = {read=FWantTab, write=FWantTab, default=0};
	__property bool WrapLines = {read=FWrapLines, write=FWrapLines, default=0};
	__property TChScreenEvent OnChar = {read=FOnChar, write=FOnChar};
	__property TEscapeEvent OnGetEscapeCodes = {read=FOnGetEscapeCodes, write=FOnGetEscapeCodes};
	__property TStrRecvEvent OnStrRecieved = {read=FOnStrRecieved, write=FOnStrRecieved};
	__property TUnhandledEvent OnUnhandledCode = {read=FOnUnhandledCode, write=FOnUnhandledCode};
public:
	/* TWinControl.CreateParented */ inline __fastcall TCustomComTerminal(HWND ParentWindow) : Vcl::Controls::TCustomControl(ParentWindow) { }
	
};


class PASCALIMPLEMENTATION TComTerminal : public TCustomComTerminal
{
	typedef TCustomComTerminal inherited;
	
__published:
	__property Align = {default=0};
	__property AltColor = {default=10921638};
	__property AppendLF = {default=0};
	__property ArrowKeys = {default=0};
	__property BorderStyle = {default=1};
	__property Color = {default=-16777211};
	__property Columns = {default=80};
	__property ComPort;
	__property Connected;
	__property Ctl3D;
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Emulation;
	__property Enabled = {default=1};
	__property Font;
	__property Force7Bit = {default=0};
	__property Hint = {default=0};
	__property ImeMode = {default=3};
	__property ImeName = {default=0};
	__property LocalEcho = {default=0};
	__property ParentCtl3D = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property Rows = {default=24};
	__property ScrollBars;
	__property SendLF = {default=0};
	__property ShowHint;
	__property SmoothScroll = {default=0};
	__property TabOrder = {default=-1};
	__property TabStop = {default=1};
	__property Caret = {default=0};
	__property Visible = {default=1};
	__property WantTab = {default=0};
	__property WrapLines = {default=0};
	__property Anchors = {default=3};
	__property AutoSize = {default=0};
	__property Constraints;
	__property DragKind = {default=0};
	__property OnChar;
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnGetEscapeCodes;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnStartDrag;
	__property OnStrRecieved;
	__property OnUnhandledCode;
	__property OnCanResize;
	__property OnConstrainedResize;
	__property OnDockDrop;
	__property OnEndDock;
	__property OnMouseWheel;
	__property OnMouseWheelDown;
	__property OnMouseWheelUp;
	__property OnResize;
	__property OnStartDock;
	__property OnUnDock;
	__property OnContextPopup;
public:
	/* TCustomComTerminal.Create */ inline __fastcall virtual TComTerminal(System::Classes::TComponent* AOwner) : TCustomComTerminal(AOwner) { }
	/* TCustomComTerminal.Destroy */ inline __fastcall virtual ~TComTerminal() { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TComTerminal(HWND ParentWindow) : TCustomComTerminal(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE Vcl::Graphics::TFont* ComTerminalFont;
}	/* namespace Cportctl */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_CPORTCTL)
using namespace Cportctl;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// CportctlHPP
