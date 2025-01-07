/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
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
 *
 *
 * EightyOne.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("SymBrowse.cpp", SymbolBrowser);
USEFORM("Speed_.cpp", Speed);
USEFORM("SplashScreen.cpp", Splash);
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEFORM("sound\SoundForm.cpp", MidiForm);
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEFORM("zxprinter\zxprinter.cpp", ZXPrinter);
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEFORM("tzx\TZXMan.cpp", TZX);
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("BasicLoader\BasicLoaderOptions_.cpp", LoadBasicListingOptionsForm);
USEFORM("BasicLister\BasicListerOptions_.cpp", SaveBasicListingOptionsForm);
USEFORM("BasicLister\BasicLister_.cpp", BasicLister);
USEFORM("BasicLister\BasicListingFormatInfo_.cpp", BasicListingFormatInfoForm);
USEFORM("DebugWin\ProfilePlot_.cpp", ProfilePlot);
USEFORM("DebugWin\Profiler.cpp", Profiler);
USEFORM("DebugWin\ProfSampEdit.cpp", ProfileSampleEdit);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEFORM("DebugWin\Debug.cpp", Dbg);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("Artifacts_.cpp", Artifacts);
USEFORM("About_.cpp", About);
USEFORM("DebugWin\SearchSequence_.cpp", SearchSequence);
USEFORM("MakeHDF.cpp", CreateHDF);
USEFORM("memoryWindow.cpp", MemoryWindow);
USEFORM("MemSave_.cpp", MemSave);
USEFORM("main_.cpp", Form1);
USEFORM("LiveMemoryWindow_.cpp", LiveMemoryWindow);
USEFORM("Parallel.cpp", ParallelPort);
USEFORM("SerialPort.cpp", SerialConfig);
USEFORM("Plus3Drives.cpp", P3Drive);
USEFORM("HW_.cpp", HW);
USEFORM("FullScreen.cpp", FSSettings);
USEFORM("DebugWin\SetBreakpoint_.cpp", SetBreakpoint);
USEFORM("Interface1.cpp", IF1);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("kb_.cpp", Kb);
USEFORM("KeyboardFunctions_.cpp", KeyboardFunctions);
//---------------------------------------------------------------------------
#include "SplashScreen.h"
#include "main_.h"
//---------------------------------------------------------------------------
char **CommandLine;
TSplash *spl;
bool ShowSplash=true;
bool Restart=false;
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdline, int)
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
