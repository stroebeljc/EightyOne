#ifndef _ACE_H
#define _ACE_H

#ifdef __cplusplus
extern "C" void zx81_fastdirty_init();
extern "C" void ace_do_code(int ts);
extern "C" ace_do_interupt();
#else
extern void zx81_fastdirty_init();
extern void ace_do_code(int ts);
extern ace_do_interupt();
#endif
#endif



