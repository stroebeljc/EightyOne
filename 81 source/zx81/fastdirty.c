#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "zx81.h"
#include "z80\z80.h"
#include "snap.h"
#include "zx81config.h"

int zx81_mode=0;
int booting=1, inputwait=0;

void zx81_do_code(int fast, int slow)
{
        int ts=zx81_mode ? slow:fast;

        if (zx81.single_step) ts=1;

        while(ts>0)
        {
                int i;

                i = z80_do_opcode();
                DebugUpdate();
                ZXPrinterClockTick();
                ts = ts - i;
        }
}

void zx81_do_interupt(void)
{
        int i,keydata,frames;
        int shifted=0;

        BYTE oldlastk1, oldlastk2;
        BYTE newlastk1, newlastk2;

        // If Z80 was halted, re-enable it
        if (z80.halted)
        {
                z80.halted=0;
                z80.pc.w ++;
        }

        //If in slow mode decrement Frames,
        //maintaining most significant bit
        if (readbyte(16443)&128)
        {
                frames = readbyte(16437)<<8 | readbyte(16436);
                frames = ((frames&32767) - 1) | (frames&32768);
                writebyte(16436, frames&255);
                writebyte(16437, frames>>8);
        }

        if (booting) return;

        if (readbyte(16443)&128) zx81_mode=1;
        else zx81_mode=0;

        // Update LASTK sysvar from the current keyboard status

        oldlastk1=readbyte(16421);
        oldlastk2=readbyte(16422);

        newlastk1 = 255;
        newlastk2 = 255;

        if (ZXKeyboard[0] & 1) shifted=1;

        for(i=0; i<8; i++)
        {
                keydata=ZXKeyboard[i];

                //if this is the first row, mask out the shift key
                if (i==0) keydata &= ~kbD0;

                if (keydata)
                {
                        newlastk1 = ~(1<<i);
                        newlastk2 = ~((keydata<<1) | shifted);
                        i=9;
                }
        }

        writebyte(16421, newlastk1);
        writebyte(16422, newlastk2);

        if (!inputwait) return;

        //Set Margin to 55
        writebyte(16424, 55);


        if (readbyte(16423)==255)
        {
                writebyte(16423,15);
        }
        else
        {
                if ( ((newlastk1!=oldlastk1) || (newlastk2!=oldlastk2))
                  && ((newlastk1!=255) && (newlastk2!=255)))
                {
                        writebyte(16443, readbyte(16443) | 1);
        //                writebyte(16423,15);
                }
        }

}

//
// This ROM patch code stolen from vb81 (for the time being)
//
// (c) Chris Cowley  -  http://freestuff.grok.co.uk/vb81/
//

void fast_dirty_rom_patch(void)
{
    // Execute 'out (0),a' when waiting for input, and
    // 'out (1),a when finished waiting (used to paint
    // the screen during idle periods when in fast mode)
    memory[0x4CA] = 0xD3; //       OUT (0),A  ; Tell the emulator we are waiting
    memory[0x4CB] = 0;    //                  ; for keyboard input
    memory[0x4CC] = 0xCB; // loop: BIT 0,(HL) ; Loop until our NMI emulation sets
    memory[0x4CD] = 0x46; //                  ; bit 0 of (CDFLAGS), which indicates
    memory[0x4CE] = 0x28; //       JR Z,loop  ; that a key has been pressed
    memory[0x4CF] = 0xFC; //
    memory[0x4D0] = 0xD3; //       OUT (1),A  ; Tell the emulator that we are no
    memory[0x4D1] = 1;    //                  ; longer waiting for keyboard input
    memory[0x4D2] = 0;    //       NOP        ; NOP an old byte (filler)

// This is required for correct implementation of the PAUSE command, as the
// PAUSE code in a real ZX81 is tied in with the FRAMES counter in the FAST
// mode display loop which we emulate outside of the routines in the ROM.
    memory[0x2A9] = 0xD3; //   OUT (0),A  ; see above
    memory[0x2AA] = 0;
    memory[0x2AB] = 118;  //   HALT       ; ensures the bit of display code that
                      //              ; we loop through only gets executed
                      //              ; once per interrupt (as it would in a
                      //              ; real ZX81)
    memory[0x2AC] = 205;  //   CALL 0229h ; jump out of the display routine
    memory[0x2AD] = 0x29; //              ; early to avoid the nasty code that
    memory[0x2AE] = 2;    //              ; jumps right into the DFILE!
    memory[0x2AF] = 0xD3; //  OUT (1),A   ; see above
    memory[0x2B0] = 1;
    memory[0x2B1] = 201;  //  RET         ; Return to PAUSE command routine

        if (zx81.save_patch)
        {
                // Patch the 'SAVE' ROM routine with an illegal operation, which we
                // trap in the Z80 emulation so the file can be saved to disk instead of
                // the ROM trying to output it to TAPE via the ULA
                memory[0x2FC] = 0xD3; //
                memory[0x2FD] = 0x03; //
                memory[0x2FE] = 0xC3; // JP 0207h ; this is where the ZX81 ROM jumps to after
                memory[0x2FF] = 0x7;  //          ; saving a program to tape
                memory[0x300] = 0x2;  //
        }

        if (zx81.load_patch)
        {
                // Patch the 'LOAD' ROM routine with an illegal op, which is trapped
                // just like the SAVE patch above.
                memory[0x347] = 0xEB; //        OUT (2),A  ; Tell the emulator we are waiting
                memory[0x348] = 0xD3; //
                memory[0x349] = 0x02; //
                memory[0x34A] = 0xC3; // JP 0207h ; this is where the ZX81 ROM jumps to after
                memory[0x34B] = 0x7;  //          ; loading a program from tape
                memory[0x34C] = 0x2;  //
        }
}

void zx81_fastdirty_init()
{
        if (zx81.machine==MACHINEZX81) fast_dirty_rom_patch();

        zx81.fastdirty=1;
        z80_reset();
        booting=1;
        inputwait=0;
}
