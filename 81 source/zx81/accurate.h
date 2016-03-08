
#ifndef ACCURATE_H
#define ACCURATE_H

#ifdef __cplusplus

extern "C" int framepos, NMI_generator, HSYNC_generator;
extern "C" void AccurateOutput(BYTE data, int address, int framepos);
extern "C" void zx81_accurate_init(void);
extern "C" int sync_len, sync_valid;
extern "C" int zx81_do_accurate();
extern "C" BYTE scanline[];
extern "C" int scanline_len;
extern "C" void AccurateUpdateFrame(void);
extern "C" void accurate_rom_patch(void);

#else

extern int framepos, NMI_generator, HSYNC_generator;
extern void AccurateOutput(BYTE data, int address);
extern void zx81_accurate_init(void);
extern int sync_len, sync_valid;
extern int zx81_do_accurate();
extern BYTE scanline[];
extern int scanline_len;
extern void AccurateUpdateFrame(void);
extern void accurate_rom_patch(void);

#endif
#endif
