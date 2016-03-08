#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "zx81.h"
#include "z80\z80.h"
#include "snap.h"
#include "fastdirty.h"
#include "accurate.h"
#include "zx81config.h"
#include "fastdirty.h"
#include "accurate.h"

extern void ZXPrinterWritePort(unsigned char Data);
extern unsigned char ZXPrinterReadPort(void);
extern int hsync_counter, rowcounter, shift_register;

int tstates;
int event_next_event;
int zx81_stop=0;
BYTE memory[65536];

BYTE ZXKeyboard[8];

BYTE get_i_reg(void)
{
        //return(0x1e);
        return(z80.i);
}

void zx81_init(void)
{
        int i;
        z80_init();

        for(i=0;i<65536;i++) memory[i]=0;//random(256);

        //memory_load(zx81.ROMfile, 0, 65536);

        if      (zx81.machine==MACHINEZX80) memory_load(zx81.ROM80, 0, 65536);
        else if (zx81.machine==MACHINEZX81) memory_load(zx81.ROM81, 0, 65536);
        else if (zx81.machine==MACHINEACE) memory_load(zx81.ROMACE, 0, 65536);

        if (zx81.shadowROM)
        {
                for(i=0;i<8192;i++) memory[i+8192]=memory[i];
                zx81.ROMTOP=16383;
        }

        if (zx81.memohrg) memory_load("memohrg.rom", 8192, 2048);

        if (zx81.fastdirty) zx81_fastdirty_init();
        else zx81_accurate_init();

        z80_reset();
}

void writebyte(int Address, int Data)
{
        if (zx81.machine == MACHINEACE)
        {
                if (Address<8192) return;
                if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
                if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
                if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
                if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
                if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;
                memory[Address]=Data;
                return;
        }

        if (Address<=zx81.ROMTOP && zx81.protectROM) return;
//        if (Address<10240 && zx81.memohrg) return;
//        if (Address<16384 && zx81.shadowROM) return;

        memory[Address]=Data;
}

BYTE readbyte(int Address)
{
        //return(memory[Address]);

        if (zx81.machine == MACHINEACE)
        {
                if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
                if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
                if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
                if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
                if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;

                if (Address>=0x2800 && Address<=0x2fff) return(255);

                return(memory[Address]);
        }

//        if (Address>=8192 && Address <16384)
//        {
//                if (zx81.RAM816k || zx81.memohrg) return(memory[Address]);
//                if (zx81.shadowROM) return(memory[Address&8191]);
//                return(255);
//        }

        if (Address>=32768 && zx81.shadowRAM) return(memory[Address&32767]);

        if (Address<=zx81.RAMTOP) return(memory[Address]);
        else return(255);
}

BYTE opcode_fetch(int Address)
{
        int NewAddress, inv;
        BYTE data;

        if (zx81.machine == MACHINEACE) return(memory[Address]);

        data = readbyte(Address&32767);

        if (Address&32768)
        {
                if (data&64) return(data);
                if (zx81.fastdirty) return(0);

                inv = data&128;
                data &= 63;

                if (z80.i >=32 && zx81.truehires)
                {
                        data=memory[(z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127)];
                        shift_register=inv ? ~data:data;
                        return(0);
                }

                data=memory[(z80.i<<8) | (data<<3) | rowcounter];
                shift_register= inv ? ~data:data;
                return(0);

        }
        return(data);
}

void writeport(int Address, int Data)
{
        switch(Address&255)
        {
        case 0:
                booting=0;
                inputwait=1;
                break;

        case 1:
                booting=0;
                inputwait=0;
                break;

        case 2:
                load_tape_buffer(z80.hl.w);
                break;

        case 3:
                save_tape_buffer(z80.hl.w);
                break;

        case 0xfb:
                ZXPrinterWritePort(Data);
                break;

        case 0xfd:
                if (zx81.machine==MACHINEZX80) break;
                NMI_generator=0;
                tstates-=4;
                if (zx81.fastdirty) z80.af_.w |= 32768;
                break;
        case 0xfe:
                if (zx81.machine==MACHINEZX80) break;
                NMI_generator=1;
                if (zx81.fastdirty) z80.af_.w &= 32767;
                break;
        default:
                break;
        }

        if (!HSYNC_generator) rowcounter=0;
        if (sync_len) sync_valid=1;
        HSYNC_generator=1;
}

BYTE readport(int Address)
{

        if (!(Address&1))
        {
                BYTE keyb, data=0;
                int i;

                if (zx81.NTSC) data=64;
                if (!GetEarState()) data |= 128;

                if (!NMI_generator)
                {
                        HSYNC_generator=0;
                        if (sync_len==0) sync_valid=0;
                }

                keyb=Address/256;
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }

                return(~data);
        }
        else
                switch(Address&255)
                {
                case 0xfb:
                        return(ZXPrinterReadPort());
                        break;
                default:
                        return(255);
                }
}

