//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ConfigureBreakpoint_.h"
#include "symbolstore.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfigureBreakpoint *ConfigureBreakpoint;
//---------------------------------------------------------------------------
__fastcall TConfigureBreakpoint::TConfigureBreakpoint(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TConfigureBreakpoint::FormKeyPress(TObject *Sender,
      char &Key)
{
        if (Key == VK_ESCAPE)
        {
                cancelled = true;
                Close();
        }
}
//---------------------------------------------------------------------------
void __fastcall TConfigureBreakpoint::OKClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
void __fastcall TConfigureBreakpoint::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();        
}
//---------------------------------------------------------------------------

void TConfigureBreakpoint::CentreOn(TForm* parent)
{
        Top = parent->Top + (parent->Height - Height) /2;
        Left = parent->Left + (parent->Width - Width) /2;
}

bool TConfigureBreakpoint::EditValues(int& address, int len, int& tStates)
{
        cancelled=false;

        EditAddress->Text = "$" + IntToHex(address, len*2);
        EditTStates->Text = tStates;

        ActiveControl = EditAddress;
        EditAddress->SelectAll();

        ShowModal();

        AnsiString addressValue = EditAddress->Text.Trim();
        AnsiString tStatesValue = EditTStates->Text.Trim();

        if (cancelled || (addressValue.Length() == 0) || (tStatesValue.Length() == 0))
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

        int tvalTStates = int(strtol(tStatesValue.c_str(), &endPtr, 0));
        if (*endPtr != 0)
        {
                // and we know when the user entered junk...
                return false;
        }

        if (tvalTStates > 65535)
        {
                return false;
        }

        address = tvalAddress;
        tStates = tvalTStates;

        return true;
<<<<<<< HEAD
}
=======
}

>>>>>>> update-1.2-to-1.5
