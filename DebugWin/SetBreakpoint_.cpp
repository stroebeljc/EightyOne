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
        cancelled=true;
        Close();               
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::OKClick(TObject *Sender)
{
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

bool TSetBreakpoint::EditValue(int& address, TDbg::BreakpointConditionType& condition)
{
        cancelled=false;

        EditAddress->Text = "$" + IntToHex(address, 4);
        if (condition == TDbg::LessThan) RadioButtonLessThan->Checked = true;
        else if (condition == TDbg::Equal) RadioButtonEqual->Checked = true;
        else if (condition == TDbg::GreaterThan) RadioButtonGreaterThan->Checked = true;

        ActiveControl = EditAddress;
        EditAddress->SelectAll();

        ShowModal();

        AnsiString addressValue = EditAddress->Text.Trim();

        if (cancelled || (addressValue.Length() == 0))
        {
                return false;
        }

        int addr;
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
        int tvalAddress = int(strtol(addressValue.c_str(), &endPtr, 0));
        if (*endPtr != 0)
        {
                // and we know when the user entered junk...
                return false;
        }

        address = tvalAddress;

        if (RadioButtonLessThan->Checked) condition = TDbg::LessThan;
        else if (RadioButtonEqual->Checked) condition = TDbg::Equal;
        else if (RadioButtonGreaterThan->Checked) condition = TDbg::GreaterThan;

        return true;
}
