
#ifndef ZX81CONFIG_H
#define ZX81CONFIG_H

#define MACHINEZX80 0
#define MACHINEZX81 1
#define MACHINEACE  2

typedef struct
{
        int fastdirty;
        int machine;
        int shadowROM;
        int RAM816k;
        int shadowRAM;
        int protectROM;
        int RAMTOP;
        int ROMTOP;
        int memohrg;
        int truehires;
        int load_patch;
        int save_patch;
        int NTSC;
        char ROM80[256];
        char ROM81[256];
        char ROMACE[256];
        char cwd[256];
        int single_step;
} ZX81;

#ifdef __cplusplus

extern "C" ZX81 zx81;
extern "C" void load_config();

#else

extern ZX81 zx81;
extern void load_config();

#endif
#endif

