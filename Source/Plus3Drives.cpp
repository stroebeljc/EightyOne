//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TP3Drive *P3Drive;

extern "C" void P3DriveMachineHasInitialised(void);

//---------------------------------------------------------------------------
__fastcall TP3Drive::TP3Drive(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DetectPhysDrives();
        BuildHDList(HD0List);
        BuildHDList(HD1List);

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        FloppyTop=FloppyDriveGroup->Top;
        HDTop=HardDriveGroup->Top;
        MicroTop=MicrodriveGroup->Top;

        FormShow(Owner);
}
//---------------------------------------------------------------------------
void TP3Drive::LoadSettings(TIniFile *ini)
{
        AnsiString Rom;

        Top=ini->ReadInteger("P3DRIVE","Top",Top);
        Left=ini->ReadInteger("P3DRIVE","Left",Left);
        OpenDialogFloppyDiskImage->FileName=ini->ReadString("P3DRIVE","LastFile",OpenDialogFloppyDiskImage->FileName);

        if (Form1->DiskDrives1->Checked) Show();
}
//---------------------------------------------------------------------------

void TP3Drive::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("P3DRIVE","Top",Top);
        ini->WriteInteger("P3DRIVE","Left",Left);
        ini->WriteString("P3DRIVE","LastFile",OpenDialogFloppyDiskImage->FileName);
}
//---------------------------------------------------------------------------

void TP3Drive::BuildHDList(TComboBox *List)
{
        AnsiString old, temp;
        int i, size;

        old=List->Items->Strings[List->ItemIndex];

        while(List->Items->Count) List->Items->Delete(0);
        List->Items->Add("HDF Image File...");

        i=0;
        while(PhysDrives[i].Drive!=-1)
        {
                if (PhysDrives[i].Type) temp="Removable Drive, ";
                else temp="Fixed Drive, ";

                size=PhysDrives[i].Size/2;

                if (size<1024) { temp = temp + size; temp=temp+"kB"; }
                else if (size<(1024*1024)) { temp = temp + (size/1024); temp=temp+"MB"; }
                else { temp = temp + (size/(1024*1024)); temp=temp+"GB"; }

                List->Items->Add(temp);
                i++;
        }

        List->ItemIndex=0;
        for(i=0;i<List->Items->Count;i++)
                if (List->Items->Strings[i]==old) List->ItemIndex=i;
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
                OpenDialogFloppyDiskImage->Filter = "+3 Disk Images (*.dsk)|*.dsk|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.dsk;*.zip)|*.dsk;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select +3 Disk";
                break;

        case FLOPPYPLUSD:
                OpenDialogFloppyDiskImage->DefaultExt = ".mgt";
                OpenDialogFloppyDiskImage->Filter = "Plus D Disk Images (*.mgt;*.img)|*.mgt;*.img|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.mgt;*.img;*.zip)|*.mgt;*.img;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Plus D Disk";
                break;

        case FLOPPYDISCIPLE:
                OpenDialogFloppyDiskImage->DefaultExt = ".mgt";
                OpenDialogFloppyDiskImage->Filter = "DISCiPLE Disk Images (*.mgt;*.img)|*.mgt;*.img|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.mgt;*.img;*.zip)|*.mgt;*.img;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select DISCiPLE Disk";
                break;

        case FLOPPYBETA:
                OpenDialogFloppyDiskImage->DefaultExt = ".trd";
                OpenDialogFloppyDiskImage->Filter = "TR-DOS Disk Images (*.trd)|*.trd|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.trd;*.zip)|*.trd;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Beta Disk";
                break;

        case FLOPPYOPUSD:
                OpenDialogFloppyDiskImage->DefaultExt = ".trd";
                OpenDialogFloppyDiskImage->Filter = "Opus Discovery Disk Images (*.opd;*.opu)|*.opd;*.opu|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*.opd;*.opu;*.zip)|*.opd;*.opu;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Opus Discovery Disk";
                break;

        case FLOPPYLARKEN81:
                OpenDialogFloppyDiskImage->DefaultExt = ".lar";
                OpenDialogFloppyDiskImage->Filter = "Larken Disk Images (*.lar)|*.lar|Compressed Disk Images (*.zip)|*.zip|All Disk Images (*lar;*.zip)|*.lar;*.zip";
                OpenDialogFloppyDiskImage->FilterIndex = 1;
                OpenDialogFloppyDiskImage->Title = "Select Opus Discovery Disk";
                break;

        case FLOPPYZX1541:
                OpenDialogFloppyDiskImage->DefaultExt = ".dsk";
                OpenDialogFloppyDiskImage->Filter = "ZX1541 Disk Images (*dsk)|*.dsk|Compressed Disk Images (*.zip)|All Disk Images (*lar;*.zip)|*.lar;*.zip";  //#### TO BE UPDATED ONCE PROPER FORMAT DETERMINED
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

        if (Sender)
        {
                ConfigureOpenFloppyDiskImageDialog();

                if (!OpenDialogFloppyDiskImage->Execute()) return;

                Filename=OpenDialogFloppyDiskImage->FileName;
        }
        else    Filename=DragFileName;

        Ext=FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenDialogFloppyDiskImage->Filter);
                if (Filename=="") return;
                Ext = FileNameGetExt(Filename);
                readonly = 1;
        }

        DriveAText->Text = Filename;
        DriveAText->SelStart=DriveAText->Text.Length()-1; DriveAText->SelLength=0;
        strcpy(spectrum.driveaimg,DriveAText->Text.c_str());
        
        FILE *f;
        if((f = fopen( spectrum.driveaimg, "r" )) != NULL )
        {
                fclose(f);
        }
        else if((f = fopen( spectrum.driveaimg, "w" )) != NULL )
        {
                fclose(f);
        }
        
        floppy_setimage(0,spectrum.driveaimg,readonly);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveAEjectBtnClick(TObject *Sender)
{
        DriveAText->Text = "< Empty >";
        spectrum.driveaimg[0]='\0';
        floppy_setimage(0,spectrum.driveaimg,1);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBFSBtnClick(TObject *Sender)
{
        AnsiString Filename, Ext;
        int readonly = 0;

        ConfigureOpenFloppyDiskImageDialog();

        if (!OpenDialogFloppyDiskImage->Execute()) return;

        Filename=OpenDialogFloppyDiskImage->FileName;
        Ext=FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenDialogFloppyDiskImage->Filter);
                if (Filename=="") return;
                Ext = FileNameGetExt(Filename);
                readonly = 1;
        }

        DriveBText->Text = Filename;
        DriveBText->SelStart=DriveBText->Text.Length()-1; DriveBText->SelLength=0;
        strcpy(spectrum.drivebimg,DriveBText->Text.c_str());
        
        FILE *f;
        if((f = fopen( spectrum.drivebimg, "r" )) != NULL )
        {
                fclose(f);
        }
        else if((f = fopen( spectrum.drivebimg, "w" )) != NULL )
        {
                fclose(f);
        }
        
        floppy_setimage(1,spectrum.drivebimg,readonly);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBEjectBtnClick(TObject *Sender)
{
        DriveBText->Text = "< Empty >";
        spectrum.drivebimg[0]='\0';
        floppy_setimage(1,spectrum.drivebimg,1);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormShow(TObject *Sender)
{
        ConfigureFloppyDiskGroup();
        ConfigureHardDiskGroup();
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

        btnNewFloppyDisk->Visible = MicrodriveGroup->Visible;

        Form1->DiskDrives1->Enabled = (FloppyDriveGroup->Visible || HardDriveGroup->Visible || MicrodriveGroup->Visible);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureFloppyDiskGroup()
{
        DriveAText->Text="< Empty >";
        DriveBText->Text="< Empty >";

        if (strlen(spectrum.driveaimg)) DriveAText->Text = spectrum.driveaimg;
        if (strlen(spectrum.drivebimg)) DriveBText->Text = spectrum.drivebimg;

        DriveAText->SelStart = DriveAText->Text.Length() - 1; DriveAText->SelLength = 0;
        DriveBText->SelStart = DriveBText->Text.Length() - 1; DriveBText->SelLength = 0;

        bool plus3NoDriveA = (spectrum.floppytype == FLOPPYPLUS3 && spectrum.driveatype == DRIVENONE);
        bool plus3NoDriveB = (spectrum.floppytype == FLOPPYPLUS3 && spectrum.drivebtype == DRIVENONE);

        DriveALabel->Enabled    = !plus3NoDriveA;
        DriveAFSBtn->Enabled    = !plus3NoDriveA;
        DriveAEjectBtn->Enabled = !plus3NoDriveA;

        DriveBLabel->Enabled    = !plus3NoDriveB;
        DriveBFSBtn->Enabled    = !plus3NoDriveB;
        DriveBEjectBtn->Enabled = !plus3NoDriveB;

        FloppyDriveGroup->Visible = (spectrum.floppytype != FLOPPYNONE && spectrum.floppytype != FLOPPYIF1);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureHardDiskGroup()
{
        HD0Text->Text="< Empty >";
        HD1Text->Text="< Empty >";

        HD0CHS->Visible = false; HD0C->Visible   = false; HD0H->Visible   = false;
        HD0S->Visible   = false; HD0HUD->Visible = false; HD0SUD->Visible = false;

        if (ATA_GetHDF(0))
        {
                int i, c, h, s;
                unsigned long size;

                HD0Text->Text = ATA_GetHDF(0);
                i = 0;

                while (PhysDrives[i].Drive != -1)
                {
                        if (HD0Text->Text == PhysDrives[i].Path) HD0List->ItemIndex = i + 1;
                        i++;
                }

                ATA_GetCHS(0, &c, &h, &s, &size);
                HD0C->Text = c;
                HD0H->Text = h; HD0HUD->Position = (short)h; HD0HUD->Min = 1; HD0HUD->Max = 15;
                HD0S->Text = s; HD0SUD->Position = (short)s; HD0SUD->Min = 1; HD0SUD->Max = 255;

                if (HD0Text->Text[1] == '\\' && HD0Text->Text[2] == '\\')
                {
                        HD0CHS->Visible = true; HD0C->Visible   = true; HD0H->Visible   = true;
                        HD0S->Visible   = true; HD0HUD->Visible = true; HD0SUD->Visible = true;
                }
        }

        HD1CHS->Visible = false; HD1C->Visible   = false; HD1H->Visible   = false;
        HD1S->Visible   = false; HD1HUD->Visible = false; HD1SUD->Visible = false;

        if (ATA_GetHDF(1))
        {
                int i, c, h, s;
                unsigned long size;

                HD1Text->Text = ATA_GetHDF(1);
                i = 0;

                while (PhysDrives[i].Drive != -1)
                {
                        if (HD1Text->Text == PhysDrives[i].Path) HD1List->ItemIndex = i + 1;
                        i++;
                }

                ATA_GetCHS(1, &c, &h, &s, &size);
                HD1C->Text = c;
                HD1H->Text = h; HD1HUD->Position = (short)h; HD1HUD->Min = 1; HD1HUD->Max = 15;
                HD1S->Text = s; HD1SUD->Position = (short)s; HD1SUD->Min = 1; HD1SUD->Max = 255;

                if (HD1Text->Text[1] == '\\' && HD1Text->Text[2] == '\\')
                {
                        HD1CHS->Visible = true; HD1C->Visible   = true; HD1H->Visible   = true;
                        HD1S->Visible   = true; HD1HUD->Visible = true; HD1SUD->Visible = true;
                }
        }

        HD0ReadOnly->Checked = ATA_GetReadOnly(0);
        HD1ReadOnly->Checked = ATA_GetReadOnly(1);

        HD0Text->SelStart = HD0Text->Text.Length() - 1; HD0Text->SelLength = 0;
        HD1Text->SelStart = HD1Text->Text.Length() - 1; HD1Text->SelLength = 0;

        HD0Label->Enabled    = true;
        HD0FSBtn->Enabled    = true;
        HD0EjectBtn->Enabled = true;

        HD1Label->Enabled    = true;
        HD1FSBtn->Enabled    = true;
        HD1EjectBtn->Enabled = true;

        HardDriveGroup->Visible = (spectrum.HDType != HDNONE);
}
//---------------------------------------------------------------------------

void TP3Drive::ConfigureMicrodriveGroup()
{
        MDV0Text->Text = IF1->MDVGetFileName(0) ? IF1->MDVGetFileName(0) : "< Empty >";
        MDV1Text->Text = IF1->MDVGetFileName(1) ? IF1->MDVGetFileName(1) : "< Empty >";
        MDV2Text->Text = IF1->MDVGetFileName(2) ? IF1->MDVGetFileName(2) : "< Empty >";
        MDV3Text->Text = IF1->MDVGetFileName(3) ? IF1->MDVGetFileName(3) : "< Empty >";
        MDV4Text->Text = IF1->MDVGetFileName(4) ? IF1->MDVGetFileName(4) : "< Empty >";
        MDV5Text->Text = IF1->MDVGetFileName(5) ? IF1->MDVGetFileName(5) : "< Empty >";
        MDV6Text->Text = IF1->MDVGetFileName(6) ? IF1->MDVGetFileName(6) : "< Empty >";
        MDV7Text->Text = IF1->MDVGetFileName(7) ? IF1->MDVGetFileName(7) : "< Empty >";

        MDV0Text->SelStart = MDV0Text->Text.Length() - 1; MDV0Text->SelLength = 0;
        MDV1Text->SelStart = MDV1Text->Text.Length() - 1; MDV1Text->SelLength = 0;
        MDV2Text->SelStart = MDV2Text->Text.Length() - 1; MDV2Text->SelLength = 0;
        MDV3Text->SelStart = MDV3Text->Text.Length() - 1; MDV3Text->SelLength = 0;
        MDV4Text->SelStart = MDV4Text->Text.Length() - 1; MDV4Text->SelLength = 0;
        MDV5Text->SelStart = MDV5Text->Text.Length() - 1; MDV5Text->SelLength = 0;
        MDV6Text->SelStart = MDV6Text->Text.Length() - 1; MDV6Text->SelLength = 0;
        MDV7Text->SelStart = MDV7Text->Text.Length() - 1; MDV7Text->SelLength = 0;

        MDV0Label->Visible    = (IF1->MDVNoDrives > 0);
        MDV0Text->Visible     = (IF1->MDVNoDrives > 0);
        MDV0FSBtn->Visible    = (IF1->MDVNoDrives > 0);
        MDV0EjectBtn->Visible = (IF1->MDVNoDrives > 0);

        MDV1Label->Visible    = (IF1->MDVNoDrives > 1);
        MDV1Text->Visible     = (IF1->MDVNoDrives > 1);
        MDV1FSBtn->Visible    = (IF1->MDVNoDrives > 1);
        MDV1EjectBtn->Visible = (IF1->MDVNoDrives > 1);

        MDV2Label->Visible    = (IF1->MDVNoDrives > 2);
        MDV2Text->Visible     = (IF1->MDVNoDrives > 2);
        MDV2FSBtn->Visible    = (IF1->MDVNoDrives > 2);
        MDV2EjectBtn->Visible = (IF1->MDVNoDrives > 2);

        MDV3Label->Visible    = (IF1->MDVNoDrives > 3);
        MDV3Text->Visible     = (IF1->MDVNoDrives > 3);
        MDV3FSBtn->Visible    = (IF1->MDVNoDrives > 3);
        MDV3EjectBtn->Visible = (IF1->MDVNoDrives > 3);

        MDV4Label->Visible    = (IF1->MDVNoDrives > 4);
        MDV4Text->Visible     = (IF1->MDVNoDrives > 4);
        MDV4FSBtn->Visible    = (IF1->MDVNoDrives > 4);
        MDV4EjectBtn->Visible = (IF1->MDVNoDrives > 4);

        MDV5Label->Visible    = (IF1->MDVNoDrives > 5);
        MDV5Text->Visible     = (IF1->MDVNoDrives > 5);
        MDV5FSBtn->Visible    = (IF1->MDVNoDrives > 5);
        MDV5EjectBtn->Visible = (IF1->MDVNoDrives > 5);

        MDV6Label->Visible    = (IF1->MDVNoDrives > 6);
        MDV6Text->Visible     = (IF1->MDVNoDrives > 6);
        MDV6FSBtn->Visible    = (IF1->MDVNoDrives > 6);
        MDV6EjectBtn->Visible = (IF1->MDVNoDrives > 6);

        MDV7Label->Visible    = (IF1->MDVNoDrives > 7);
        MDV7Text->Visible     = (IF1->MDVNoDrives > 7);
        MDV7FSBtn->Visible    = (IF1->MDVNoDrives > 7);
        MDV7EjectBtn->Visible = (IF1->MDVNoDrives > 7);

        MicrodriveGroup->Visible = (spectrum.floppytype == FLOPPYIF1 && IF1->MDVNoDrives > 0);
        MicrodriveGroup->Height  = MDV0Text->Top + ((MDV1Text->Top - MDV0Text->Top) * IF1->MDVNoDrives) + 8;

        btnNewFloppyDisk->Caption = "New Microdrive Cartridge...";
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->DiskDrives1->Checked=false;
        if (Height<80) Form1->DiskDrives1->Enabled=false;
        else Form1->DiskDrives1->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0FSBtnClick(TObject *Sender)
{
        int c,h,s;
        unsigned long size;
        AnsiString Filename, Ext;

        if (DragFileName!="")
        {
                Filename=DragFileName;
        }
        else
        {
                if (!Sender && (HD0List->ItemIndex!=0))
                {
                        Filename=PhysDrives[HD0List->ItemIndex-1].Path;
                }
                else
                {
                        HD0List->ItemIndex=0;
                        if (ATA_GetHDF(0)) OpenDialogHardDriveImage->FileName=ATA_GetHDF(0);

                        if (OpenDialogHardDriveImage->FileName.Length() == 0 || *(OpenDialogHardDriveImage->FileName.AnsiLastChar()) == '\\')
                        {
                                OpenDialogHardDriveImage->FileName = "";
                        }

                        if (!OpenDialogHardDriveImage->Execute()) return;

                        Filename=OpenDialogHardDriveImage->FileName;
                        Ext=FileNameGetExt(Filename);

                        if (Ext!=".HDF" && Ext!=".VHD") Filename += ".hdf";
                        Ext=FileNameGetExt(Filename);

                        if (access(Filename.c_str(), 0) && Ext==".HDF")
                        {
                                char Message[256];
                                int ret;

                                strcpy(Message,Filename.c_str());
                                strcat(Message," Does not exist.\nWould you like to create it?");

                                ret=Application->MessageBox(Message, "File does not exist", MB_OKCANCEL | MB_ICONWARNING);

                                if (ret!=IDOK) return;

                                CreateHDF->FileName=Filename;
                                CreateHDF->ShowModal();
                        }
                        if (access(Filename.c_str(), 0)) return;
                }
        }

        HD0Text->Text = Filename;
        HD0Text->SelStart=HD0Text->Text.Length()-1; HD0Text->SelLength=0;
        if (ATA_LoadHDF(0,Filename.c_str()))
        {
                HD0EjectBtnClick(NULL);
                return;
        }
        ATA_GetCHS(0, &c, &h, &s, &size);
        HD0ReadOnly->Checked=ATA_GetReadOnly(0);

        HD0C->Text=c;
        HD0H->Text=h; HD0HUD->Position=(short)h; HD0HUD->Min=1; HD0HUD->Max=15;
        HD0S->Text=s; HD0SUD->Position=(short)s; HD0SUD->Min=1; HD0SUD->Max=255;
        if (HD0Text->Text[1]=='\\' && HD0Text->Text[2]=='\\')
        {
                HD0CHS->Visible=true;
                HD0C->Visible=true;
                HD0H->Visible=true;
                HD0S->Visible=true;
                HD0HUD->Visible=true;
                HD0SUD->Visible=true;
        }
        else
        {
                HD0CHS->Visible=false;
                HD0C->Visible=false;
                HD0H->Visible=false;
                HD0S->Visible=false;
                HD0HUD->Visible=false;
                HD0SUD->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1FSBtnClick(TObject *Sender)
{
        int c,h,s;
        unsigned long size;
        AnsiString Filename, Ext;

        if (!Sender && (HD1List->ItemIndex!=0))
        {
                Filename=PhysDrives[HD1List->ItemIndex-1].Path;
        }
        else
        {
                HD1List->ItemIndex=0;
                if (ATA_GetHDF(1)) OpenDialogHardDriveImage->FileName=ATA_GetHDF(1);

                if (OpenDialogHardDriveImage->FileName.Length() == 0 || *(OpenDialogHardDriveImage->FileName.AnsiLastChar()) == '\\')
                {
                        OpenDialogHardDriveImage->FileName = "";
                }

                if (!OpenDialogHardDriveImage->Execute()) return;

                Filename=OpenDialogHardDriveImage->FileName;
                Ext=FileNameGetExt(Filename);

                if (Ext!=".HDF") Filename += ".hdf";

                if (access(Filename.c_str(), 0))
                {
                        char Message[256];
                        int ret;

                        strcpy(Message,Filename.c_str());
                        strcat(Message," Does not exist.\nWould you like to create it?");

                        ret=Application->MessageBox(Message, "File does not exist", MB_OKCANCEL | MB_ICONWARNING);

                        if (ret!=IDOK) return;

                        CreateHDF->FileName=Filename;
                        CreateHDF->ShowModal();
                        if (access(Filename.c_str(), 0)) return;
                }
        }


        HD1Text->Text = Filename;
        HD1Text->SelStart=HD1Text->Text.Length()-1; HD1Text->SelLength=0;
        if (ATA_LoadHDF(1,Filename.c_str()))
        {
                HD1EjectBtnClick(NULL);
                return;
        }

        ATA_GetCHS(1, &c, &h, &s, &size);
        HD1ReadOnly->Checked=ATA_GetReadOnly(1);
        HD1C->Text=c;
        HD1H->Text=h; HD1HUD->Position=(short)h; HD1HUD->Min=1; HD1HUD->Max=15;
        HD1S->Text=s; HD1SUD->Position=(short)s; HD1SUD->Min=1; HD1SUD->Max=255;
        if (HD1Text->Text[1]=='\\' && HD1Text->Text[2]=='\\')
        {
                HD1CHS->Visible=true; HD1C->Visible=true; HD1H->Visible=true;
                HD1S->Visible=true; HD1HUD->Visible=true; HD1SUD->Visible=true;
        }
        else
        {
                HD1CHS->Visible=false; HD1C->Visible=false; HD1H->Visible=false;
                HD1S->Visible=false; HD1HUD->Visible=false; HD1SUD->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0EjectBtnClick(TObject *Sender)
{
        HD0Text->Text = "< Empty >";
        ATA_EjectHDF(0);
        HD0List->ItemIndex=0;
        HD0CHS->Visible=false; HD0C->Visible=false; HD0H->Visible=false;
        HD0S->Visible=false; HD0HUD->Visible=false; HD0SUD->Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1EjectBtnClick(TObject *Sender)
{
        HD1Text->Text = "< Empty >";
        ATA_EjectHDF(1);
        HD1List->ItemIndex=0;
        HD1CHS->Visible=false; HD1C->Visible=false; HD1H->Visible=false;
        HD1S->Visible=false; HD1HUD->Visible=false; HD1SUD->Visible=false;
}
//---------------------------------------------------------------------------

int TP3Drive::GetMDVNo(TObject *Sender)
{
        if (Sender==MDV0Text || Sender==MDV0FSBtn || Sender==MDV0EjectBtn) return(0);
        if (Sender==MDV1Text || Sender==MDV1FSBtn || Sender==MDV1EjectBtn) return(1);
        if (Sender==MDV2Text || Sender==MDV2FSBtn || Sender==MDV2EjectBtn) return(2);
        if (Sender==MDV3Text || Sender==MDV3FSBtn || Sender==MDV3EjectBtn) return(3);
        if (Sender==MDV4Text || Sender==MDV4FSBtn || Sender==MDV4EjectBtn) return(4);
        if (Sender==MDV5Text || Sender==MDV5FSBtn || Sender==MDV5EjectBtn) return(5);
        if (Sender==MDV6Text || Sender==MDV6FSBtn || Sender==MDV6EjectBtn) return(6);
        if (Sender==MDV7Text || Sender==MDV7FSBtn || Sender==MDV7EjectBtn) return(7);
        return(0);
}

TObject *TP3Drive::GetTextBox(int Drive)
{
        switch(Drive)
        {
        case 0: return(MDV0Text);
        case 1: return(MDV1Text);
        case 2: return(MDV2Text);
        case 3: return(MDV3Text);
        case 4: return(MDV4Text);
        case 5: return(MDV5Text);
        case 6: return(MDV6Text);
        case 7: return(MDV7Text);
        default: return(MDV0Text);
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDV0EjectBtnClick(TObject *Sender)
{
        int Drive = GetMDVNo(Sender);
        IF1->MDVSetFileName(Drive,NULL);
        ((TEdit *)GetTextBox(Drive))->Text="< Empty >";

}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDV0FSBtnClick(TObject *Sender)
{
        int Drive = GetMDVNo(Sender);
        AnsiString FileName;
        TEdit *Text;

        Text=(TEdit *)GetTextBox(Drive);

        if (Sender)
        {
                ConfigureOpenFloppyDiskImageDialog();

                FileName = Text->Text;
                if (FileName!="< Empty >")
                        OpenDialogFloppyDiskImage->FileName = FileName;

                if (!OpenDialogFloppyDiskImage->Execute()) return;
                FileName=OpenDialogFloppyDiskImage->FileName;
        }
        else    FileName=DragFileName;

        IF1->MDVSetFileName(Drive,FileName.c_str());
        Text->Text=FileName;
        Text->SelStart=Text->Text.Length()-1; Text->SelLength=0;
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
        if (HD0List->ItemIndex>0) HD0FSBtnClick(NULL);
        else HD0EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ListChange(TObject *Sender)
{
        if (HD1List->ItemIndex>0) HD1FSBtnClick(NULL);
        else HD1EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TP3Drive::HD0HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(0, &c, &h, &s, &size);

        h=HD0HUD->Position;
        s=HD0SUD->Position;
        c=size/(h*s); if (c>65535) c=65535;

        HD0H->Text=h; HD0HUD->Position=(short)h; HD0HUD->Min=1; HD0HUD->Max=15;
        HD0S->Text=s; HD0SUD->Position=(short)s; HD0SUD->Min=1; HD0SUD->Max=255;
        HD0C->Text=c;

        ATA_SetCHS(0, c, h, s);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(1, &c, &h, &s, &size);

        h=HD0HUD->Position;
        s=HD0SUD->Position;
        c=size/(h*s); if (c>65535) c=65535;

        HD1H->Text=h; HD1HUD->Position=(short)h; HD1HUD->Min=1; HD1HUD->Max=15;
        HD1S->Text=s; HD1SUD->Position=(short)s; HD1SUD->Min=1; HD1SUD->Max=255;
        HD1C->Text=c;

        ATA_SetCHS(1, c, h, s);

}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(0,HD0ReadOnly->Checked);
        HD0ReadOnly->Checked=ATA_GetReadOnly(0);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(1,HD1ReadOnly->Checked);
        HD1ReadOnly->Checked=ATA_GetReadOnly(1);
}
//---------------------------------------------------------------------------

void TP3Drive::InsertFile(AnsiString Filename)
{
        AnsiString Ext;

        DragFileName=Filename;

        Ext = GetExt(Filename);

        if (Ext==".MDR" || Ext==".MDV") MDV0FSBtnClick(NULL);
        else if (Ext==".HDF") HD0FSBtnClick(NULL);
        else if (Ext==".DSK" || Ext==".MGT" || Ext==".IMG"
                  || Ext==".OPD" || Ext==".OPU" || Ext==".TRD" || Ext==".LAR") DriveAFSBtnClick(NULL);

        DragFileName="";
}
//---------------------------------------------------------------------------

void P3DriveMachineHasInitialised(void)
{
        if (P3Drive->DriveAText->Text != "< Empty >")
                floppy_setimage(0, P3Drive->DriveAText->Text.c_str(),0);

        if (P3Drive->DriveBText->Text != "< Empty >")
                floppy_setimage(1, P3Drive->DriveBText->Text.c_str(),0);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::btnNewFloppyDiskClick(TObject *Sender)
{
        btnNewFloppyDisk->Enabled = false;

        switch (spectrum.floppytype)
        {
        case FLOPPYIF1:
                NewMicrodriveCartridge();
                break;

        case FLOPPYPLUS3:
        case FLOPPYPLUSD:
        case FLOPPYDISCIPLE:
        case FLOPPYBETA:
        case FLOPPYOPUSD:
        case FLOPPYLARKEN81:
        case FLOPPYZX1541:
        default:
                break;
        }

        btnNewFloppyDisk->Enabled = true;
}
//---------------------------------------------------------------------------

void TP3Drive::NewMicrodriveCartridge()
{
        SaveDialogNewFloppyDisk-> Title = "Create New Microdrive Cartridge";
        SaveDialogNewFloppyDisk->Filter = "Microdrive Cartridge (*.mdr)|*.mdr";
        SaveDialogNewFloppyDisk->DefaultExt = ".mdr";

        if (SaveDialogNewFloppyDisk->Execute())
        {
                AnsiString FileName = SaveDialogNewFloppyDisk->FileName;
                bool success = true;

                FILE* f = fopen(FileName.c_str(), "wb");
                if (f)
                {
                        try
                        {
                                for (int i = 0; i < 137922; i++)
                                {
                        	        fputc(0xFC, f);
                                }
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

                if (!success)
                {
                        ShowMessage("Failed to create Microdrive Cartridge");
                }
        }
}

