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

#ifndef SetBreakpoint_H
#define SetBreakpoint_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Debug.h"

//---------------------------------------------------------------------------
class TSetBreakpoint : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TButton *Cancel;
        TLabel *LabelBreakAddress;
        TEdit *BreakAddress;
        TLabel *LabelConditionValue;
        TEdit *BreakValue;
        TLabel *LabelBreakValue;
        TComboBox *BreakConditionAddr;
        TLabel *LabelType;
        TComboBox *BreakType;
        TComboBox *RegisterList;
        TComboBox *FlagList;
        TComboBox *BreakConditionValue;
        TLabel *LabelConditionAddr;
        TComboBox *ComboBoxBreakEnabled;
        TLabel *LabelBreakEnabled;
        TComboBox *BreakHitCount;
        TLabel *LabelBreakHitcount;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall BreakConditionAddrChange(TObject *Sender);
        void __fastcall BreakValueChange(TObject *Sender);
        void __fastcall BreakAddressChange(TObject *Sender);
        void __fastcall BreakTypeChange(TObject *Sender);
        void __fastcall RegisterListChange(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);

private:	// User declarations
        bool cancelled;
        bool Register16Bit(RegisterType registerIndex);
        void EnableOkButton();
        void ValidateBreakAddress();
        void ValidateBreakValue();
        void GetBreakAddressLimits(BreakpointType type, int& lowerLimit, int& upperLimit);
        void GetBreakValueLimits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex, int& lowerLimit, int& upperLimit);
        int GetBreakAddressMaxDigits(BreakpointType type);
        int GetBreakValueMaxDigits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex);
        void ConfigureBreakpointFields(struct breakpoint& bp);
        bool GetBreakpointFields(struct breakpoint& bp);
        void BreakTypeChangeExe();
        void BreakTypeChangeRdWrInOut();
        void BreakTypeChangeInOutByte();
        void BreakTypeChangeRegister();
        void BreakTypeChangeFlag();
        void BreakTypeChangeMem();
        void BreakTypeChangeTStates();
        void SetConditionList(TComboBox* const conditionList, AnsiString conditions);
        void SetEditBox(TEdit* const editBox, AnsiString defaultText);
        void SetEditBoxLabels(AnsiString breakAddressLabel, AnsiString breakValueLabel);

public:		// User declarations
        __fastcall TSetBreakpoint(TComponent* Owner);
        void SetTitle(AnsiString& title);
        bool EditBreakpoint(struct breakpoint& bp);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
#endif
