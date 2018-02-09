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

bool SanitiseAddress(String addressValue, int& addr)
{
        if (symbolstore::symbolToAddress(addressValue, addr))
        {
                addressValue = "0x" + IntToHex(addr, 4);
        }

        // replace the zx-world hex identifier with the standard c library one.
        //
        if (addressValue[1] == '$')
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

        // which allows us to use a ninja converter.
        // suddenly 0xABCD, $ABCD, 01234 [octal] and decimal are all valid.
        //
        char* endPtr;
        addr = int(strtol(addressValue.c_str(), &endPtr, 0));

        return *endPtr == 0;
}


bool TSetBreakpoint::EditBreakpoint(struct breakpoint& bp)
{
        UpdateRBStates();

        EditAddress->Text = "$" + IntToHex(bp.Addr, 4);
        EditAddressHi->Text = "$0000";

        if (bp.Condition == LessThan) RadioButtonLessThan->Checked = true;
        else if (bp.Condition == Equal) RadioButtonEqual->Checked = true;
        else if (bp.Condition == GreaterThan) RadioButtonGreaterThan->Checked = true;
        else if (bp.Condition == InRange)
        {
                EditAddressHi->Text = "$" + IntToHex(bp.AddrHi, 4);
                RadioButtonInRange->Checked = true;
        }

        ActiveControl = EditAddress;
        EditAddress->SelectAll();

        // OK button resets this. Every other way of closing the dialog is a cancel
        cancelled = true;
        ShowModal();

        AnsiString addressValue = EditAddress->Text.Trim();
        AnsiString addressValueHi = EditAddressHi->Text.Trim();

        if (cancelled ||
                addressValue.Length() == 0 ||
                (bp.Condition == InRange && addressValue.Length() == 0))
        {
                return false;
        }

        if (!SanitiseAddress(addressValue, bp.Addr))
                return false;
        if (!SanitiseAddress(addressValueHi,bp.AddrHi))
                return false;

        if (bp.Condition == InRange && bp.Addr > bp.AddrHi)
        {
                int temp = bp.Addr;
                bp.Addr = bp.AddrHi;
                bp.AddrHi = temp;
        }

        if (RadioButtonLessThan->Checked) bp.Condition = LessThan;
        else if (RadioButtonEqual->Checked) bp.Condition = Equal;
        else if (RadioButtonGreaterThan->Checked) bp.Condition = GreaterThan;
        else if (RadioButtonInRange->Checked) bp.Condition = InRange;

        return true;
}

void __fastcall TSetBreakpoint::UpdateRBStates()
{
        EditAddressHi->Enabled = RadioButtonInRange->Checked;
        EditAddressHi->Visible = RadioButtonInRange->Checked;
}

void __fastcall TSetBreakpoint::RadioButtonClick(TObject *Sender)
{
        UpdateRBStates();
}
//---------------------------------------------------------------------------

