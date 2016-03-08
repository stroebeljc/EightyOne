#ifndef __ZXPANDCLASS_H
#define __ZXPANDCLASS_H

extern void DelayMillis(char);


#include "integer.h"
#include "smbsd.h"
#include <io.h>

class zxpand_t
{
public:
   zxpand_t();

   void power_on_reset(void)
   {
      LATD = 0xff;
      CardType = 0xff;
      WASWRITE = TRUE;
      LoadEEPROM();
      init();
      delay = 0;
   }

   void periodic_update(int milliseconds)
   {
      static int mouseupdatetime = 0;

      mouseupdatetime += milliseconds;
      if (mouseupdatetime > 50)
      {
         updateMouse();
         mouseupdatetime -= 50;
      }

      if (delay)
      {
         if (milliseconds <= 1)
         {
            delay -= 10;
         }
         else
         {
            delay -= milliseconds;
         }
         if (delay <= 0)
         {
            delay = 0;
            LATD = tLATD;
         }
      }
   }

   unsigned char iord(unsigned char address)
   {
      PORTA = address >> 5;
      WASWRITE = FALSE;

      // the retuen value is already latched in LATD
      unsigned char latd = LATD;

      // this will set up the next value to be read
      process();

      return latd;
   }

   void iowr(unsigned char address, unsigned char data)
   {
      // 3 bit address in bits 7:5

      PORTA = address >> 5;
      PORTD = data;

      WASWRITE = TRUE;
      fnPtr = NULL;
      process();
      if (fnPtr != NULL)
      {
         // this write invoked a slow function
         // save latd then perform the worker function
         // save away the past-worker latd then replace it with the saved version
         // the post-worker latd will be released when the delay expires
         //
         unsigned char temp = LATD;
         fnPtr();
         tLATD = LATD;
         LATD = temp;

         fnPtr = NULL;
      }
   }

   bool isDisabled(void);
   bool isLow(void);

protected:
   void (*init)(void);
   void (*process)(void);

private:
   unsigned char tLATD;
};


#endif
