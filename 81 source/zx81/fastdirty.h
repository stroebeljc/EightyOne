
#ifndef _FASTDIRTY_H
#define _FASTDIRTY_H

#ifdef __cplusplus

extern "C" int zx81_mode, booting, inputwait;
extern "C" void zx81_do_code(int fast, int slow);
extern "C" void zx81_do_interupt(void);
extern "C" void fast_dirty_rom_patch(void);
extern "C" void zx81_fastdirty_init();
#else

extern int zx81_mode, booting, inputwait;
extern void zx81_do_code(int fast, int slow);
extern void zx81_do_interupt(void);
extern void fast_dirty_rom_patch(void);
extern void zx81_fastdirty_init();
#endif
#endif
