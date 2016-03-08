#include "z80.h"
#include "zx81.h"
#include "accurate.h"
#include "snap.h"
#include "zx81config.h"

#define LOOPER  190

int framepos=0;
int NMI_generator=0;
int HSYNC_generator=0;
int sync_len=0, sync_valid=0;
int outputbyte=-1;
int rowcounter=0;
int nmi_counter=LOOPER;
int hsync_counter=LOOPER+4;

BYTE scanline[800*50];
int scanline_len=0;


void zx81_accurate_init(void)
{
        memory_load("zx81.rom", 0, 8192);
        accurate_rom_patch();
        zx81.fastdirty=0;

        NMI_generator=0;
        HSYNC_generator=0;
        sync_len=0;
        sync_valid=0;

        z80_reset();
}
void AccurateOutput(BYTE data, int address)
{
        int addr, inv=0;

        if (data&128)
        {
                inv=1;
                data &= 127;
        }

        addr=z80.i*256 + data*8 + rowcounter;

        outputbyte=inv ? (~memory[addr])&255 : memory[addr];
}

int zx81_do_accurate()
{
        int ts=0,i;
        int framepos=0;

        scanline_len=0;
        do
        {

                ts = z80_do_opcode();

                if (outputbyte==-1)
                {
                        for(i=0; i<ts; i+= 4)
                        scanline[scanline_len++] = HSYNC_generator ? 0:255;
                }
                else
                {
                        scanline[scanline_len++]=outputbyte;
                        outputbyte=-1;
                }

                nmi_counter -= ts;

                if (nmi_counter<=0)
                {
                        nmi_counter += LOOPER;
                        if (NMI_generator) z80_nmi();
                }

                hsync_counter -= ts;

                if (hsync_counter<=0)
                {
                        if (HSYNC_generator)
                        {
                                //scanline_len += (hsync_counter-3)/4;

                                sync_len=20;
                                sync_valid=1;
                                scanline[scanline_len++] = 255;
                                scanline[scanline_len++] = 255;

                                rowcounter = (++rowcounter)&7;
                        }
                        hsync_counter +=LOOPER;
                }

                if (!HSYNC_generator)
                {
                        sync_len += ts;
                        rowcounter=7;
                }

                if ((!(z80.r & 64)))
                {
                        z80_interrupt();
                }

                if (sync_valid) return(sync_valid);
        } while(scanline_len<1000);

        return(sync_valid);
}

void accurate_rom_patch(void)
{
    // Patch the 'SAVE' ROM routine with an illegal operation, which we
    // trap in the Z80 emulation so the file can be saved to disk instead of
    // the ROM trying to output it to TAPE via the ULA
//    memory[0x2FC] = 0xED; // ED FD    ; the illegal Z80 op we use for SAVE
//    memory[0x2FD] = 0xFD; //
//    memory[0x2FE] = 0xC3; // JP 0207h ; this is where the ZX81 ROM jumps to after
//    memory[0x2FF] = 0x7;  //          ; saving a program to tape
//    memory[0x300] = 0x2;  //

    // Patch the 'LOAD' ROM routine with an illegal op, which is trapped
    // just like the SAVE patch above.
    memory[0x347] = 0xEB; //        OUT (2),A  ; Tell the emulator we are waiting
                      //
    memory[0x348] = 0xD3; //
    memory[0x349] = 0x02; //
    memory[0x34A] = 0xC3; // JP 0207h ; this is where the ZX81 ROM jumps to after
    memory[0x34B] = 0x7;  //          ; loading a program from tape
    memory[0x34C] = 0x2;  //
}

