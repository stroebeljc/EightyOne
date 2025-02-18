#include <vcl4.h>
#include <io.h>
#pragma hdrstop

#include "zx81config.h"
#include "Plus3Drives.h"
#include "floppy.h"
#include "ide.h"
#include "utils.h"
#include "main_.h"
#include "MakeHDF.h"
#include "interface1.h"
#include "ZipFile_.h"

#define	F_OK	0
#define	R_OK	4
#define	W_OK	2
#define	X_OK	1

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TP3Drive *P3Drive;

extern "C" void P3DriveMachineHasInitialised(void);

//---------------------------------------------------------------------------
__fastcall TP3Drive::TP3Drive(TComponent* Owner)
        : TForm(Owner)
{
        DetectPhysDrives();
        BuildHDList(HD0List);
        BuildHDList(HD1List);

        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        FormShow(Owner);
}
//---------------------------------------------------------------------------
void TP3Drive::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("DRIVES", "Top", Top);
        Left = ini->ReadInteger("DRIVES", "Left", Left);
        OpenDialogFloppyDiskImage->FileName = ini->ReadString("DRIVES", "LastFile", OpenDialogFloppyDiskImage->FileName);

        ATA_LoadHDF(0, AnsiString(ini->ReadString("DRIVES", "HD0", "")).c_str());
        ATA_LoadHDF(1, AnsiString(ini->ReadString("DRIVES", "HD1", "")).c_str());
        ATA_SetReadOnly(0, ini->ReadBool("DRIVES", "HD0RO", FALSE));
        ATA_SetReadOnly(1, ini->ReadBool("DRIVES", "HD1RO", FALSE));

        DriveAText->Text = ini->ReadString("DRIVES", "DriveA", "< Empty >");
        if ((DriveAText->Text != "< Empty >") && !FileExists(DriveAText->Text))
                DriveAText->Text = "< Empty >";
        DriveBText->Text = ini->ReadString("DRIVES", "DriveB", "< Empty >");
        if ((DriveBText->Text != "< Empty >") && !FileExists(DriveBText->Text))
                DriveBText->Text = "< Empty >";

        IF1->MDVNoDrives = ini->ReadInteger("DRIVES", "MDVNoDrives", 0);
        for (int i = 0; i < 8; i++)
        {
                IF1->MDVSetFileName(i, AnsiString(ini->ReadString("DRIVES", "MDV" + AnsiString(i), "< Empty >")).c_str());
        }

        if (Form1->DiskDrives1->Checked) Show();
}
//---------------------------------------------------------------------------

void TP3Drive::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("DRIVES", "Top", Top);
        ini->WriteInteger("DRIVES", "Left", Left);
        ini->WriteString("DRIVES", "LastFile", OpenDialogFloppyDiskImage->FileName);

        ini->WriteString("DRIVES", "HD0", ATA_GetHDF(0) ? ATA_GetHDF(0) : "");
        ini->WriteString("DRIVES", "HD1", ATA_GetHDF(1) ? ATA_GetHDF(1) : "");
        ini->WriteBool("DRIVES", "HD0RO", ATA_GetReadOnly(0));
        ini->WriteBool("DRIVES", "HD1RO", ATA_GetReadOnly(1));

        ini->WriteString("DRIVES", "DriveA", DriveAText->Text);
        ini->WriteString("DRIVES", "DriveB", DriveBText->Text);

        ini->WriteInteger("DRIVES", "MDVNoDrives", IF1->MDVNoDrives);
        for (int i = 0; i < 8; i++)
        {
                ini->WriteString("DRIVES", "MDV" + AnsiString(i),
                        IF1->MDVGetFileName(i) ? IF1->MDVGetFileName(i) : "< Empty >");
        }   
}
//---------------------------------------------------------------------------

void TP3Drive::BuildHDList(TComboBox *List)
{
        AnsiString old, temp;
        int i, size;

        old = List->Items->Strings[List->ItemIndex];

        while (List->Items->Count) List->Items->Delete(0);
        List->Items->Add("HDF Image File...");

        i = 0;
        while (PhysDrives[i].Drive != -1)
        {
                if (PhysDrives[i].Type) temp = "Removable Drive, ";
                else temp = "Fixed Drive, ";

                size = PhysDrives[i].Size / 2;

                if (size < 1024) { temp = temp + size; temp = temp + "KB"; }
                else if (size < (1024*1024)) { temp = temp + (size/1024); temp = temp + "MB"; }
                else { temp = temp + (size/(1024*1024)); temp = temp + "GB"; }

                List->Items->Add(temp);
                i++;
        }

        List->ItemIndex = 0;
        for(i = 0; i < List->Items->Count; i++)
        {
                if (List->Items->Strings[i] == old) List->ItemIndex = i;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureOpenFloppyDiskImageDialog()
{
        switch (spectrum.floppytype)
        {
        case FLOPPYIF1:
                OpenDialogFloppyDiskImage->DefaultExt = ".mdr";
                OpenDialogFloppyDiskImage->Filter = "Microdrive Cartridges (*.mdr;*.mdv)|*.mdr;*.mdv";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Microdrive Cartridge";
                break;

        case FLOPPYPLUS3:
                OpenDialogFloppyDiskImage->DefaultExt = ".dsk";
                OpenDialogFloppyDiskImage->Filter = "DSK Disk Images (*.dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.dsk;*.zip)|*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select +3 Disk";
                break;

        case FLOPPYPLUSD:
                OpenDialogFloppyDiskImage->DefaultExt = ".mgt";
                OpenDialogFloppyDiskImage->Filter = "Plus D Disk Images (*.mgt;*.img)|*.mgt;*.img|DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.mgt;*.img;*.dsk*.zip)|*.mgt;*.img;*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Plus D Disk";
                break;

        case FLOPPYDISCIPLE:
                OpenDialogFloppyDiskImage->DefaultExt = ".mgt";
                OpenDialogFloppyDiskImage->Filter = "DISCiPLE Disk Images (*.mgt;*.img)|*.mgt;*.img|DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.mgt;*.img;*.dsk;*.zip)|*.mgt;*.img;*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select DISCiPLE Disk";
                break;

        case FLOPPYBETA:
                OpenDialogFloppyDiskImage->DefaultExt = ".trd";
                OpenDialogFloppyDiskImage->Filter = "TR-DOS Disk Images (*.trd)|*.trd|DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.trd;*.dsk;*.zip)|*.trd;*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Beta Disk";
                break;

        case FLOPPYOPUSD:
                OpenDialogFloppyDiskImage->DefaultExt = ".trd";
                OpenDialogFloppyDiskImage->Filter = "Opus Discovery Disk Images (*.opd;*.opu)|*.opd;*.opu|DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.opd;*.opu;*.dsk;*.zip)|*.opd;*.opu;*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Opus Discovery Disk";
                break;

        case FLOPPYLARKEN81:
                OpenDialogFloppyDiskImage->DefaultExt = ".lar";
                OpenDialogFloppyDiskImage->Filter = "Larken Disk Images (*.lar)|*.lar|DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*lar;*.dsk;*.zip)|*.lar;*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Larken Disk";
                break;

        case FLOPPYZX1541:
                OpenDialogFloppyDiskImage->DefaultExt = ".dsk";
                OpenDialogFloppyDiskImage->Filter = "DSK Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|All Disk Images (*dsk;*.zip)|*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select ZX1541 Disk";
                break;

        default:
                break;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveAFSBtnClick(TObject *Sender)
{
        AnsiString Filename, Ext;
        int readonly = 0;

        ConfigureOpenFloppyDiskImageDialog();

        if (!OpenDialogFloppyDiskImage->Execute()) return;

        Filename = OpenDialogFloppyDiskImage->FileName;
        Ext = FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename = ZipFile->ExpandZIP(Filename, OpenDialogFloppyDiskImage->Filter);
                if (Filename == "") return;
                Ext = FileNameGetExt(Filename);
                readonly = 1;
        }

        DriveAText->Text = Filename;
        DriveAText->SelStart = DriveAText->Text.Length() - 1;
        DriveAText->SelLength = 0;

        OpenFloppyDriveImage(0, spectrum.driveaimg, DriveAText, readonly);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBFSBtnClick(TObject *Sender)
{
        AnsiString Filename, Ext;
        int readonly = 0;
        ConfigureOpenFloppyDiskImageDialog();

        if (!OpenDialogFloppyDiskImage->Execute()) return;

        Filename = OpenDialogFloppyDiskImage->FileName;
        Ext = FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename = ZipFile->ExpandZIP(Filename, OpenDialogFloppyDiskImage->Filter);
                if (Filename == "") return;
                Ext = FileNameGetExt(Filename);
                readonly = 1;
        }

        DriveBText->Text = Filename;
        DriveBText->SelStart = DriveBText->Text.Length() - 1;
        DriveBText->SelLength = 0;

        OpenFloppyDriveImage(1, spectrum.drivebimg, DriveBText, readonly);
}
//---------------------------------------------------------------------------

void TP3Drive::OpenFloppyDriveImage(int driveNumber, char* driveimg, TEdit* driveText, int readonly)
{
        strcpy(driveimg, driveText->Text.c_str());

        if (spectrum.floppytype!=FLOPPYPLUS3 && access(driveimg, F_OK) && !readonly)
        {
                FILE *f;
                if ((f = fopen(driveimg, "w")) != NULL)
                {
                        fclose(f);
                }
        }

        floppy_setimage(driveNumber, driveimg, readonly);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveAEjectBtnClick(TObject *Sender)
{
        FloppyDiskEject(0, DriveAText, spectrum.driveaimg);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBEjectBtnClick(TObject *Sender)
{
        FloppyDiskEject(1, DriveBText, spectrum.drivebimg);
}
//---------------------------------------------------------------------------

void TP3Drive::FloppyDiskEject(int driveNumber, TEdit* DriveText, char* driveimg)
{
        DriveText->Text = "< Empty >";
        driveimg[0] = '\0';
        floppy_eject(driveNumber);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormShow(TObject *Sender)
{
        ConfigureFloppyDriveGroup();
        ConfigureHardDriveGroup();
        ConfigureMicrodriveGroup();

        int yPos = 8;

        if (FloppyDriveGroup->Visible)
        {
                FloppyDriveGroup->Top = yPos;
                yPos += FloppyDriveGroup->Height + 8;
        }

        if (MicrodriveGroup->Visible)
        {
                MicrodriveGroup->Top = yPos;
                yPos += MicrodriveGroup->Height + 8;
        }

        if (HardDriveGroup->Visible)
        {
                HardDriveGroup->Top = yPos;
                yPos += HardDriveGroup->Height + 8;
        }

        Height = yPos + OK->Height + 32;

        Form1->DiskDrives1->Enabled = (FloppyDriveGroup->Visible || HardDriveGroup->Visible || MicrodriveGroup->Visible);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureFloppyDriveGroup()
{
        if (strlen(spectrum.driveaimg)) DriveAText->Text = spectrum.driveaimg;
        if (strlen(spectrum.drivebimg)) DriveBText->Text = spectrum.drivebimg;

        DriveAText->SelStart = DriveAText->Text.Length() - 1;
        DriveAText->SelLength = 0;
        DriveBText->SelStart = DriveBText->Text.Length() - 1;
        DriveBText->SelLength = 0;

        bool plus3NoDriveA = (spectrum.floppytype == FLOPPYPLUS3 && spectrum.driveatype == DRIVENONE);
        bool plus3NoDriveB = (spectrum.floppytype == FLOPPYPLUS3 && spectrum.drivebtype == DRIVENONE);

        DriveALabel->Enabled    = !plus3NoDriveA;
        DriveAFSBtn->Enabled    = !plus3NoDriveA;
        DriveANewBtn->Enabled   = !plus3NoDriveA;
        DriveAEjectBtn->Enabled = !plus3NoDriveA;

        DriveBLabel->Enabled    = !plus3NoDriveB;
        DriveBFSBtn->Enabled    = !plus3NoDriveB;
        DriveBNewBtn->Enabled   = !plus3NoDriveB;
        DriveBEjectBtn->Enabled = !plus3NoDriveB;

        FloppyDriveGroup->Visible = (spectrum.floppytype != FLOPPYNONE && spectrum.floppytype != FLOPPYIF1);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureHardDriveGroup()
{
        ConfigureHardDrive(0, HD0Label, HD0Text, HD0List, HD0ReadOnly, HD0FSBtn, HD0NewBtn, HD0EjectBtn);
        ConfigureHardDrive(1, HD1Label, HD1Text, HD1List, HD1ReadOnly, HD1FSBtn, HD1NewBtn, HD1EjectBtn);

        HardDriveGroup->Visible = (spectrum.HDType != HDNONE);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureHardDrive(int driveNumber, TLabel* HDLabel, TEdit* HDText, TComboBox* HDList, TCheckBox* HDReadOnly, TButton* HDFSBtn, TButton* HDNewBtn, TButton* HDEjectBtn)
{
        bool parametersVisible = false;

        HDText->Text="< Empty >";

        if (ATA_GetHDF(driveNumber))
        {
                HDText->Text = ATA_GetHDF(driveNumber);
                int i = 0;

                while (PhysDrives[i].Drive != -1)
                {
                        if (HDText->Text == PhysDrives[i].Path)
                        {
                                HDList->ItemIndex = i + 1;
                                break;
                        }
                        i++;
                }

                int c, h, s;
                unsigned long size;
                ATA_GetCHS(driveNumber, &c, &h, &s, &size);
                SetHardDriveParameters(driveNumber, c, h, s);

                if (HDText->Text[1] == '\\' && HDText->Text[2] == '\\')
                {
                        parametersVisible = true;
                }
        }

        SetHardDriveParameterVisibility(driveNumber, parametersVisible);

        HDReadOnly->Checked = ATA_GetReadOnly(driveNumber);

        HDText->SelStart = HDText->Text.Length() - 1;
        HDText->SelLength = 0;

        HDLabel->Enabled    = true;
        HDFSBtn->Enabled    = true;
        HDNewBtn->Enabled   = true;
        HDEjectBtn->Enabled = true;
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureMicrodriveGroup()
{
        ConfigureMicrodrive(MDV0Label, MDV0Text, MDV0FSBtn, MDV0NewBtn, MDV0EjectBtn);
        ConfigureMicrodrive(MDV1Label, MDV1Text, MDV1FSBtn, MDV1NewBtn, MDV1EjectBtn);
        ConfigureMicrodrive(MDV2Label, MDV2Text, MDV2FSBtn, MDV2NewBtn, MDV2EjectBtn);
        ConfigureMicrodrive(MDV3Label, MDV3Text, MDV3FSBtn, MDV3NewBtn, MDV3EjectBtn);
        ConfigureMicrodrive(MDV4Label, MDV4Text, MDV4FSBtn, MDV4NewBtn, MDV4EjectBtn);
        ConfigureMicrodrive(MDV5Label, MDV5Text, MDV5FSBtn, MDV5NewBtn, MDV5EjectBtn);
        ConfigureMicrodrive(MDV6Label, MDV6Text, MDV6FSBtn, MDV6NewBtn, MDV6EjectBtn);
        ConfigureMicrodrive(MDV7Label, MDV7Text, MDV7FSBtn, MDV7NewBtn, MDV7EjectBtn);

        MicrodriveGroup->Visible = (spectrum.floppytype == FLOPPYIF1 && IF1->MDVNoDrives > 0);
        MicrodriveGroup->Height  = MDV0Text->Top + ((MDV1Text->Top - MDV0Text->Top) * IF1->MDVNoDrives) + 8;
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureMicrodrive(TLabel* MDVLabel, TEdit* MDVText, TButton* MDVFSBtn, TButton* MDVNewBtn, TButton* MDVEjectBtn)
{
        int driveNumber = GetMDVNo((TObject*)MDVLabel);

        MDVText->Text = IF1->MDVGetFileName(driveNumber) ? IF1->MDVGetFileName(driveNumber) : "< Empty >";
        MDVText->SelStart = MDVText->Text.Length() - 1;
        MDVText->SelLength = 0;

        bool drivePresent = (IF1->MDVNoDrives > driveNumber);
        MDVLabel->Visible    = drivePresent;
        MDVText->Visible     = drivePresent;
        MDVFSBtn->Visible    = drivePresent;
        MDVNewBtn->Visible   = drivePresent;
        MDVEjectBtn->Visible = drivePresent;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->DiskDrives1->Checked = false;                                                                             
        Form1->DiskDrives1->Enabled = (FloppyDriveGroup->Visible || HardDriveGroup->Visible || MicrodriveGroup->Visible);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0FSBtnClick(TObject *Sender)
{
        AnsiString Filename;

        if (DragFileName != "")
        {
                Filename = DragFileName;
        }
        else
        {
                if (!Sender && (HD0List->ItemIndex != 0))
                {
                        Filename = PhysDrives[HD0List->ItemIndex - 1].Path;
                }
                else
                {
                        HD0List->ItemIndex = 0;

                        Filename = SelectHardDiskImage(0);
                        if (Filename == NULL) return;
                }
        }

        CreateHardDiskImage(0, Filename, HD0Text, HD0ReadOnly, HD0List);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1FSBtnClick(TObject *Sender)
{
        AnsiString Filename;

        if (!Sender && (HD1List->ItemIndex != 0))
        {
                Filename = PhysDrives[HD1List->ItemIndex - 1].Path;
        }
        else
        {
                HD1List->ItemIndex = 0;

                Filename = SelectHardDiskImage(1);
                if (Filename == NULL) return;
        }

        CreateHardDiskImage(1, Filename, HD1Text, HD1ReadOnly, HD1List);
}
//---------------------------------------------------------------------------

AnsiString TP3Drive::SelectHardDiskImage(int driveNumber)
{
        char* lastFileName = ATA_GetHDF(driveNumber);

        if (lastFileName) OpenDialogHardDriveImage->FileName = lastFileName;

        if (OpenDialogHardDriveImage->FileName.Length() == 0 || *(OpenDialogHardDriveImage->FileName.AnsiLastChar()) == '\\')
        {
                OpenDialogHardDriveImage->FileName = "";
        }

        if (!OpenDialogHardDriveImage->Execute()) return NULL;

        AnsiString Filename = OpenDialogHardDriveImage->FileName;
        AnsiString Ext = FileNameGetExt(Filename);

        if (Ext != ".HDF" && Ext != ".VHD") Filename += ".hdf";
        Ext = FileNameGetExt(Filename);

        if (access(Filename.c_str(), F_OK) && Ext == ".HDF")
        {
                AnsiString message = Filename + " does not exist.\nWould you like to create it?";
                int ret = Application->MessageBox(message.c_str(), "Select Hard Disk Image", MB_OKCANCEL | MB_ICONQUESTION);

                if (ret != IDOK) return NULL;

                CreateHDF->FileName = Filename;
                CreateHDF->ShowModal();
        }

        if (access(Filename.c_str(), F_OK)) return NULL;

        return Filename;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0NewBtnClick(TObject *Sender)
{
        NewHardDiskImage(0, HD0Text, HD0ReadOnly, HD0List);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1NewBtnClick(TObject *Sender)
{
        NewHardDiskImage(1, HD1Text, HD1ReadOnly, HD1List);
}
//---------------------------------------------------------------------------

void TP3Drive::NewHardDiskImage(int driveNumber, TEdit* HDText, TCheckBox* HDReadOnly, TComboBox* HDList)
{
        if (!SaveDialogNewHardDriveImage->Execute()) return;

        AnsiString Filename = SaveDialogNewHardDriveImage->FileName;

        CreateHDF->FileName = Filename;
        CreateHDF->ShowModal();

        if (access(Filename.c_str(), F_OK)) return;

        CreateHardDiskImage(driveNumber, Filename, HDText, HDReadOnly, HDList);
}
//---------------------------------------------------------------------------

void TP3Drive::CreateHardDiskImage(int driveNumber, AnsiString Filename, TEdit* HDText, TCheckBox* HDReadOnly, TComboBox* HDList)
{
        HDText->Text = Filename;
        HDText->SelStart = HDText->Text.Length() - 1;
        HDText->SelLength = 0;

        if (ATA_LoadHDF(driveNumber, HDText->Text.c_str()))
        {
                HardDiskEject(driveNumber, HDText, HDList);
                return;
        }

        HDReadOnly->Checked = ATA_GetReadOnly(driveNumber);

        int c, h, s;
        unsigned long size;
        ATA_GetCHS(driveNumber, &c, &h, &s, &size);
        SetHardDriveParameters(driveNumber, c, h, s);

        bool showParameters = (HDText->Text[1] == '\\' && HDText->Text[2] == '\\');
        SetHardDriveParameterVisibility(driveNumber, showParameters);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0EjectBtnClick(TObject *Sender)
{
        HardDiskEject(0, HD0Text, HD0List);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1EjectBtnClick(TObject *Sender)
{
        HardDiskEject(1, HD1Text, HD1List);
}
//---------------------------------------------------------------------------

void TP3Drive::HardDiskEject(int driveNumber, TEdit* HDText, TComboBox* HDList)
{
        HDText->Text = "< Empty >";
        ATA_EjectHDF(driveNumber);
        HDList->ItemIndex = 0;
        SetHardDriveParameterVisibility(driveNumber, false);
}
//---------------------------------------------------------------------------

void TP3Drive::SetHardDriveParameterVisibility(int driveNumber, bool visible)
{
        if (driveNumber == 0)
        {
                HD0CHS->Visible = visible; HD0C->Visible   = visible; HD0H->Visible   = visible;
                HD0S->Visible   = visible; HD0HUD->Visible = visible; HD0SUD->Visible = visible;
        }
        else
        {
                HD1CHS->Visible = visible; HD1C->Visible   = visible; HD1H->Visible   = visible;
                HD1S->Visible   = visible; HD1HUD->Visible = visible; HD1SUD->Visible = visible;
        }
}
//---------------------------------------------------------------------------

void TP3Drive::SetHardDriveParameters(int driveNumber, int c, int h, int s)
{
        if (driveNumber == 0)
        {
                HD0C->Text = c;
                HD0H->Text = h; HD0HUD->Position = (short)h; HD0HUD->Min = 1; HD0HUD->Max = 15;
                HD0S->Text = s; HD0SUD->Position = (short)s; HD0SUD->Min = 1; HD0SUD->Max = 255;
        }
        else
        {
                HD1C->Text = c;
                HD1H->Text = h; HD1HUD->Position = (short)h; HD1HUD->Min = 1; HD1HUD->Max = 15;
                HD1S->Text = s; HD1SUD->Position = (short)s; HD1SUD->Min = 1; HD1SUD->Max = 255;
        }
}
//---------------------------------------------------------------------------

int TP3Drive::GetMDVNo(TObject *Sender)
{
        if (Sender == MDV0Label || Sender == MDV0Text || Sender == MDV0FSBtn || Sender == MDV0NewBtn || Sender == MDV0EjectBtn) return 0;
        if (Sender == MDV1Label || Sender == MDV1Text || Sender == MDV1FSBtn || Sender == MDV1NewBtn || Sender == MDV1EjectBtn) return 1;
        if (Sender == MDV2Label || Sender == MDV2Text || Sender == MDV2FSBtn || Sender == MDV2NewBtn || Sender == MDV2EjectBtn) return 2;
        if (Sender == MDV3Label || Sender == MDV3Text || Sender == MDV3FSBtn || Sender == MDV3NewBtn || Sender == MDV3EjectBtn) return 3;
        if (Sender == MDV4Label || Sender == MDV4Text || Sender == MDV4FSBtn || Sender == MDV4NewBtn || Sender == MDV4EjectBtn) return 4;
        if (Sender == MDV5Label || Sender == MDV5Text || Sender == MDV5FSBtn || Sender == MDV5NewBtn || Sender == MDV5EjectBtn) return 5;
        if (Sender == MDV6Label || Sender == MDV6Text || Sender == MDV6FSBtn || Sender == MDV6NewBtn || Sender == MDV6EjectBtn) return 6;
        if (Sender == MDV7Label || Sender == MDV7Text || Sender == MDV7FSBtn || Sender == MDV7NewBtn || Sender == MDV7EjectBtn) return 7;

        return 0;
}
//---------------------------------------------------------------------------

TEdit* TP3Drive::GetMDVTextBox(int Drive)
{
        switch (Drive)
        {
        case 0: return MDV0Text;
        case 1: return MDV1Text;
        case 2: return MDV2Text;
        case 3: return MDV3Text;
        case 4: return MDV4Text;
        case 5: return MDV5Text;
        case 6: return MDV6Text;
        case 7: return MDV7Text;
        default: return MDV0Text;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDVEjectBtnClick(TObject *Sender)
{
        int Drive = GetMDVNo(Sender);
        IF1->MDVSetFileName(Drive, NULL);
        GetMDVTextBox(Drive)->Text = "< Empty >";
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDVFSBtnClick(TObject *Sender)
{
        AnsiString FileName;

        int driveNumber = GetMDVNo(Sender);
        TEdit* Text = GetMDVTextBox(driveNumber);

        if (Sender)
        {
                ConfigureOpenFloppyDiskImageDialog();

                FileName = Text->Text;
                if (FileName != "< Empty >")
                {
                        OpenDialogFloppyDiskImage->FileName = FileName;
                }

                if (!OpenDialogFloppyDiskImage->Execute()) return;
                FileName = OpenDialogFloppyDiskImage->FileName;

                if (!CreateMicrodriveCartridge(FileName)) return;
        }
        else
        {
                FileName = DragFileName;
        }

        IF1->MDVSetFileName(driveNumber, FileName.c_str());
        
        Text->Text = FileName;
        Text->SelStart = Text->Text.Length() - 1;
        Text->SelLength = 0;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::RedetectDrivesClick(TObject *Sender)
{
        DetectPhysDrives();
        BuildHDList(HD0List);
        BuildHDList(HD1List);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0ListChange(TObject *Sender)
{
        if (HD0List->ItemIndex > 0) HD0FSBtnClick(NULL);
        else HD0EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ListChange(TObject *Sender)
{
        if (HD1List->ItemIndex > 0) HD1FSBtnClick(NULL);
        else HD1EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------
          
void __fastcall TP3Drive::HD0HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(0, &c, &h, &s, &size);

        h = HD0HUD->Position;
        s = HD0SUD->Position;
        c = size / (h * s);
        if (c > 65535) c = 65535;

        SetHardDriveParameters(0, c, h, s);

        ATA_SetCHS(0, c, h, s);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(1, &c, &h, &s, &size);

        h = HD0HUD->Position;
        s = HD0SUD->Position;
        c = size / (h * s);
        if (c > 65535) c = 65535;

        SetHardDriveParameters(1, c, h, s);

        ATA_SetCHS(1, c, h, s);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(0, HD0ReadOnly->Checked);
        HD0ReadOnly->Checked = ATA_GetReadOnly(0);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(1, HD1ReadOnly->Checked);
        HD1ReadOnly->Checked = ATA_GetReadOnly(1);
}
//---------------------------------------------------------------------------

void TP3Drive::InsertFile(AnsiString Filename)
{
        DragFileName = Filename;

        AnsiString Ext = GetExt(Filename);

        if (Ext == ".MDR" || Ext == ".MDV")
        {
                MDVFSBtnClick(NULL);
        }
        else if (Ext == ".HDF" || Ext == ".VHD")
        {
                HD0FSBtnClick(NULL);
        }
        else if (Ext == ".DSK" || Ext == ".MGT" || Ext == ".IMG" || Ext == ".OPD" || Ext == ".OPU" || Ext == ".TRD" || Ext == ".LAR")
        {
                DriveAFSBtnClick(NULL);
        }

        DragFileName = "";
}
//---------------------------------------------------------------------------

void P3DriveMachineHasInitialised(void)
{
        if (P3Drive->DriveAText->Text != "< Empty >")
                floppy_setimage(0, P3Drive->DriveAText->Text.c_str(), 0);

        if (P3Drive->DriveBText->Text != "< Empty >")
                floppy_setimage(1, P3Drive->DriveBText->Text.c_str(), 0);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveANewBtnClick(TObject *Sender)
{
        AnsiString filePath;

        if (NewFloppyDisk(filePath))
        {
                DriveAText->Text = filePath;
                DriveAText->SelStart = DriveAText->Text.Length() - 1;
                DriveAText->SelLength = 0;

                int readonly = 0;
                OpenFloppyDriveImage(0, spectrum.driveaimg, DriveAText, readonly);
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBNewBtnClick(TObject *Sender)
{
        AnsiString filePath;

        if (NewFloppyDisk(filePath))
        {
                DriveBText->Text = filePath;
                DriveBText->SelStart = DriveBText->Text.Length() - 1;
                DriveBText->SelLength = 0;

                int readonly = 0;
                OpenFloppyDriveImage(1, spectrum.drivebimg, DriveBText, readonly);
        }
}
//---------------------------------------------------------------------------

bool TP3Drive::NewFloppyDisk(AnsiString& filePath)
{
        bool success;

        switch (spectrum.floppytype)
        {
        case FLOPPYPLUS3:
                success = CreateFloppyDiskImage("Create New +3 Floppy Disk", "DSK Disk Images (*.dsk)|*.dsk", ".dsk", filePath);
                break;

        case FLOPPYPLUSD:
                success = CreateFloppyDiskImage("Create New Plus D Floppy Disk", "Plus D Disk Images (*.mgt;*.img)|*.mgt;*.img|DSK Disk Images (*.dsk)|*.dsk", ".mgt", filePath);
                break;

        case FLOPPYDISCIPLE:
                success = CreateFloppyDiskImage("Create New DISCiPLE Floppy Disk", "DISCiPLE Disk Images (*.mgt;*.img)|*.mgt;*.img|DSK Disk Images (*.dsk)|*.dsk", ".mgt", filePath);
                break;

        case FLOPPYBETA:
                success = CreateFloppyDiskImage("Create New Beta Floppy Disk", "TR-DOS Disk Images (*.trd)|*.trd|DSK Disk Images (*.dsk)|*.dsk", ".trd", filePath);
                break;

        case FLOPPYOPUSD:
                success = CreateFloppyDiskImage("Create New Opus Discovory Floppy Disk", "Opus Discovery Disk Images (*.opd;*.opu)|*.opd;*.opu|DSK Disk Images (*.dsk)|*.dsk", ".opd", filePath);
                break;

        case FLOPPYLARKEN81:
                success = CreateFloppyDiskImage("Create New Larken Floppy Disk", "Larken Disk Images (*.lar)|*.lar|DSK Disk Images (*.dsk)|*.dsk", ".lar", filePath);
                break;

        case FLOPPYZX1541:
                success = CreateFloppyDiskImage("Create New ZX1541 Floppy Disk", "DSK Disk Images (*.dsk)|*.dsk", ".dsk", filePath);
                break;

        default:
                filePath = "";
                success = false;
                break;
        }

        return success;
}
//---------------------------------------------------------------------------

bool TP3Drive::CreateFloppyDiskImage(AnsiString title, AnsiString filter, AnsiString defaultExt, AnsiString& filePath)
{
        P3Drive->Enabled = false;

        bool success = true;

        SaveDialogNewFloppyDisk-> Title = title;
        SaveDialogNewFloppyDisk->Filter = filter;
        SaveDialogNewFloppyDisk->DefaultExt = defaultExt;
        SaveDialogNewFloppyDisk->FilterIndex = 1;

        if (SaveDialogNewFloppyDisk->Execute())
        {
                filePath = SaveDialogNewFloppyDisk->FileName;

                if (!access(filePath.c_str(), F_OK))
                {
                        ShowMessage("File already exists.");
                        success = false;
                }
        }
        else
        {
                success = false;
        }

        P3Drive->Enabled = true;

        return success;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDVNewBtnClick(TObject *Sender)
{
        AnsiString filePath;

        if (CreateMicrodriveCartridge(filePath))
        {
                int driveNumber = GetMDVNo(Sender);
                GetMDVTextBox(driveNumber)->Text = filePath;
                IF1->MDVSetFileName(driveNumber,filePath.c_str());
        }
}
//---------------------------------------------------------------------------

bool TP3Drive::CreateMicrodriveCartridge(AnsiString& filePath)
{
        P3Drive->Enabled = false;

        bool success = true;

        if (filePath.IsEmpty())
        {
                SaveDialogNewFloppyDisk-> Title = "Create New Microdrive Cartridge";
                SaveDialogNewFloppyDisk->Filter = "Microdrive Cartridge (*.mdr)|*.mdr";
                SaveDialogNewFloppyDisk->DefaultExt = ".mdr";
                SaveDialogNewFloppyDisk->FilterIndex = 1;

                if (SaveDialogNewFloppyDisk->Execute())
                {
                        filePath = SaveDialogNewFloppyDisk->FileName;

                        if (!access(filePath.c_str(), F_OK))
                        {
                                ShowMessage("File already exists.");
                                success = false;
                        }
                }
        }

        if (success && !filePath.IsEmpty())
        {
                if (access(filePath.c_str(),F_OK))
                {
                        FILE* f = fopen(filePath.c_str(), "wb");
                        if (f)
                        {
                                try
                                {
                                        const int sizeOfMicrodriveFile = (254 * 543) + 1;

                                        for (int i = 0; i < sizeOfMicrodriveFile - 1; i++)
                                        {
                                                // 'Blank' data
                                	        fputc(0xFC, f);
                                        }

                                        // The cartridge is not write protected (this byte is not used by EightyOne)
                                        fputc(0x00, f);
                                }
                                catch (...)
                                {
                                        success = false;
                                }

                                if (fclose(f))
                                {
                                        success = false;
                                }
                        }
                        else
                        {
                                success = false;
                        }
                }

                if (!success)
                {
                        ShowMessage("Failed to create Microdrive cartridge");
                }
        }
        else
        {
                success = false;
        }

        P3Drive->Enabled = true;

        return success;
}
//---------------------------------------------------------------------------


