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
USERES("EightyOne.res");
USEUNIT("z80\z80_ops.c");
USEUNIT("z80\z80.c");
USEUNIT("kbstatus.cpp");
USEFORM("About_.cpp", About);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("Speed_.cpp", Speed);
USEFORM("kb_.cpp", Kb);
USEUNIT("zx81\snap.cpp");
USEUNIT("AccDraw_.cpp");
USEFORM("zxprinter\zxprinter.cpp", Printer);
USEUNIT("wavtape\WavFile.cpp");
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEUNIT("wavtape\WavCInterface.cpp");
USEFORM("DebugWin\Debug.cpp", Dbg);
USEUNIT("Utils.cpp");
USEFORM("Artifacts_.cpp", Artifacts);
USEUNIT("sound\dxsound.c");
USEUNIT("sound\sound.c");
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEFORM("MemSave_.cpp", MemSave);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEUNIT("peripherals\dev8255.c");
USEFORM("SerialPort.cpp", SerialConfig);
USEUNIT("tzx\tzxevent.cpp");
USEUNIT("tzx\tzxEventTurbo.cpp");
USEUNIT("tzx\tzxfile.cpp");
USEUNIT("tzx\tzxload.cpp");
USEFORM("tzx\TZXMan.cpp", TZX);
USEUNIT("tzx\tzxsave.cpp");
USEUNIT("tzx\tzxadd.cpp");
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("FullScreen.cpp", FSSettings);
USEUNIT("zx81config.cpp");
USEUNIT("ace\ace.cpp");
USEUNIT("Spectrum\spec48.cpp");
USEUNIT("tzx\tzxEventGeneral.cpp");
USEUNIT("tzx\tzxEventMisc.cpp");
USERC("WinXP.rc");
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("HW_.cpp", HW);
USEFORM("Interface1.cpp", IF1);
USEUNIT("Spectrum\spec48snap.cpp");
USEUNIT("lib765\765drive.c");
USEUNIT("lib765\765dsk.c");
USEUNIT("lib765\765fdc.c");
USEUNIT("lib765\765ldsk.c");
USEUNIT("lib765\error.c");
USEFORM("Plus3Drives.cpp", P3Drive);
USEUNIT("floppy.c");
USEUNIT("ide\ide.cpp");
USEFORM("MakeHDF.cpp", CreateHDF);
USELIB("ddraw.lib");
USEUNIT("68k\68000.c");
USEUNIT("ql\ql.cpp");
USEUNIT("68k\op68kadd.c");
USEUNIT("68k\op68ksub.c");
USEUNIT("68k\op68karith.c");
USEUNIT("68k\op68klogop.c");
USEUNIT("68k\op68kmisc.c");
USEUNIT("68k\op68kmove.c");
USEUNIT("68k\op68kshift.c");
USEUNIT("1772\1770disk.c");
USEUNIT("1772\wd1770.c");
USEUNIT("sp0256.c");
USEFORM("Parallel.cpp", ParallelPort);
USEUNIT("sound\midi.c");
USEFORM("midifrm.cpp", MidiForm);
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("Debug68k\debug68.cpp", Debug68k);
USEFORM("SplashScreen.cpp", Splash);
USEUNIT("rzx\rzx.c");
USEUNIT("Debug68k\d68k.c");
USEFORM("main_.cpp", Form1);
USEUNIT("IECBus\iecbus.c");
USEUNIT("1541\1541.c");
USEFORM("memoryWindow.cpp", MemoryWindow);
USEUNIT("symbolstore.cpp");
USEUNIT("zx81\zx81.cpp");
USEFORM("SymBrowse.cpp", SymbolBrowser);
USELIB("libdsk.lib");
USELIB("zlib.lib");
USEUNIT("zx81\rompatch.cpp");
USEUNIT("RomCartridge\IF2RomCartridge.cpp");
USEUNIT("Spectra\Spectra.cpp");
USEUNIT("zxpand\ZXpand-emu.cpp");
USEUNIT("zxpand\zxpandclass.cpp");
USEUNIT("zxpand\zxpandcore.cpp");
USEUNIT("zxpand\diskio.cpp");
USEUNIT("zxpand\ff.cpp");
USEUNIT("zxpand\wildcard.cpp");
USEUNIT("zxpand\zxpandcom.cpp");
USEUNIT("zxpand\smbsd.cpp");
USEUNIT("zxpand\js.cpp");
USEUNIT("zxpand\dirlib.cpp");
USEUNIT("Chroma\Chroma.cpp");
//---------------------------------------------------------------------------
#include "SplashScreen.h"
#include "main_.h"
//---------------------------------------------------------------------------
char **CommandLine;
TSplash *spl;
bool ShowSplash=true;

extern HANDLE Mutex;

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

        Mutex=NULL;

        try
        {
                 Application->Initialize();
                 Application->Title = "EightyOne";

                 #include "splashImpl.cpp"
                 #if 0
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TAbout), &About);
                 Application->CreateForm(__classid(TKeyboard), &Keyboard);
                 Application->CreateForm(__classid(TSpeed), &Speed);
                 Application->CreateForm(__classid(TKb), &Kb);
                 Application->CreateForm(__classid(TWavLoad), &WavLoad);
                 Application->CreateForm(__classid(TDbg), &Dbg);
                 Application->CreateForm(__classid(TMemoryWindow), &MemoryWindow);
                 Application->CreateForm(__classid(TPrinter), &Printer);
                 Application->CreateForm(__classid(TArtifacts), &Artifacts);
                 Application->CreateForm(__classid(TSoundOutput), &SoundOutput);
                 Application->CreateForm(__classid(THistoryBox), &HistoryBox);
                 Application->CreateForm(__classid(TMemSave), &MemSave);
                 Application->CreateForm(__classid(TZX97Dialog), &ZX97Dialog);
                 Application->CreateForm(__classid(TSerialConfig), &SerialConfig);
                 Application->CreateForm(__classid(TTZX), &TZX);
                 Application->CreateForm(__classid(TEditPauseForm), &EditPauseForm);
                 Application->CreateForm(__classid(TEditArchiveInfo), &EditArchiveInfo);
                 Application->CreateForm(__classid(TEditTextForm), &EditTextForm);
                 Application->CreateForm(__classid(TEditHWInfoForm), &EditHWInfoForm);
                 Application->CreateForm(__classid(TEditDataForm), &EditDataForm);
                 Application->CreateForm(__classid(TEditValue), &EditValue);
                 Application->CreateForm(__classid(TFSSettings), &FSSettings);
                 Application->CreateForm(__classid(TEditGeneralForm), &EditGeneralForm);
                 Application->CreateForm(__classid(TIF1), &IF1);
                 Application->CreateForm(__classid(TP3Drive), &P3Drive);
                 Application->CreateForm(__classid(TCreateHDF), &CreateHDF);
                 Application->CreateForm(__classid(THW), &HW);
                 Application->CreateForm(__classid(TParallelPort), &ParallelPort);
                 Application->CreateForm(__classid(TMidiForm), &MidiForm);
                 Application->CreateForm(__classid(TZipFile), &ZipFile);
                 Application->CreateForm(__classid(TDebug68k), &Debug68k);
                 Application->CreateForm(__classid(TSymbolBrowser), &SymbolBrowser);
                 #endif

                 Application->ProcessMessages();
                 delete spl;

                 Form1->AnimTimer1->Enabled=true;
                 Form1->Timer2->Enabled=true;
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }

        if (Mutex) CloseHandle(Mutex);

        return 0;
}
//---------------------------------------------------------------------------
