//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "zx81.h"
#include "z80thread.h"
#include "kbstatus.h"
#include "snap.h"
#include "fastdirty.h"
#include "zx81config.h"
#include "accurate.h"
#include "ace.h"

#include "main_.h"
#include "fastdraw_.h"
#include "accdraw_.h"
#include "About_.h"
#include "keyboard_.h"
#include "hardware_.h"
#include "speed_.h"
#include "kb_.h"
#include "tapeman_.h"
#include "zxprinter.h"
#include "wavload_.h"
#include "debug.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "dbits"
#pragma link "AnimTimer"
#pragma resource "*.dfm"
TForm1 *Form1;
//Tz80 *z80thread;

extern "C" void z80_reset();

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        FullScreen=false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        DragAcceptFiles(Form1->Handle, true);
        Application->OnMessage = AppMessage;

        load_config();

        if (zx81.fastdirty) FastInit();
        else AccurateInit();

        PCKbInit();

        //DFlipSurface->Width=BaseWidth;
        //DFlipSurface->Height=BaseHeight;
        //ClientWidth=BaseWidth;
        //ClientHeight=BaseHeight;
        //z80thread = new Tz80(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
        DFlipSurface->Width=ClientWidth;
        DFlipSurface->Height=ClientHeight;

        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        if (ClientWidth==BaseWidth && ClientHeight==BaseHeight)
                N1001->Checked=true;
        else if (ClientWidth==(BaseWidth*2) && ClientHeight==(BaseHeight*2))
                N2001->Checked=true;
        else if (ClientWidth==(BaseWidth*4) && ClientHeight==(BaseHeight*4))
                N4001->Checked=true;
        else UserDefined1->Checked=true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
        int i;

        if (zx81_stop) return;

        fps++;
        if (zx81.fastdirty)
        {
                if (zx81.machine==MACHINEACE)
                {
                        AceFastDraw();
                        ace_do_interupt();
                        ace_do_code(Speed->DirtyFastTStates);
                }
                else
                {
                        FastDraw();
                        zx81_do_interupt();
                        //Printer->ClockTick();
                        zx81_do_code(Speed->DirtyFastTStates, Speed->DirtySlowTStates);
                }
        }
        else
        {
                for(i=0; i < (zx81.single_step?1:Speed->AccurateScanlines); i++)
                {
                        zx81_do_accurate();
                        //Printer->ClockTick();
                        AccurateDraw();
                        if (zx81_stop) return;
                }
        }


        //z80_do_opcodes(64000);
        //z80thread->Resume();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        PCKeyDown(Key);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        PCKeyUp(Key);
}
//---------------------------------------------------------------------------
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
                KeyboardMap1->Checked=true;
        }
        else
        {
                Keyboard->Close();
                KeyboardMap1->Checked=false;
        }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Accurate1Click(TObject *Sender)
{
        if (zx81.machine==MACHINEACE) return;

        Accurate1->Checked=true;
        Fast1->Checked=false;
        Interlaced->Enabled=true;
        Speed->ChangeMode();
        //Speed1->Enabled=false;
//        Speed->SlowMode->Position=4;
//        Speed->SlowMode->Enabled=false;
//        Speed->Label2->Enabled=false;
//        Speed->Label6->Enabled=false;
//        Speed->Label7->Enabled=false;
//        Speed->Label8->Enabled=false;

        AccurateInit();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Fast1Click(TObject *Sender)
{
        Accurate1->Checked=false;
        Fast1->Checked=true;
        //Speed1->Enabled=true;
        Speed->ChangeMode();
//        Speed->SlowMode->Enabled=true;
//        Speed->Label2->Enabled=true;
//        Speed->Label6->Enabled=true;
//        Speed->Label7->Enabled=true;
//        Speed->Label8->Enabled=true;

        FastInit();

        if (N1001->Checked)
        {
                ClientWidth=BaseWidth;
                ClientHeight=BaseHeight;
        }

        if (N2001->Checked)
        {
                ClientWidth=BaseWidth*2;
                ClientHeight=BaseHeight*2;
        }

        if (N4001->Checked)
        {
                ClientWidth=BaseWidth*4;
                ClientHeight=BaseHeight*4;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N1001Click(TObject *Sender)
{
        N1001->Checked=true;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth;
        ClientHeight=BaseHeight;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N2001Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=true;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth*2;
        ClientHeight=BaseHeight*2;


}
//---------------------------------------------------------------------------

void __fastcall TForm1::N4001Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=true;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth*4;
        ClientHeight=BaseHeight*4;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::UserDefined1Click(TObject *Sender)
{
        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Display1Click(TObject *Sender)
{
        PCAllKeysUp();
        Hardware->ShowModal();
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
        Kb->ShowModal();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::InsertTape1Click(TObject *Sender)
{
        PCAllKeysUp();
        Tape->LoadTapeBtnClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SaveSnapshot1Click(TObject *Sender)
{
        Timer1->Enabled=false;
        SaveSnapDialog->Execute();
        save_snap(SaveSnapDialog->FileName.c_str());
        Timer1->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadSnapshot1Click(TObject *Sender)
{
        Timer1->Enabled=false;
        LoadSnapDialog->Execute();
        load_snap(LoadSnapDialog->FileName.c_str());
        Timer1->Enabled=true;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Tape1Click(TObject *Sender)
{
        if (Tape1->Checked)
        {
                Tape->Close();
                Tape1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                Tape->Show();
                Tape1->Checked=true;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CloseTape1Click(TObject *Sender)
{
        PCAllKeysUp();
        Tape->SaveTapeBtnClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NewTape1Click(TObject *Sender)
{
        PCAllKeysUp();
        Tape->NewTape();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Toolbar1Click(TObject *Sender)
{
        Toolbar1->Checked = !Toolbar1->Checked;

        if (Toolbar1->Checked)
        {
                ToolBar->Visible = true;
        }
        else
        {
                ToolBar->Visible = false;
        }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetButtonClick(TObject *Sender)
{
        zx81_stop=1;
        zx81_init();
        zx81_init();
        zx81_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InsertTapeButtonClick(TObject *Sender)
{
        PCAllKeysUp();
        Tape->LoadTapeBtnClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CloseTapeButtonClick(TObject *Sender)
{
        PCAllKeysUp();
        Tape->CloseTape();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ResetZX811Click(TObject *Sender)
{
        PCAllKeysUp();
        zx81_stop=1;
        //zx81_init();
        //zx81_init();
        z80_reset();
        zx81_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        PCAllKeysUp();
        Tape->NewTape();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewPrinterClick(TObject *Sender)
{
        ViewPrinter->Checked = !ViewPrinter->Checked;

        if (ViewPrinter->Checked)
        {
                PCAllKeysUp();
                Printer->Show();
        }
        else    Printer->Close();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::InterlacedClick(TObject *Sender)
{
        Interlaced->Checked = !Interlaced->Checked;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
        AnsiString text;

        if (FullScreen) return;
        
        text="EightyOne (";

        if (zx81_stop) text +="Paused)";
        else
        {
                if (zx81.single_step) text +="Debug Mode)";
                else
                {
                        text += fps;
                        text += "fps)";
                }
        }

        Form1->Caption = text;
        fps=0;
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

void __fastcall TForm1::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key==27)
        {
                FullScreen = !FullScreen;

                if (FullScreen)
                {
                        SaveX=Left;
                        SaveY=Top;
                        SaveW=ClientWidth;
                        SaveH=ClientHeight;

                        DFlipSurface->Width=1024;
                        DFlipSurface->Height=768;
                        DFlipSurface->ChangeMode=true;
                        Screen->Cursor = crNone;
                }
                else
                {
                        DFlipSurface->ChangeMode=false;
                        //DFlipSurface->Width=SaveW;
                        //DFlipSurface->Height=SaveH;
                        ClientWidth=SaveW;
                        ClientHeight=SaveH;
                        Left=SaveX;
                        Top=SaveY;
                        Screen->Cursor = crDefault;
                }

        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DebugWinClick(TObject *Sender)
{
        DebugWin->Checked = !DebugWin->Checked;
        if (DebugWin->Checked)
                Dbg->Show();
        else
                Dbg->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AppMessage(TMsg &Msg, bool &Handled)
{
        WORD BufferLength=255;
        AnsiString Filename;
        WORD FileIndex=0;
        WORD QtyDroppedFiles;
        char pDroppedFilename[255];
        WORD DroppedFileLength;

        if (Msg.message == WM_DROPFILES)
        {
                FileIndex = 0xFFFF;
                QtyDroppedFiles = DragQueryFile((void *)Msg.wParam, -1,
                                                pDroppedFilename, BufferLength);

                for(FileIndex=0; FileIndex<=(QtyDroppedFiles - 1); FileIndex++)
                {
                        DroppedFileLength = DragQueryFile((void *)Msg.wParam, FileIndex,
                                                pDroppedFilename, BufferLength);

                        Filename = pDroppedFilename;

                        if (Tape->IsTape(Filename))
                        {
                                Tape->CloseTape();
                                Tape->LoadTapeFile(Filename);
                        }

                        //Label1->Caption=DroppedFilename;
                }

                DragFinish((void *)Msg.wParam);
                Handled = true;
        }
}


void __fastcall TForm1::PauseZX81Click(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        //DebugUpdate();
        Dbg->UpdateVals();
}
//---------------------------------------------------------------------------

