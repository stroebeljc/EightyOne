//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "z80thread.h"
#include "zx81.h"
#include "fastdirty.h"
#include "z80.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall Tz80::Tz80(bool CreateSuspended)
        : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall Tz80::Execute()
{
        //---- Place thread code here ----
        do
        {
                if (!zx81_stop)
                {
                        int ts=65000;

                        while(ts>0) ts -= z80_do_opcode();
                        Suspend();
                }

        } while(true);
}
//---------------------------------------------------------------------------
