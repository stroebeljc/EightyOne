//---------------------------------------------------------------------------

#include <vcl.h>
#include <dir.h>
#pragma hdrstop

#include "Hardware_.h"
#include "main_.h"
#include "keyboard_.h"
#include "kbstatus.h"
#include "zx81config.h"
#include "zx81.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THardware *Hardware;
//---------------------------------------------------------------------------
__fastcall THardware::THardware(TComponent* Owner)
        : TForm(Owner)
{
        Model->ItemIndex=1;
        RamPack->ItemIndex=4;
}
//---------------------------------------------------------------------------

void __fastcall THardware::Button1Click(TObject *Sender)
{
        switch(Model->ItemIndex)
        {
        case 0:
                zx81.machine=MACHINEZX80;
                strcpy(zx81.ROM80, ROMFileName->Text.c_str());
                Form1->Accurate1Click(Sender);
                Keyboard->zx80kb->Visible=true;
                Keyboard->acekb->Visible=false;
                Keyboard->zx81kb->Visible=false;
                break;
        case 1:
                zx81.machine=MACHINEZX81;
                strcpy(zx81.ROM81, ROMFileName->Text.c_str());
                //Form1->Accurate1Click(Sender);
                Keyboard->zx80kb->Visible=false;
                Keyboard->acekb->Visible=false;
                Keyboard->zx81kb->Visible=true;
                break;
        case 2:
                zx81.machine=MACHINEACE;
                strcpy(zx81.ROMACE, ROMFileName->Text.c_str());
                Form1->Fast1Click(Sender);
                Keyboard->zx80kb->Visible=false;
                Keyboard->zx81kb->Visible=false;
                Keyboard->acekb->Visible=true;
                break;
        }

        zx81.shadowROM = ShadowROM->Checked;
        zx81.shadowRAM = ShadowRAM->Checked;
        zx81.RAM816k = EnableRAM8k->Checked;
        zx81.protectROM = ProtectROM->Checked;
        zx81.NTSC = NTSCMachine->Checked;

        Form1->Timer1->Enabled=false;
        if (NTSCMachine->Checked) Form1->Timer1->Interval=16;
        else Form1->Timer1->Interval=20;
        Form1->Timer1->Enabled=true;


        zx81.RAMTOP = (1 << (RamPack->ItemIndex + 10)) +16383;
        if (RamPack->ItemIndex == 6) zx81.RAMTOP = 65535;

        zx81.load_patch = PatchLoad->Checked;
        zx81.save_patch = PatchSave->Checked;
        zx81.memohrg = 0;
        zx81.truehires=TrueHRG->Checked;
        PCKbInit();
        zx81_init();
        Close();
}
//---------------------------------------------------------------------------
void __fastcall THardware::RamPackChange(TObject *Sender)
{
        //ShadowROM->Enabled=true;
        //EnableRAM8k->Enabled=true;
        //ShadowRAM->Enabled=true;

        if (RamPack->ItemIndex >= RAM32k)
        {
                //ShadowROM->Enabled=false;
                //ShadowROM->Checked=false;
                //EnableRAM8k->Enabled=false;
                //EnableRAM8k->Checked=true;
                ShadowRAM->Enabled=false;
                ShadowRAM->Checked=false;
        }
        else    ShadowRAM->Enabled=true;

}
//---------------------------------------------------------------------------
void __fastcall THardware::EnableRAM8kClick(TObject *Sender)
{
        if (EnableRAM8k->Checked)
        {
                //ShadowROM->Enabled=false;
                ShadowROM->Checked=false;
        }
        //else
        //        ShadowROM->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall THardware::TrueHRGClick(TObject *Sender)
{
/*        if (MemoHRG->Checked)
        {
                ShadowROM->Checked=false;
                ShadowROM->Enabled=false;
        }
        else
                ShadowROM->Enabled=true;
*/
}
//---------------------------------------------------------------------------

void __fastcall THardware::ModelChange(TObject *Sender)
{
        switch(Model->ItemIndex)
        {
        case 0:
                ROMFileName->Text = zx81.ROM80;
                break;
        case 1:
                ROMFileName->Text = zx81.ROM81;
                break;
        case 2:
                ROMFileName->Text = zx81.ROMACE;
                break;
        }
}
//---------------------------------------------------------------------------

void __fastcall THardware::ROMBrowseClick(TObject *Sender)
{
        char cwd[MAXPATH];
        AnsiString NewName;
        int p;

        getcwd(cwd, MAXPATH);

        LoadROMDialog->FileName = ROMFileName->Text;
        LoadROMDialog->InitialDir = cwd;
        if (!LoadROMDialog->Execute()) return;

        NewName=LoadROMDialog->FileName;

        p=NewName.Length();

        while(p>0)

        {
                if (NewName[p]=='\\')
                {
                        p++;
                        NewName=NewName.SubString(p, 1 + NewName.Length() - p);
                        ROMFileName->Text = NewName;
                        return;
                }
                p--;
        }

        ROMFileName->Text = NewName;
}

//---------------------------------------------------------------------------


void __fastcall THardware::ShadowROMClick(TObject *Sender)
{
        if (ShadowROM->Checked)
        {
                //ShadowROM->Enabled=false;
                EnableRAM8k->Checked=false;
        }

}
//---------------------------------------------------------------------------

