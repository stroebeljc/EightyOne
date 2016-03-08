#ifndef zx81_h
#define zx81_h

#include "z80\config.h"

#define kbD0 1
#define kbD1 2
#define kbD2 4
#define kbD3 8
#define kbD4 16
#define kbD5 32

#define kbA8  0
#define kbA9  1
#define kbA10 2
#define kbA11 3
#define kbA12 4
#define kbA13 5
#define kbA14 6
#define kbA15 7

#ifdef __cplusplus

extern "C" BYTE memory[];
extern "C" void zx81_init(void);
extern "C" void writebyte(int Address, int Data);
extern "C" BYTE readbyte(int Address);
extern "C" BYTE readbyte_internal(int Address);
extern "C" void writeport(int Address, int Data);
extern "C" BYTE readport(int Address);
extern "C" int tstates, event_next_event;
extern "C" BYTE get_i_reg(void);
extern "C" BYTE ZXKeyboard[];
extern "C" int zx81_stop;

#else

extern BYTE memory[];
extern void zx81_init(void);
extern void writebyte(int Address, int Data);
extern BYTE readbyte(int Address);
extern BYTE readbyte_internal(int Address);
extern void writeport(int Address, int Data);
extern BYTE readport(int Address);
extern int tstates, event_next_event;
extern int zx81_stop;
extern BYTE ZXKeyboard[];

#endif
#endif
