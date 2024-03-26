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

#include <vcl.h>
#pragma hdrstop
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("SymBrowse.cpp", SymbolBrowser);
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEFORM("sound\SoundForm.cpp", MidiForm);
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("SerialPort.cpp", SerialConfig);
USEFORM("Speed_.cpp", Speed);
USEFORM("SplashScreen.cpp", Splash);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEFORM("zxprinter\zxprinter.cpp", ZXPrinter);
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEFORM("tzx\TZXMan.cpp", TZX);
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("BasicLoader\BasicLoaderOptions_.cpp", LoadBasicListingOptionsForm);
USEFORM("BasicLister\BasicListingFormatInfo_.cpp", BasicListingFormatInfoForm);
USEFORM("Artifacts_.cpp", Artifacts);
USEFORM("BasicLister\BasicListerOptions_.cpp", SaveBasicListingOptionsForm);
USEFORM("BasicLister\BasicLister_.cpp", BasicLister);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEFORM("DebugWin\ProfilePlot_.cpp", ProfilePlot);
USEFORM("DebugWin\Profiler.cpp", Profiler);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("Debug68k\debug68.cpp", Debug68k);
USEFORM("DebugWin\Debug.cpp", Dbg);
USEFORM("About_.cpp", About);
USEFORM("LiveMemoryWindow_.cpp", LiveMemoryWindow);
USEFORM("main_.cpp", Form1);
USEFORM("MakeHDF.cpp", CreateHDF);
USEFORM("Plus3Drives.cpp", P3Drive);
USEFORM("memoryWindow.cpp", MemoryWindow);
USEFORM("MemSave_.cpp", MemSave);
USEFORM("Parallel.cpp", ParallelPort);
USEFORM("FullScreen.cpp", FSSettings);
USEFORM("HW_.cpp", HW);
USEFORM("DebugWin\ProfSampEdit.cpp", ProfileSampleEdit);
USEFORM("DebugWin\SearchSequence_.cpp", SearchSequence);
USEFORM("DebugWin\SetBreakpoint_.cpp", SetBreakpoint);
USEFORM("KeyboardFunctions_.cpp", KeyboardFunctions);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("kb_.cpp", Kb);
USEFORM("Interface1.cpp", IF1);
//---------------------------------------------------------------------------
#include "SplashScreen.h"
#include "main_.h"
//---------------------------------------------------------------------------
char **CommandLine;
bool ShowSplash=true;
bool Restart=false;
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdline, int)
{
		int i;
		char *p, *CmdLineRaw;
		bool quote;

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
				TSplash *spl = new TSplash(Application);
				#include "splashImpl.cpp"
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
