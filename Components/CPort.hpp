// CodeGear C++Builder
// Copyright (c) 1995, 2022 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'CPort.pas' rev: 35.00 (Windows)

#ifndef CportHPP
#define CportHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <Winapi.Windows.hpp>
#include <Winapi.Messages.hpp>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <System.IniFiles.hpp>
#include <System.Win.Registry.hpp>

//-- user supplied -----------------------------------------------------------

namespace Cport
{
//-- forward type declarations -----------------------------------------------
struct TAsync;
class DELPHICLASS TComLink;
class DELPHICLASS TComThread;
class DELPHICLASS TComTimeouts;
class DELPHICLASS TComFlowControl;
class DELPHICLASS TComParity;
class DELPHICLASS TComBuffer;
class DELPHICLASS TCustomComPort;
class DELPHICLASS TComPort;
class DELPHICLASS TComDataPacket;
class DELPHICLASS TComStream;
class DELPHICLASS EComPort;
//-- type declarations -------------------------------------------------------
typedef System::UnicodeString TPort;

enum DECLSPEC_DENUM TBaudRate : unsigned char { brCustom, br110, br300, br600, br1200, br2400, br4800, br9600, br14400, br19200, br38400, br56000, br57600, br115200, br128000, br256000 };

enum DECLSPEC_DENUM TStopBits : unsigned char { sbOneStopBit, sbOne5StopBits, sbTwoStopBits };

enum DECLSPEC_DENUM TDataBits : unsigned char { dbFive, dbSix, dbSeven, dbEight };

enum DECLSPEC_DENUM TParityBits : unsigned char { prNone, prOdd, prEven, prMark, prSpace };

enum DECLSPEC_DENUM TDTRFlowControl : unsigned char { dtrDisable, dtrEnable, dtrHandshake };

enum DECLSPEC_DENUM TRTSFlowControl : unsigned char { rtsDisable, rtsEnable, rtsHandshake, rtsToggle };

enum DECLSPEC_DENUM TFlowControl : unsigned char { fcHardware, fcSoftware, fcNone, fcCustom };

enum DECLSPEC_DENUM TComEvent : unsigned char { evRxChar, evTxEmpty, evRxFlag, evRing, evBreak, evCTS, evDSR, evError, evRLSD, evRx80Full };

typedef System::Set<TComEvent, TComEvent::evRxChar, TComEvent::evRx80Full> TComEvents;

enum DECLSPEC_DENUM TComSignal : unsigned char { csCTS, csDSR, csRing, csRLSD };

typedef System::Set<TComSignal, TComSignal::csCTS, TComSignal::csRLSD> TComSignals;

enum DECLSPEC_DENUM TComError : unsigned char { ceFrame, ceRxParity, ceOverrun, ceBreak, ceIO, ceMode, ceRxOver, ceTxFull };

typedef System::Set<TComError, TComError::ceFrame, TComError::ceTxFull> TComErrors;

enum DECLSPEC_DENUM TSyncMethod : unsigned char { smThreadSync, smWindowSync, smNone };

enum DECLSPEC_DENUM TStoreType : unsigned char { stRegistry, stIniFile };

enum DECLSPEC_DENUM TStoredProp : unsigned char { spBasic, spFlowControl, spBuffer, spTimeouts, spParity, spOthers };

typedef System::Set<TStoredProp, TStoredProp::spBasic, TStoredProp::spOthers> TStoredProps;

enum DECLSPEC_DENUM TComLinkEvent : unsigned char { leConn, leCTS, leDSR, leRLSD, leRing, leRx, leTx, leTxEmpty, leRxFlag };

typedef void __fastcall (__closure *TRxCharEvent)(System::TObject* Sender, int Count);

typedef void __fastcall (__closure *TRxBufEvent)(System::TObject* Sender, const void *Buffer, int Count);

typedef void __fastcall (__closure *TComErrorEvent)(System::TObject* Sender, TComErrors Errors);

typedef void __fastcall (__closure *TComSignalEvent)(System::TObject* Sender, bool OnOff);

enum DECLSPEC_DENUM TOperationKind : unsigned char { okWrite, okRead };

struct DECLSPEC_DRECORD TAsync
{
public:
	_OVERLAPPED Overlapped;
	TOperationKind Kind;
	void *Data;
	int Size;
};


typedef TAsync *PAsync;

class PASCALIMPLEMENTATION TComLink : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	TComSignalEvent FOnConn;
	TRxBufEvent FOnRxBuf;
	TRxBufEvent FOnTxBuf;
	System::Classes::TNotifyEvent FOnTxEmpty;
	System::Classes::TNotifyEvent FOnRxFlag;
	TComSignalEvent FOnCTSChange;
	TComSignalEvent FOnDSRChange;
	TComSignalEvent FOnRLSDChange;
	System::Classes::TNotifyEvent FOnRing;
	TComSignalEvent FOnTx;
	TComSignalEvent FOnRx;
	
public:
	__property TComSignalEvent OnConn = {read=FOnConn, write=FOnConn};
	__property TRxBufEvent OnRxBuf = {read=FOnRxBuf, write=FOnRxBuf};
	__property TRxBufEvent OnTxBuf = {read=FOnTxBuf, write=FOnTxBuf};
	__property System::Classes::TNotifyEvent OnTxEmpty = {read=FOnTxEmpty, write=FOnTxEmpty};
	__property System::Classes::TNotifyEvent OnRxFlag = {read=FOnRxFlag, write=FOnRxFlag};
	__property TComSignalEvent OnCTSChange = {read=FOnCTSChange, write=FOnCTSChange};
	__property TComSignalEvent OnDSRChange = {read=FOnDSRChange, write=FOnDSRChange};
	__property TComSignalEvent OnRLSDChange = {read=FOnRLSDChange, write=FOnRLSDChange};
	__property System::Classes::TNotifyEvent OnRing = {read=FOnRing, write=FOnRing};
	__property TComSignalEvent OnTx = {read=FOnTx, write=FOnTx};
	__property TComSignalEvent OnRx = {read=FOnRx, write=FOnRx};
public:
	/* TObject.Create */ inline __fastcall TComLink() : System::TObject() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TComLink() { }
	
};


class PASCALIMPLEMENTATION TComThread : public System::Classes::TThread
{
	typedef System::Classes::TThread inherited;
	
private:
	TCustomComPort* FComPort;
	NativeUInt FStopEvent;
	TComEvents FEvents;
	
protected:
	void __fastcall DispatchComMsg();
	void __fastcall DoEvents();
	virtual void __fastcall Execute();
	void __fastcall SendEvents();
	void __fastcall Stop();
	
public:
	__fastcall TComThread(TCustomComPort* AComPort);
	__fastcall virtual ~TComThread();
};


#pragma pack(push,4)
class PASCALIMPLEMENTATION TComTimeouts : public System::Classes::TPersistent
{
	typedef System::Classes::TPersistent inherited;
	
private:
	TCustomComPort* FComPort;
	int FReadInterval;
	int FReadTotalM;
	int FReadTotalC;
	int FWriteTotalM;
	int FWriteTotalC;
	void __fastcall SetComPort(TCustomComPort* const AComPort);
	void __fastcall SetReadInterval(const int Value);
	void __fastcall SetReadTotalM(const int Value);
	void __fastcall SetReadTotalC(const int Value);
	void __fastcall SetWriteTotalM(const int Value);
	void __fastcall SetWriteTotalC(const int Value);
	
protected:
	virtual void __fastcall AssignTo(System::Classes::TPersistent* Dest);
	
public:
	__fastcall TComTimeouts();
	__property TCustomComPort* ComPort = {read=FComPort};
	
__published:
	__property int ReadInterval = {read=FReadInterval, write=SetReadInterval, default=-1};
	__property int ReadTotalMultiplier = {read=FReadTotalM, write=SetReadTotalM, default=0};
	__property int ReadTotalConstant = {read=FReadTotalC, write=SetReadTotalC, default=0};
	__property int WriteTotalMultiplier = {read=FWriteTotalM, write=SetWriteTotalM, default=100};
	__property int WriteTotalConstant = {read=FWriteTotalC, write=SetWriteTotalC, default=1000};
public:
	/* TPersistent.Destroy */ inline __fastcall virtual ~TComTimeouts() { }
	
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION TComFlowControl : public System::Classes::TPersistent
{
	typedef System::Classes::TPersistent inherited;
	
private:
	TCustomComPort* FComPort;
	bool FOutCTSFlow;
	bool FOutDSRFlow;
	TDTRFlowControl FControlDTR;
	TRTSFlowControl FControlRTS;
	bool FXonXoffOut;
	bool FXonXoffIn;
	bool FDSRSensitivity;
	bool FTxContinueOnXoff;
	System::WideChar FXonChar;
	System::WideChar FXoffChar;
	void __fastcall SetComPort(TCustomComPort* const AComPort);
	void __fastcall SetOutCTSFlow(const bool Value);
	void __fastcall SetOutDSRFlow(const bool Value);
	void __fastcall SetControlDTR(const TDTRFlowControl Value);
	void __fastcall SetControlRTS(const TRTSFlowControl Value);
	void __fastcall SetXonXoffOut(const bool Value);
	void __fastcall SetXonXoffIn(const bool Value);
	void __fastcall SetDSRSensitivity(const bool Value);
	void __fastcall SetTxContinueOnXoff(const bool Value);
	void __fastcall SetXonChar(const System::WideChar Value);
	void __fastcall SetXoffChar(const System::WideChar Value);
	void __fastcall SetFlowControl(const TFlowControl Value);
	TFlowControl __fastcall GetFlowControl();
	
protected:
	virtual void __fastcall AssignTo(System::Classes::TPersistent* Dest);
	
public:
	__fastcall TComFlowControl();
	__property TCustomComPort* ComPort = {read=FComPort};
	
__published:
	__property TFlowControl FlowControl = {read=GetFlowControl, write=SetFlowControl, stored=false, nodefault};
	__property bool OutCTSFlow = {read=FOutCTSFlow, write=SetOutCTSFlow, nodefault};
	__property bool OutDSRFlow = {read=FOutDSRFlow, write=SetOutDSRFlow, nodefault};
	__property TDTRFlowControl ControlDTR = {read=FControlDTR, write=SetControlDTR, nodefault};
	__property TRTSFlowControl ControlRTS = {read=FControlRTS, write=SetControlRTS, nodefault};
	__property bool XonXoffOut = {read=FXonXoffOut, write=SetXonXoffOut, nodefault};
	__property bool XonXoffIn = {read=FXonXoffIn, write=SetXonXoffIn, nodefault};
	__property bool DSRSensitivity = {read=FDSRSensitivity, write=SetDSRSensitivity, default=0};
	__property bool TxContinueOnXoff = {read=FTxContinueOnXoff, write=SetTxContinueOnXoff, default=0};
	__property System::WideChar XonChar = {read=FXonChar, write=SetXonChar, default=17};
	__property System::WideChar XoffChar = {read=FXoffChar, write=SetXoffChar, default=19};
public:
	/* TPersistent.Destroy */ inline __fastcall virtual ~TComFlowControl() { }
	
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION TComParity : public System::Classes::TPersistent
{
	typedef System::Classes::TPersistent inherited;
	
private:
	TCustomComPort* FComPort;
	TParityBits FBits;
	bool FCheck;
	bool FReplace;
	System::WideChar FReplaceChar;
	void __fastcall SetComPort(TCustomComPort* const AComPort);
	void __fastcall SetBits(const TParityBits Value);
	void __fastcall SetCheck(const bool Value);
	void __fastcall SetReplace(const bool Value);
	void __fastcall SetReplaceChar(const System::WideChar Value);
	
protected:
	virtual void __fastcall AssignTo(System::Classes::TPersistent* Dest);
	
public:
	__fastcall TComParity();
	__property TCustomComPort* ComPort = {read=FComPort};
	
__published:
	__property TParityBits Bits = {read=FBits, write=SetBits, nodefault};
	__property bool Check = {read=FCheck, write=SetCheck, default=0};
	__property bool Replace = {read=FReplace, write=SetReplace, default=0};
	__property System::WideChar ReplaceChar = {read=FReplaceChar, write=SetReplaceChar, default=0};
public:
	/* TPersistent.Destroy */ inline __fastcall virtual ~TComParity() { }
	
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION TComBuffer : public System::Classes::TPersistent
{
	typedef System::Classes::TPersistent inherited;
	
private:
	TCustomComPort* FComPort;
	int FInputSize;
	int FOutputSize;
	void __fastcall SetComPort(TCustomComPort* const AComPort);
	void __fastcall SetInputSize(const int Value);
	void __fastcall SetOutputSize(const int Value);
	
protected:
	virtual void __fastcall AssignTo(System::Classes::TPersistent* Dest);
	
public:
	__fastcall TComBuffer();
	__property TCustomComPort* ComPort = {read=FComPort};
	
__published:
	__property int InputSize = {read=FInputSize, write=SetInputSize, default=1024};
	__property int OutputSize = {read=FOutputSize, write=SetOutputSize, default=1024};
public:
	/* TPersistent.Destroy */ inline __fastcall virtual ~TComBuffer() { }
	
};

#pragma pack(pop)

class PASCALIMPLEMENTATION TCustomComPort : public System::Classes::TComponent
{
	typedef System::Classes::TComponent inherited;
	
private:
	TComThread* FEventThread;
	bool FThreadCreated;
	NativeUInt FHandle;
	NativeUInt FWindow;
	int FUpdateCount;
	System::Classes::TList* FLinks;
	bool FTriggersOnRxChar;
	System::Classes::TThreadPriority FEventThreadPriority;
	bool FHasLink;
	bool FConnected;
	TBaudRate FBaudRate;
	int FCustomBaudRate;
	System::UnicodeString FPort;
	TStopBits FStopBits;
	TDataBits FDataBits;
	bool FDiscardNull;
	System::WideChar FEventChar;
	TComEvents FEvents;
	TComBuffer* FBuffer;
	TComParity* FParity;
	TComTimeouts* FTimeouts;
	TComFlowControl* FFlowControl;
	TSyncMethod FSyncMethod;
	TStoredProps FStoredProps;
	TRxCharEvent FOnRxChar;
	TRxBufEvent FOnRxBuf;
	System::Classes::TNotifyEvent FOnTxEmpty;
	System::Classes::TNotifyEvent FOnBreak;
	System::Classes::TNotifyEvent FOnRing;
	TComSignalEvent FOnCTSChange;
	TComSignalEvent FOnDSRChange;
	TComSignalEvent FOnRLSDChange;
	TComErrorEvent FOnError;
	System::Classes::TNotifyEvent FOnRxFlag;
	System::Classes::TNotifyEvent FOnAfterOpen;
	System::Classes::TNotifyEvent FOnAfterClose;
	System::Classes::TNotifyEvent FOnBeforeOpen;
	System::Classes::TNotifyEvent FOnBeforeClose;
	System::Classes::TNotifyEvent FOnRx80Full;
	bool __fastcall GetTriggersOnRxChar();
	void __fastcall SetTriggersOnRxChar(const bool Value);
	void __fastcall SetConnected(const bool Value);
	void __fastcall SetBaudRate(const TBaudRate Value);
	void __fastcall SetCustomBaudRate(const int Value);
	void __fastcall SetPortW(const System::UnicodeString Value);
	void __fastcall SetStopBits(const TStopBits Value);
	void __fastcall SetDataBits(const TDataBits Value);
	void __fastcall SetDiscardNull(const bool Value);
	void __fastcall SetEventChar(const System::WideChar Value);
	void __fastcall SetSyncMethod(const TSyncMethod Value);
	void __fastcall SetEventThreadPriority(const System::Classes::TThreadPriority Value);
	void __fastcall SetParity(TComParity* const Value);
	void __fastcall SetTimeouts(TComTimeouts* const Value);
	void __fastcall SetBuffer(TComBuffer* const Value);
	void __fastcall SetFlowControl(TComFlowControl* const Value);
	bool __fastcall HasLink();
	void __fastcall TxNotifyLink(const void *Buffer, int Count);
	void __fastcall NotifyLink(TComLinkEvent FLinkEvent);
	void __fastcall SendSignalToLink(TComLinkEvent Signal, bool OnOff);
	void __fastcall CheckSignals(bool Open);
	void __fastcall WindowMethod(Winapi::Messages::TMessage &Message);
	void __fastcall CallAfterOpen();
	void __fastcall CallAfterClose();
	void __fastcall CallBeforeOpen();
	void __fastcall CallBeforeClose();
	void __fastcall CallRxChar();
	void __fastcall CallTxEmpty();
	void __fastcall CallBreak();
	void __fastcall CallRing();
	void __fastcall CallRxFlag();
	void __fastcall CallCTSChange();
	void __fastcall CallDSRChange();
	void __fastcall CallError();
	void __fastcall CallRLSDChange();
	void __fastcall CallRx80Full();
	
protected:
	virtual void __fastcall Loaded();
	DYNAMIC void __fastcall DoAfterClose();
	DYNAMIC void __fastcall DoAfterOpen();
	DYNAMIC void __fastcall DoBeforeClose();
	DYNAMIC void __fastcall DoBeforeOpen();
	DYNAMIC void __fastcall DoRxChar(int Count);
	DYNAMIC void __fastcall DoRxBuf(const void *Buffer, int Count);
	DYNAMIC void __fastcall DoTxEmpty();
	DYNAMIC void __fastcall DoBreak();
	DYNAMIC void __fastcall DoRing();
	DYNAMIC void __fastcall DoRxFlag();
	DYNAMIC void __fastcall DoCTSChange(bool OnOff);
	DYNAMIC void __fastcall DoDSRChange(bool OnOff);
	DYNAMIC void __fastcall DoError(TComErrors Errors);
	DYNAMIC void __fastcall DoRLSDChange(bool OnOff);
	DYNAMIC void __fastcall DoRx80Full();
	virtual void __fastcall StoreRegistry(System::Win::Registry::TRegistry* Reg);
	virtual void __fastcall StoreIniFile(System::Inifiles::TIniFile* IniFile);
	virtual void __fastcall LoadRegistry(System::Win::Registry::TRegistry* Reg);
	virtual void __fastcall LoadIniFile(System::Inifiles::TIniFile* IniFile);
	virtual void __fastcall CreateHandle();
	virtual void __fastcall DestroyHandle();
	DYNAMIC void __fastcall ApplyDCB();
	DYNAMIC void __fastcall ApplyTimeouts();
	DYNAMIC void __fastcall ApplyBuffer();
	virtual void __fastcall SetupComPort();
	
public:
	__fastcall virtual TCustomComPort(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TCustomComPort();
	void __fastcall BeginUpdate();
	void __fastcall EndUpdate();
	void __fastcall StoreSettings(TStoreType StoreType, System::UnicodeString StoreTo);
	void __fastcall LoadSettings(TStoreType StoreType, System::UnicodeString LoadFrom);
	void __fastcall Open();
	void __fastcall Close();
	void __fastcall ShowSetupDialog();
	int __fastcall InputCount();
	int __fastcall OutputCount();
	TComSignals __fastcall Signals();
	Winapi::Windows::TComStateFlags __fastcall StateFlags();
	void __fastcall SetDTR(bool OnOff);
	void __fastcall SetRTS(bool OnOff);
	void __fastcall SetXonXoff(bool OnOff);
	void __fastcall SetBreak(bool OnOff);
	void __fastcall ClearBuffer(bool Input, bool Output);
	TComErrors __fastcall LastErrors();
	int __fastcall Write(const void *Buffer, int Count);
	int __fastcall WriteStr(const System::UnicodeString Str);
	int __fastcall Read(void *Buffer, int Count);
	int __fastcall ReadStr(System::UnicodeString &Str, int Count);
	int __fastcall WriteAsync(const void *Buffer, int Count, PAsync &AsyncPtr);
	int __fastcall WriteStrAsync(const System::UnicodeString Str, PAsync &AsyncPtr);
	int __fastcall ReadAsync(void *Buffer, int Count, PAsync &AsyncPtr);
	int __fastcall ReadStrAsync(System::UnicodeString &Str, int Count, PAsync &AsyncPtr);
	int __fastcall WaitForAsync(PAsync &AsyncPtr);
	bool __fastcall IsAsyncCompleted(PAsync AsyncPtr);
	void __fastcall WaitForEvent(TComEvents &Events, NativeUInt StopEvent, int Timeout);
	void __fastcall AbortAllAsync();
	void __fastcall TransmitChar(System::WideChar Ch);
	void __fastcall RegisterLink(TComLink* AComLink);
	void __fastcall UnRegisterLink(TComLink* AComLink);
	__property NativeUInt Handle = {read=FHandle, nodefault};
	__property bool TriggersOnRxChar = {read=GetTriggersOnRxChar, write=SetTriggersOnRxChar, nodefault};
	__property System::Classes::TThreadPriority EventThreadPriority = {read=FEventThreadPriority, write=SetEventThreadPriority, nodefault};
	__property TStoredProps StoredProps = {read=FStoredProps, write=FStoredProps, nodefault};
	__property bool Connected = {read=FConnected, write=SetConnected, default=0};
	__property TBaudRate BaudRate = {read=FBaudRate, write=SetBaudRate, nodefault};
	__property int CustomBaudRate = {read=FCustomBaudRate, write=SetCustomBaudRate, nodefault};
	__property System::UnicodeString Port = {read=FPort, write=SetPortW};
	__property TComParity* Parity = {read=FParity, write=SetParity};
	__property TStopBits StopBits = {read=FStopBits, write=SetStopBits, nodefault};
	__property TDataBits DataBits = {read=FDataBits, write=SetDataBits, nodefault};
	__property bool DiscardNull = {read=FDiscardNull, write=SetDiscardNull, default=0};
	__property System::WideChar EventChar = {read=FEventChar, write=SetEventChar, default=0};
	__property TComEvents Events = {read=FEvents, write=FEvents, nodefault};
	__property TComBuffer* Buffer = {read=FBuffer, write=SetBuffer};
	__property TComFlowControl* FlowControl = {read=FFlowControl, write=SetFlowControl};
	__property TComTimeouts* Timeouts = {read=FTimeouts, write=SetTimeouts};
	__property TSyncMethod SyncMethod = {read=FSyncMethod, write=SetSyncMethod, default=0};
	__property System::Classes::TNotifyEvent OnAfterOpen = {read=FOnAfterOpen, write=FOnAfterOpen};
	__property System::Classes::TNotifyEvent OnAfterClose = {read=FOnAfterClose, write=FOnAfterClose};
	__property System::Classes::TNotifyEvent OnBeforeOpen = {read=FOnBeforeOpen, write=FOnBeforeOpen};
	__property System::Classes::TNotifyEvent OnBeforeClose = {read=FOnBeforeClose, write=FOnBeforeClose};
	__property TRxCharEvent OnRxChar = {read=FOnRxChar, write=FOnRxChar};
	__property TRxBufEvent OnRxBuf = {read=FOnRxBuf, write=FOnRxBuf};
	__property System::Classes::TNotifyEvent OnTxEmpty = {read=FOnTxEmpty, write=FOnTxEmpty};
	__property System::Classes::TNotifyEvent OnBreak = {read=FOnBreak, write=FOnBreak};
	__property System::Classes::TNotifyEvent OnRing = {read=FOnRing, write=FOnRing};
	__property TComSignalEvent OnCTSChange = {read=FOnCTSChange, write=FOnCTSChange};
	__property TComSignalEvent OnDSRChange = {read=FOnDSRChange, write=FOnDSRChange};
	__property TComSignalEvent OnRLSDChange = {read=FOnRLSDChange, write=FOnRLSDChange};
	__property System::Classes::TNotifyEvent OnRxFlag = {read=FOnRxFlag, write=FOnRxFlag};
	__property TComErrorEvent OnError = {read=FOnError, write=FOnError};
	__property System::Classes::TNotifyEvent OnRx80Full = {read=FOnRx80Full, write=FOnRx80Full};
};


class PASCALIMPLEMENTATION TComPort : public TCustomComPort
{
	typedef TCustomComPort inherited;
	
__published:
	__property Connected = {default=0};
	__property BaudRate;
	__property Port = {default=0};
	__property Parity;
	__property StopBits;
	__property DataBits;
	__property DiscardNull = {default=0};
	__property EventChar = {default=0};
	__property Events;
	__property Buffer;
	__property FlowControl;
	__property Timeouts;
	__property SyncMethod = {default=0};
	__property OnAfterOpen;
	__property OnAfterClose;
	__property OnBeforeOpen;
	__property OnBeforeClose;
	__property OnRxChar;
	__property OnRxBuf;
	__property OnTxEmpty;
	__property OnBreak;
	__property OnRing;
	__property OnCTSChange;
	__property OnDSRChange;
	__property OnRLSDChange;
	__property OnRxFlag;
	__property OnError;
	__property OnRx80Full;
public:
	/* TCustomComPort.Create */ inline __fastcall virtual TComPort(System::Classes::TComponent* AOwner) : TCustomComPort(AOwner) { }
	/* TCustomComPort.Destroy */ inline __fastcall virtual ~TComPort() { }
	
};


typedef void __fastcall (__closure *TComStrEvent)(System::TObject* Sender, const System::UnicodeString Str);

typedef void __fastcall (__closure *TCustPacketEvent)(System::TObject* Sender, const System::UnicodeString Str, int &Pos);

class PASCALIMPLEMENTATION TComDataPacket : public System::Classes::TComponent
{
	typedef System::Classes::TComponent inherited;
	
private:
	TComLink* FComLink;
	TCustomComPort* FComPort;
	System::UnicodeString FStartString;
	System::UnicodeString FStopString;
	int FMaxBufferSize;
	int FSize;
	bool FIncludeStrings;
	bool FCaseInsensitive;
	bool FInPacket;
	System::UnicodeString FBuffer;
	TComStrEvent FOnPacket;
	TComStrEvent FOnDiscard;
	TCustPacketEvent FOnCustomStart;
	TCustPacketEvent FOnCustomStop;
	void __fastcall SetComPort(TCustomComPort* const Value);
	void __fastcall SetCaseInsensitive(const bool Value);
	void __fastcall SetSize(const int Value);
	void __fastcall SetStartString(const System::UnicodeString Value);
	void __fastcall SetStopString(const System::UnicodeString Value);
	void __fastcall RxBuf(System::TObject* Sender, const void *Buffer, int Count);
	void __fastcall CheckIncludeStrings(System::UnicodeString &Str);
	System::UnicodeString __fastcall Upper(const System::UnicodeString Str);
	void __fastcall EmptyBuffer();
	bool __fastcall ValidStop();
	
protected:
	virtual void __fastcall Notification(System::Classes::TComponent* AComponent, System::Classes::TOperation Operation);
	DYNAMIC void __fastcall DoDiscard(const System::UnicodeString Str);
	DYNAMIC void __fastcall DoPacket(const System::UnicodeString Str);
	DYNAMIC void __fastcall DoCustomStart(const System::UnicodeString Str, int &Pos);
	DYNAMIC void __fastcall DoCustomStop(const System::UnicodeString Str, int &Pos);
	virtual void __fastcall HandleBuffer();
	__property System::UnicodeString Buffer = {read=FBuffer, write=FBuffer};
	
public:
	__fastcall virtual TComDataPacket(System::Classes::TComponent* AOwner);
	__fastcall virtual ~TComDataPacket();
	void __fastcall AddData(const System::UnicodeString Str);
	
__published:
	__property TCustomComPort* ComPort = {read=FComPort, write=SetComPort};
	__property bool CaseInsensitive = {read=FCaseInsensitive, write=SetCaseInsensitive, default=0};
	__property bool IncludeStrings = {read=FIncludeStrings, write=FIncludeStrings, default=0};
	__property int MaxBufferSize = {read=FMaxBufferSize, write=FMaxBufferSize, default=1024};
	__property System::UnicodeString StartString = {read=FStartString, write=SetStartString};
	__property System::UnicodeString StopString = {read=FStopString, write=SetStopString};
	__property int Size = {read=FSize, write=SetSize, default=0};
	__property TComStrEvent OnDiscard = {read=FOnDiscard, write=FOnDiscard};
	__property TComStrEvent OnPacket = {read=FOnPacket, write=FOnPacket};
	__property TCustPacketEvent OnCustomStart = {read=FOnCustomStart, write=FOnCustomStart};
	__property TCustPacketEvent OnCustomStop = {read=FOnCustomStop, write=FOnCustomStop};
};


#pragma pack(push,4)
class PASCALIMPLEMENTATION TComStream : public System::Classes::TStream
{
	typedef System::Classes::TStream inherited;
	
private:
	TCustomComPort* FComPort;
	
public:
	__fastcall TComStream(TCustomComPort* AComPort);
	virtual int __fastcall Read(void *Buffer, int Count)/* overload */;
	virtual int __fastcall Write(const void *Buffer, int Count)/* overload */;
	virtual int __fastcall Seek(int Offset, System::Word Origin)/* overload */;
public:
	/* TObject.Destroy */ inline __fastcall virtual ~TComStream() { }
	
	/* Hoisted overloads: */
	
public:
	inline int __fastcall  Read(System::DynamicArray<System::Byte> Buffer, int Offset, int Count){ return System::Classes::TStream::Read(Buffer, Offset, Count); }
	inline int __fastcall  Read(System::DynamicArray<System::Byte> &Buffer, int Count){ return System::Classes::TStream::Read(Buffer, Count); }
	inline int __fastcall  Write(const System::DynamicArray<System::Byte> Buffer, int Offset, int Count){ return System::Classes::TStream::Write(Buffer, Offset, Count); }
	inline int __fastcall  Write(const System::DynamicArray<System::Byte> Buffer, int Count){ return System::Classes::TStream::Write(Buffer, Count); }
	inline __int64 __fastcall  Seek(const __int64 Offset, System::Classes::TSeekOrigin Origin){ return System::Classes::TStream::Seek(Offset, Origin); }
	inline __int64 __fastcall  Seek(const __int64 Offset, System::Word Origin){ return System::Classes::TStream::Seek(Offset, Origin); }
	
};

#pragma pack(pop)

#pragma pack(push,4)
class PASCALIMPLEMENTATION EComPort : public System::Sysutils::Exception
{
	typedef System::Sysutils::Exception inherited;
	
private:
	int FWinCode;
	int FCode;
	
public:
	__fastcall EComPort(int ACode, int AWinCode);
	__fastcall EComPort(int ACode);
	__property int WinCode = {read=FWinCode, write=FWinCode, nodefault};
	__property int Code = {read=FCode, write=FCode, nodefault};
public:
	/* Exception.CreateFmt */ inline __fastcall EComPort(const System::UnicodeString Msg, const System::TVarRec *Args, const int Args_High) : System::Sysutils::Exception(Msg, Args, Args_High) { }
	/* Exception.CreateRes */ inline __fastcall EComPort(NativeUInt Ident)/* overload */ : System::Sysutils::Exception(Ident) { }
	/* Exception.CreateRes */ inline __fastcall EComPort(System::PResStringRec ResStringRec)/* overload */ : System::Sysutils::Exception(ResStringRec) { }
	/* Exception.CreateResFmt */ inline __fastcall EComPort(NativeUInt Ident, const System::TVarRec *Args, const int Args_High)/* overload */ : System::Sysutils::Exception(Ident, Args, Args_High) { }
	/* Exception.CreateResFmt */ inline __fastcall EComPort(System::PResStringRec ResStringRec, const System::TVarRec *Args, const int Args_High)/* overload */ : System::Sysutils::Exception(ResStringRec, Args, Args_High) { }
	/* Exception.CreateHelp */ inline __fastcall EComPort(const System::UnicodeString Msg, int AHelpContext) : System::Sysutils::Exception(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EComPort(const System::UnicodeString Msg, const System::TVarRec *Args, const int Args_High, int AHelpContext) : System::Sysutils::Exception(Msg, Args, Args_High, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EComPort(NativeUInt Ident, int AHelpContext)/* overload */ : System::Sysutils::Exception(Ident, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EComPort(System::PResStringRec ResStringRec, int AHelpContext)/* overload */ : System::Sysutils::Exception(ResStringRec, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EComPort(System::PResStringRec ResStringRec, const System::TVarRec *Args, const int Args_High, int AHelpContext)/* overload */ : System::Sysutils::Exception(ResStringRec, Args, Args_High, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EComPort(NativeUInt Ident, const System::TVarRec *Args, const int Args_High, int AHelpContext)/* overload */ : System::Sysutils::Exception(Ident, Args, Args_High, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EComPort() { }
	
};

#pragma pack(pop)

//-- var, const, procedure ---------------------------------------------------
static const int WaitInfinite = int(-1);
static const System::Int8 CError_OpenFailed = System::Int8(0x1);
static const System::Int8 CError_WriteFailed = System::Int8(0x2);
static const System::Int8 CError_ReadFailed = System::Int8(0x3);
static const System::Int8 CError_InvalidAsync = System::Int8(0x4);
static const System::Int8 CError_PurgeFailed = System::Int8(0x5);
static const System::Int8 CError_AsyncCheck = System::Int8(0x6);
static const System::Int8 CError_SetStateFailed = System::Int8(0x7);
static const System::Int8 CError_TimeoutsFailed = System::Int8(0x8);
static const System::Int8 CError_SetupComFailed = System::Int8(0x9);
static const System::Int8 CError_ClearComFailed = System::Int8(0xa);
static const System::Int8 CError_ModemStatFailed = System::Int8(0xb);
static const System::Int8 CError_EscapeComFailed = System::Int8(0xc);
static const System::Int8 CError_TransmitFailed = System::Int8(0xd);
static const System::Int8 CError_ConnChangeProp = System::Int8(0xe);
static const System::Int8 CError_EnumPortsFailed = System::Int8(0xf);
static const System::Int8 CError_StoreFailed = System::Int8(0x10);
static const System::Int8 CError_LoadFailed = System::Int8(0x11);
static const System::Int8 CError_RegFailed = System::Int8(0x12);
static const System::Int8 CError_LedStateFailed = System::Int8(0x13);
static const System::Int8 CError_ThreadCreated = System::Int8(0x14);
static const System::Int8 CError_WaitFailed = System::Int8(0x15);
static const System::Int8 CError_HasLink = System::Int8(0x16);
static const System::Int8 CError_RegError = System::Int8(0x17);
extern DELPHI_PACKAGE void __fastcall InitAsync(PAsync &AsyncPtr);
extern DELPHI_PACKAGE void __fastcall DoneAsync(PAsync &AsyncPtr);
extern DELPHI_PACKAGE void __fastcall EnumComPorts(System::Classes::TStrings* Ports);
extern DELPHI_PACKAGE TBaudRate __fastcall StrToBaudRate(System::UnicodeString Str);
extern DELPHI_PACKAGE TStopBits __fastcall StrToStopBits(System::UnicodeString Str);
extern DELPHI_PACKAGE TDataBits __fastcall StrToDataBits(System::UnicodeString Str);
extern DELPHI_PACKAGE TParityBits __fastcall StrToParity(System::UnicodeString Str);
extern DELPHI_PACKAGE TFlowControl __fastcall StrToFlowControl(System::UnicodeString Str);
extern DELPHI_PACKAGE System::UnicodeString __fastcall BaudRateToStr(TBaudRate BaudRate);
extern DELPHI_PACKAGE System::UnicodeString __fastcall StopBitsToStr(TStopBits StopBits);
extern DELPHI_PACKAGE System::UnicodeString __fastcall DataBitsToStr(TDataBits DataBits);
extern DELPHI_PACKAGE System::UnicodeString __fastcall ParityToStr(TParityBits Parity);
extern DELPHI_PACKAGE System::UnicodeString __fastcall FlowControlToStr(TFlowControl FlowControl);
}	/* namespace Cport */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_CPORT)
using namespace Cport;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// CportHPP
