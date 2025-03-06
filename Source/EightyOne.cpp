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

#include <vcl4.h>
#pragma hdrstop
USERES("EightyOne.res");
USEUNIT("z80\z80_ops.c");
USEUNIT("z80\z80.c");
USEUNIT("kbstatus.cpp");
USEFORM("About_.cpp", About);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("Speed_.cpp", Speed);
USEFORM("kb_.cpp", Kb);
USEFORM("DebugWin\Debug.cpp", Dbg);
USEUNIT("Utils.cpp");
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEFORM("MemSave_.cpp", MemSave);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEUNIT("peripherals\dev8255.c");
USEFORM("SerialPort.cpp", SerialConfig);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("DebugWin\SetBreakpoint_.cpp", SetBreakpoint);
USEUNIT("zx81config.cpp");
USERC("WinXP.rc");
USEFORM("HW_.cpp", HW);
USELIB("ddraw.lib");
USEFORM("Parallel.cpp", ParallelPort);
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("SplashScreen.cpp", Splash);
USEUNIT("rzx\rzx.c");
USEFORM("main_.cpp", Form1);
USEUNIT("IECBus\iecbus.c");
USEUNIT("1541\1541.c");
USEFORM("memoryWindow.cpp", MemoryWindow);
USEUNIT("symbolstore.cpp");
USEFORM("SymBrowse.cpp", SymbolBrowser);
USELIB("libdsk.lib");
USELIB("zlib.lib");
USEUNIT("RomCartridge\IF2RomCartridge.cpp");
USEUNIT("Spectra\Spectra.cpp");
USEUNIT("Chroma\Chroma.cpp");
USEFORM("DebugWin\SearchSequence_.cpp", SearchSequence);
USEFORM("LiveMemoryWindow_.cpp", LiveMemoryWindow);
USEFORM("DebugWin\Profiler.cpp", Profiler);
USEFORM("DebugWin\ProfSampEdit.cpp", ProfileSampleEdit);
USEFORM("DebugWin\ProfilePlot_.cpp", ProfilePlot);
USEFORM("KeyboardFunctions_.cpp", KeyboardFunctions);
USEFORM("BasicLister\BasicLister_.cpp", BasicLister);
USEUNIT("BasicLister\IBasicLister.cpp");
USEUNIT("BasicLoader\IBasicLoader.cpp");
USEFORM("BasicLoader\BasicLoaderOptions_.cpp", LoadBasicListingOptionsForm);
USEFORM("BasicLister\BasicListerOptions_.cpp", SaveBasicListingOptionsForm);
USEFORM("BasicLister\BasicListingFormatInfo_.cpp", BasicListingFormatInfoForm);
USELIB("..\components\directsound\dsound.lib");
USEUNIT("sound\sound.cpp");
USEUNIT("sound\SoundDX.cpp");
USEUNIT("Ace\ace.cpp");
USEUNIT("zx81\rompatch.cpp");
USEUNIT("zx81\snap.cpp");
USEUNIT("zx81\zx80BasicLister.cpp");
USEUNIT("zx81\zx80BasicLoader.cpp");
USEUNIT("zx81\zx81.cpp");
USEUNIT("zx81\zx81BasicLister.cpp");
USEUNIT("zx81\zx81BasicLoader.cpp");
USEUNIT("Spectrum\spec48.cpp");
USEUNIT("Spectrum\spec48BasicLister.cpp");
USEUNIT("Spectrum\spec48snap.cpp");
USEUNIT("Spectrum\spec128BasicLister.cpp");
USEUNIT("Spectrum\specBasicLoader.cpp");
USEUNIT("1772\1770disk.c");
USEUNIT("ide\ide.cpp");
USEUNIT("tzx\tzxadd.cpp");
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEUNIT("tzx\tzxevent.cpp");
USEUNIT("tzx\tzxEventGeneral.cpp");
USEUNIT("tzx\tzxEventMisc.cpp");
USEUNIT("tzx\tzxEventTurbo.cpp");
USEUNIT("tzx\TZXFILE.cpp");
USEUNIT("tzx\tzxload.cpp");
USEFORM("tzx\TZXMan.cpp", TZX);
USEUNIT("tzx\tzxsave.cpp");
USEUNIT("wavtape\WavCInterface.cpp");
USEUNIT("wavtape\WavFile.cpp");
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEUNIT("1772\wd1770.c");
USEUNIT("zxpand\dirlib.cpp");
USEUNIT("zxpand\diskio.cpp");
USEUNIT("zxpand\ff.cpp");
USEUNIT("zxpand\smbsd.cpp");
USEUNIT("zxpand\wildcard.cpp");
USEUNIT("zxpand\zxpandclass.cpp");
USEUNIT("zxpand\zxpandcom.cpp");
USEUNIT("zxpand\zxpandcore.cpp");
USEUNIT("zxpand\ZXpand-emu.cpp");
USEFORM("zxprinter\zxprinter.cpp", ZXPrinter);
USEFORM("Plus3Drives.cpp", P3Drive);
USEUNIT("floppy.c");
USEFORM("MakeHDF.cpp", CreateHDF);
USEUNIT("lib765\765drive.c");
USEUNIT("lib765\765dsk.c");
USEUNIT("lib765\765fdc.c");
USEUNIT("lib765\765ldsk.c");
USEUNIT("lib765\error.c");
USEUNIT("AccDraw_.cpp");
USEFORM("Fullscreen.cpp", FSSettings);
USEFORM("Artifacts_.cpp", Artifacts);
USEFORM("sound\SoundForm.cpp", MidiForm);
USEFORM("Interface1.cpp", IF1);
USEUNIT("SP0256\sp0256_al2.cpp");
USEUNIT("SP0256\sp0256drv.cpp");
USEUNIT("SP0256\sp0256.c");
USEUNIT("SP0256\sp0256_012.cpp");
USEUNIT("Digitalker\Digitalk.cpp");
USEUNIT("Digitalker\Digitalkdrv.cpp");
USEUNIT("Joystick\Joystick.cpp");
USEUNIT("sound\midi.cpp");
USEFORM("Keypad_.cpp", Keypad128);
USEUNIT("Keypad\keypad.cpp");
USEUNIT("BuildLineQueue.cpp");
//---------------------------------------------------------------------------
#include "SplashScreen.h"
#include "main_.h"
//---------------------------------------------------------------------------
char **CommandLine;
TSplash *spl;
bool ShowSplash=true;
bool Restart=false;
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdline, int)
{
        int i;
        char *p, *CmdLineRaw;
        bool quote;

        spl = new TSplash(Splash);

        CmdLineRaw=(char *)malloc(strlen(cmdline)+2);
        strcpy(CmdLineRaw,cmdline);

        p=CmdLineRaw;
        quote=false;

        while(*p)
        {
                if (*p=='\"') quote=!quote;
                if ((*p==' ') && (quote==false)) *p=0;
                p++;
        }
        *++p=0;

        i=0;
        p=CmdLineRaw;
        while(strlen(p))
        {
                i++;
                p+=strlen(p)+1;
        }
        i++;

        CommandLine=(char **)malloc(i*sizeof(char *));

        p=CmdLineRaw;
        i=0;
        while(strlen(p))
        {
                CommandLine[i++]=p;
                p+=strlen(p)+1;
        }
        CommandLine[i]=NULL;

        i=0;
        while(CommandLine[i])
        {
                p=CommandLine[i];

                if ((p[0]=='\"') && (p[strlen(p)-1]=='\"'))
                {
                        p[strlen(p)-1]=0;
                        CommandLine[i]++;
                }
                i++;
        }

        try
        {
                Application->Initialize();
                Application->Title = "EightyOne";
                 #include "splashImpl.cpp"

                Application->ProcessMessages();
                delete spl;

                Form1->GatherWindowsIfRequired();
                Form1->Caption = "EightyOne";
                Form1->Timer2->Enabled=true;
                Form1->RunFrameEnable=1;
                Form1->SetFocus();

                Application->Run();

                if (Restart)
                {
                        AnsiString exeFile = Application->ExeName;
                        ShellExecute(NULL, "open", exeFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
                }
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }

        return 0;
}
//---------------------------------------------------------------------------
