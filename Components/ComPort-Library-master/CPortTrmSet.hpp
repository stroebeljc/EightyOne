// CodeGear C++Builder
// Copyright (c) 1995, 2022 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'CPortTrmSet.pas' rev: 35.00 (Windows)

#ifndef CporttrmsetHPP
#define CporttrmsetHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <Winapi.Windows.hpp>
#include <Winapi.Messages.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.StdCtrls.hpp>
#include <CPortCtl.hpp>

//-- user supplied -----------------------------------------------------------

namespace Cporttrmset
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TComTrmSetForm;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TComTrmSetForm : public Vcl::Forms::TForm
{
	typedef Vcl::Forms::TForm inherited;
	
__published:
	Vcl::Stdctrls::TButton* Button1;
	Vcl::Stdctrls::TButton* Button2;
	Vcl::Stdctrls::TCheckBox* CheckBox1;
	Vcl::Stdctrls::TCheckBox* CheckBox2;
	Vcl::Stdctrls::TCheckBox* CheckBox3;
	Vcl::Stdctrls::TCheckBox* CheckBox4;
	Vcl::Stdctrls::TCheckBox* CheckBox5;
	Vcl::Stdctrls::TComboBox* ComboBox1;
	Vcl::Stdctrls::TComboBox* ComboBox2;
	Vcl::Stdctrls::TComboBox* ComboBox3;
	Vcl::Stdctrls::TEdit* Edit1;
	Vcl::Stdctrls::TEdit* Edit2;
	Vcl::Stdctrls::TGroupBox* GroupBox1;
	Vcl::Stdctrls::TGroupBox* GroupBox2;
	Vcl::Stdctrls::TLabel* Label1;
	Vcl::Stdctrls::TLabel* Label2;
	Vcl::Stdctrls::TLabel* Label3;
	Vcl::Stdctrls::TLabel* Label4;
	Vcl::Stdctrls::TLabel* Label5;
public:
	/* TCustomForm.Create */ inline __fastcall virtual TComTrmSetForm(System::Classes::TComponent* AOwner) : Vcl::Forms::TForm(AOwner) { }
	/* TCustomForm.CreateNew */ inline __fastcall virtual TComTrmSetForm(System::Classes::TComponent* AOwner, int Dummy) : Vcl::Forms::TForm(AOwner, Dummy) { }
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TComTrmSetForm() { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TComTrmSetForm(HWND ParentWindow) : Vcl::Forms::TForm(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE void __fastcall EditComTerminal(Cportctl::TCustomComTerminal* ComTerminal);
}	/* namespace Cporttrmset */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_CPORTTRMSET)
using namespace Cporttrmset;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// CporttrmsetHPP
