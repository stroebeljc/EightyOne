#include <stdlib.h>
#include "zxpandclass.h"

bool L_LOW;
bool disabled;
extern void zx_initprocessor(void);
extern void zx_process_write(void);

extern void initJoy(void);
extern unsigned char readJoy(void);

BYTE* gdp;
BYTE mode;

BYTE ring_error;

void DelayMillis(char x)
{
}

extern void mapJS(unsigned char, unsigned char);

void ringReset(void){}
int serialAvailable(void){ return 0; }
BYTE serialRead(void){ return 0; }
void serialWrite(BYTE b){}
void tryProgramCPLD(const char* p){}

zxpand_t::zxpand_t()
{
   init = zx_initprocessor;
   process = zx_process_write;
   PORTB = 0xf9;

   disabled = false;
   L_LOW = true;

   mapJS(0, 0x23); // 7
   mapJS(1, 0x22); // 6
   mapJS(2, 0x21); // 5
   mapJS(3, 0x24); // 8
   mapJS(4, 0x1b); // 0

   initJoy();
}

bool zxpand_t::isDisabled(void)
{
   return disabled;
}

bool zxpand_t::isLow(void)
{
   return L_LOW;
}
