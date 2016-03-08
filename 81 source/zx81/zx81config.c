#include "zx81config.h"

ZX81 zx81;

void load_config(void)
{
        zx81.fastdirty=0;
        zx81.machine=MACHINEZX81;

        zx81.shadowROM=0;
        zx81.RAM816k=1;
        zx81.shadowRAM=1;
        zx81.protectROM=1;
        zx81.RAMTOP=32767;
        zx81.ROMTOP=8191;
        zx81.memohrg=0;
        zx81.load_patch=1;
        zx81.save_patch=1;
        zx81.NTSC=0;
        zx81.truehires=1;

        zx81.single_step=0;

        strcpy(zx81.ROM80, "zx80.rom");
        strcpy(zx81.ROM81, "zx81.rom");
        strcpy(zx81.ROMACE, "ace.rom");

        getcwd(zx81.cwd,256);
}

