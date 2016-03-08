#include "z80.h"
#include "zx81.h"
#include "accurate.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"

#define LOOPER  207

#define DEBUG 1

#define HBLANKCOLOUR 0
#define VBLANKCOLOUR 0

extern void debug(int data);
extern void DebugUpdate(void);
extern int RasterY;


int NMI_generator=0;
int HSYNC_generator=0;
int sync_len=0, sync_valid=0;
int rowcounter=0;
int hsync_counter=LOOPER;
int borrow=0;

BYTE shift_register=0, shift_store=0;
BYTE scanline[800*50];
int scanline_len=0;

void zx81_accurate_init(void)
{
        if (zx81.machine==MACHINEZX81) accurate_rom_patch();

        zx81.fastdirty=0;
        NMI_generator=0;
        HSYNC_generator=0;
        sync_len=0;
        sync_valid=0;

        z80_reset();
}

#if DEBUG==1
void add_blank(int tstates, BYTE colour)
{
        while(tstates-- > 0)
        {
                scanline[scanline_len++]=colour;
                scanline[scanline_len++]=colour;
        }
}
#else
#define add_blank(a,b)
#endif

int zx81_do_accurate()
{
        int ts,i;
        static int NMI=0;
        int MaxScanLen;
        scanline_len=0;

        if (zx81.single_step) MaxScanLen=1;
        else MaxScanLen=420;

        if (sync_valid==-1)
        {
                add_blank(borrow, HSYNC_generator ? 255:VBLANKCOLOUR);
                sync_valid=0;
        }

        do
        {
                ts = z80_do_opcode();
                WavClockTick(ts);
                ZXPrinterClockTick();
                if (z80.pc.w == 0x03e5) WavStop();  // Loading Error
                if (z80.pc.w == 0x0206) WavStop();  // Load Success
                if (z80.pc.w == 0x0340) WavStart(); // Start Loading

                shift_store=shift_register;
                for (i=0; i<(ts<<1); i++)
                {
                        scanline[scanline_len++] =
                                HSYNC_generator ? (shift_register&128?0:255):VBLANKCOLOUR;
                        shift_register<<=1;
                }

                if (sync_valid) borrow=ts;

                hsync_counter -= ts;

                if (hsync_counter<=0 && !NMI && NMI_generator)
                {
                        ts=z80_nmi();
                        hsync_counter -= ts;
                        //add_blank(2,14); //HSYNC_generator ? 0:255);
                        //rowcounter=4;
                        NMI=1;
                }

                if (hsync_counter<=0)
                {
                        NMI=0;

                        //if (scanline_len >= -hsync_counter*2)
                        //        scanline_len += hsync_counter*2;

                        //borrow= -hsync_counter;

                        if (HSYNC_generator && sync_len==0)
                        {
                                sync_len=16;
                                sync_valid=1;
                                add_blank(16,HBLANKCOLOUR);
                                //scanline[scanline_len++] = 255;
                                //scanline[scanline_len++] = 255;
                                rowcounter = (++rowcounter)&7;
                                //NMI=0;
                        }
                        hsync_counter += LOOPER;
                }

                if (!HSYNC_generator)
                {
                        sync_len += ts;
                        //add_blank(ts,1);
                        //rowcounter=7;
                        //hsync_counter=LOOPER-32;
                }

                if ((!(z80.r & 64)) && !NMI_generator)
                {
                        ts=z80_interrupt();
                        hsync_counter -=  ts;
                        //add_blank(2,9);
                }

                DebugUpdate();

                //if (RasterY<56) rowcounter=0;
        } while(scanline_len<MaxScanLen && !sync_valid && !zx81_stop);

        if (sync_valid)
        {
                hsync_counter=LOOPER;
                //borrow=0;
        }
        return(sync_valid);
}

void accurate_rom_patch(void)
{
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


void debug(int data)
{
        add_blank(2,data);
}
