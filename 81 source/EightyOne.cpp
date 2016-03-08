//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("EightyOne.res");
USEFORM("main_.cpp", Form1);
USEUNIT("z80\z80_ops.c");
USEUNIT("z80\z80.c");
USEUNIT("FastDraw_.cpp");
USEUNIT("z80thread.cpp");
USEUNIT("kbstatus.cpp");
USEFORM("About_.cpp", About);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("Speed_.cpp", Speed);
USEFORM("kb_.cpp", Kb);
USEFORM("Hardware_.cpp", Hardware);
USEUNIT("zx81\fastdirty.c");
USEUNIT("zx81\snap.c");
USEUNIT("zx81\zx81.c");
USEUNIT("zx81\zx81config.c");
USEUNIT("zx81\accurate.c");
USEUNIT("AccDraw_.cpp");
USEFORM("TapeMan\TapeMan_.cpp", Tape);
USEUNIT("TapeMan\TapeCInterface.cpp");
USEFORM("zxprinter\zxprinter.cpp", Printer);
USEUNIT("zx81\ace.c");
USEUNIT("wavtape\WavFile.cpp");
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEUNIT("wavtape\WavCInterface.cpp");
USEFORM("DebugWin\Debug.cpp", Dbg);
USEFORM("zxprinter\PrintOptions_.cpp", PrintOptions);
USEFORM("DebugWin\AddBreak_.cpp", NewBreakForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "EightyOne";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TAbout), &About);
                 Application->CreateForm(__classid(TKeyboard), &Keyboard);
                 Application->CreateForm(__classid(TSpeed), &Speed);
                 Application->CreateForm(__classid(TKb), &Kb);
                 Application->CreateForm(__classid(THardware), &Hardware);
                 Application->CreateForm(__classid(TTape), &Tape);
                 Application->CreateForm(__classid(TPrinter), &Printer);
                 Application->CreateForm(__classid(TWavLoad), &WavLoad);
                 Application->CreateForm(__classid(TDbg), &Dbg);
                 Application->CreateForm(__classid(TPrintOptions), &PrintOptions);
                 Application->CreateForm(__classid(TNewBreakForm), &NewBreakForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
