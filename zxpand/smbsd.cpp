
// global data and definitions for both atommc2 and zxpand implementations

#include "integer.h"
#include "smbsd.h"

#include <stdio.h>
#include <direct.h>
#include <string.h>


BYTE globalData[256];
BYTE sectorData[512];
BYTE windowData[512];

BYTE TRISB;
BYTE PORTB;
BYTE LATB;
BYTE PORTD;
BYTE PORTA;
BYTE WASWRITE;

BYTE CardType;
BYTE configByte;
BYTE fsConfig;
BYTE blVersion;
BYTE portBVal;

BYTE P_LOW;
BYTE P_ROMBK0;

BYTE LATD = 0xff;

WORD globalAmount;
BYTE globalIndex;

BYTE LEDs;

int TRISD;

extern volatile int mousex;
extern volatile int mousey;
extern volatile int counts;

int mousespeed = 0x100;


typedef void (*WORKERFN)(void);

extern unsigned char readJoy(void);

WORKERFN fnPtr;
int delay;

BYTE eeprom[256];

BYTE XFER_SPI(BYTE x)
{
   return 0;
}

void saveEEPROM(void)
{
   FILE* eep = fopen("eeprom.bin","wb");
   if (eep)
   {
      fwrite(eeprom, 1, 256, eep);
      fclose(eep);
   }
}


void LoadEEPROM(void)
{
   FILE* eep = fopen("eeprom.bin","rb");
   if (eep)
   {
      fread(eeprom, 1, 256, eep);
      fclose(eep);
   }
   else
   {
      memset(eeprom, 255, 256);
   }
}
void WriteEEPROM(BYTE address, BYTE val)
{
    eeprom[address] = val;
    saveEEPROM();
}

BYTE ReadEEPROM(BYTE address)
{
   return eeprom[address];
}


void updateMouse(void)
{
   unsigned char jv = ~readJoy();

   if (jv & 0x80) // up
   {
      mousey -= mousespeed;
   }
   else if (jv & 0x40) // down
   {
      mousey += mousespeed;
   }

   if (jv & 0x20) // left
   {
      mousex -= mousespeed;
   }
   else if (jv & 0x10) // right
   {
      mousex += mousespeed;
   }

   if (jv & 8) // button
      PORTB &= 0xf7;
   else
      PORTB |= 8;
}


void setMouseSpeed(int speed)
{
   mousespeed = speed;
}