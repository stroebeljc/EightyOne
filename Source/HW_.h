//---------------------------------------------------------------------------

#ifndef HW_H
#define HW_H
#include "OffBtn.hpp"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <vector>

using namespace std;

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OffBtn.hpp"
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <Graphics.hpp>
#include "Joystick.h"
#include "Utils.h"

//---------------------------------------------------------------------------

struct RomCartridgeEntry
{
public:
        RomCartridgeEntry(AnsiString title, AnsiString path)
        {
                Title = title;
                Path = path;
        }

        AnsiString Title;
        AnsiString Path;
};

struct HWFormValues
{
        AnsiString MachineName;
        AnsiString RomBoxText;
        AnsiString RamPackBoxText;
        AnsiString SoundCardBoxText;
        AnsiString ChrGenBoxText;
        AnsiString HiResBoxText;
        AnsiString ColourBoxText;
        AnsiString SpeechBoxText;
        AnsiString JoystickBoxText;
        AnsiString RomCartridgeBoxText;
        AnsiString ZXC1ConfigurationBoxText;
        AnsiString RomCartridgeFileBoxText;
        AnsiString SinclairRomCartridgeFileBoxText;
        AnsiString TS1510RomCartridgeFileBoxText;
        AnsiString TC2068RomCartridgeFileBoxText;
        AnsiString TS2068RomCartridgeFileBoxText;
        AnsiString ProgrammableJoystickLeft;
        AnsiString ProgrammableJoystickRight;
        AnsiString ProgrammableJoystickUp;
        AnsiString ProgrammableJoystickDown;
        AnsiString ProgrammableJoystickFire;
        AnsiString FDCBoxText;
        AnsiString IDEBoxText;
        AnsiString FDCRomBoxText;
        AnsiString IDERomBoxText;
        AnsiString DriveATypeText;
        AnsiString DriveBTypeText;
        AnsiString ZXCFRAMText;
        AnsiString NoMicrodrivesComboBoxText;
        bool ZXpandChecked;
        bool SpecDrumChecked;
        bool ProtectROMChecked;
        bool NTSCChecked;
        bool EnableLowRAMChecked;
        bool M1NotChecked;
        bool ImprovedWaitChecked;
        bool TS2050Checked;
        bool Issue2Checked;
        bool KMouseChecked;
        bool MultifaceChecked;
        bool ZXPrinterChecked;
        bool Z80AssemblerChecked;
        bool MemocalcChecked;
        bool FloatingPointHardwareFixChecked;
        bool UploadChecked;
        bool uSourceChecked;
        ZX97 ZX97Form;
};

class THW : public TForm
{
__published:	// IDE-managed Components
        TPageControl *Machine;
        TTabSheet *Sinclair;
        TTabSheet *Amstrad;
        TTabSheet *Timex;
        TOffice97Button *ZX80Btn;
        TOffice97Button *ZX81Btn;
        TOffice97Button *Spec48Btn;
        TOffice97Button *SpecPlusBtn;
        TOffice97Button *Spec128Btn;
        TTabSheet *Others;
        TOffice97Button *LambdaBtn;
        TOffice97Button *R470Btn;
        TOffice97Button *TK85Btn;
        TOffice97Button *AceBtn;
        TOffice97Button *TS1000Btn;
        TOffice97Button *TS1500Btn;
        TOffice97Button *SpecP2Btn;
        TOffice97Button *SpecP2aBtn;
        TButton *OK;
        TLabel *RamPackLbl;
        TComboBox *RamPackBox;
        TOffice97Button *SpecP3Btn;
        TOffice97Button *Spec16Btn;
        TOpenDialog *RomSelect;
        TOffice97Button *TC2048Btn;
        TOffice97Button *TS2068Btn;
        TTabSheet *HomeBrew;
        TOffice97Button *ZX97LEBtn;
        TPageControl *Advanced;
        TTabSheet *Interfaces;
        TTabSheet *FloppyDrives;
        TLabel *SoundCardLbl;
        TComboBox *SoundCardBox;
        TLabel *ChrGenLbl;
        TComboBox *ChrGenBox;
        TLabel *HiResLbl;
        TComboBox *HiResBox;
        TCheckBox *KMouse;
        TCheckBox *TS2050;
        TCheckBox *Multiface;
        TButton *TS2050Config;
        TTabSheet *AdvSettings;
        TLabel *LabelRomFile;
        TButton *BrowseROM;
        TCheckBox *ProtectROM;
        TCheckBox *NTSC;
        TCheckBox *EnableLowRAM;
        TCheckBox *M1Not;
        TCheckBox *Issue2;
        TCheckBox *ZXPrinter;
        TCheckBox *ZXpand;
        TComboBox *ColourBox;
        TLabel *ColourLabel;
        TLabel *RomCartridgeLabel;
        TComboBox *RomCartridgeBox;
        TEdit *RomCartridgeFileBox;
        TButton *BrowseRomCartridge;
        TCheckBox *FloatingPointHardwareFix;
        TButton *ButtonZXpandSDCard;
        TComboBox *ZXC1ConfigurationBox;
        TButton *ButtonAdvancedMore;
        TCheckBox *ImprovedWait;
        TComboBox *RomBox;
        TImageList *ZX80Icons;
        TComboBox *SinclairRomCartridgeFileBox;
        TLabel *LabelTotalRAM;
        TCheckBox *uSource;
        TImageList *Spec128Icons;
        TComboBox *TS1510RomCartridgeFileBox;
        TComboBox *TS2068RomCartridgeFileBox;
        TOffice97Button *TC2068Btn;
        TComboBox *TC2068RomCartridgeFileBox;
        TImage *ZXpandEmulationInfo;
        TLabel *SpeechBoxLbl;
        TComboBox *SpeechBox;
        TCheckBox *SpecDrum;
        TComboBox *JoystickBox;
        TLabel *JoystickBoxLabel;
        TLabel *JoystickLeftBoxLabel;
        TEdit *JoystickLeftBox;
        TLabel *JoystickRightBoxLabel;
        TEdit *JoystickRightBox;
        TLabel *JoystickUpBoxLabel;
        TEdit *JoystickUpBox;
        TLabel *JoystickDownBoxLabel;
        TEdit *JoystickDownBox;
        TLabel *JoystickFireBoxLabel;
        TEdit *JoystickFireBox;
        TButton *DefaultsButton;
        TButton *Apply;
        TButton *RestoreButton;
        TGroupBox *GroupBox1;
        TLabel *LabelFDC;
        TComboBox *FDCBox;
        TLabel *DriveATypeLabel;
        TComboBox *DriveAType;
        TLabel *DriveBTypeLabel;
        TComboBox *DriveBType;
        TComboBox *FDCRomBox;
        TLabel *FDCRomBoxLabel;
        TGroupBox *GroupBox2;
        TLabel *LabelIDE;
        TComboBox *IDEBox;
        TLabel *ZXCFLabel;
        TComboBox *ZXCFRAM;
        TComboBox *IDERomBox;
        TLabel *IDERomBoxLabel;
        TButton *Interface1Configure;
        TButton *FDCRomBoxBrowse;
        TButton *IDERomBoxBrowse;
        TLabel *NoMicrodrivesLabel;
        TComboBox *NoMicrodrivesComboBox;
        TLabel *Plus3eNoticeLabel;
        TCheckBox *Z80Assembler;
        TCheckBox *Memocalc;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ZX80BtnClick(TObject *Sender);
        void __fastcall ZX81BtnClick(TObject *Sender);
        void __fastcall Spec48BtnClick(TObject *Sender);
        void __fastcall SpecPlusBtnClick(TObject *Sender);
        void __fastcall Spec128BtnClick(TObject *Sender);
        void __fastcall Spec16BtnClick(TObject *Sender);
        void __fastcall SpecP2BtnClick(TObject *Sender);
        void __fastcall SpecP2aBtnClick(TObject *Sender);
        void __fastcall SpecP3BtnClick(TObject *Sender);
        void __fastcall TS1000BtnClick(TObject *Sender);
        void __fastcall TS1500BtnClick(TObject *Sender);
        void __fastcall LambdaBtnClick(TObject *Sender);
        void __fastcall R470BtnClick(TObject *Sender);
        void __fastcall TK85BtnClick(TObject *Sender);
        void __fastcall AceBtnClick(TObject *Sender);
        void __fastcall TS2050Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TS2050ConfigClick(TObject *Sender);
        void __fastcall BrowseROMClick(TObject *Sender);
        void __fastcall TC2048BtnClick(TObject *Sender);
        void __fastcall TS2068BtnClick(TObject *Sender);
        void __fastcall ZX97LEBtnClick(TObject *Sender);
        void __fastcall RamPackBoxChange(TObject *Sender);
        void __fastcall NTSCClick(TObject *Sender);
        void __fastcall EnableLowRAMClick(TObject *Sender);
        void __fastcall M1NotClick(TObject *Sender);
        void __fastcall SoundCardBoxChange(TObject *Sender);
        void __fastcall ChrGenBoxChange(TObject *Sender);
        void __fastcall HiResBoxChange(TObject *Sender);
        void __fastcall DriveATypeChange(TObject *Sender);
        void __fastcall DriveBTypeChange(TObject *Sender);
        void __fastcall Interface1ConfigureClick(TObject *Sender);
        void __fastcall MultifaceClick(TObject *Sender);
        void __fastcall IDEBoxChange(TObject *Sender);
        void __fastcall FDCBoxChange(TObject *Sender);
        void __fastcall ZXpandClick(TObject *Sender);
        void __fastcall BrowseRomCartridgeClick(TObject *Sender);
        void __fastcall RomCartridgeBoxChange(TObject *Sender);
        void __fastcall ColourBoxChange(TObject *Sender);
        void __fastcall RomBoxChange(TObject *Sender);
        void __fastcall ZXPrinterClick(TObject *Sender);
        void __fastcall FloatingPointHardwareFixClick(TObject *Sender);
        void __fastcall ButtonZXpandSDCardClick(TObject *Sender);
        void __fastcall ButtonAdvancedMoreClick(TObject *Sender);
        void __fastcall ImprovedWaitClick(TObject *Sender);
        void __fastcall SinclairRomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall TS1510RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall TS2068RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall uSourceClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall TC2068BtnClick(TObject *Sender);
        void __fastcall TC2068RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall ZXpandEmulationInfoClick(TObject *Sender);
        void __fastcall SpeechBoxChange(TObject *Sender);
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall JoystickBoxChange(TObject *Sender);
        void __fastcall JoystickBoxMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall DefaultsButtonClick(TObject *Sender);
        void __fastcall JoystickBoxExit(TObject *Sender);
        void __fastcall JoystickBoxKeyPress(TObject *Sender,
          char &Key);
        void __fastcall JoystickBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall ApplyClick(TObject *Sender);
        void __fastcall KMouseClick(TObject *Sender);
        void __fastcall SpecDrumClick(TObject *Sender);
        void __fastcall JoystickLeftBoxChange(TObject *Sender);
        void __fastcall JoystickRightBoxChange(TObject *Sender);
        void __fastcall JoystickUpBoxChange(TObject *Sender);
        void __fastcall JoystickDownBoxChange(TObject *Sender);
        void __fastcall JoystickFireBoxChange(TObject *Sender);
        void __fastcall ProtectROMClick(TObject *Sender);
        void __fastcall Issue2Click(TObject *Sender);
        void __fastcall ZXCFRAMChange(TObject *Sender);
        void __fastcall RestoreButtonClick(TObject *Sender);
        void __fastcall FDCRomBoxBrowseClick(TObject *Sender);
        void __fastcall IDERomBoxBrowseClick(TObject *Sender);
        void __fastcall FDCRomBoxChange(TObject *Sender);
        void __fastcall IDERomBoxChange(TObject *Sender);
        void __fastcall NoMicrodrivesComboBoxChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall Z80AssemblerClick(TObject *Sender);
        void __fastcall MemocalcClick(TObject *Sender);
private:	// User declarations
        int RamPackHeight;
        int NewMachine, NewSpec;
        int defaultRamPackIndex, baseRamSize;
        bool ramPackSupplementsInternalRam;
        AnsiString NewMachineName;
        HWFormValues Hwform;

        void LoadFromInternalSettings();
        void SaveToInternalSettings();
        void RefreshDefaultRamSettings();
        void SetupForZX81(void);
        void SetupForSpectrum(void);
        void EnableRomCartridgeOption(bool enable);
        void ResetDisplaySize();
        void SetZX80Icon();
        void SetSpectrum128Icon();
        void PopulateRomCartridgeSinclairList();
        void PopulateRomCartridgeTS1510List();
        void PopulateRomCartridgeTS2068List();
        void PopulateRomCartridgeTC2068List();
        void AddRomCartridgeFile(TComboBox* romCartridgeFileBox, RomCartridgeEntry* romCartridgeEntry, AnsiString fileExt);
        AnsiString DirectSoundError(unsigned int errorCode);
        void ConfigureDefaultRamSettings();
        void DisplayTotalRam();
        int UpdateRomCartridgeControls(int machine, int spectrumModel);
        int DetermineRomCartridgeType(AnsiString cartridgeText, int machine, int spectrumModel);
        bool ValidCharacter(TEdit* textBox, char newKey);
        void SetCharacter(TEdit* textBox, KeyInfo& keyInfo);
        int FindEntry(TComboBox* comboBox, AnsiString text);
        void WriteNVMemory(BYTE* memory, int size, int count, const char* fileName);
        void ReadNVMemory(BYTE* memory, int size, int count, const char* fileName);
        void AccessIniFile(TIniFile* ini, IniFileAccessType accessType);
        bool NewKey(TEdit* textBox, char key);
        void LoadRomBox();
        void UpdateApplyButton();
        void SelectDefaultRom();
        void LoadFdcRomBox();
        void LoadIdeRomBox();
        bool BrowseROMFile(AnsiString folder, TComboBox* romBox);

        void ConfigureRzxSupport();
        void ResetDebugger();
        void ConfigureDisplayArtifacts();
        void ConfigureM1Not();
        void ConfigureModem();
        void ConfigurePrinterCentronicsPort();
        void SelectGroupboxVisibility();
        void ConfigureRamTop();
        void InitialiseSound(bool machineChanged);
        void CloseLiveMemoryWindow(bool machineChanged);
        void DetermineRamSizeLabel(AnsiString newMachineName);
        void ConfigureMultifaceRom();
        void ConfigureRom();
        void ConfigureZXpand();
        void ConfigureBasicLister();
        void ConfigureColour();
        void ConfigureChroma(bool prevChromaColourSwitchOn);
        void ConfigureSpectra(bool prevSpectraColourSwitchOn);
        void ConfigureRomCartridge();
        void ConfigureCharacterGenerator();
        void ConfigureHiRes();
        void ConfigureSound();
        void ConfigureSpeech();
        void ConfigureIDE();
        void ConfigureFDC();
        void ConfigureMachineSettings();
        AnsiString DetermineRomBase();
        void ConfigureSymbolFile(AnsiString romBase);
        void ConfigureCharacterBitmapFile(AnsiString romBase);
        void SetUpRomCartridges();
        void Configure8K16KRam();
        void ConfigureJoystick();
        void ConfigureKeypad();
        void ConfigureIDERom();
        void ConfigureFDCRom();
        void ConfigureTools();

        vector<RomCartridgeEntry> ts1510RomCartridges;
        vector<RomCartridgeEntry> ts2068RomCartridges;
        vector<RomCartridgeEntry> tc2068RomCartridges;
        vector<RomCartridgeEntry> sinclairRomCartridges;

public:		// User declarations
        __fastcall THW(TComponent* Owner);
        bool ResetRequired;
        void UpdateHardwareSettings(bool disableReset);
        void SaveSettings(TIniFile* ini);
        void LoadSettings(TIniFile* ini);
        void SetZXpandState(bool checked, bool enabled);
        void UpdateSinclairJoystickKeys();
        int SelectEntry(TComboBox* comboBox, AnsiString text);
};
//---------------------------------------------------------------------------
extern PACKAGE THW *HW;
//---------------------------------------------------------------------------
#endif
