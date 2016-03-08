#include "z80.h"
#include "zx81config.h"

void ace_do_code(int ts)
{
        if (zx81.single_step) ts=1;

        while(ts>0)
        {
                int i;

                i = z80_do_opcode();
                DebugUpdate();
                ts = ts - i;
                ZXPrinterClockTick();
        }
}

ace_do_interupt()
{
        z80_interrupt();
}
