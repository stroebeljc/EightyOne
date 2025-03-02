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

#ifndef MakeHDFH
#define MakeHDFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TCreateHDF : public TForm
{
__published:	// IDE-managed Components
        TEdit *FName;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *CylText;
        TEdit *HeadText;
        TEdit *SecText;
        TUpDown *CylUpDown;
        TUpDown *HeadUpDown;
        TUpDown *SecUpDown;
        TCheckBox *EightBit;
        TEdit *EffSize;
        TEdit *HDFSize;
        TLabel *Label5;
        TLabel *Label6;
        TButton *OK;
        TProgressBar *Progress;
        void __fastcall CylUpDownClick(TObject *Sender, TUDBtnType Button);
        void __fastcall HeadUpDownClick(TObject *Sender,
          TUDBtnType Button);
        void __fastcall SecUpDownClick(TObject *Sender, TUDBtnType Button);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall EightBitClick(TObject *Sender);
        void __fastcall CylTextChange(TObject *Sender);
        void __fastcall HeadTextChange(TObject *Sender);
        void __fastcall SecTextChange(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
        void ValueChanged(void);
        AnsiString ConvertValue(long Val);
        void EnableAll(void);
        void DisableAll(void);
        void WriteWord(unsigned char *base, int WordNo, int Value);
public:		// User declarations
        __fastcall TCreateHDF(TComponent* Owner);
        AnsiString FileName;
};
//---------------------------------------------------------------------------
extern PACKAGE TCreateHDF *CreateHDF;
//---------------------------------------------------------------------------
#endif
