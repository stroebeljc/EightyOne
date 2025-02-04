//---------------------------------------------------------------------------

#ifndef Plus3DrivesH
#define Plus3DrivesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <IniFiles.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TP3Drive : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TOpenDialog *OpenDialogFloppyDiskImage;
        TOpenDialog *OpenDialogHardDriveImage;
        TGroupBox *FloppyDriveGroup;
        TLabel *DriveALabel;
        TLabel *DriveBLabel;
        TEdit *DriveAText;
        TEdit *DriveBText;
        TButton *DriveAFSBtn;
        TButton *DriveBFSBtn;
        TButton *DriveAEjectBtn;
        TButton *DriveBEjectBtn;
        TGroupBox *HardDriveGroup;
        TLabel *HD1Label;
        TEdit *HD1Text;
        TButton *HD1FSBtn;
        TButton *HD1EjectBtn;
        TGroupBox *MicrodriveGroup;
        TLabel *MDV0Label;
        TEdit *MDV0Text;
        TButton *MDV0FSBtn;
        TButton *MDV0EjectBtn;
        TButton *MDV1EjectBtn;
        TButton *MDV1FSBtn;
        TEdit *MDV1Text;
        TLabel *MDV1Label;
        TLabel *MDV3Label;
        TLabel *MDV2Label;
        TEdit *MDV2Text;
        TEdit *MDV3Text;
        TButton *MDV3FSBtn;
        TButton *MDV2FSBtn;
        TButton *MDV2EjectBtn;
        TButton *MDV3EjectBtn;
        TLabel *MDV4Label;
        TLabel *MDV5Label;
        TLabel *MDV6Label;
        TLabel *MDV7Label;
        TEdit *MDV7Text;
        TEdit *MDV6Text;
        TEdit *MDV5Text;
        TEdit *MDV4Text;
        TButton *MDV4FSBtn;
        TButton *MDV5FSBtn;
        TButton *MDV6FSBtn;
        TButton *MDV7FSBtn;
        TButton *MDV7EjectBtn;
        TButton *MDV6EjectBtn;
        TButton *MDV5EjectBtn;
        TButton *MDV4EjectBtn;
        TButton *RedetectDrives;
        TComboBox *HD1List;
        TLabel *HD1CHS;
        TEdit *HD1C;
        TEdit *HD1H;
        TUpDown *HD1HUD;
        TEdit *HD1S;
        TUpDown *HD1SUD;
        TLabel *HD0Label;
        TComboBox *HD0List;
        TEdit *HD0Text;
        TButton *HD0FSBtn;
        TButton *HD0EjectBtn;
        TLabel *HD0CHS;
        TEdit *HD0C;
        TEdit *HD0H;
        TUpDown *HD0HUD;
        TEdit *HD0S;
        TUpDown *HD0SUD;
        TCheckBox *HD0ReadOnly;
        TCheckBox *HD1ReadOnly;
        TSaveDialog *SaveDialogNewFloppyDisk;
        TButton *DriveANewBtn;
        TButton *DriveBNewBtn;
        TButton *MDV0NewBtn;
        TButton *MDV1NewBtn;
        TButton *MDV2NewBtn;
        TButton *MDV3NewBtn;
        TButton *MDV4NewBtn;
        TButton *MDV5NewBtn;
        TButton *MDV6NewBtn;
        TButton *MDV7NewBtn;
        TButton *HD0NewBtn;
        TButton *HD1NewBtn;
        TSaveDialog *SaveDialogNewHardDriveImage;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall DriveAFSBtnClick(TObject *Sender);
        void __fastcall DriveAEjectBtnClick(TObject *Sender);
        void __fastcall DriveBFSBtnClick(TObject *Sender);
        void __fastcall DriveBEjectBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall HD0FSBtnClick(TObject *Sender);
        void __fastcall HD1FSBtnClick(TObject *Sender);
        void __fastcall HD0EjectBtnClick(TObject *Sender);
        void __fastcall HD1EjectBtnClick(TObject *Sender);
        void __fastcall MDV0EjectBtnClick(TObject *Sender);
        void __fastcall MDV0FSBtnClick(TObject *Sender);
        void __fastcall RedetectDrivesClick(TObject *Sender);
        void __fastcall HD0ListChange(TObject *Sender);
        void __fastcall HD1ListChange(TObject *Sender);
        void __fastcall HD0HUDClick(TObject *Sender, TUDBtnType Button);
        void __fastcall HD1HUDClick(TObject *Sender, TUDBtnType Button);
        void __fastcall HD0ReadOnlyClick(TObject *Sender);
        void __fastcall HD1ReadOnlyClick(TObject *Sender);
        void __fastcall DriveANewBtnClick(TObject *Sender);
        void __fastcall DriveBNewBtnClick(TObject *Sender);
        void __fastcall MDVNewBtnClick(TObject *Sender);
        void __fastcall HD0NewBtnClick(TObject *Sender);
        void __fastcall HD1NewBtnClick(TObject *Sender);

private:	// User declarations
        int GetMDVNo(TObject *Sender);
        TEdit* GetMDVTextBox(int Drive);
        void BuildHDList(TComboBox *List);
        AnsiString DragFileName;
        void ConfigureOpenFloppyDiskImageDialog();

        void ConfigureFloppyDriveGroup();
        void ConfigureHardDriveGroup();
        void ConfigureMicrodriveGroup();

        void ConfigureMicrodrive(TLabel* MDVLabel, TEdit* MDVText, TButton* MDVFSBtn, TButton* MDVNewBtn, TButton* MDVEjectBtn);
        bool CreateMicrodriveCartridge(AnsiString& filePath);

        bool NewFloppyDisk(AnsiString& filePath);
        bool CreateFloppyDiskImage(AnsiString title, AnsiString filter, AnsiString defaultExt, AnsiString& filePath);
        void OpenFloppyDriveImage(int driveNumber, char* driveimg, TEdit* driveText, int readonly);

        void NewHardDiskImage(int driveNumber, TEdit* HDText, TCheckBox* HDReadOnly, TComboBox* HDList);
        void CreateHardDiskImage(int driveNumber, AnsiString Filename, TEdit* HDText, TCheckBox* HDReadOnly, TComboBox* HDList);
        AnsiString SelectHardDiskImage(int driveNumber);
        void SetHardDriveParameterVisibility(int driveNumber, bool visible);
        void SetHardDriveParameters(int driveNumber, int c, int h, int s);
        void ConfigureHardDrive(int driveNumber, TLabel* HDLabel, TEdit* HDText, TComboBox* HDList, TCheckBox* HDReadOnly, TButton* HDFSBtn, TButton* HDNewBtn, TButton* HDEjectBtn);
        void HardDiskEject(int driveNumber, TEdit* HDText, TComboBox* HDList);

public:		// User declarations
        __fastcall TP3Drive(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void InsertFile(AnsiString Filename);
};
//---------------------------------------------------------------------------
extern PACKAGE TP3Drive *P3Drive;
//---------------------------------------------------------------------------
#endif
