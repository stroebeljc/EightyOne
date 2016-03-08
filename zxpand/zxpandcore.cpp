#include "zxpand.h"

#include "integer.h"
#include "zxpandio.h"

#include <string.h>

#include "zxpandcom.h"

extern BYTE globalIndex;
extern WORD globalAmount;
extern void DelayMillis(char);
extern void decodeJS(void);

extern char defaultExtension;
extern WORD defaultLoadAddr;

extern volatile int mousex;
extern volatile int mousey;

static char mouseSerialNum;

BYTE lastJSCVal = 0;

void zx_process(void)
{
   void (*worker)(void) = NULL;

   // Reading PORTD will clear the 'input buffer full' bit which is required
   // in order to tell when a write has occurred. Under no circumstances should
   // you read PORTD before checking the buffer state with the WASWRITE macro.

   // port a holds the latched address
   //
   switch (PORTA & 0x07)
   {
   // data channel
   case 0x00:
      {
         // poke 0 here to prepare for reading,
         // poke nonzero here to prepare for writing to globalbuffer.
         if (WASWRITE)
         {
            if (PORTD == 0u)
            {
               // initialise the output latch with the 1st globalbuffer byte,
               // set the read index to the next byte to be read
               //
               LATD = globalData[0];
               globalIndex = 1;
            }
            else
            {
               // reset buffer index ready for writing
               globalIndex = 0;
            }
         }
      }
      break;


   // get data from globalbuffer
   case 0x01:
      {
         // must have poked zero at port 0 before starting to peek data from here.
         //
         LATD=(globalData[(int)globalIndex]);
         ++globalIndex;
      }
      break;


   // put data into globalbuffer
   case 0x02:
      {
         // must have poked nonzero at port 0 before starting to poke data to here.
         //
         if (WASWRITE)
         {
            globalData[globalIndex] = PORTD;
            ++globalIndex;
         }
      }
      break;


   // directory control port
   case 0x03:
      {
         if (WASWRITE)
         {
            LATD=0x80;

            if (PORTD == 0u)
            {
               // reset the directory reader
               //
               worker = COM_DirectoryOpen;
            }
            else
            {
               // get next directory entry
               //
               worker = COM_DirectoryRead;
            }
         }
      }
      break;

   // file control port
   case 0x04:
      {
         if (WASWRITE)
         {
            BYTE pd = PORTD;
            LATD=0x80;

            if (pd & 2)
            {
               defaultExtension = 'O';
               defaultLoadAddr = 0x4000;
            }
            else
            {
               defaultExtension = 'P';
               defaultLoadAddr = 0x4009;
            }

            // remove bit 1 - the zx80/zx81 flag  bit 1 set = zx80 file
            pd &= 0xfd;

            if (pd == 0u)
            {
               // open the file with name in global data buffer for read access
               // indicate there are no bytes in the sector buffer
               // leaves 32 byte file info in global data: size (W), load address (W), flags (W) + 26 others
               //
               worker = COM_FileOpenRead;
            }
            else if (pd == 1u)
            {
               // open the file with name in global data buffer for write access
               // creating if necessary. global buffer not damaged in case a delete() is following
               //
               worker = COM_FileOpenWrite;
            }
            else if (pd == 0x80)
            {
               // close the open file
               //
               worker = COM_FileClose;
            }
            else if (pd == 0xe0)
            {
               // delete the file with name in global data buffer
               //
               worker = COM_FileRename;
            }
            else if (pd == 0xf0)
            {
               // delete the file with name in global data buffer
               //
               worker = COM_FileDelete;
            }
         }
      }
      break;

   // get data from file
   case 0x05:
      {
         if (WASWRITE)
         {
            LATD=0x80;

            // read the next N bytes into the data buffer
            // PORTD = 0 means read 256 bytes.
            //
            globalAmount = PORTD;
            worker = COM_FileRead;
         }
      }
      break;

   // put data to file
   case 0x06:
      {
         // write the next N bytes from the global data buffer
         // PORTD = 0 means write 256 bytes.
         //
         if (WASWRITE)
         {
            LATD=0x80;

            globalAmount = PORTD;
            worker = COM_FileWrite;
         }
      }
      break;

   // interface control
   case 0x7:
      {
         //
         if (WASWRITE)
         {
            unsigned char pd = PORTD;

            LATD=0x80;

            if (pd == 0x00)
            {
               // parse a command buffer
               worker = COM_ParseBuffer;
            }
            else
            /*  0xAn functions return something */
            if (pd == 0xa0)
            {
               // assemble joystick value/card detect and pop it into latd
               // 0b11111001
               //
               char temp = GETJS;
               LATD = temp & 0xf9;
            }
            else
            if (pd == 0xa1)
            {
               // fully decoded j/s - this command may take a few nanos.
               // used by INKEY$
               //
               decodeJS();
            }
			else
            if (pd == 0xa1)
            {
               // fully decoded j/s - this command may take a few nanos.
               // used by INKEY$
               //
               decodeJS();
            }
			else
            if (pd == 0xa2)
            {
               	BYTE temp = ~GETJS;
				BYTE fb = (temp & 16) << 1;
				BYTE db = temp >> 4;
				LATD = fb | db;
            }
            //else
            //if (pd == 0xaa) : see heartbeat, below
            else
            if (pd == 0xab)
            {
               if (mousex < 0) mousex = 0;
               if (mousex > 255) mousex = 255;
               if (mousey < 0) mousey = 0;
               if (mousey > 191) mousey = 191;
               globalData[0] = mouseSerialNum;
               globalData[1] = mousex;
               globalData[2] = mousey;
               globalData[3] = (PORTB & 8) | (PORTC & 128);
               globalData[4] = ~mouseSerialNum;
               ++mouseSerialNum;
               LATD = 0x40;
            }
            else
            if (pd == 0xac)
            {
               LATD = configByte;
            }
            else
            if (pd == 0xae)
            {
               // get EEPROM data; global data contains start,count
               BYTE i = 0;
               BYTE j = 2;
               BYTE k = globalData[0] + 5;
               while (i < globalData[1])
               {
                  globalData[j] = ReadEEPROM(k);
                  ++j;
                  ++k;
                  ++i;
               }
               LATD = 0x40;
            }
            else
            if (pd == 0xaf)
            {
               // return whether a card is present
               //
               LATD = CARDPRESENT();
            }
            else
            /*  0xBn functions set something and may take time */
            if (pd == 0xb0)
            {
               // temporarily disable overlay until next zeddy reset
               DISABLE_OVERLAY;
            }
            else
            if (pd == 0xb1)
            {
               // temporarily disable overlay until next zeddy reset
               ENABLE_OVERLAY;
            }
            else
            if (pd == 0xb2)
            {
               // M=L
               GO_LOW;
            }
            else
            if (pd == 0xb3)
            {
               // M=H
               GO_HIGH;
            }
            else
            if (pd == 0xb4)
            {
               GO_BANK0;
            }
            else
            if (pd == 0xb5)
            {
               GO_BANK1;
            }
            else
            if (pd == 0xb6)
            {
               GREENLEDON();
            }
            else
            if (pd == 0xb7)
            {
               GREENLEDOFF();
            }
            else
            if (pd == 0xb8)
            {
               REDLEDON();
            }
            else
            if (pd == 0xb9)
            {
               REDLEDOFF();
            }
            else
            if (pd == 0xba)
            {
               // enable mouse mode

               mousex = 0;
               mousey = 0;
               GOMOUSE;
            }
            else
            if (pd == 0xbb)
            {
               mousex = globalData[0];
               mousey = globalData[1];
            }
            else
            if (pd == 0xbc)
            {
               // set config byte from global data buffer
               //
               configByte = globalData[0];
               WriteEEPROM(0x04, configByte);
               LATD = 0x40;
            }
            else
            if (pd == 0xbe)
            {
               BYTE i = 0;
               BYTE j = 2;
               BYTE k = globalData[0] + 5;
               while(i < globalData[1])
               {
                  WriteEEPROM(k, globalData[j]);
                  ++j;
                  ++k;
                  ++i;
               }
               LATD = 0x40;
            }
            else
            if (pd == 0xf0)
            {
               // delay a couple of milliseconds, then disable
               DelayMillis(2);
               DISABLE_OVERLAY;
            }
            else
            if (pd == 0xaa)
            {
               // heartbeat/debug 0
               LATD = 0xf0;
            }
            else
            if (pd == 0x55)
            {
               // heartbeat/debug 1
               LATD = 0x0f;
            }

            if ((pd & 0xf0) == 0xb0)
            {
               LATD = 0x40;
            }
         }
      }
      break;
   }

   if (worker)
   {
      worker();
   }
}
