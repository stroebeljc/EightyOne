#ifndef Z80_CONFIG
#define Z80_CONFIG

#define WORDS_LITTLEENDIAN
#define HAVE_ENOUGH_MEMORY

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef signed char         SBYTE;
typedef unsigned short      WORD;

#define readbyte_internal(Addr) opcode_fetch(Addr)

#endif
