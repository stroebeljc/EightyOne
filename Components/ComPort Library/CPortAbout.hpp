// CodeGear C++Builder
// Copyright (c) 1995, 2022 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'CPortAbout.pas' rev: 35.00 (Windows)

#ifndef CportaboutHPP
#define CportaboutHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <Winapi.Windows.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>

//-- user supplied -----------------------------------------------------------

namespace Cportabout
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TAboutBox;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TAboutBox : public Vcl::Forms::TForm
{
	typedef Vcl::Forms::TForm inherited;
	
__published:
	Vcl::Extctrls::TPanel* Panel1;
	Vcl::Extctrls::TImage* ProgramIcon;
	Vcl::Stdctrls::TLabel* ProductName;
	Vcl::Stdctrls::TLabel* Version;
	Vcl::Stdctrls::TLabel* Copyright;
	Vcl::Stdctrls::TButton* OKButton;
	void __fastcall FormCreate(System::TObject* Sender);
public:
	/* TCustomForm.Create */ inline __fastcall virtual TAboutBox(System::Classes::TComponent* AOwner) : Vcl::Forms::TForm(AOwner) { }
	/* TCustomForm.CreateNew */ inline __fastcall virtual TAboutBox(System::Classes::TComponent* AOwner, int Dummy) : Vcl::Forms::TForm(AOwner, Dummy) { }
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TAboutBox() { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAboutBox(HWND ParentWindow) : Vcl::Forms::TForm(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
#define CPortLibraryVersion L"2.64"
extern DELPHI_PACKAGE void __fastcall ShowAbout(void);
}	/* namespace Cportabout */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_CPORTABOUT)
using namespace Cportabout;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// CportaboutHPP
