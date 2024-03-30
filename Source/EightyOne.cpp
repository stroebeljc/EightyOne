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
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("SymBrowse.cpp", SymbolBrowser);
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("sound\SoundForm.cpp", MidiForm);
USEFORM("Speed_.cpp", Speed);
USEFORM("SplashScreen.cpp", Splash);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEFORM("zxprinter\zxprinter.cpp", ZXPrinter);
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEFORM("tzx\TZXMan.cpp", TZX);
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("SerialPort.cpp", SerialConfig);
USEFORM("BasicLoader\BasicLoaderOptions_.cpp", LoadBasicListingOptionsForm);
USEFORM("BasicLister\BasicLister_.cpp", BasicLister);
USEFORM("BasicLister\BasicListingFormatInfo_.cpp", BasicListingFormatInfoForm);
USEFORM("DebugWin\ProfilePlot_.cpp", ProfilePlot);
USEFORM("DebugWin\Profiler.cpp", Profiler);
USEFORM("DebugWin\ProfSampEdit.cpp", ProfileSampleEdit);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEFORM("Debug68k\debug68.cpp", Debug68k);
USEFORM("DebugWin\Debug.cpp", Dbg);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("BasicLister\BasicListerOptions_.cpp", SaveBasicListingOptionsForm);
USEFORM("Artifacts_.cpp", Artifacts);
USEFORM("About_.cpp", About);
USEFORM("main_.cpp", Form1);
USEFORM("MakeHDF.cpp", CreateHDF);
USEFORM("memoryWindow.cpp", MemoryWindow);
USEFORM("LiveMemoryWindow_.cpp", LiveMemoryWindow);
USEFORM("Plus3Drives.cpp", P3Drive);
USEFORM("MemSave_.cpp", MemSave);
USEFORM("Parallel.cpp", ParallelPort);
USEFORM("FullScreen.cpp", FSSettings);
USEFORM("HW_.cpp", HW);
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
_TCHAR **CommandLine;
bool ShowSplash=true;
bool Restart=false;
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR cmdline, int)
{
		int i;
		_TCHAR *p, *CmdLineRaw;
		bool quote;

		CmdLineRaw=(_TCHAR *)malloc(_tcslen(cmdline)+2);
		_tcscpy(CmdLineRaw,cmdline);

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
		while(_tcslen(p))
		{
				i++;
				p+=_tcslen(p)+1;
		}
		i++;

		CommandLine=(_TCHAR **)malloc(i*sizeof(_TCHAR *));

		p=CmdLineRaw;
		i=0;
		while(_tcslen(p))
		{
				CommandLine[i++]=p;
				p+=_tcslen(p)+1;
		}
		CommandLine[i]=NULL;

		i=0;
		while(CommandLine[i])
		{
				p=CommandLine[i];

				if ((p[0]=='\"') && (p[_tcslen(p)-1]=='\"'))
				{
						p[_tcslen(p)-1]=0;
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
                        ZXString exeFile = Application->ExeName;
                        ShellExecute(NULL, _TEXT("open"), exeFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
                }
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }

        return 0;
}
//---------------------------------------------------------------------------
