//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TapeCInterface.h"
#include "TapeMan_.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)



cdecl void tape_rewind()
{
        Tape->Rewind();
}

int  tape_get_pointer(char *fname, BYTE **ptr)
{
        int len;

        if (!Tape->FindFile(fname)) return(0);

        *ptr = Tape->GetCurrentFile();
        len = Tape->GetCurrentFileLen();
        Tape->NextFile();
        return(len);
}

void tape_new_block(char *name, int len, BYTE * ptr)
{
        Tape->AddBlock(name, ptr, len);
}

