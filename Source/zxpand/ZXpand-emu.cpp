// ZXpand_dll.cpp : Defines the exported functions for the DLL application.
//

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "ZXpand_emu.h"
#include "zxpandclass.h"


extern void SetRoot(const char*);

ZXpand::ZXpand()
{
   try
   {
      _zxpand = new zxpand_t;
      _zxpand->power_on_reset();

      SetRoot("ZXpand_SD_Card");

      LoadEEPROM();
      configByte = eeprom[4];
      fsConfig = eeprom[5];
   }
   catch(...)
   {
   }
}


ZXpand::~ZXpand()
{
   try
   {
      delete _zxpand;
   }
   catch(...)
   {
   }
}

void ZXpand::Update(int millis)
{
   try
   {
      if (_zxpand)
      {
         _zxpand->periodic_update(millis);
      }
   }
   catch(...)
   {
   }
}

void ZXpand::IO_Write(int addr, int data)
{
   try
   {
      if(addr == 0xff)
      {
         FILE* dbgfile = fopen("dbg.txt", "a");
         if (dbgfile)
         {
            fputc(data, dbgfile);
            fclose(dbgfile);
         }
      }
      else if (_zxpand)
      {
         _zxpand->iowr((BYTE)addr, (BYTE)data);
      }
   }
   catch(...)
   {
   }
}

int ZXpand::IO_ReadStatus()
{
   return _zxpand->activityFlag;
}

int ZXpand::IO_Read(int addr)
{
   int retval = 0xfd;

   try
   {
      if (_zxpand)
      {
         retval = _zxpand->iord(BYTE(addr));
      }
   }
   catch(...)
   {
   }

   return retval;
}

bool ZXpand::GetConfig(int& config)
{
   int oldConfig = config;

   try
   {
      if (_zxpand)
      {
         config = _zxpand->isDisabled() ? (1<<CFG_BIT_DISABLED) : 0;
         config += _zxpand->isLow() ? (1<<CFG_BIT_LOW) : 0;
      }
   }
   catch(...)
   {
   }

   return config != oldConfig;
}
