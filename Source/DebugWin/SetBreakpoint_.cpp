//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SetBreakpoint_.h"
#include "symbolstore.h"
#include "z80.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
__fastcall TSetBreakpoint::TSetBreakpoint(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::CancelClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::OKClick(TObject *Sender)
{
        cancelled = false;
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key == VK_ESCAPE)
        {
                cancelled = true;
                Close();
        }        
}
//---------------------------------------------------------------------------

void TSetBreakpoint::CentreOn(TForm* parent)
{
        Top = parent->Top + (parent->Height - Height) /2;
        Left = parent->Left + (parent->Width - Width) /2;
}
//---------------------------------------------------------------------------
void TSetBreakpoint::SetTitle(AnsiString& title)
{
        Text = title;
}
//---------------------------------------------------------------------------

bool SanitiseAddress(String addressValue, int& addr, int maxValue)
{
        if (addressValue.Length()  == 0)
        {
                return false;
        }

        if (symbolstore::symbolToAddress(addressValue, addr))
        {
                addressValue = "0x" + IntToHex(addr, 4);
        }

        bool hex = (addressValue[1] == '$');

        // replace the zx-world hex identifier with the standard c library one.
        //
        if (hex)
        {
                addressValue = "0x" + addressValue.SubString(2, addressValue.Length() - 1);
        }

        // z80 qualifiers
        //
        if (CompareText(addressValue, "PC") == 0)
        {
                addressValue = z80.pc.w;
        }
        else if (CompareText(addressValue, "HL") == 0)
        {
                addressValue = z80.hl.w;
        }
        else if (CompareText(addressValue, "DE") == 0)
        {
                addressValue = z80.de.w;
        }
        else if (CompareText(addressValue, "BC") == 0)
        {
                addressValue = z80.bc.w;
        }
        else if (CompareText(addressValue, "IX") == 0)
        {
                addressValue = z80.ix.w;
        }
        else if (CompareText(addressValue, "IY") == 0)
        {
                addressValue = z80.iy.w;
        }

        char* endPtr;
        if (hex)
        {
                addr = int(strtol(addressValue.c_str(), &endPtr, 16));
        }
        else
        {
                addr = int(strtol(addressValue.c_str(), &endPtr, 10));
        }

        return (*endPtr == 0) && (addr <= maxValue);
}


bool TSetBreakpoint::EditBreakpoint(struct breakpoint& bp)
{
        BreakCondition->Enabled = true;

        BreakCondition->ItemIndex = Equal;

        UpdateRBStates();

        EditAddress->Text = "$" + IntToHex(bp.Addr, 4);
        EditAddressArgument->MaxLength = 5;

        if (bp.Condition == LessThan) BreakCondition->Text = "<";
        else if (bp.Condition == Equal) BreakCondition->Text = "=";
        else if (bp.Condition == GreaterThan) BreakCondition->Text = ">";
        else if (bp.Condition == Range) BreakCondition->Text = "-->";
        else if (bp.Condition == NotEqual) BreakCondition->Text = "<>";

        EditAddressArgument->Text = "$" + IntToHex(bp.Argument, 4);

        ActiveControl = EditAddress;
        EditAddress->SelectAll();

        // OK button resets this. Every other way of closing the dialog is a cancel
        cancelled = true;
        ShowModal();

        AnsiString addressValue = EditAddress->Text.Trim();
        AnsiString addressValueHi = EditAddressArgument->Text.Trim();

        if (cancelled ||
                addressValue.Length() == 0 ||
                (bp.Condition == Range && addressValue.Length() == 0))
        {
                return false;
        }

        if (!SanitiseAddress(addressValue, bp.Addr, 65535))
                return false;
        if (!SanitiseAddress(addressValueHi, bp.Argument, 65535))
                return false;

         if (bp.Condition == Range && bp.Addr > bp.EndAddr)
        {
                int temp = bp.Addr;
                bp.Addr = bp.EndAddr;
                bp.EndAddr = temp;
        }

        if (BreakCondition->Text == "<") bp.Condition = LessThan;
        else if (BreakCondition->Text == "=") bp.Condition = Equal;
        else if (BreakCondition->Text == ">") bp.Condition = GreaterThan;
        else if (BreakCondition->Text == "-->") bp.Condition = Range;
        else if (BreakCondition->Text == "<>") bp.Condition = NotEqual;

        return true;
}

//---------------------------------------------------------------------------#
bool TSetBreakpoint::EditTSetBreakpoint(int& address, int len, int& tStates)
{
        cancelled=false;

	BreakCondition->ItemIndex = Equal;
        BreakCondition->Enabled = false;

        BreakAddress->Caption = "Address:";
        BreakAddressArgument->Caption = "T-States:";

        EditAddress->Text = "$" + IntToHex(address, len*2);
        EditAddressArgument->MaxLength = 6;
        EditAddressArgument->Text = tStates;

        ActiveControl = EditAddress;
        EditAddress->SelectAll();

        ShowModal();

        AnsiString addressValue = EditAddress->Text.Trim();
        AnsiString tStatesValue = EditAddressArgument->Text.Trim();

        if (cancelled || (addressValue.Length() == 0) || (tStatesValue.Length() == 0))
        {
                return false;
        }

        if (!SanitiseAddress(addressValue, address, 65535))
                return false;
        if (!SanitiseAddress(tStatesValue, tStates, 999999))
                return false;


/*        int addr;
        if (symbolstore::symbolToAddress(addressValue, addr))
        {
                addressValue = "0x" + IntToHex(addr, 4);
        }

        if (addressValue.Length()  == 0)
        {
                return false;
        }

        bool hex = (addressValue[1] == '$');

        // replace the zx-world hex identifier with the standard c library one.
        //
        if (hex)
        {
                addressValue = "0x" + addressValue.SubString(2, addressValue.Length() - 1);
        }

        if (addressValue == "PC")
        {
                addressValue = z80.pc.w;
        }

        char* endPtr;
        int tvalAddress;
        if (hex)
        {
                tvalAddress = int(strtol(addressValue.c_str(), &endPtr, 16));
        }
        else
        {
                tvalAddress = int(strtol(addressValue.c_str(), &endPtr, 10));
        }

        if ((*endPtr != 0) || (tvalAddress > 999999))
        {
                // and we know when the user entered junk...
                return false;
        }

        int tvalTStates = int(strtol(tStatesValue.c_str(), &endPtr, 0));
        if (*endPtr != 0)
        {
                // and we know when the user entered junk...
                return false;
        }

        if (tvalTStates > 999999)
        {
                return false;
        }

        address = tvalAddress;
        tStates = tvalTStates;
            */

        return true;
}

void __fastcall TSetBreakpoint::UpdateRBStates()
{
        if (BreakCondition->Text == "-->")
        {
                BreakAddress->Caption = "From:";
                BreakAddressArgument->Caption = "To:";
        }
        else
        {
                BreakAddress->Caption = "Address:";
                BreakAddressArgument->Caption = "Mask:";
        }
}
//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::BreakConditionChange(TObject *Sender)
{
        UpdateRBStates();
}
//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::EditAddressArgumentChange(TObject *Sender)
{
        bool valid;

        if (BreakAddressArgument->Caption == "T-States:")
        {
                int value;
                valid = SanitiseAddress(EditAddressArgument->Text, value, 999999);
        }
        else
        {
                int value;
                valid = SanitiseAddress(EditAddressArgument->Text, value, 65535);
        }

        EditAddressArgument->Font->Color = valid ? clWindowText : clRed;

        OK->Enabled = (EditAddress->Font->Color == clWindowText) && (EditAddressArgument->Font->Color == clWindowText);
}
//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::EditAddressChange(TObject *Sender)
{
        int value;
        bool valid = SanitiseAddress(EditAddress->Text, value, 65535);
        EditAddress->Font->Color = valid ? clWindowText : clRed;

        OK->Enabled = (EditAddress->Font->Color == clWindowText) && (EditAddressArgument->Font->Color == clWindowText);
}
//---------------------------------------------------------------------------

