/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

 //---------------------------------------------------------------------------

#define NO_WIN32_LEAN_AND_MEAN

#include <vcl4.h>
#include <io.h>
#include <dirent.h>
#include <ctype.h>
#include <dir.h>
#include <stdio.h>
#include <algorithm>
#include <sys/stat.h>

#pragma hdrstop

#include "zx81.h"
#include "z80thread.h"
#include "kbstatus.h"
#include "snap.h"
#include "zx81config.h"
#include "FullScreen.h"
#include "main_.h"
#include "accdraw_.h"
#include "About_.h"
#include "keyboard_.h"
#include "speed_.h"
#include "kb_.h"
#include "zxprinter.h"
#include "wavload_.h"
#include "debug.h"
#include "utils.h"
#include "artifacts_.h"
#include "sound\SoundOP.h"
#include "memsave_.h"
#include "zx97config.h"
#include "SerialPort.h"
#include "tzxman.h"
#include "HW_.h"
#include "interface1.h"
#include "Plus3Drives.h"
#include "ide.h"
#include "z80.h"
#include "parallel.h"
#include "sound\SoundForm.h"
#include "ZipFile_.h"
#include "symbolstore.h"
#include "SymBrowse.h"
#include "Spectra\Spectra.h"
#include "Chroma\Chroma.h"
#include "MemoryWindow.h"
#include "Hist.h"
#include "MakeHDF.h"
#include "LiveMemoryWindow_.h"
#include "ProfilePlot_.h"
#include "Profiler.h"
#include "BasicLister\BasicLister_.h"
#include "BasicLister\BasicListerOptions_.h"
#include "BasicLoader\BasicLoaderOptions_.h"
#include "ROMCartridge\IF2ROMCartridge.h"
#include "sound\sound.h"
#include "sp0256drv.h"
#include "Digitalkdrv.h"
#if __CODEGEARC__ >= 0x0620
#include <System.IOUtils.hpp>
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define ZXDB(msg) Application->MessageBox(msg, "Debug", MB_OK);

TForm1 *Form1;
ULONG nID;

extern PACKAGE TMouse* Mouse;
extern void SpecStartUp(void);
extern BYTE spec48_getbyte(int Address);
extern void spec48_LoadRZX(char *FileName);
extern int AccurateDraw(SCANLINE *Line);
extern void loadFileSymbolsProxy(const char*);

extern bool ShowSplash;
extern int frametstates;

extern "C" void z80_reset();
extern "C" int z80_nmi();
extern char **CommandLine;
extern void spec_load_z80(char *fname);
extern void spec_load_sna(char *fname);
extern void spec_save_z80(char *fname);
extern void spec_save_sna(char *fname);
extern "C" void rzx_close(void);
extern bool Restart;

int VKRSHIFT=VK_RSHIFT, VKLSHIFT=VK_LSHIFT;

int AutoLoadCount=0;

SCANLINE *BuildLine, Video;

static bool iniFileExists = false;

const int bufferLength = 255;
char webBuffer[bufferLength];

//---------------------------------------------------------------------------

void __fastcall TForm1::WndProc(TMessage &Message)
{
        switch(Message.Msg)
        {
        case WM_USER:
                if (RunFrameEnable)
                        Form1->RunFrame();
                break;

        default:
                break;
        }
	TForm::WndProc(Message);
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        AnsiString IniPath;
        char path[256];
        int i;

        RunFrameEnable=false;

        strcpy(emulator.cwd, (FileNameGetPath(Application->ExeName)).c_str());
        if (emulator.cwd[strlen(emulator.cwd)-1]!='\\')
        {
                emulator.cwd[strlen(emulator.cwd)-1]='\\';
                emulator.cwd[strlen(emulator.cwd)]='\0';
        }

        GetTempPath(256, emulator.temppath);
        strcat(emulator.temppath, temporaryFolder);
        mkdir(emulator.temppath);

        if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path))
        {
                IniPath=path;
                if (IniPath[IniPath.Length()] != '\\') IniPath += "\\";
                IniPath += iniFolder;
                mkdir(IniPath.c_str());
                strcpy(emulator.configpath, IniPath.c_str());

                IniPath += FileNameGetFname(Application->ExeName);
                IniPath += ".ini";
                strcpy(emulator.inipath, IniPath.c_str());
        }
        else
        {
                IniPath=ChangeFileExt(Application->ExeName, ".ini" );
                strcpy(emulator.inipath, IniPath.c_str());
                IniPath=FileNameGetPath(Application->ExeName);
                strcpy(emulator.configpath, IniPath.c_str());
        }

        for(i=0; CommandLine[i]!=NULL; i++)
        {
                if (FileNameGetExt(CommandLine[i]) == ".INI")
                {
                        IniPath=CommandLine[i];
                        if (IniPath.Pos("\\")==0)
                                IniPath=emulator.configpath + IniPath;
                        strcpy(emulator.inipath, IniPath.c_str());

                }
        }

        FullScreen=false;
        startup=true;
        RenderMode=RENDERGDI;
        Application->OnDeactivate=FormDeactivate;
        DrivesChanged=false;

        BuildLine=&Video;

        SpecStartUp();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        TIniFile *ini;

        RunFrameEnable=false;

        Application->OnMessage = AppMessage;

        ATA_Init();
        load_config();
        PCKbInit();
        Digitalker.Init(emulator.ROMSPEECHPATH);

        Application->OnDeactivate=FormDeactivate;

        ini = new TIniFile(emulator.inipath);
        ShowSplash = ini->ReadBool("MAIN","ShowSplash", ShowSplash);
        EnableSplashScreen->Checked = ShowSplash;

        iniFileExists = FileExists(emulator.inipath);
        LoadSettings(ini);

        RenderMode=ini->ReadInteger("MAIN","RenderMode", RENDERGDI);

        if (!RenderInit())
        {
                RenderEnd();
                exit(0);
        }

        delete ini;

        Timer2->Interval=1000;

        LEDGreenOn = new Graphics::TBitmap;
        LEDGreenOff = new Graphics::TBitmap;
        LEDRedOn = new Graphics::TBitmap;
        LEDRedOff = new Graphics::TBitmap;
        ImageList1->GetBitmap(0,LEDGreenOff);
        ImageList1->GetBitmap(1,LEDGreenOn);
        ImageList1->GetBitmap(2,LEDRedOff);
        ImageList1->GetBitmap(3,LEDRedOn);

        if (!access("eightyone.chm",0)) HelpTopics2->Enabled=true;
        else HelpTopics2->Enabled=false;

        BuildConfigMenu();
        BuildDocumentationMenu();
        BuildZX81ExamplesMenu();
        BuildSpectrumExamplesMenu();

        if (Sound.Initialise(Form1->Handle, machine.fps, 0, 0, 0)) MessageBox(NULL, "", "Sound Error", 0);

        if (emulator.checkInstallationPathLength && strlen(emulator.cwd) >= 180)
        {
                int ret = MessageBox(NULL, "The location of the EightyOne folder may cause selection of alternate ROMs\nor ROM cartridges to exceed to maximum path length supported by Windows.\n\nCheck and show this warning next time EightyOne is started?", "Warning", MB_YESNO |MB_ICONWARNING);
                if (ret == IDNO)
                {
                        emulator.checkInstallationPathLength = 0;
                }
        }
}

//---------------------------------------------------------------------------

void TForm1::GatherWindowsIfRequired()
{
        if (!iniFileExists)
        {
                Form1->GatherWindows1Click(this);
        }
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
        extern void RecalcWinSize(void);

        RecalcWinSize();

        StatusBar1->Panels->Items[3]->Width =
                StatusBar1->ClientWidth -
                        (StatusBar1->Panels->Items[0]->Width
                         +StatusBar1->Panels->Items[1]->Width
                         +StatusBar1->Panels->Items[2]->Width);

        StatusBar1->Refresh();
        StatusBar1->Invalidate();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        if (IsKeyPressed(VK_ESCAPE))
        {
                Key = NULL;
                return;
        }

        if ((Key==VK_SHIFT) && emulator.UseRShift) return;
        if (Key==VK_LSHIFT) Key=VK_SHIFT;
        if (Key==VK_RSHIFT) Key=VK_CONTROL;
        PCKeyDown(Key);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        if ((Key==VK_SHIFT) && emulator.UseRShift) return;
        if (Key==VK_LSHIFT) Key=VK_SHIFT;
        if (Key==VK_RSHIFT) Key=VK_CONTROL;
        PCKeyUp(Key);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AboutEightyOne1Click(TObject *Sender)
{
        PCAllKeysUp();
        About->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::KeyboardMap1Click(TObject *Sender)
{
        if (!KeyboardMap1->Checked)
        {
                PCAllKeysUp();
                Keyboard->Show();
                Keyboard->KbChange();
                KeyboardMap1->Checked=true;
        }
        else
        {
                Keyboard->Close();
                KeyboardMap1->Checked=false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N1001Click(TObject *Sender)
{
        N1001->Checked=true;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        StatusBar1->SizeGrip = false;
        BorderStyle = bsSingle;
        TBorderIcons newBorderIcons = BorderIcons;
        newBorderIcons >> biMinimize;
        newBorderIcons >> biMaximize;
        BorderIcons = newBorderIcons;

        ClientWidth=BaseWidth;
        ClientHeight=BaseHeight;
        if (StatusBar1->Visible)
        {
                ClientHeight += StatusBar1->Height;
                StatusBar1->Refresh();
                StatusBar1->Invalidate();
        }
}
//-----------------------------------------------------------------------

void __fastcall TForm1::N2001Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=true;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        StatusBar1->SizeGrip = false;
        BorderStyle = bsSingle;
        TBorderIcons newBorderIcons = BorderIcons;
        newBorderIcons >> biMinimize;
        newBorderIcons >> biMaximize;
        BorderIcons = newBorderIcons;

        ClientWidth=BaseWidth*2;
        ClientHeight=BaseHeight*2;
        if (StatusBar1->Visible)
        {
                ClientHeight += StatusBar1->Height;
                StatusBar1->Refresh();
                StatusBar1->Invalidate();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N4001Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=true;
        UserDefined1->Checked=false;

        StatusBar1->SizeGrip = false;
        BorderStyle = bsSingle;
        TBorderIcons newBorderIcons = BorderIcons;
        newBorderIcons >> biMinimize;
        newBorderIcons >> biMaximize;
        BorderIcons = newBorderIcons;

        ClientWidth=BaseWidth*4;
        ClientHeight=BaseHeight*4;
        if (StatusBar1->Visible)
        {
                ClientHeight += StatusBar1->Height;
                StatusBar1->Refresh();
                StatusBar1->Invalidate();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UserDefined1Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=true;

        int baseWidth=ClientWidth;
        int baseHeight=ClientHeight;
        if (StatusBar1->Visible)
        {
                baseHeight -= StatusBar1->Height;
        }

        StatusBar1->SizeGrip = true;
        TBorderIcons newBorderIcons = BorderIcons;
        newBorderIcons << biMinimize;
        newBorderIcons << biMaximize;
        BorderIcons = newBorderIcons;
        BorderStyle = bsSizeable;

        ClientWidth=baseWidth;
        ClientHeight=baseHeight;
        if (StatusBar1->Visible)
        {
                ClientHeight += StatusBar1->Height;
                StatusBar1->Refresh();
                StatusBar1->Invalidate();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Hardware1Click(TObject *Sender)
{
        if (!Hardware1->Checked)
        {
                PCAllKeysUp();
                HW->Show();
                Hardware1->Checked=true;
        }
        else if (!HW->Apply->Enabled)
        {
                HW->Close();
                Hardware1->Checked=false;
        }
        else ShowMessage("Changes pending on Hardware Dialog");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Speed1Click(TObject *Sender)
{
        if (Speed1->Checked)
        {
                Speed->Close();
                Speed1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                Speed->Show();
                Speed1->Checked=true;
        }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Keyboard1Click(TObject *Sender)
{
        PCAllKeysUp();
        if (Keyboard1->Checked)
        {
                Kb->Close();
                Keyboard1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                Kb->Show();
                Keyboard1->Checked=true;
        }
        //Kb->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InsertTape1Click(TObject *Sender)
{
        AnsiString Extension, Filename;
        int stopped;

        stopped=emulation_stop;
        emulation_stop=true;

        PCAllKeysUp();
        if (!OpenTape1->Execute())
        {
                emulation_stop=stopped;
                return;
        }

        Filename=OpenTape1->FileName;
        Extension=FileNameGetExt(Filename);

        if (Extension == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenTape1->Filter);
                if (Filename=="") return;
                Extension = FileNameGetExt(Filename);
        }

        if (Extension == ".WAV" || Extension == ".CSW")
        {
                        WavLoad->LoadWavDialog->FileName = Filename;
                        WavLoad->LoadFile(Filename);
                        InWaveLoaderClick(NULL);
                        OutWaveLoaderClick(NULL);
        }

        if (Extension == ".TZX"
                || Extension == ".T81"
                || Extension == ".TAP"
                || Extension == ".P"
                || Extension == ".81"
                || Extension == ".P81"
                || Extension == ".80"
                || Extension == ".O"
                || Extension == ".A83"
                || Extension == ".B80"
                || Extension == ".B81"
                || Extension == ".B82"
                || Extension == ".TXT"
                || Extension == ".BAS")
        {
                        TZX->StopBtnClick(NULL);
                        TZX->OpenDialog->FileName = Filename;
                        TZX->LoadFile(Filename, false);
                        TZX->UpdateTable(true);
                        InTZXManagerClick(NULL);
                        OutTZXManagerClick(NULL);

                        loadFileSymbolsProxy(Filename.c_str());
        }

        emulation_stop=stopped;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveSnapshot1Click(TObject *Sender)
{
        AnsiString Path, Ext;
        int stopped;
        stopped=emulation_stop;
        emulation_stop=1;

        if (emulator.machine==MACHINEACE)
        {
                SaveSnapDialog->Filter = ".ACE Snapshot|*.ace";
                SaveSnapDialog->DefaultExt = "ACE";
        }
        else if (emulator.machine==MACHINESPECTRUM)
        {
                SaveSnapDialog->Filter = ".Z80 Snapshot|*.z80|.SNA Snapshot|*.sna";
                SaveSnapDialog->DefaultExt = "Z80";
        }
        else
        {
                SaveSnapDialog->Filter = ".Z81 Snapshot|*.z81";
                SaveSnapDialog->DefaultExt = "Z81";
        }

        if (!SaveSnapDialog->Execute())
        {
                emulation_stop=stopped;
                return;
        }
        Path=SaveSnapDialog->FileName;
        Ext = FileNameGetExt(Path);

        if ((Ext == ".Z81") || (Ext == ".ACE")) save_snap(Path.c_str());
        if (Ext == ".Z80") spec_save_z80(Path.c_str());
        if (Ext == ".SNA") spec_save_sna(Path.c_str());

        emulation_stop=stopped;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadSnapshot1Click(TObject *Sender)
{
        int stopped;
        AnsiString Path, Ext;
        stopped=emulation_stop;

        if (emulator.machine==MACHINEACE)
        {
                LoadSnapDialog->Filter = ".ACE Snapshot|*.ace|Compressed Snapshot|*.zip|All Snapshot Files|*.ace;*.sna;*.z80;*.z81;*.zip";
                LoadSnapDialog->DefaultExt = "ACE";
        }
        else if (emulator.machine==MACHINESPECTRUM)
        {
                LoadSnapDialog->Filter = "Spectrum Snapshots|*.z80;*.sna|.Z80 Snapshot|*.z80|.SNA Snapshot|*.sna|Compressed Snapshot|*.zip|All Snapshot Files|*.ace;*.sna;*.z80;*.z81;*.zip";
                LoadSnapDialog->DefaultExt = "Z80";
        }
        else
        {
                LoadSnapDialog->Filter = ".Z81 Snapshot|*.z81|Compressed Snapshot|*.zip|All Snapshot Files|*.ace;*.sna;*.z80;*.z81;*.zip";
                LoadSnapDialog->DefaultExt = "Z81";
        }

        if (!LoadSnapDialog->Execute()) return;

        Path=LoadSnapDialog->FileName;
        Ext = FileNameGetExt(Path);

        if (Ext == ".ZIP")
        {
                Path=ZipFile->ExpandZIP(Path, LoadSnapDialog->Filter);
                if (Path=="") return;
                Ext = FileNameGetExt(Path);
        }

        emulation_stop=1;
        Sound.InitDevices();

        if (BasicLister->ListerAvailable())
        {
                BasicLister->Clear();
        }

        if (HistoryBox->Visible)
        {
                HistoryBox->ToolButtonClearClick(NULL);
        }

        if ((Ext == ".Z81") || (Ext == ".ACE")) load_snap(Path.c_str());
        if (Ext == ".Z80") spec_load_z80(Path.c_str());
        if (Ext == ".SNA") spec_load_sna(Path.c_str());
        emulation_stop=stopped;

        Dbg->UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CloseTape1Click(TObject *Sender)
{
        PCAllKeysUp();

        if (OutTZXManager->Checked) TZX->Save1Click(Sender);
        if (OutWaveLoader->Checked) WavLoad->SaveWav1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NewTape1Click(TObject *Sender)
{
        PCAllKeysUp();

        if (OutTZXManager->Checked) TZX->NewTZXClick(Sender);
        if (OutWaveLoader->Checked) WavLoad->NewWav1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetButtonClick(TObject *Sender)
{
        emulation_stop=1;
        machine.reset();
        emulation_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetZX811Click(TObject *Sender)
{
        int initialStopState = emulation_stop;

        rzx_close();
        PCAllKeysUp();
        emulation_stop=1;
        z80_reset();
        if (machine.aytype==AY_TYPE_SINCLAIR) Sound.AYReset();
        InitialiseChroma();
        DisableSpectra();
        Dbg->ClearSkipAddresses();
        if (machine.reset) machine.reset();
        emulation_stop=initialStopState;
        DebugUpdate();
        if (BasicLister->ListerAvailable())
        {
                BasicLister->Clear();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        TIniFile *ini;
        DIR *dir;
        AnsiString TempFile;

        struct dirent *ent;

        Dbg->DisableMemoryWindowAutoUpdates();

        char escKey = VK_ESCAPE;
        if (FullScreen) FormKeyPress(NULL, escKey);
        
        if (!Restart)
        {
                ini = new TIniFile(emulator.inipath);
                SaveSettings(ini);
                delete ini;
        }

        if (machine.exit) machine.exit();

        emulation_stop=true;
        RunFrameEnable=false;

        PCAllKeysUp();

        Sound.End();

        RenderEnd();

        if ((dir = opendir(emulator.temppath)) != NULL)
        {
                while ((ent = readdir(dir)) != NULL)
                {
                        TempFile = emulator.temppath;
                        TempFile += ent->d_name;
                        DeleteFile(TempFile);
                }

                closedir(dir);
                _rmdir(emulator.temppath);
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewPrinterClick(TObject *Sender)
{
        ViewPrinter->Checked = !ViewPrinter->Checked;

        if (ViewPrinter->Checked)
        {
                PCAllKeysUp();
                ZXPrinter->Show();
        }
        else    ZXPrinter->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
        static int startup=0;
        int targetfps;
        AnsiString Filename, Ext;
        int i=0;

        static HWND OldhWnd=NULL;

        if (Form1->Handle != OldhWnd)
        {
                OldhWnd=Form1->Handle;

                RenderEnd();
                RenderInit();
                AccurateInit(true);
        }


        if (startup<=6) startup++;

        switch(startup)
        {
        case 1:

                if (StartUpWidth==0 || StartUpHeight==0) N1001Click(NULL);
                else
                {
                       ClientWidth=StartUpWidth;
                       ClientHeight=StartUpHeight;
                }
                Kb->OKClick(NULL);
                break;

        case 2:
                startup++;
                while(CommandLine[i])
                {
                        Filename=CommandLine[i];

                        char escKey = VK_ESCAPE;
                        if (Filename.UpperCase()=="FULLSCREEN") FormKeyPress(NULL, escKey);

                        Ext = FileNameGetExt(Filename);

                        if (Ext == ".ZIP")
                        {
                                Filename=ZipFile->ExpandZIP(Filename, "*.wav;*.z81;*.ace;*.z80;*.sna;*.tzx;*.tap;*.t81;*.p;*.p81;*.o;*.a83;*.81;*.80;*.mdr;*.mdv;*.hdf;*.dsk;*.mgt;*.img;*.opd;*.opu;*.trd;*rzx;*.zip");
                                Ext = FileNameGetExt(Filename);
                        }

                        if (Ext==".WAV") WavLoad->LoadFile(Filename);
                        else if (Ext==".Z81" || Ext==".ACE") load_snap(Filename.c_str());
                        else if (Ext==".Z80") spec_load_z80(Filename.c_str());
                        else if (Ext==".SNA") spec_load_sna(Filename.c_str());
                        else if (Ext==".TZX" || Ext==".TAP" || Ext==".T81"
                                  || Ext==".P" || Ext==".O" || Ext==".A83"
                                  || Ext==".81" || Ext==".80" || Ext==".P81")
                        {
                                TZX->LoadFile(Filename, false);
                                TZX->UpdateTable(true);
                        }
                        else if (Ext==".MDR" || Ext==".MDV" || Ext==".HDF"
                                  || Ext==".DSK" || Ext==".MGT" || Ext==".IMG"
                                  || Ext==".OPD" || Ext==".OPU" || Ext==".TRD")
                                        P3Drive->InsertFile(Filename);
                        else if (Ext==".RZX")
                                spec48_LoadRZX(Filename.c_str());
                        i++;
                }

                break;
        default:
                break;
        }

        if (P3Drive->Height<80)
        {
                P3Drive->Close();
                DiskDrives1->Enabled=false;
        }

        if (DrivesChanged)
        {
                P3Drive->RedetectDrivesClick(NULL);
                DrivesChanged=false;
        }

        targetfps = machine.NTSC ? 60:50;
        if (((targetfps-1) == fps) || ((targetfps+1)==fps)) fps=targetfps;
        targetfps = (targetfps  * 9) / 10;
        if (fps > (targetfps+2) && emulator.frameskip>0) emulator.frameskip--;
        if (fps < targetfps && emulator.frameskip<10 && emulator.frameskip>=0) emulator.frameskip++;

        if (FullScreen)
        {
                if (!FScreen.Stretch) RenderDrawBorder();
                return;
        }

        AnsiString text="";

        if (emulation_stop)
        {
                text +="Paused";
        }
        else
        {
                if (emulator.single_step)
                        text +="Debug Mode";
                else
                {
                        text += fps;
                        text += "fps";
                        if (emulator.frameskip>0)
                        {
                                text += " FS ";
                                text += emulator.frameskip;
                        }
                }
        }

        AnsiString scanlinesInfo = "";
        if (emulator.scanlinesPerFrame > 0)
        {
                scanlinesInfo = "     ";
                scanlinesInfo += emulator.scanlinesPerFrame;
                scanlinesInfo += " Scanlines";
        }
        StatusBar1->Panels->Items[3]->Text = scanlinesInfo;
        
        StatusBar1->Panels->Items[1]->Text = text;
        fps=0;

        zx81.vsyncsound=Sound1->Checked;
        TZX->RecStopCheck();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::WavLoadBtnClick(TObject *Sender)
{
        if (WavLoadBtn->Checked)
        {
                WavLoad->Close();
                WavLoadBtn->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                WavLoad->Show();
                WavLoadBtn->Checked=true;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyPress(TObject *Sender, char& Key)
{
        extern void RecalcWinSize(void);
        if (Key == ' ') rzx_close();

        // CTRL + [ generates the same key code as ESC and so an additional check is made to see if the right control key is being pressed
        if (Key == VK_ESCAPE && !IsAsyncKeyPressed(VK_RCONTROL))
        {
                FullScreen = !FullScreen;

                if (FullScreen)
                {
                        SaveX=Left;
                        SaveY=Top;
                        SaveW=ClientWidth;
                        SaveH=ClientHeight;
                        Screen->Cursor = crNone;
                        RenderInit();
                        RecalcPalette();
                        RecalcWinSize();
                        AccurateInit(true);
                        Artifacts->TrackBarChange(NULL);
                        if (RenderMode==RENDERGDI)
                        {
                                DEVMODE Mode;
                                int i, retval;

#if __CODEGEARC__ >= 0x0620
                                TPoint p;
#else
                                POINT p={0,0};
#endif

                                SaveScrW = GetSystemMetrics(SM_CXSCREEN);
                                SaveScrH = GetSystemMetrics(SM_CYSCREEN);
                                SaveScrBpp = GetDeviceCaps(Form1->Canvas->Handle, BITSPIXEL)
                                                * GetDeviceCaps(Form1->Canvas->Handle, PLANES);

                                p=Form1->ClientToScreen(p);

                                i=0;
                                memset(&Mode, 0, sizeof(DEVMODE));
                                do
                                {
                                        retval=EnumDisplaySettings(NULL,i, &Mode);

                                        if ((unsigned short)Mode.dmPelsWidth == FScreen.Width
                                                && (unsigned short)Mode.dmPelsHeight == FScreen.Height
                                                && (unsigned short)Mode.dmBitsPerPel == FScreen.Bpp)
                                        {
                                                ChangeDisplaySettings(&Mode, CDS_FULLSCREEN);
                                                retval=0;
                                        }

                                        //r=Mode.dmDisplayFrequency;
                                        i++;
                                } while(retval);

                                SetWindowPos(Form1->Handle, HWND_TOPMOST,
                                                Left-p.x,Top-p.y,
                                                GetSystemMetrics(SM_CXSCREEN) + (Width-ClientWidth),
                                                GetSystemMetrics(SM_CYSCREEN)
                                                        + (Height-ClientHeight)
                                                        + StatusBar1->Height, 0x400);
                        }
                }
                else
                {
                        if (RenderMode==RENDERGDI)
                        {
                                ChangeDisplaySettings(NULL, 0);
                        }
                        RenderInit();
                        Screen->Cursor = crDefault;
                        AccurateInit(true);
                        ClientWidth=SaveW;
                        ClientHeight=SaveH;
                        Left=SaveX;
                        Top=SaveY;
                        SetWindowPos(Form1->Handle, HWND_NOTOPMOST,
                                        Left,Top,
                                        Width,
                                        Height, NULL);
                        RecalcPalette();
                        RecalcWinSize();
                        Artifacts->TrackBarChange(NULL);

                        //BorderStyle=bsSizeable;
                }

        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DebugWinClick(TObject *Sender)
{
        DebugWin->Checked = !DebugWin->Checked;
        if (DebugWin->Checked)
        {
                Dbg->Show();
        }
        else
        {
                Dbg->Close();
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AppMessage(TMsg &Msg, bool &Handled)
{
        WORD BufferLength=255;
        AnsiString Filename, Ext;
        WORD FileIndex;
        WORD QtyDroppedFiles;
        char pDroppedFilename[255];

        if (Msg.message == WM_DROPFILES)
        {
                QtyDroppedFiles = (WORD)DragQueryFile((HDROP)Msg.wParam, -1,
                                                pDroppedFilename, BufferLength);

                for(FileIndex=0; FileIndex<=(QtyDroppedFiles - 1); FileIndex++)
                {
                        DragQueryFile((HDROP)Msg.wParam, FileIndex, pDroppedFilename, BufferLength);

                        Filename = pDroppedFilename;
                        Ext = GetExt(Filename);

                        if (Ext == ".ZIP")
                        {
                                Filename=ZipFile->ExpandZIP(Filename, "*.wav;*.z81;*.ace;*.z80;*.sna;*.tzx;*.tap;*.t81;*.p;*.p81;*.o;*.81;*.80;*.a83;*.mdr;*.mdv;*.hdf;*.dsk;*.mgt;*.img;*.opd;*.opu;*.trd;*rzx;**.zip;*.b80;*.b81;*.b82;*bas;*.txt");
                                if (Filename=="") return;
                                Ext = FileNameGetExt(Filename);
                        }

                        if (Ext==".SYM")
                        {
                                symbolstore::loadSymFileSymbols(Filename.c_str());
                                SymbolBrowser->RefreshContent();
                                return;
                        }

                        if (Ext==".MAP")
                        {
                                symbolstore::loadZ88FileSymbols(Filename.c_str());
                                SymbolBrowser->RefreshContent();
                                return;
                        }

                        if (Ext==".WAV") WavLoad->LoadFile(Filename);
                        else if (Ext==".Z81" || Ext==".ACE") load_snap(Filename.c_str());
                        else if (Ext==".Z80") spec_load_z80(Filename.c_str());
                        else if (Ext==".SNA") spec_load_sna(Filename.c_str());
                        else if (Ext==".TZX" || Ext==".TAP" || Ext==".T81"
                                  || Ext==".P" || Ext==".O" || Ext==".A83"
                                  || Ext==".81" || Ext==".80" || Ext==".P81"
                                  || Ext==".B80" || Ext==".B81" || Ext==".B82" || Ext==".TXT" || Ext==".BAS")
                        {
                                TZX->LoadFile(Filename, false);
                                loadFileSymbolsProxy(Filename.c_str());
                                TZX->UpdateTable(true);
                        }
                        else if (Ext==".MDR" || Ext==".MDV" || Ext==".HDF"
                                  || Ext==".DSK" || Ext==".MGT" || Ext==".IMG"
                                  || Ext==".OPD" || Ext==".OPU" || Ext==".TRD")
                                        P3Drive->InsertFile(Filename);
                        else if (Ext==".RZX")
                                spec48_LoadRZX(Filename.c_str());

                }

                DragFinish((HDROP)Msg.wParam);
                Handled = true;
        }

//        if (Msg.message == WM_DEVICE)
//        {
//                DrivesChanged=true;
//                Handled = true;
//        }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::PauseZX81Click(TObject *Sender)
{
        emulation_stop = !emulation_stop;
        Dbg->UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InverseVideoClick(TObject *Sender)
{
        InverseVideo->Checked = !InverseVideo->Checked;
        emulator.inverse = (CFGBYTE)(1 - emulator.inverse);
        if (Sender) Artifacts->TrackBarChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDeactivate(TObject *Sender)
{
        char escKey = VK_ESCAPE;
        if (FullScreen) FormKeyPress(NULL, escKey);
}
//---------------------------------------------------------------------------

void TForm1::LoadSettings(TIniFile *ini)
{
        if (ini->ReadBool("MAIN", "InverseVideo", InverseVideo->Checked)) InverseVideoClick(NULL);

        emulator.checkInstallationPathLength = ini->ReadBool("MAIN", "CheckInstallationPathLength", 1);

        Keyboard1->Checked = ini->ReadBool("MAIN", "Keyboard1", Keyboard1->Checked);
        Hardware1->Checked = ini->ReadBool("MAIN", "Hardware1", Hardware1->Checked);
        Speed1->Checked    = ini->ReadBool("MAIN", "Speed1",    Speed1->Checked);
        Sound1->Checked    = ini->ReadBool("MAIN", "Sound1",    Sound1->Checked);

        DisplayArt->Checked=ini->ReadBool("MAIN","DisplayArt",DisplayArt->Checked);

        DebugWin->Checked          = ini->ReadBool("MAIN", "DebugWin",     DebugWin->Checked);
        ViewPrinter->Checked       = ini->ReadBool("MAIN", "ViewPrinter",  ViewPrinter->Checked);
        WavLoadBtn->Checked        = ini->ReadBool("MAIN", "WavLoadBtn",   WavLoadBtn->Checked);
        KeyboardMap1->Checked      = ini->ReadBool("MAIN", "KeyMap",       KeyboardMap1->Checked);
        TZXMan->Checked            = ini->ReadBool("MAIN", "TZXManager",   TZXMan->Checked);
        DiskDrives1->Checked       = ini->ReadBool("MAIN", "DiskDrives",   DiskDrives1->Checked);
        SoundOutput1->Checked      = ini->ReadBool("MAIN", "SoundOutput",  SoundOutput1->Checked);
        BasicListerOption->Checked = ini->ReadBool("MAIN", "BasicListing", BasicListerOption->Checked);

        InWaveLoader->Checked  = ini->ReadBool("MAIN", "InWave",   InWaveLoader->Checked);
        InTZXManager->Checked  = ini->ReadBool("MAIN", "InTZX",    InTZXManager->Checked);
        OutWaveLoader->Checked = ini->ReadBool("MAIN", "OutWave",  OutWaveLoader->Checked);
        OutTZXManager->Checked = ini->ReadBool("MAIN", "OutTZX",   OutTZXManager->Checked);
        OutAudioOut->Checked   = ini->ReadBool("MAIN", "OutAudio", OutAudioOut->Checked);

        None1->Checked      = ini->ReadBool("MAIN", "BorderNone",   None1->Checked);
        Small1->Checked     = ini->ReadBool("MAIN", "BorderSmall",  Small1->Checked);
        Normal1->Checked    = ini->ReadBool("MAIN", "BorderNormal", Normal1->Checked);
        Large1->Checked     = ini->ReadBool("MAIN", "BorderLarge",  Large1->Checked);
        FullImage1->Checked = ini->ReadBool("MAIN", "BorderFull",   FullImage1->Checked);
        StatusBar2->Checked = ini->ReadBool("MAIN", "StatusBar",    StatusBar2->Checked);

        OpenTape1->FileName    = ini->ReadString( "MAIN", "LoadFile",       OpenTape1->FileName);
        OpenTape1->FilterIndex = ini->ReadInteger("MAIN", "LoadFileFilter", OpenTape1->FilterIndex);

        SpectraColourEnable->Checked      = ini->ReadBool("MAIN", "SpectraColourEnable",      SpectraColourEnable->Checked);
        ChromaColourEnable->Checked       = ini->ReadBool("MAIN", "ChromaColourEnable",       ChromaColourEnable->Checked);
        ConnectSpectrum128Keypad->Checked = ini->ReadBool("MAIN", "ConnectSpectrum128Keypad", ConnectSpectrum128Keypad->Checked);
        ConnectJoystick1->Checked         = ini->ReadBool("MAIN", "ConnectJoystick1",         ConnectJoystick1->Checked);
        ConnectJoystick2->Checked         = ini->ReadBool("MAIN", "ConnectJoystick2",         ConnectJoystick2->Checked);
        EnableJoystick1AutoFire->Checked  = ini->ReadBool("MAIN", "EnableJoystick1AutoFire",  EnableJoystick1AutoFire->Checked);
        EnableJoystick2AutoFire->Checked  = ini->ReadBool("MAIN", "EnableJoystick2AutoFire",  EnableJoystick2AutoFire->Checked);

        zx81.z80AssemblerOn = (CFGBYTE)((ini->ReadBool("MAIN", "SwitchOnZ80Assembler", SwitchOnZ80Assembler->Checked)) ? 1 : 0);
        zx81.memocalcOn     = (CFGBYTE)((ini->ReadBool("MAIN", "SwitchOnMemocalc",     SwitchOnMemocalc->Checked))     ? 1 : 0);
//        zx81.memotextOn     = (CFGBYTE)((ini->ReadBool("MAIN", "SwitchOnMemotext",     SwitchOnMemotext->Checked))     ? 1 : 0);
        BuildMemotechInterfaceSelection();

        machine.joystick1Controller = (CFGBYTE)ini->ReadInteger("MAIN", "Joystick1Controller", -1);
        machine.joystick2Controller = (CFGBYTE)ini->ReadInteger("MAIN", "Joystick2Controller", -1);

        divIDEJumperEClosed->Checked    = ini->ReadBool("MAIN", "divIDEJumperEClosed",    divIDEJumperEClosed->Checked);
        ZXCFUploadJumperClosed->Checked = ini->ReadBool("MAIN", "ZXCFUploadJumperClosed", ZXCFUploadJumperClosed->Checked);
        SimpleIdeRomEnabled->Checked    = ini->ReadBool("MAIN", "SimpleIdeRomEnabled",    SimpleIdeRomEnabled->Checked);

        HorizontalSyncPulse->Checked                         = ini->ReadBool("MAIN", "ColouriseHorizontalSyncPulse",                         HorizontalSyncPulse->Checked);
        VerticalSyncPulse->Checked                           = ini->ReadBool("MAIN", "ColouriseVerticalSyncPulse",                           VerticalSyncPulse->Checked);
        RomDisplayDriver->Checked                            = ini->ReadBool("MAIN", "ColouriseRomDisplayDriver",                            RomDisplayDriver->Checked);
        BackPorch->Checked                                   = ini->ReadBool("MAIN", "ColouriseBackPorch",                                   BackPorch->Checked);
        NonMaskableInterruptResponse->Checked                = ini->ReadBool("MAIN", "ColouriseNonMaskableInterruptResponse",                NonMaskableInterruptResponse->Checked);
        NonMaskableInterruptResponseWaitStates->Checked      = ini->ReadBool("MAIN", "ColouriseNonMaskableInterruptResponseWaitStates",      NonMaskableInterruptResponseWaitStates->Checked);
        NonMaskableInterruptServiceRoutine->Checked          = ini->ReadBool("MAIN", "ColouriseNonMaskableInterruptServiceRoutine",          NonMaskableInterruptServiceRoutine->Checked);
        NonMaskableInterruptServiceRoutineRecursion->Checked = ini->ReadBool("MAIN", "ColouriseNonMaskableInterruptServiceRoutineRecursion", NonMaskableInterruptServiceRoutineRecursion->Checked);
        MaskableInterruptResponse->Checked                   = ini->ReadBool("MAIN", "ColouriseMaskableInterruptResponse",                   MaskableInterruptResponse->Checked);
        MaskableInterruptServiceRoutine->Checked             = ini->ReadBool("MAIN", "ColouriseMaskableInterruptServiceRoutine",             MaskableInterruptServiceRoutine->Checked);
        InstructionStraddlingNMI->Checked                    = ini->ReadBool("MAIN", "ColouriseInstructionStraddlingNMI",                    InstructionStraddlingNMI->Checked);
        InstructionStraddlingNMIWaitStates->Checked          = ini->ReadBool("MAIN", "ColouriseInstructionStraddlingNMIWaitStates",          InstructionStraddlingNMIWaitStates->Checked);
        Z80Halted->Checked                                   = ini->ReadBool("MAIN", "ColouriseZ80Halted",                                   Z80Halted->Checked);
        UserProgramInstructionStartPositions->Checked        = ini->ReadBool("MAIN", "ColouriseUserProgramInstructionStartPositions",        UserProgramInstructionStartPositions->Checked);

        UpdateEmulatorAnnotationSettings();
        UpdateAnnotationImages();

        if (None1->Checked)      { emulator.bordersize=BORDERNONE;   None1Click(NULL); }
        if (Small1->Checked)     { emulator.bordersize=BORDERSMALL;  Small1Click(NULL); }
        if (Normal1->Checked)    { emulator.bordersize=BORDERNORMAL; Normal1Click(NULL); }
        if (Large1->Checked)     { emulator.bordersize=BORDERLARGE;  Large1Click(NULL); }
        if (FullImage1->Checked) { emulator.bordersize=BORDERFULL;   FullImage1Click(NULL); }

        emulator.audioout = (CFGBYTE)(OutAudioOut->Checked   ? 1 : 0);
        emulator.TZXin    = (CFGBYTE)(InTZXManager->Checked  ? 1 : 0);
        emulator.TZXout   = (CFGBYTE)(OutTZXManager->Checked ? 1 : 0);

        spectrum.spectrum128Keypad       = (CFGBYTE)(ConnectSpectrum128Keypad->Checked ? 1 : 0);
        spectrum.spectraColourSwitchOn   = (CFGBYTE)(SpectraColourEnable->Checked      ? 1 : 0);
        zx81.chromaColourSwitchOn        = (CFGBYTE)(ChromaColourEnable->Checked       ? 1 : 0);

        machine.joystick1Connected       = (CFGBYTE)(ConnectJoystick1->Checked         ? 1 : 0);
        machine.joystick2Connected       = (CFGBYTE)(ConnectJoystick2->Checked         ? 1 : 0);
        machine.joystick1AutoFireEnabled = (CFGBYTE)(EnableJoystick1AutoFire->Checked  ? 1 : 0);
        machine.joystick2AutoFireEnabled = (CFGBYTE)(EnableJoystick2AutoFire->Checked  ? 1 : 0);

        AccurateInit(true);

        if (ini->ReadBool("MAIN", "N1001",N1001->Checked)) N1001Click(NULL);
        if (ini->ReadBool("MAIN", "N2001",N2001->Checked)) N2001Click(NULL);
        if (ini->ReadBool("MAIN", "N4001",N4001->Checked)) N4001Click(NULL);
        if (ini->ReadBool("MAIN", "UserDefined",UserDefined1->Checked)) UserDefined1Click(NULL);
        if (!StatusBar2->Checked) StatusBar2Click(NULL);

        Top  = ini->ReadInteger("MAIN", "Top",  0);
        Left = ini->ReadInteger("MAIN", "Left", 0);

        // The start up height and width are transferred to the real height and width on the first timer event.
        StartUpHeight = ini->ReadInteger("MAIN", "Height", 0);
        StartUpWidth  = ini->ReadInteger("MAIN", "Width",  0);

        // Always default to the 100% to begin with, before changing to real dimensions upon the next timer event
        ClientHeight = BaseHeight + (StatusBar1->Visible ? StatusBar1->Height : 0);
        ClientWidth  = BaseWidth;
}

void TForm1::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("MAIN", "Top",    Top);
        ini->WriteInteger("MAIN", "Left",   Left);
        ini->WriteInteger("MAIN", "Height", ClientHeight);
        ini->WriteInteger("MAIN", "Width",  ClientWidth);

        ini->WriteBool(   "MAIN", "ShowSplash", ShowSplash);
        ini->WriteInteger("MAIN", "RenderMode", RenderMode);
        ini->WriteInteger("MAIN", "CheckInstallationPathLength", emulator.checkInstallationPathLength);

        ini->WriteBool("MAIN", "N1001",       N1001->Checked);
        ini->WriteBool("MAIN", "N2001",       N2001->Checked);
        ini->WriteBool("MAIN", "N4001",       N4001->Checked);
        ini->WriteBool("MAIN", "UserDefined", UserDefined1->Checked);

        ini->WriteBool("MAIN", "InverseVideo", InverseVideo->Checked);
        ini->WriteBool("MAIN", "DisplayArt",   DisplayArt->Checked);
        ini->WriteBool("MAIN", "StatusBar",    StatusBar2->Checked);

        ini->WriteBool("MAIN", "Keyboard1", Keyboard1->Checked);
        ini->WriteBool("MAIN", "Hardware1", Hardware1->Checked);
        ini->WriteBool("MAIN", "Speed1",    Speed1->Checked);
        ini->WriteBool("MAIN", "InWave",    InWaveLoader->Checked);
        ini->WriteBool("MAIN", "InTZX",     InTZXManager->Checked);
        ini->WriteBool("MAIN", "OutWave",   OutWaveLoader->Checked);
        ini->WriteBool("MAIN", "OutTZX",    OutTZXManager->Checked);
        ini->WriteBool("MAIN", "OutAudio",  OutAudioOut->Checked);
        ini->WriteBool("MAIN", "Sound1",    Sound1->Checked);

        ini->WriteBool("MAIN", "BorderNone",   None1->Checked);
        ini->WriteBool("MAIN", "BorderSmall",  Small1->Checked);
        ini->WriteBool("MAIN", "BorderNormal", Normal1->Checked);
        ini->WriteBool("MAIN", "BorderLarge",  Large1->Checked);
        ini->WriteBool("MAIN", "BorderFull",   FullImage1->Checked);

        ini->WriteBool("MAIN", "DebugWin",     DebugWin->Checked);
        ini->WriteBool("MAIN", "ViewPrinter",  ViewPrinter->Checked);
        ini->WriteBool("MAIN", "WavLoadBtn",   WavLoadBtn->Checked);
        ini->WriteBool("MAIN", "KeyMap",       KeyboardMap1->Checked);
        ini->WriteBool("MAIN", "TZXManager",   TZXMan->Checked);
        ini->WriteBool("MAIN", "DiskDrives",   DiskDrives1->Checked);
        ini->WriteBool("MAIN", "SoundOutput",  SoundOutput1->Checked);
        ini->WriteBool("MAIN", "BasicListing", BasicListerOption->Checked);

        ini->WriteString("MAIN", "LoadFile",        OpenTape1->FileName);
        ini->WriteInteger("MAIN", "LoadFileFilter", OpenTape1->FilterIndex);

        ini->WriteBool("MAIN", "SpectraColourEnable",      SpectraColourEnable->Checked);
        ini->WriteBool("MAIN", "ChromaColourEnable",       ChromaColourEnable->Checked);
        ini->WriteBool("MAIN", "ConnectSpectrum128Keypad", ConnectSpectrum128Keypad->Checked);
        ini->WriteBool("MAIN", "ConnectJoystick1",         ConnectJoystick1->Checked);
        ini->WriteBool("MAIN", "ConnectJoystick2",         ConnectJoystick2->Checked);
        ini->WriteBool("MAIN", "EnableJoystick1AutoFire",  EnableJoystick1AutoFire->Checked);
        ini->WriteBool("MAIN", "EnableJoystick2AutoFire",  EnableJoystick2AutoFire->Checked);
        ini->WriteBool("MAIN", "SwitchOnZ80Assembler",     SwitchOnZ80Assembler->Checked);
        ini->WriteBool("MAIN", "SwitchOnMemocalc",         SwitchOnMemocalc->Checked);
//        ini->WriteBool("MAIN", "SwitchOnMemotext",         SwitchOnMemotext->Checked);

        ini->WriteInteger("MAIN", "Joystick1Controller", machine.joystick1Controller);
        ini->WriteInteger("MAIN", "Joystick2Controller", machine.joystick2Controller);

        ini->WriteBool("MAIN", "divIDEJumperEClosed",    divIDEJumperEClosed->Checked);
        ini->WriteBool("MAIN", "ZXCFUploadJumperClosed", ZXCFUploadJumperClosed->Checked);
        ini->WriteBool("MAIN", "SimpleIdeRomEnabled",    SimpleIdeRomEnabled->Checked);

        ini->WriteBool("MAIN", "ColouriseHorizontalSyncPulse",                         HorizontalSyncPulse->Checked);
        ini->WriteBool("MAIN", "ColouriseVerticalSyncPulse",                           VerticalSyncPulse->Checked);
        ini->WriteBool("MAIN", "ColouriseRomDisplayDriver",                            RomDisplayDriver->Checked);
        ini->WriteBool("MAIN", "ColouriseBackPorch",                                   BackPorch->Checked);
        ini->WriteBool("MAIN", "ColouriseNonMaskableInterruptResponse",                NonMaskableInterruptResponse->Checked);
        ini->WriteBool("MAIN", "ColouriseNonMaskableInterruptResponseWaitStates",      NonMaskableInterruptResponseWaitStates->Checked);
        ini->WriteBool("MAIN", "ColouriseNonMaskableInterruptServiceRoutine",          NonMaskableInterruptServiceRoutine->Checked);
        ini->WriteBool("MAIN", "ColouriseNonMaskableInterruptServiceRoutineRecursion", NonMaskableInterruptServiceRoutineRecursion->Checked);
        ini->WriteBool("MAIN", "ColouriseMaskableInterruptResponse",                   MaskableInterruptResponse->Checked);
        ini->WriteBool("MAIN", "ColouriseMaskableInterruptServiceRoutine",             MaskableInterruptServiceRoutine->Checked);
        ini->WriteBool("MAIN", "ColouriseInstructionStraddlingNMI",                    InstructionStraddlingNMI->Checked);
        ini->WriteBool("MAIN", "ColouriseInstructionStraddlingNMIWaitStates",          InstructionStraddlingNMIWaitStates->Checked);
        ini->WriteBool("MAIN", "ColouriseZ80Halted",                                   Z80Halted->Checked);
        ini->WriteBool("MAIN", "ColouriseUserProgramInstructionStartPositions",        UserProgramInstructionStartPositions->Checked);

        Keyboard->SaveSettings(ini);
        Speed->SaveSettings(ini);
        ZipFile->SaveSettings(ini);
        Kb->SaveSettings(ini);
        ZXPrinter->SaveSettings(ini);
        WavLoad->SaveSettings(ini);
        Dbg->SaveSettings(ini);
        HW->SaveSettings(ini);
        Artifacts->SaveSettings(ini);
        SerialConfig->SaveSettings(ini);
        TZX->SaveSettings(ini);
        FSSettings->SaveSettings(ini);
        P3Drive->SaveSettings(ini);
        SoundOutput->SaveSettings(ini);
        ZX97Dialog->SaveSettings(ini);
        IF1->SaveSettings(ini);
        ParallelPort->SaveSettings(ini);
        MidiForm->SaveSettings(ini);
        BasicLister->SaveSettings(ini);
        LoadBasicListingOptionsForm->SaveSettings(ini);
        SaveBasicListingOptionsForm->SaveSettings(ini);
}

void __fastcall TForm1::DisplayArtClick(TObject *Sender)
{
        DisplayArt->Checked = !DisplayArt->Checked;
        if (DisplayArt->Checked) Artifacts->Show();
        else Artifacts->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Sound1Click(TObject *Sender)
{
        Sound1->Checked = !Sound1->Checked;
        zx81.vsyncsound = Sound1->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::HelpTopics2Click(TObject *Sender)
{
        ShellExecute(NULL, NULL, "eightyone.chm", NULL, NULL, SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InWaveLoaderClick(TObject *Sender)
{
        InWaveLoader->Checked=true;
        InTZXManager->Checked=false;
        emulator.TZXin=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OutWaveLoaderClick(TObject *Sender)
{
        OutWaveLoader->Checked=true;
        OutAudioOut->Checked=false;
        OutTZXManager->Checked=false;
        emulator.audioout=0;
        emulator.TZXout=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutAudioOutClick(TObject *Sender)
{
        OutWaveLoader->Checked=false;
        OutTZXManager->Checked=false;
        OutAudioOut->Checked=true;
        emulator.audioout=1;
        emulator.TZXout=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SoundOutput1Click(TObject *Sender)
{
        SoundOutput1->Checked = !SoundOutput1->Checked;
        if (SoundOutput1->Checked) SoundOutput->Show();
        else SoundOutput->Close();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ROMClick(TObject *Sender)
{
        PCAllKeysUp();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadMemoryBlock1Click(TObject *Sender)
{
        MemSave->LoadMem();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveMemoryBlock1Click(TObject *Sender)
{
        MemSave->SaveMem();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::HardReset1Click(TObject *Sender)
{
        int initialStopState = emulation_stop;

        rzx_close();
        emulation_stop=1;
        z80_reset();
        AccurateInit(false);
        machine.initialise();
        Sound.InitDevices();
        emulation_stop=initialStopState;
        Dbg->Reset();
        DebugUpdate();
        LiveMemoryWindow->Reset();
        if (BasicLister->ListerAvailable())
        {
                BasicLister->Clear();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TS20501Click(TObject *Sender)
{
        SerialConfig->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TZXManClick(TObject *Sender)
{
        Form1->TZXMan->Checked = !Form1->TZXMan->Checked;

        if (Form1->TZXMan->Checked) TZX->Show();
        else TZX->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InTZXManagerClick(TObject *Sender)
{
        InWaveLoader->Checked=false;
        InTZXManager->Checked=true;
        emulator.TZXin=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutTZXManagerClick(TObject *Sender)
{
        OutWaveLoader->Checked=false;
        OutTZXManager->Checked=true;
        OutAudioOut->Checked=false;
        emulator.TZXout=1;
        emulator.audioout=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Small1Click(TObject *Sender)
{
        None1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (Small1->Checked) return;
        Small1->Checked=true;
        emulator.bordersize=BORDERSMALL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Normal1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (Normal1->Checked) return;
        Normal1->Checked=true;
        emulator.bordersize=BORDERNORMAL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Large1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Normal1->Checked=false;
        FullImage1->Checked=false;
        if (Large1->Checked) return;
        Large1->Checked=true;
        emulator.bordersize=BORDERLARGE;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FullImage1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        if (FullImage1->Checked) return;
        FullImage1->Checked=true;
        emulator.bordersize=BORDERFULL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------
void TForm1::DoAutoLoad(void)
{
#define AUTOINC(i)  (340+i*10)

        if (emulator.machine==MACHINEACE) return;

        bool zx80 = (emulator.machine == MACHINEZX80) && (emulator.romcrc == CRCZX80);

        switch(AutoLoadCount)
        {
        case 2:
                 ResetZX811Click(NULL);
                 break;
        case AUTOINC(0): if (emulator.machine==MACHINESPECTRUM && spectrum.model>=SPECCY128)
                                PCKeyDown(VK_RETURN);
                         else if (emulator.machine==MACHINELAMBDA) PCKeyDown('L');
                         else if (zx80)
                                PCKeyDown('W');
                         else PCKeyDown('J');
                         break;

        case AUTOINC(1): if (emulator.machine==MACHINESPECTRUM && spectrum.model==SPECCY128)
                                PCKeyUp(VK_RETURN);
                         else if (emulator.machine==MACHINELAMBDA) PCKeyUp('L');
                         else if (zx80) PCKeyUp('W');
                         else PCKeyUp('J');
                         break;

        case AUTOINC(2):  if (emulator.machine==MACHINESPECTRUM) PCKeyDown(VK_CONTROL);
                          else if (emulator.machine==MACHINELAMBDA) PCKeyDown('O');
                          else if (zx80)
                                PCKeyDown(VK_RETURN);
                          else PCKeyDown(VK_SHIFT); break;

        case AUTOINC(3): if (emulator.machine==MACHINELAMBDA) PCKeyUp('O');
                         else if (zx80) PCKeyUp(VK_RETURN);
                         else PCKeyDown('P'); break;

        case AUTOINC(4): if (emulator.machine==MACHINELAMBDA) PCKeyDown('A');
                         else PCKeyUp('P'); break;

        case AUTOINC(5): if (emulator.machine==MACHINELAMBDA) PCKeyUp('A');
                         else PCKeyDown('P'); break;

        case AUTOINC(6): if (emulator.machine==MACHINELAMBDA) PCKeyDown('D');
                         else PCKeyUp('P'); break;

        case AUTOINC(7): if (emulator.machine==MACHINELAMBDA) PCKeyUp('D');
                         else if (emulator.machine==MACHINESPECTRUM) PCKeyUp(VK_CONTROL);
                         else PCKeyUp(VK_SHIFT); break;

        case AUTOINC(8): if (emulator.machine==MACHINELAMBDA) PCKeyDown(VK_SHIFT);
                         else PCKeyDown(VK_RETURN); break;

        case AUTOINC(9): if (emulator.machine==MACHINELAMBDA) PCKeyDown('5');
                         else PCKeyUp(VK_RETURN); break;

        case AUTOINC(10): if (emulator.machine==MACHINELAMBDA) PCKeyUp('5');
                         break;

        case AUTOINC(11): if (emulator.machine==MACHINELAMBDA) PCKeyDown('5');
                         break;

        case AUTOINC(12): if (emulator.machine==MACHINELAMBDA) PCKeyUp('5');
                         break;

        case AUTOINC(13): if (emulator.machine==MACHINELAMBDA) PCKeyUp(VK_SHIFT);
                         break;

        case AUTOINC(14): if (emulator.machine==MACHINELAMBDA) PCKeyDown(VK_RETURN);
                         break;

        case AUTOINC(15): if (emulator.machine==MACHINELAMBDA) PCKeyUp(VK_RETURN);
                         break;
        default: break;
        }
        machine.NTSC ? AutoLoadCount++ : AutoLoadCount+=2;
        if (AutoLoadCount==1000) AutoLoadCount=0;
}

void __fastcall TForm1::FullScreenSettings1Click(TObject *Sender)
{
        if (FullScreenSettings1->Checked==true)
        {
                FSSettings->Close();
        }
        else
        {
                FullScreenSettings1->Checked=true;
                FSSettings->Show();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::None1Click(TObject *Sender)
{
        Small1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (None1->Checked) return;
        None1->Checked=true;
        emulator.bordersize=BORDERNONE;
        AccurateInit(true);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::StatusBar2Click(TObject *Sender)
{
        StatusBar1->Visible = !StatusBar1->Visible;

        if (StatusBar1->Visible) Height += StatusBar1->Height;
        else Height -= StatusBar1->Height;

        StatusBar2->Checked = StatusBar1->Visible;
}

//---------------------------------------------------------------------------


void __fastcall TForm1::Interface1PortsClick(TObject *Sender)
{
        IF1->ShowModal();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::SaveSnapDialogTypeChange(TObject *Sender)
{
        int i,p;
        AnsiString filter, newext;
        AnsiString Fname;

        THandle *h;
        TSaveDialog *d;

        filter=SaveSnapDialog->Filter;
        p=1;
        i=SaveSnapDialog->FilterIndex;
        i=(i*2)-1;

        while(i)
        {
                if (filter[p]=='|') i--;
                p++;
        }
        newext = "";
        p++;

        while(filter[p]!='|')
        {
                newext += filter[p];
                p++;
                if (p>filter.Length()) break;
        }

        Fname = RemovePath(RemoveExt(SaveSnapDialog->FileName) + newext);

        d=(TSaveDialog *)Sender;
        h=(THandle *)GetParent(d->Handle);
        SendMessage((HWND)h, CDM_SETCONTROLTEXT, edt1, (long)(Fname.c_str()));

}
//---------------------------------------------------------------------------

void __fastcall TForm1::DiskDrives1Click(TObject *Sender)
{
        DiskDrives1->Checked = !DiskDrives1->Checked;

        if (DiskDrives1->Checked) P3Drive->Show();
        else P3Drive->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StatusBar1DrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
        if ( Panel->Index == 2)
        {
                TRect R;

                R=Rect;
                StatusBar->Canvas->Brush->Color = clBtnFace;
                StatusBar->Canvas->FillRect(R);
                if (machine.drivebusy!=-1)
                {
                        if (machine.floppytype == FLOPPYIF1)
                        {
                                StatusBar->Canvas->Draw( R.Left+2, R.Top+2, machine.drivebusy ? LEDRedOn:LEDRedOff );
                        }
                        else
                        {
                                StatusBar->Canvas->Draw( R.Left+2, R.Top+2, machine.drivebusy ? LEDGreenOn:LEDGreenOff );
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        switch(Button)
        {
        case mbLeft: mouse.buttons &= ~2; break;
        case mbRight: mouse.buttons &= ~1; break;
        case mbMiddle: mouse.buttons &= ~4; break;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        switch(Button)
        {
        case mbLeft: mouse.buttons |= 2; break;
        case mbRight: mouse.buttons |= 1; break;
        case mbMiddle: mouse.buttons |= 4; break;
        }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::GenerateNMI1Click(TObject *Sender)
{
        nmiOccurred = 1;
}
//---------------------------------------------------------------------------
void FetchFolderList(vector<AnsiString>* pEntries, AnsiString path)
{
        DIR* dir;
        struct dirent *ent;

        if ((dir = opendir(path.c_str())) != NULL)
        {
                while ((ent = readdir(dir)) != NULL)
                {
                        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
                        {
                                pEntries->push_back(AnsiString(ent->d_name));
                        }
                }

                closedir(dir);
        }

        sort(pEntries->begin(), pEntries->end());
}
//---------------------------------------------------------------------------

void TForm1::BuildConfigMenu()
{
        vector<AnsiString> files;
        vector<AnsiString>::iterator iter;

        while(Config1->Count > 4) Config1->Delete(4);
        while(DeleteConfigurations->Count > 2) DeleteConfigurations->Delete(2);

        FetchFolderList(&files, emulator.configpath);

        for (iter = files.begin(); iter != files.end(); iter++)
        {
                AnsiString FileName, Extension;

                FileName=(*iter);
                Extension=FileNameGetExt(FileName);

                if (Extension == ".INI" && !FileName.Pos("&"))
                {
                        TMenuItem *NewItem;
                        NewItem = new TMenuItem(Config1);
                        NewItem->Caption=RemoveExt(FileName);
                        NewItem->OnClick=ConfigItem1Click;
                        Config1->Add(NewItem);

                        if (FileName != "EightyOne.ini")
                        {
                                TMenuItem *NewItem;
                                NewItem = new TMenuItem(DeleteConfigurations);
                                NewItem->Caption=RemoveExt(FileName);
                                NewItem->OnClick=DeleteConfigItem1Click;
                                DeleteConfigurations->Add(NewItem);
                        }
                }
        }

        DeleteConfigurations->Enabled = (DeleteConfigurations->Count > 2);
        DeleteAll->Enabled = (DeleteConfigurations->Count > 2);
}
//---------------------------------------------------------------------------
void TForm1::BuildDocumentationMenu()
{
        AnsiString path = emulator.cwd;
        path += documentationFolder;

        AddDocumentationFiles(DocumentationMenuEntry, path);
}
//---------------------------------------------------------------------------
void TForm1::AddDocumentationFiles(TMenuItem* CategoryMenu, AnsiString path)
{
        vector<AnsiString> files;
        vector<AnsiString>::iterator iter;
        int folderIndex = 0;

        FetchFolderList(&files, path);

        for (iter = files.begin(); iter != files.end(); iter++)
        {
                AnsiString FileName = (*iter).c_str();
                AnsiString Title = RemoveExt(FileName);

                if (Title == FileName)
                {
                        TMenuItem* CategorySubMenu = new TMenuItem(CategoryMenu);
                        CategoryMenu->Insert(folderIndex, CategorySubMenu);
                        CategorySubMenu->Caption = (*iter).c_str();
                        folderIndex++;

                        AnsiString SubCategoryPath = path;
                        SubCategoryPath += (*iter).c_str();
                        SubCategoryPath += "\\";
                        AddDocumentationFiles(CategorySubMenu, SubCategoryPath);
                }
                else
                {
                        TMenuItem* InstructionEntry = new TMenuItem(CategoryMenu);
                        CategoryMenu->Add(InstructionEntry);
                        InstructionEntry->Caption = Title;
                        InstructionEntry->OnClick = InstructionMenuItemClick;
                }
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::InstructionMenuItemClick(TObject *Sender)
{
        TMenuItem* ClickedItem = dynamic_cast<TMenuItem*>(Sender);
        TMenuItem* ParentItem = ClickedItem->Parent;

        AnsiString subPath;

        while (!ParentItem->Tag)
        {
                subPath = ParentItem->Caption + "\\" + subPath;
                ParentItem = ParentItem->Parent;
        }

        AnsiString Path = emulator.cwd;
        Path += documentationFolder;
        Path += subPath;

        struct stat buffer;
        AnsiString webPath = Path + ClickedItem->Caption + ".web";
        AnsiString pdfPath = Path + ClickedItem->Caption + ".pdf";
        AnsiString refPath = Path + ClickedItem->Caption + ".ref";

        if (stat(webPath.c_str(), &buffer) == 0)
        {
                FILE* filePointer = fopen(webPath.c_str(), "r");
                if (filePointer)
                {
                        bool readText = (fgets(webBuffer, bufferLength, filePointer) != NULL);
                        fclose(filePointer);

                        if (readText)
                        {
                                ShellExecute(0, NULL, webBuffer, NULL, NULL, SW_NORMAL);
                        }
                }
        }
        else if (stat(pdfPath.c_str(), &buffer) == 0)
        {
                Path += ClickedItem->Caption + ".pdf";
                ShellExecute(NULL, "open", Path.c_str(), NULL, NULL, SW_NORMAL);
        }
        else if (stat(refPath.c_str(), &buffer) == 0)
        {
                char refText[256];
                FILE* f = fopen(refPath.c_str(), "rb");
                if (f)
                {
                        fgets(refText, sizeof(refText), f);
                        fclose(f);
                }

                Path += refText;
                ShellExecute(NULL, "open", Path.c_str(), NULL, NULL, SW_NORMAL);
        }
        else
        {
                Path += ClickedItem->Caption + ".txt";
                ShellExecute(NULL, "open", Path.c_str(), NULL, NULL, SW_NORMAL);
        }
}
//---------------------------------------------------------------------------
void TForm1::BuildZX81ExamplesMenu()
{
        vector<AnsiString> folders;
        vector<AnsiString>::iterator iter;

        AnsiString path = emulator.cwd;
        path += exampleZX81ProgramsFolder;

        FetchFolderList(&folders, path);

        for (iter = folders.begin(); iter != folders.end(); iter++)
        {
                TMenuItem* CategorySubMenu = new TMenuItem(ExampleZX81ProgramsMenuEntry);
                ExampleZX81ProgramsMenuEntry->Add(CategorySubMenu);
                CategorySubMenu->Caption = (*iter).c_str();

                AnsiString CategoryFolder = path;
                CategoryFolder += (*iter).c_str();
                CategoryFolder += "\\";

                AddZX81ExampleFolders(CategorySubMenu, CategoryFolder);
        }
}
//---------------------------------------------------------------------------
void TForm1::BuildSpectrumExamplesMenu()
{
        vector<AnsiString> folders;
        vector<AnsiString>::iterator iter;

        AnsiString path = emulator.cwd;
        path += exampleSpectrumProgramsFolder;

        FetchFolderList(&folders, path);

        for (iter = folders.begin(); iter != folders.end(); iter++)
        {
                TMenuItem* CategorySubMenu = new TMenuItem(ExampleSpectrumProgramsMenuEntry);
                ExampleSpectrumProgramsMenuEntry->Add(CategorySubMenu);
                CategorySubMenu->Caption = (*iter).c_str();

                AnsiString CategoryFolder = path;
                CategoryFolder += (*iter).c_str();
                CategoryFolder += "\\";

                AddSpectrumExampleFolders(CategorySubMenu, CategoryFolder);
        }
}
//---------------------------------------------------------------------------
void TForm1::AddZX81ExampleFolders(TMenuItem* CategorySubMenu, AnsiString path)
{
        vector<AnsiString> files;
        vector<AnsiString>::iterator iter;

        FetchFolderList(&files, path);

        for (iter = files.begin(); iter != files.end(); iter++)
        {
                TMenuItem* ExampleFolderEntry = new TMenuItem(CategorySubMenu);
                CategorySubMenu->Add(ExampleFolderEntry);
                ExampleFolderEntry->Caption = (*iter).c_str();
                ExampleFolderEntry->OnClick = ExampleZX81ProgramsMenuEntryClick;
        }
}
//---------------------------------------------------------------------------
void TForm1::AddSpectrumExampleFolders(TMenuItem* CategorySubMenu, AnsiString path)
{
        vector<AnsiString> files;
        vector<AnsiString>::iterator iter;

        FetchFolderList(&files, path);

        for (iter = files.begin(); iter != files.end(); iter++)
        {
                TMenuItem* ExampleFolderEntry = new TMenuItem(CategorySubMenu);
                CategorySubMenu->Add(ExampleFolderEntry);
                ExampleFolderEntry->Caption = (*iter).c_str();
                ExampleFolderEntry->OnClick = ExampleSpectrumProgramsMenuEntryClick;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ExampleZX81ProgramsMenuEntryClick(TObject *Sender)
{
        TMenuItem* ClickedItem = dynamic_cast<TMenuItem*>(Sender);
        TMenuItem* ParentItem = ClickedItem->Parent;

        AnsiString Path = emulator.cwd;
        Path += exampleZX81ProgramsFolder;
        Path += ParentItem->Caption;
        Path += "\\";
        Path += ClickedItem->Caption;
        Path += "\\";

        ShellExecute(NULL, "open", Path.c_str(), "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ExampleSpectrumProgramsMenuEntryClick(
      TObject *Sender)
{
        TMenuItem* ClickedItem = dynamic_cast<TMenuItem*>(Sender);
        TMenuItem* ParentItem = ClickedItem->Parent;

        AnsiString Path = emulator.cwd;
        Path += exampleSpectrumProgramsFolder;
        Path += ParentItem->Caption;
        Path += "\\";
        Path += ClickedItem->Caption;
        Path += "\\";

        ShellExecute(NULL, "open", Path.c_str(), "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveCurrentConfigClick(TObject *Sender)
{
        TIniFile *ini;
        AnsiString FileName;

        SaveConfigDialog->FileName="";
        SaveConfigDialog->InitialDir=emulator.configpath;

        if (!SaveConfigDialog->Execute()) return;

        FileName=SaveConfigDialog->FileName;
        if (FileNameGetExt(FileName) != ".INI")
                FileName=FileName+".ini";

        ini = new TIniFile(FileName);
        SaveSettings(ini);
        delete ini;

        BuildConfigMenu();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ConfigItem1Click(TObject *Sender)
{
        AnsiString FileName;
        AnsiString ConfigName;
        int i;

        ConfigName = ((TMenuItem *)Sender)->Caption;

        while((i = ConfigName.Pos("&")) != 0)
        {
                int len;
                AnsiString Before="", After="";

                len=ConfigName.Length();

                if (i>1) Before=ConfigName.SubString(1,i-1);
                if (i<len) After=ConfigName.SubString(i+1, len-i);
                ConfigName=Before+After;
        }

        FileName = emulator.configpath;
        FileName = FileName + ConfigName;
        FileName = FileName + ".ini";

        LoadIniFile(FileName);
}
//---------------------------------------------------------------------------

void TForm1::LoadIniFile(AnsiString FileName)
{                      
        TIniFile *ini;
        
        ini = new TIniFile(FileName);

        Form1->LoadSettings(ini);
        Keyboard->LoadSettings(ini);
        Speed->LoadSettings(ini);
        Kb->LoadSettings(ini);
        WavLoad->LoadSettings(ini);
        Dbg->LoadSettings(ini);
        ZXPrinter->LoadSettings(ini);
        Artifacts->LoadSettings(ini);
        SerialConfig->LoadSettings(ini);
        TZX->LoadSettings(ini);
        FSSettings->LoadSettings(ini);
        HW->LoadSettings(ini);
        IF1->LoadSettings(ini);
        P3Drive->LoadSettings(ini);
        SoundOutput->LoadSettings(ini);
        ZX97Dialog->LoadSettings(ini);
        ParallelPort->LoadSettings(ini);
        MidiForm->LoadSettings(ini);
        ZipFile->LoadSettings(ini);
        BasicLister->LoadSettings(ini);
        LoadBasicListingOptionsForm->LoadSettings(ini);
        SaveBasicListingOptionsForm->LoadSettings(ini);

        delete ini;

        const bool disableResetStatus = false;
        HW->UpdateHardwareSettings(disableResetStatus);

        HardReset1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetMemotechHRGClick(TObject *Sender)
{
        memotechResetPressed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetQuicksilvaHiResClick(TObject *Sender)
{
        extern int QuicksilvaHiResMode;

        QuicksilvaHiResMode = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveScreenshot1Click(TObject *Sender)
{
        AnsiString Extension, Filename;
        FILE *f;

        SaveScrDialog->Filter = "Windows Bitmap (.bmp)|*.bmp";

        if (emulator.machine==MACHINESPECTRUM)
                SaveScrDialog->Filter += "|Spectrum Screen (.scr)|*.scr";    

        if (!SaveScrDialog->Execute()) return;

        Filename=SaveScrDialog->FileName;
        Extension=FileNameGetExt(Filename);

        switch(SaveScrDialog->FilterIndex)
        {
        case 1:
                if (Extension!=".BMP") Filename += ".bmp";
                SaveScreenShot(Filename);
                break;

        case 2:
                if (Extension!=".SCR") Filename += ".scr";
                f=fopen(Filename.c_str(), "wb");
                if (f)
                {
                        int i;

                        for(i=0;i<6912;i++)
                                fputc(spec48_getbyte(16384+i),f);
                        fclose(f);
                }
                break;
        default:
                break;
        }
}

//---------------------------------------------------------------------------
void __fastcall TForm1::PrinterPortClick(TObject *Sender)
{
        ParallelPort->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Midi1Click(TObject *Sender)
{
        if (Midi1->Checked)
        {
                MidiForm->Close();
                Midi1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                MidiForm->Show();
                Midi1->Checked=true;
        }
        //MidiForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Play1Click(TObject *Sender)
{
        if (!OpenRZX->Execute()) return;

        spec48_LoadRZX(AnsiString(OpenRZX->FileName).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::QSChrEnableClick(TObject *Sender)
{
        QSChrEnable->Checked = !QSChrEnable->Checked;
        zx81.enableQSchrgen = QSChrEnable->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpectraColourEnableClick(TObject *Sender)
{
        bool previousSpectraColourEnable = SpectraColourEnable->Checked;

        SpectraColourEnable->Checked = !SpectraColourEnable->Checked;
        spectrum.spectraColourSwitchOn = SpectraColourEnable->Checked;

        if (previousSpectraColourEnable)
        {
                DisableSpectra();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ChromaColourEnableClick(TObject *Sender)
{
        bool previousChromaColourEnable = ChromaColourEnable->Checked;

        ChromaColourEnable->Checked = !ChromaColourEnable->Checked;
        zx81.chromaColourSwitchOn = ChromaColourEnable->Checked;

        if (previousChromaColourEnable)
        {
                DisableChroma();
        }
}
//---------------------------------------------------------------------------

void MoveWindow(TForm* form, int& x, int& y)
{
        if (x != -1 && y != -1)
        {
                form->Left = x;
                form->Top = y;

                if (form->Visible)
                {
                        x += 20;
                        y += 20;
                }
        }
        
        form->BringToFront();
}

//---------------------------------------------------------------------------

void MoveWindows(int l = -1, int t = -1)
{
        MoveWindow(Dbg, l, t);
        MoveWindow(MemoryWindow, l, t);
        MoveWindow(SymbolBrowser, l, t);
        MoveWindow(SoundOutput, l, t);
        MoveWindow(TZX, l, t);
        MoveWindow(WavLoad, l, t);
        MoveWindow(Speed, l, t);
        MoveWindow(Artifacts, l, t);
        MoveWindow(Keyboard, l, t);
        MoveWindow(FSSettings, l, t);
        MoveWindow(ZXPrinter, l, t);
        MoveWindow(MidiForm, l, t);
        MoveWindow(MemSave, l, t);
        MoveWindow(SerialConfig, l, t);
        MoveWindow(IF1, l, t);
        MoveWindow(HistoryBox, l, t);
        MoveWindow(ParallelPort, l, t);
        MoveWindow(Kb, l, t);
        MoveWindow(ZipFile, l, t);
        MoveWindow(P3Drive, l, t);
        MoveWindow(CreateHDF, l, t);
        MoveWindow(HW, l, t);
        MoveWindow(LiveMemoryWindow, l, t);
        MoveWindow(ProfilePlot, l, t);
        MoveWindow(Profiler, l, t);
        MoveWindow(BasicLister, l, t);}

void __fastcall TForm1::GatherWindows1Click(TObject *Sender)
{
        int l = 0;
        int t = 0;
        MoveWindow(this, l, t);
        l = (this->Width / 2);
        t = (this->Height / 2);
        MoveWindows(l, t);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BringWindowsToFrontClick(TObject *Sender)
{
        MoveWindows();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::EnableSplashScreenClick(TObject *Sender)
{
        EnableSplashScreen->Checked = !EnableSplashScreen->Checked;
        ShowSplash = EnableSplashScreen->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LiveMemoryOverviewClick(TObject *Sender)
{
        LiveMemoryOverview->Checked = !LiveMemoryOverview->Checked;
        if (LiveMemoryOverview->Checked)
        {
                LiveMemoryWindow->Show();
        }
        else
        {
                LiveMemoryWindow->Hide();
        }
}

//---------------------------------------------------------------------------

void __fastcall TForm1::BasicListerOptionClick(TObject *Sender)
{
        BasicListerOption->Checked = !BasicListerOption->Checked;
        if (BasicListerOption->Checked)
        {
                BasicLister->Show();
        }
        else
        {
                BasicLister->Close();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
        DragAcceptFiles(Handle, true);

        StatusBar1->Refresh();
        StatusBar1->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RunFrame()
{
        static int j, borrow, Drive;
        unsigned short rshift = VK_RSHIFT;
        unsigned short lshift = VK_LSHIFT;

        if (emulator.UseRShift)
        {
                bool L=IsAsyncKeyPressed(VK_LSHIFT);
                bool R=IsAsyncKeyPressed(VK_RSHIFT);
                TShiftState z;

                if (R != RShift)
                {
                        RShift=R;
                        if (R) FormKeyDown(NULL, rshift, z);
                        else FormKeyUp(NULL, rshift,z);
                }

                if (L != LShift)
                {
                        LShift=L;
                        if (L) FormKeyDown(NULL, lshift,z);
                        else FormKeyUp(NULL, lshift,z);
                }
        }

        Sound.Frame(emulation_stop || emulator.single_step);

        if (emulation_stop)
        {
                AccurateUpdateDisplay(false);
                return;
        }
        if (AutoLoadCount) DoAutoLoad();

        if (machine.drivebusy != Drive)
        {
                StatusBar1->Refresh();
                StatusBar1->Invalidate();
                Drive=machine.drivebusy;
        }

        if (spectrum.kmouse)
        {
                mouse.x = Controls::Mouse->CursorPos.x;
                mouse.y = Screen->Height - Controls::Mouse->CursorPos.y;
        }

        fps++;
        frametstates=0;

        j=emulator.single_step?1:(machine.tperframe + borrow);

        if (emulator.machine != MACHINESPECTRUM && j!=1 && !AutoLoadCount)
        {
                j += (emulator.speedup * machine.tperframe) / machine.tperscanline;
        }

        while (j>0 && !emulation_stop)
        {
                j-= machine.do_scanline(BuildLine);
                AccurateDraw(BuildLine);
                //WaitForSingleObject(Mutex,INFINITE);
                //templine=BuildLine;
                //BuildLine=DisplayLine;
                //DisplayLine=templine;
                //BuildLine->sync_len=DisplayLine->sync_len;
                //BuildLine->sync_valid=DisplayLine->sync_valid;
                //ReleaseMutex(Mutex);
                //SwitchToThread();
                //Sleep(0);
                //AccurateDraw(DisplayLine);
        }

        if (!emulation_stop) borrow=j;

        if (romcartridge.type == ROMCARTRIDGEZXC1)
        {
                RomCartridgeZXC1TimerTick();
        }
}

//---------------------------------------------------------------------------

const int ColourIndex_HorizontalSyncPulse = 1;
const int ColourIndex_VerticalSyncPulse = 2;
const int ColourIndex_NonMaskableInterruptResponse = 3;
const int ColourIndex_NonMaskableInterruptServiceRoutine = 4;
const int ColourIndex_MaskableInterruptServiceRoutine = 5;
const int ColourIndex_InstructionStraddlingNMI = 6;
const int ColourIndex_Z80Halted = 7;
const int ColourIndex_BackPorch = 9;
const int ColourIndex_RomDisplayDriver = 10;
const int ColourIndex_NonMaskableInterruptResponseWaitStates = 11;
const int ColourIndex_NonMaskableInterruptServiceRoutineRecursion = 12;
const int ColourIndex_MaskableInterruptResponse = 13;
const int ColourIndex_InstructionStraddlingNMIWaitStates = 14;
const int ColourIndex_UserProgramInstructionPositions = 15;
const int ColourIndex_Tick = 16;

//---------------------------------------------------------------------------

void TForm1::EnableAnnotationOptions()
{
        BOOL enableOption = (emulator.machine != MACHINEZX97LE && emulator.machine != MACHINESPECTRUM && emulator.machine != MACHINEACE);
        BOOL annotatableROM = IsAnnotatableROM();

        SelectAllColourisations->Enabled = enableOption;
        DeselectAllColourisations->Enabled = enableOption;

        if (!enableOption)
        {
                HorizontalSyncPulse->Checked = false;
                VerticalSyncPulse->Checked = false;
                RomDisplayDriver->Checked = false;
                Z80Halted->Checked = false;
                MaskableInterruptResponse->Checked = false;
                MaskableInterruptServiceRoutine->Checked = false;
        }
        else if (!annotatableROM)
        {
                RomDisplayDriver->Checked = false;
                MaskableInterruptServiceRoutine->Checked = false;
        }

        HorizontalSyncPulse->Enabled = enableOption;
        VerticalSyncPulse->Enabled = enableOption;
        RomDisplayDriver->Enabled = enableOption && annotatableROM;
        Z80Halted->Enabled = enableOption;
        MaskableInterruptResponse->Enabled = enableOption;
        MaskableInterruptServiceRoutine->Enabled = enableOption && annotatableROM;

        enableOption = enableOption && (emulator.machine != MACHINEZX80);
        BackPorch->Enabled = enableOption;
        NonMaskableInterruptResponse->Enabled = enableOption;
        NonMaskableInterruptResponseWaitStates->Enabled = enableOption;
        NonMaskableInterruptServiceRoutine->Enabled = enableOption && annotatableROM;
        NonMaskableInterruptServiceRoutineRecursion->Enabled = enableOption && annotatableROM;
        InstructionStraddlingNMI->Enabled = enableOption;
        InstructionStraddlingNMIWaitStates->Enabled = enableOption;
        UserProgramInstructionStartPositions->Enabled = enableOption;

        if (!enableOption)
        {
                BackPorch->Checked = false;
                NonMaskableInterruptResponse->Checked = false;
                NonMaskableInterruptResponseWaitStates->Checked = false;
                NonMaskableInterruptServiceRoutine->Checked = false;
                NonMaskableInterruptServiceRoutineRecursion->Checked = false;
                InstructionStraddlingNMI->Checked = false;
                InstructionStraddlingNMIWaitStates->Checked = false;
                UserProgramInstructionStartPositions->Checked = false;
        }
        else if (!annotatableROM)
        {
                NonMaskableInterruptServiceRoutine->Checked = false;
                NonMaskableInterruptServiceRoutineRecursion->Checked = false;
        }

        UpdateEmulatorAnnotationSettings();
        UpdateAnnotationImages();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::SelectAllColourisationsClick(TObject *Sender)
{
        BOOL selectOption = (emulator.machine != MACHINEZX97LE && emulator.machine != MACHINESPECTRUM && emulator.machine != MACHINEACE);
        BOOL annotatableROM = IsAnnotatableROM();
        
        HorizontalSyncPulse->Checked = selectOption;
        VerticalSyncPulse->Checked = selectOption;
        RomDisplayDriver->Checked = selectOption && annotatableROM;
        MaskableInterruptResponse->Checked = selectOption;
        MaskableInterruptServiceRoutine->Checked = selectOption && annotatableROM;
        Z80Halted->Checked = selectOption;

        selectOption = selectOption && (emulator.machine != MACHINEZX80);

        BackPorch->Checked = selectOption;
        NonMaskableInterruptResponse->Checked = selectOption;
        NonMaskableInterruptResponseWaitStates->Checked = selectOption;
        NonMaskableInterruptServiceRoutine->Checked = selectOption;
        NonMaskableInterruptServiceRoutineRecursion->Checked = selectOption;
        InstructionStraddlingNMI->Checked = selectOption;
        InstructionStraddlingNMIWaitStates->Checked = selectOption;
        UserProgramInstructionStartPositions->Checked = selectOption;

        UpdateEmulatorAnnotationSettings();
        UpdateAnnotationImages();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DeselectAllColourisationsClick(TObject *Sender)
{
        HorizontalSyncPulse->Checked = false;
        VerticalSyncPulse->Checked = false;
        RomDisplayDriver->Checked = false;
        BackPorch->Checked = false;
        NonMaskableInterruptResponse->Checked = false;
        NonMaskableInterruptResponseWaitStates->Checked = false;
        NonMaskableInterruptServiceRoutine->Checked = false;
        NonMaskableInterruptServiceRoutineRecursion->Checked = false;
        MaskableInterruptResponse->Checked = false;
        MaskableInterruptServiceRoutine->Checked = false;
        InstructionStraddlingNMI->Checked = false;
        InstructionStraddlingNMIWaitStates->Checked = false;
        Z80Halted->Checked = false;
        UserProgramInstructionStartPositions->Checked = false;

        UpdateEmulatorAnnotationSettings();
        UpdateAnnotationImages();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::HorizontalSyncPulseClick(TObject *Sender)
{
        HorizontalSyncPulse->Checked = !HorizontalSyncPulse->Checked;
        emulator.ColouriseHorizontalSyncPulse = HorizontalSyncPulse->Checked;

        UpdateAnnotationImage(HorizontalSyncPulse);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::VerticalSyncPulseClick(TObject *Sender)
{
        VerticalSyncPulse->Checked = !VerticalSyncPulse->Checked;
        emulator.ColouriseVerticalSyncPulse = VerticalSyncPulse->Checked;

        UpdateAnnotationImage(VerticalSyncPulse);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BackPorchClick(TObject *Sender)
{
        BackPorch->Checked = !BackPorch->Checked;
        emulator.ColouriseBackPorch = BackPorch->Checked;

        UpdateAnnotationImage(BackPorch);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NonMaskableInterruptResponseClick(TObject *Sender)
{
        NonMaskableInterruptResponse->Checked = !NonMaskableInterruptResponse->Checked;
        emulator.ColouriseNonMaskableInterruptResponse = NonMaskableInterruptResponse->Checked;

        UpdateAnnotationImage(NonMaskableInterruptResponse);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NonMaskableInterruptServiceRoutineClick(
      TObject *Sender)
{
        NonMaskableInterruptServiceRoutine->Checked = !NonMaskableInterruptServiceRoutine->Checked;
        emulator.ColouriseNonMaskableInterruptServiceRoutine = NonMaskableInterruptServiceRoutine->Checked;

        UpdateAnnotationImage(NonMaskableInterruptServiceRoutine);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MaskableInterruptResponseClick(TObject *Sender)
{
        MaskableInterruptResponse->Checked = !MaskableInterruptResponse->Checked;
        emulator.ColouriseMaskableInterruptResponse = MaskableInterruptResponse->Checked;

        UpdateAnnotationImage(MaskableInterruptResponse);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MaskableInterruptServiceRoutineClick(
      TObject *Sender)
{
        MaskableInterruptServiceRoutine->Checked = !MaskableInterruptServiceRoutine->Checked;
        emulator.ColouriseMaskableInterruptServiceRoutine = MaskableInterruptServiceRoutine->Checked;

        UpdateAnnotationImage(MaskableInterruptServiceRoutine);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InstructionStraddlingNMIClick(TObject *Sender)
{
        InstructionStraddlingNMI->Checked = !InstructionStraddlingNMI->Checked;
        emulator.ColouriseInstructionStraddlingNMI = InstructionStraddlingNMI->Checked;

        UpdateAnnotationImage(InstructionStraddlingNMI);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InstructionStraddlingNMIWaitStatesClick(TObject *Sender)
{
        InstructionStraddlingNMIWaitStates->Checked = !InstructionStraddlingNMIWaitStates->Checked;
        emulator.ColouriseInstructionStraddlingNMIWaitStates = InstructionStraddlingNMIWaitStates->Checked;

        UpdateAnnotationImage(InstructionStraddlingNMIWaitStates);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Z80HaltedClick(TObject *Sender)
{
        Z80Halted->Checked = !Z80Halted->Checked;
        emulator.ColouriseZ80Halted = Z80Halted->Checked;

        UpdateAnnotationImage(Z80Halted);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RomDisplayDriverClick(TObject *Sender)
{
        RomDisplayDriver->Checked = !RomDisplayDriver->Checked;
        emulator.ColouriseRomDisplayDriver = RomDisplayDriver->Checked;

        UpdateAnnotationImage(RomDisplayDriver);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NonMaskableInterruptServiceRoutineRecursionClick(TObject *Sender)
{
        NonMaskableInterruptServiceRoutineRecursion->Checked = !NonMaskableInterruptServiceRoutineRecursion->Checked;
        emulator.ColouriseNonMaskableInterruptServiceRoutineRecursion = NonMaskableInterruptServiceRoutineRecursion->Checked;

        UpdateAnnotationImage(NonMaskableInterruptServiceRoutineRecursion);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NonMaskableInterruptResponseWaitStatesClick(TObject *Sender)
{
        NonMaskableInterruptResponseWaitStates->Checked = !NonMaskableInterruptResponseWaitStates->Checked;
        emulator.ColouriseNonMaskableInterruptResponseWaitStates = NonMaskableInterruptResponseWaitStates->Checked;

        UpdateAnnotationImage(NonMaskableInterruptResponseWaitStates);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UserProgramInstructionStartPositionsClick(
      TObject *Sender)
{
        UserProgramInstructionStartPositions->Checked = !UserProgramInstructionStartPositions->Checked;
        emulator.ColouriseUserProgramInstructionStartPositions = UserProgramInstructionStartPositions->Checked;
        
        UpdateAnnotationImage(UserProgramInstructionStartPositions);
}
//---------------------------------------------------------------------------

void TForm1::UpdateAnnotationImages()
{
        UpdateAnnotationImage(HorizontalSyncPulse);
        UpdateAnnotationImage(VerticalSyncPulse);
        UpdateAnnotationImage(RomDisplayDriver);
        UpdateAnnotationImage(BackPorch);
        UpdateAnnotationImage(NonMaskableInterruptResponse);
        UpdateAnnotationImage(NonMaskableInterruptResponseWaitStates);
        UpdateAnnotationImage(NonMaskableInterruptServiceRoutine);
        UpdateAnnotationImage(NonMaskableInterruptServiceRoutineRecursion);
        UpdateAnnotationImage(MaskableInterruptResponse);
        UpdateAnnotationImage(MaskableInterruptServiceRoutine);
        UpdateAnnotationImage(InstructionStraddlingNMI);
        UpdateAnnotationImage(InstructionStraddlingNMIWaitStates);
        UpdateAnnotationImage(Z80Halted);
        UpdateAnnotationImage(UserProgramInstructionStartPositions);
}

void TForm1::UpdateAnnotationImage(TMenuItem* item)
{
        item->ImageIndex = item->Checked? item->ImageIndex | ColourIndex_Tick : item->ImageIndex & ~ColourIndex_Tick;
}

void TForm1::UpdateEmulatorAnnotationSettings()
{
        emulator.ColouriseHorizontalSyncPulse = HorizontalSyncPulse->Checked;
        emulator.ColouriseVerticalSyncPulse = VerticalSyncPulse->Checked;
        emulator.ColouriseRomDisplayDriver = RomDisplayDriver->Checked;
        emulator.ColouriseBackPorch = BackPorch->Checked;
        emulator.ColouriseNonMaskableInterruptResponse = NonMaskableInterruptResponse->Checked;
        emulator.ColouriseNonMaskableInterruptResponseWaitStates = NonMaskableInterruptResponseWaitStates->Checked;
        emulator.ColouriseNonMaskableInterruptServiceRoutine = NonMaskableInterruptServiceRoutine->Checked;
        emulator.ColouriseNonMaskableInterruptServiceRoutineRecursion = NonMaskableInterruptServiceRoutineRecursion->Checked;
        emulator.ColouriseMaskableInterruptResponse = MaskableInterruptResponse->Checked;
        emulator.ColouriseMaskableInterruptServiceRoutine = MaskableInterruptServiceRoutine->Checked;
        emulator.ColouriseInstructionStraddlingNMI = InstructionStraddlingNMI->Checked;
        emulator.ColouriseInstructionStraddlingNMIWaitStates = InstructionStraddlingNMIWaitStates->Checked;
        emulator.ColouriseZ80Halted = Z80Halted->Checked;
        emulator.ColouriseUserProgramInstructionStartPositions = UserProgramInstructionStartPositions->Checked;
}

void __fastcall TForm1::DeleteAllClick(TObject *Sender)
{
        DIR *dir;
        struct dirent *ent;

        AnsiString iniFile;
        char iniPath[256];
        if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, iniPath))
        {
                if ((dir = opendir(emulator.configpath)) != NULL)
                {
                        while ((ent = readdir(dir)) != NULL)
                        {
                                AnsiString FileName, Extension;

                                FileName=ent->d_name;
                                Extension=FileNameGetExt(FileName);

                                if (Extension == ".INI" && !FileName.Pos("&") && FileName != "EightyOne.ini")
                                {
                                        iniFile = iniPath;
                                        if (iniFile[iniFile.Length()] != '\\') iniFile += "\\";
                                        iniFile += iniFolder;
                                        iniFile += FileName;

                                        DeleteFile(iniFile);
                                }
                        }
                        closedir(dir);
                }

                BuildConfigMenu();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DeleteConfigItem1Click(TObject *Sender)
{
        AnsiString iniFile;
        char iniPath[256];
        if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, iniPath))
        {
                iniFile = iniPath;
                if (iniFile[iniFile.Length()] != '\\') iniFile += "\\";
                iniFile += iniFolder;
                AnsiString name = ((TMenuItem *)Sender)->Caption;
                iniFile += name;
                iniFile += ".ini";

                DeleteFile(iniFile);

                BuildConfigMenu();
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::divIDEJumperEClosedClick(TObject *Sender)
{
        divIDEJumperEClosed->Checked = !divIDEJumperEClosed->Checked;
        machine.divIDEJumperEClosed = divIDEJumperEClosed->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckForUpdatesClick(TObject *Sender)
{
        ShellExecute(NULL, "open", "https://sourceforge.net/projects/eightyone-sinclair-emulator/", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::WriteProtect8KRAMClick(TObject *Sender)
{
        WriteProtect8KRAM->Checked = !WriteProtect8KRAM->Checked;
        zx81.RAM816kWriteProtected = WriteProtect8KRAM->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetToDefaultSettingsClick(TObject *Sender)
{
        AnsiString iniFile;
        char iniPath[256];
        if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, iniPath))
        {
                iniFile = iniPath;
                if (iniFile[iniFile.Length()] != '\\') iniFile += "\\";
                iniFile += iniFolder;
                iniFile += "EightyOne.ini";

                DeleteFile(iniFile);

                Restart = true;
                Close();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ReleaseHistoryNotesClick(TObject *Sender)
{
        AnsiString releaseHistoryFile = emulator.cwd;
        releaseHistoryFile += "Release history.txt";
        ShellExecute(NULL, "open", releaseHistoryFile.c_str(), "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetSpeechClick(TObject *Sender)
{
        insertWaitsWhileSP0256Busy = false;
        sp0256_AL2.Reset();        
}
//---------------------------------------------------------------------------
           
void __fastcall TForm1::EnableJoystick1AutoFireClick(TObject *Sender)
{
        EnableJoystick1AutoFire->Checked = !EnableJoystick1AutoFire->Checked; 
        machine.joystick1AutoFireEnabled = EnableJoystick1AutoFire->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::EnableJoystick2AutoFireClick(TObject *Sender)
{
        EnableJoystick2AutoFire->Checked = !EnableJoystick2AutoFire->Checked;
        machine.joystick2AutoFireEnabled = EnableJoystick2AutoFire->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ZXCFUploadJumperClosedClick(TObject *Sender)
{
        ZXCFUploadJumperClosed->Checked = !ZXCFUploadJumperClosed->Checked;
        machine.zxcfUploadJumperClosed = ZXCFUploadJumperClosed->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ConnectSpectrum128KeypadClick(TObject *Sender)
{
        ConnectSpectrum128Keypad->Checked = !ConnectSpectrum128Keypad->Checked;
        spectrum.spectrum128Keypad = (CFGBYTE)(ConnectSpectrum128Keypad->Checked ? 1 : 0);
        Keyboard->KbChange();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ConnectJoystick1Click(TObject *Sender)
{
        ConnectJoystick1->Checked = !ConnectJoystick1->Checked;
        machine.joystick1Connected = ConnectJoystick1->Checked;

        bool joystick2MappedToGameController = (machine.joystick2Controller >= 0);
        if (!joystick2MappedToGameController)
        {
                machine.joystick2Connected = 0;
        }

        UpdateJoystickMenuOptions();
        HW->UpdateSinclairJoystickKeys();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ConnectJoystick2Click(TObject *Sender)
{
        ConnectJoystick2->Checked = !ConnectJoystick2->Checked;
        machine.joystick2Connected = ConnectJoystick2->Checked;

        bool joystick1MappedToGameController = (machine.joystick1Controller >= 0);
        if (!joystick1MappedToGameController)
        {
                machine.joystick1Connected = 0;
        }

        UpdateJoystickMenuOptions();
        HW->UpdateSinclairJoystickKeys();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ZoomFullScreenClick(TObject *Sender)
{
        char escKey = VK_ESCAPE;
        FormKeyPress(Sender, escKey);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SimpleIdeRomEnabledClick(TObject *Sender)
{
        SimpleIdeRomEnabled->Checked = !SimpleIdeRomEnabled->Checked;
        machine.simpleIdeRomEnabled = SimpleIdeRomEnabled->Checked;
}
//---------------------------------------------------------------------------

void TForm1::BuildMemotechInterfaceSelection()
{
        Form1->SwitchOnZ80Assembler->Enabled = zx81.z80Assembler == 0 ? false : true;
        Form1->SwitchOnMemocalc->Enabled     = zx81.memocalc     == 0 ? false : true;
//        Form1->SwitchOnMemotext->Enabled     = zx81.memotext     == 0 ? false : true;

        Form1->SwitchOnZ80Assembler->Checked = zx81.z80AssemblerOn == 0 ? false : true;
        Form1->SwitchOnMemocalc->Checked     = zx81.memocalcOn     == 0 ? false : true;
//        Form1->SwitchOnMemotext->Checked     = zx81.memotextOn     == 0 ? false : true;
}
//---------------------------------------------------------------------------

void TForm1::BuildMenuJoystickSelection()
{
        if (machine.joystick1Controller >= 0 && !controllerPresent[machine.joystick1Controller])
        {
                machine.joystick1Controller = -1;
        }

        if (machine.joystick2Controller >= 0 && !controllerPresent[machine.joystick2Controller])
        {
                machine.joystick2Controller = -1;
        }

        SelectJoystick1->Clear();
        SelectJoystick2->Clear();

        for (int i = -1; i < 16; i++)
        {
                TMenuItem* selectJoystick1SubMenuEntry = new TMenuItem(SelectJoystick1);
                TMenuItem* selectJoystick2SubMenuEntry = new TMenuItem(SelectJoystick2);

                AnsiString menuEntryCaption;
                if (i >= 0)
                {
                        menuEntryCaption = "Game Controller " + IntToStr(i + 1);
                }
                else
                {
                        menuEntryCaption = "None";
                }

                SelectJoystick1->Add(selectJoystick1SubMenuEntry);
                selectJoystick1SubMenuEntry->Caption = menuEntryCaption.c_str();
                selectJoystick1SubMenuEntry->Tag = i;
                selectJoystick1SubMenuEntry->Visible = (i== -1) || controllerPresent[i];
                selectJoystick1SubMenuEntry->OnClick = SelectJoystick1Click;

                SelectJoystick2->Add(selectJoystick2SubMenuEntry);
                selectJoystick2SubMenuEntry->Caption = menuEntryCaption.c_str();
                selectJoystick2SubMenuEntry->Tag = i;
                selectJoystick2SubMenuEntry->Visible = (i== -1) || controllerPresent[i];
                selectJoystick2SubMenuEntry->OnClick = SelectJoystick2Click;

                if (machine.joystick1Controller == i)
                {
                        selectJoystick1SubMenuEntry->Checked = true;
                        if (i >= 0) selectJoystick2SubMenuEntry->Enabled = false;
                }
                
                if (machine.joystick2Controller == i)
                {
                        selectJoystick2SubMenuEntry->Checked = true;
                        if (i >= 0) selectJoystick1SubMenuEntry->Enabled = false;
                }
        }

        UpdateJoystickMenuOptions();
}

void TForm1::UpdateJoystickMenuOptions()
{
        bool joystickInterfaceSelected     = (machine.joystickInterfaceType != JOYSTICK_NONE);
        bool twinJoystickInterfaceSelected = (machine.joystickInterfaceType == JOYSTICK_INTERFACE2 || machine.joystickInterfaceType == JOYSTICK_TIMEX);

        SelectJoystick1->Enabled = joystickInterfaceSelected;
        SelectJoystick2->Enabled = twinJoystickInterfaceSelected;

        SetJoystick1Controller(machine.joystick1Controller);
        SetJoystick2Controller(machine.joystick2Controller);

        bool joystick1MappedToGameController = (machine.joystick1Controller >= 0);
        bool joystick1Available = (joystickInterfaceSelected && (joystick1MappedToGameController || emulator.UseNumericPadForJoystick1 != 0));
        ConnectJoystick1->Enabled = joystick1Available;
        ConnectJoystick1->Checked = machine.joystick1Connected;
        EnableJoystick1AutoFire->Enabled = machine.joystick1Connected;
        EnableJoystick1AutoFire->Checked = machine.joystick1AutoFireEnabled;

        bool joystick2MappedToGameController = (machine.joystick2Controller >= 0);
        bool joystick2Available = (twinJoystickInterfaceSelected && (joystick2MappedToGameController || emulator.UseNumericPadForJoystick2 != 0));
        ConnectJoystick2->Enabled = joystick2Available;
        ConnectJoystick2->Checked = machine.joystick2Connected;
        EnableJoystick2AutoFire->Enabled = machine.joystick2Connected;
        EnableJoystick2AutoFire->Checked = machine.joystick2AutoFireEnabled;
}

void __fastcall TForm1::SelectJoystick1Click(TObject *Sender)
{
        TMenuItem* menuItem = (TMenuItem*)Sender;

        if (menuItem->Tag == -1 || (menuItem->Tag != machine.joystick1Controller && menuItem->Tag != machine.joystick2Controller))
        {
                machine.joystick1Controller = (CFGBYTE)menuItem->Tag;
                BuildMenuJoystickSelection();
        }
}

void __fastcall TForm1::SelectJoystick2Click(TObject *Sender)
{
        TMenuItem* menuItem = (TMenuItem*)Sender;

        if (menuItem->Tag == -1 || (menuItem->Tag != machine.joystick2Controller && menuItem->Tag != machine.joystick1Controller))
        {
                machine.joystick2Controller = (CFGBYTE)menuItem->Tag;
                BuildMenuJoystickSelection();
        }
}
//---------------------------------------------------------------------------
/*
void __fastcall TForm1::SwitchOnMemotextClick(TObject *Sender)
{
        SwitchOnMemotext->Checked = !SwitchOnMemotext->Checked;
        zx81.memotextOn = (CFGBYTE)(SwitchOnMemotext->Checked ?  1 : 0);
}
*/
//---------------------------------------------------------------------------

void __fastcall TForm1::SwitchOnMemocalcClick(TObject *Sender)
{
        SwitchOnMemocalc->Checked = !SwitchOnMemocalc->Checked;
        zx81.memocalcOn = (CFGBYTE)(SwitchOnMemocalc->Checked ?  1 : 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SwitchOnZ80AssemblerClick(TObject *Sender)
{
        SwitchOnZ80Assembler->Checked = !SwitchOnZ80Assembler->Checked;
        zx81.z80AssemblerOn = (CFGBYTE)(SwitchOnZ80Assembler->Checked ?  1 : 0);
}
//---------------------------------------------------------------------------

