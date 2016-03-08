//---------------------------------------------------------------------------

#ifndef Hardware_H
#define Hardware_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------

#define RAM1k   0
#define RAM2k   1
#define RAM4k   2
#define RAM8k   3
#define RAM16k  4
#define RAM32k  5
#define RAM48k  6

class THardware : public TForm
{
__published:	// IDE-managed Components
        TComboBox *RamPack;
        TLabel *Label1;
        TCheckBox *ShadowROM;
        TCheckBox *EnableRAM8k;
        TCheckBox *ShadowRAM;
        TCheckBox *ProtectROM;
        TCheckBox *TrueHRG;
        TCheckBox *CheckBox6;
        TCheckBox *CheckBox7;
        TCheckBox *CheckBox8;
        TButton *Button1;
        TLabel *Label2;
        TComboBox *Model;
        TEdit *ROMFileName;
        TButton *ROMBrowse;
        TLabel *Label3;
        TOpenDialog *LoadROMDialog;
        TCheckBox *PatchLoad;
        TCheckBox *NTSCMachine;
        TCheckBox *PatchSave;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall RamPackChange(TObject *Sender);
        void __fastcall EnableRAM8kClick(TObject *Sender);
        void __fastcall TrueHRGClick(TObject *Sender);
        void __fastcall ModelChange(TObject *Sender);
        void __fastcall ROMBrowseClick(TObject *Sender);
        void __fastcall ShadowROMClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall THardware(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THardware *Hardware;
//---------------------------------------------------------------------------
#endif
