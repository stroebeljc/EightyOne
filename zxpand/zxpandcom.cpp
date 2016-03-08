#include "zxpand.h"

#include "integer.h"
#include "ff.h"
#include "wildcard.h"
#include "zxpandio.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static BYTE res;

static DIR dir;
static FIL fil;
static FILINFO filinfo;
static FATFS fatfs;

extern BYTE windowData[];

extern WORD globalAmount;
extern BYTE globalIndex;
extern BYTE globalDataPresent;

#define WILD_LEN  16
char  WildPattern[WILD_LEN+1];

// use only immediately after open
extern void get_fileinfo_special(FILINFO *);

char defaultExtension;
WORD defaultLoadAddr;

extern void mem_cpy (void* dst, const void* src, int cnt);
extern char chk_chr (const rom char* str, char chr);

/*
BYTE L007E[] =
{
00   0x3F,0x3D,
02   0x28,0x3B,
04   0x26,0x38,
06   0x29,0x2B,

08   0x2C,0x36,
10   0x3C,0x2A,
12   0x37,0x39,
14   0x1D,0x1E,

16   0x1F,0x20,
18   0x21,0x1C,
20   0x25,0x24,
22   0x23,0x22,

24   0x35,0x34,
26   0x2E,0x3A,
28   0x3E,0x76,
30   0x31,0x30,

32   0x2F,0x2D,
34   0x00,0x1B,
36   0x32,0x33,
38   0x27
};
*/

// take the character required for the equivalent joystick - 0x1b and
// report the offset at that index; Z = 0x3f. 0x3f-0x1b = 36d. offs2char[36] = 0.
//
// '0', or space, is hardcoded as 34d, n/l as 29d
//
static BYTE ROM char2offs[] =
{
   35,19, // 1b
   14,15, // 1d
   16,17, // 1f
   18,23, // 21
   22,21, // 23
   20, 4, // 25
   38, 2, // 27
    6,11, // 29
    7, 8, // 2b
   33,26, // 2d
   32,31, // 2f
   30,36, // 31
   37,25, // 33
   24, 9, // 35
   12, 5, // 37
   13,27, // 39
    3,10, // 3b
    1,28, // 3d
    0, 0  // 3f + pad
};

static char ROM ascii2zxData[] = {
   0x00,0x0f,0x0b,0x0f,0x0d,0x0f,0x0f,0x0f,
   0x10,0x11,0x17,0x15,0x1a,0x16,0x1b,0x18,

   0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,
   0x24,0x25,0x0e,0x19,0x13,0x14,0x12,0x0f,

   0x0f,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
   0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,

   0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
   0x3d,0x3e,0x3f,0x0f,0x18,0x0f,0x0f,0x0f,

   0x0f,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
   0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,

   0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
   0x3d,0x3e,0x3f,0x0f,0x0f,0x0f,0x0f,0x0f
};

BYTE ascii2zx(char n)
{
   if (n < 32) return 0x0f;
   return ascii2zxData[n - 32];
}

#define ZEDDY_LT 0x13
#define ZEDDY_GT 0x12


static char ROM zx2ascii81[] = " ??????????\"£$:?()><=+-*/;,"             // 0..26 inclusive (indexed in zx->ascii conversion) - watch out for the \" escape sequence!
                             ".0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"    // 27-63 inclusive (indexed in zx->ascii conversion)
                             "-()$;\0";                                 // zero-terminated additions for the valid filename test

static char ROM zx2ascii80[] = "....";

char ROM* zx2ascii = zx2ascii81;
// TODO: have ZX80 mode switch


// dot onwards, and on to the filename test additions
//
static const ROM char* validfns = &zx2ascii81[24];


/* zx80 tokens are present in character strings:

        DEFB    $99                     ; ;     $d7
        DEFB    $9A                     ; ,     $d8
        DEFB    $91                     ; (     $d9
        DEFB    $90                     ; )     $da
        DEFB    $33,$34,$B9             ; NOT
        DEFB    $92                     ; -     $dc
        DEFB    $93                     ; +     $dd
        DEFB    $94                     ; *     $de
        DEFB    $95                     ; /     $df
        DEFB    $26,$33,$A9             ; AND
        DEFB    $34,$B7                 ; OR
        DEFB    $14,$14+$80             ; **
        DEFB    $96                     ; =     $e3
        DEFB    $97                     ; <     $e4
        DEFB    $98                     ; >     $e5
*/
static char ROM zx80Token2ascii[] = ";,()?-+*/???=<>";




static const rom char* SEPARATOR = "=";
static const rom char* SEMICOL = ";";
static const rom char* EIGHT40 =   "8-40K";
static const rom char* SIXTEEN48 = "16-48K";
static const rom char* VERSION = "ZXPAND 2.3";

typedef const rom far char* RFC;


BYTE jsmap[6];

void mapJS(BYTE dirn, BYTE val)
{
   int q;

   if (val == 0)
   {
      q = 39;
   }
   else
   if (val == 0x76)
   {
      q = 29;
   }
   else
   {
      q = char2offs[val - 0x1b];
   }

   jsmap[dirn] = q;
}

// decode joystick bits into INKEY$ directions
// ATM only UDLR & fire are reported - no diagonals
void decodeJS(void)
{
   BYTE idx = 5;
   BYTE temp = GETJS;

   // fire gets priority
   //
   if ((temp & 0x08) == 0)
   {
      idx = 4;
   }
   else if((temp & 0x80) == 0)
   {
      idx = 0;
   }
   else if ((temp & 0x40) == 0)
   {
      idx = 1;
   }
   else if ((temp & 0x20) == 0)
   {
      idx = 2;
   }
   else if ((temp & 0x10) == 0)
   {
      idx = 3;
   }

   if (idx != 5)
   {
      LATD = jsmap[idx];
   }
   else
   {
      LATD = 0xff;
   }
}

void deZeddify(unsigned char* buffer)
{
   unsigned char q;
   while (*buffer)
   {
      q = *buffer;
      if (q & 0x40)
      {
         if (q > 0xd6 && q < 0xe6)
         {
            *buffer = zx80Token2ascii[q - 0xd7];
         }
         else
         {
            *buffer = '?';
         }
      }
      else
      {
         *buffer = zx2ascii[q & 0x3f];
      }

      ++buffer;
   }
}



void zeddify(BYTE* buffer)
{
   while (*buffer)
   {
      *buffer = ascii2zx(*buffer);
      ++buffer;
   }
   // force 'zeddy' type termination
   *buffer = 255;
}



// check that the supplied ascii filename only consists of alphanums slash and dot.
char isValidFN(unsigned char* buffer)
{
   while (*buffer)
   {
      char c = chk_chr(validfns, *buffer);
      if (c == 0 || c == ';')
         return c;
      ++buffer;
   }

   return 1;
}



void zx_initprocessor(void)
{
   fatfs.win = windowData;

   f_chdrive(0);
   f_mount(0, &fatfs);
}



void GetWildcard(void)
{
   unsigned int   Idx         = 0;
   int   WildPos     = -1;
   int   LastSlash   = -1;

   //log0("GetWildcard() %s\n",(const char *)globalData);

   while ((Idx<strlen((const char*)globalData)) && (WildPos<0))
   {
      // Check for wildcard character
      if((globalData[Idx]=='?') || (globalData[Idx]=='*'))
         WildPos=Idx;

      // Check for path seperator
      if((globalData[Idx]=='\\') || (globalData[Idx]=='/'))
         LastSlash=Idx;

      Idx++;
   }

   //log0("GetWildcard() Idx=%d, WildPos=%d, LastSlash=%d\n",Idx,WildPos,LastSlash);

   if(WildPos>-1)
   {
      if(LastSlash>-1)
      {
         // Path followed by wildcard
         // Terminate dir filename at last slash and copy wildcard
         globalData[LastSlash]=0x00;
         strncpy(WildPattern,(const char*)&globalData[LastSlash+1],WILD_LEN);
      }
      else
      {
         // Wildcard on it's own
         // Copy wildcard, then set path to null
         strncpy(WildPattern,(const char*)globalData,WILD_LEN);
         globalData[0]=0x00;
      }
   }
   else
   {
      WildPattern[0]='*';
      WildPattern[1]=0;
   }

   //log0("GetWildcard() globalData=%s WildPattern=%s\n",(const char*)globalData,WildPattern);
}





void comDirectoryOpen(void)
{
   char ret = 0x40 + FR_INVALID_NAME;

   deZeddify(globalData);

   filinfo.fattrib = 0; // normal dir mode

   if (*globalData == '>')
   {
      // we will change to the directory
      ret = 0x40 | f_chdir((char*)(globalData + 1));
      if (ret == 0x40)
      {
         // the CD succeeded, so instruct the 'read directory entry' routine to exit with 'all done'
         filinfo.fattrib = 0x42;
      }
   }
   else if (*globalData == '+')
   {
      // we will try to create a directory
      ret = 0x40 | f_mkdir((char*)(globalData + 1));
      if (ret == 0x40)
      {
         // the CD succeeded, so instruct the 'read directory entry' routine to exit with 'all done'
         filinfo.fattrib = 0x42;
      }
   }
   else
   {
      // Separate wildcard and path
      GetWildcard();

      if (isValidFN(globalData))
      {
         ret = 0x40 + f_opendir(&dir, (const char*)&globalData[0]);
      }
   }

   LATD = ret;
}




void comDirectoryRead(void)
{
   char *p, *g;
   int   Match;

   if (filinfo.fattrib == 0x42)
   {
      // early bath
      filinfo.fattrib = 0;
      LATD = 0x3f;
      return;
   }

   while (1)
   {
      res = f_readdir(&dir, &filinfo);
      if (res != FR_OK)
      {
         LATD = 0x40 | res;
         return;
      }

      p = &filinfo.fname[0];
      if (!*p)
      {
         // done!
         LATD = 0x3f;
         return;
      }

      // need to do this test first
      if (filinfo.fattrib & AM_HID)
      {
         continue;
      }

      Match=wildcmp(WildPattern,p);

      if(Match)
      {
         g = (char*)&globalData[0];

         if (filinfo.fattrib & AM_DIR)
         {
            *g = ZEDDY_LT;
            ++g;
         }

         // no LFNs here ;)
         //
         for (; *p; ++p)
         {
            *g = ascii2zx(*p);
            ++g;
         }

         if (filinfo.fattrib & AM_DIR)
         {
            *g = ZEDDY_GT;
            ++g;
         }

         ////if (filinfo.fattrib & AM_RDO)
         ////{
         ////   *g = 0x16; // ascii2zx('-');
         ////   ++g;
         ////}

         *g = 0;
         ++g;

         // just for giggles put the attribute & filesize in the buffer
         //
         *g = filinfo.fattrib;
         ++g;

         mem_cpy(g, (void*)(&filinfo.fsize), sizeof(DWORD));

         LATD=0x40;
         return;
      }
   }
}

static WORD flags;
static WORD start;
static WORD length;

static char* fp_fn = (char*)(&globalData[128]);
static char* fp_fnBak = (char*)(&globalData[128+32]);


static unsigned char fileOpen(unsigned char mode)
{
   char* token;
   char autogenext = 1;
   char* p = (char*)globalData;

   deZeddify((BYTE*)p);

   if (*p == '+' || *p == '>')
   {
      ++p;
   }

   if (*p == '/')
   {
      autogenext = 0;
   }

   if (!isValidFN((unsigned char*)p))
   {
      return 0x40 + FR_INVALID_NAME;
   }

   token = strtokpgmram(p, (RFC)SEMICOL);
   if (NULL == token)
   {
      // no filename specified
      return 0x40 + FR_INVALID_NAME;
   }

   start = defaultLoadAddr;
   length = 0;
   flags = 0;

   // parse optional parameters
   //
   while ((token = strtokpgmram((char*)NULL, (RFC)SEMICOL)) != NULL)
   {
      // if it starts with an alpha then it's a flag - add it to the bitset
      if (isalpha(*token))
      {
         if (*token == 'X')
         {
            flags |= 1;
         }
      }
      else
      {
         // see if it's of the form start,end: if not it's just start
         //
         char* comma = strchr(token,',');
         start = atoi(token);
         if (comma)
         {
            length = atoi(comma+1);
         }
      }
   }

   // now all the params are parsed, we can create the filename
   //
   {
      char* newFN = fp_fn;
      char found = 0;
      for(token = p; *token; ++token, ++newFN)
      {
         *newFN = *token;
         if (*token == '.')
         {
            found = 1;
         }
      }
      *newFN = 0;
      if (!found && autogenext)
      {
         *newFN = '.';
         ++newFN;
         *newFN = defaultExtension;
         ++newFN;
         *newFN = 0;
      }
   }

   return 0x40 | f_open(&fil, fp_fn, mode);
}

void comFileOpenRead(void)
{
   res = fileOpen(FA_OPEN_EXISTING|FA_READ);

   if (0x40 == res)
   {
      get_fileinfo_special(&filinfo);

      if (length == 0)
      {
         length = (WORD)filinfo.fsize;
      }

      // hack to make programs auto-disable ROM if read-only attribute is set
      if (filinfo.fattrib & AM_RDO)
      {
         flags |= 1;
      }

      globalData[0] = length & 255;
      globalData[1] = length / 256;
      globalData[2] = start & 255;
      globalData[3] = start / 256;
      globalData[4] = flags & 255;
      globalData[5] = flags / 256;

      memset(&globalData[6], 0, 32-6);
   }

   LATD = res;
}

void comFileOpenWrite(void)
{
   res = fileOpen(FA_CREATE_NEW|FA_WRITE);

   if (res == 0x48)
   {
      // file exists
      if (globalData[0] == '+' || (fsConfig & 0x03) == 1)
      {
         char* p = fp_fnBak;
         memcpy((void*)fp_fnBak, (void*)fp_fn, 32);
         while(*p != '.'){++p;}
         strcpypgm2ram(p, (RFC)".BAK");
         res = 0x40 | f_rename(fp_fn, fp_fnBak);
      }

      if (globalData[0] == '>' || (fsConfig & 0x03) == 2)
      {
         // overwrite (ala dos pipe)
         res = 0x40 | f_unlink(fp_fn);
      }

      if (0x40 == res)
      {
         // now try again
         res = 0x40 | f_open(&fil, fp_fn, FA_CREATE_NEW|FA_WRITE);
      }
   }

   if (0x40 == res)
   {
      globalData[0] = length & 255;
      globalData[1] = length / 256;
      globalData[2] = start & 255;
      globalData[3] = start / 256;
      globalData[4] = flags & 255;
      globalData[5] = flags / 256;

      memset(&globalData[6], 0, 32-6);
   }

   LATD = res;
}




void comFileRead(void)
{
   UINT read;

   if (globalAmount == 0)
   {
      globalAmount = 256;
   }

   LATD = 0x40 | f_read(&fil, globalData, globalAmount, &read);
}




void comFileWrite(void)
{
   UINT written;

   if (globalAmount == 0)
   {
      globalAmount = 256;
   }

   LATD = 0x40 | f_write(&fil, (void*)globalData, globalAmount, &written);
}




void comFileClose(void)
{
   LATD=0x40 | f_close(&fil);
}


void comFileRename(void)
{
   char* token;
   char* p = (char*)globalData;

   char ret = 0x40 + FR_INVALID_NAME;

   deZeddify(globalData);

   token = strtokpgmram(p, (RFC)SEMICOL);
   if (NULL != token)
   {
      token = strtokpgmram((char*)NULL, (RFC)SEMICOL);
      if (NULL != token)
      {
         if (isValidFN(globalData) && isValidFN((BYTE*)token))
         {
            ret = 0x40 | f_rename((const XCHAR*)&globalData[0], (const XCHAR*)token);
         }
      }
   }

   LATD = ret;
}


void comFileDelete(void)
{
   char ret = 0x40 + FR_INVALID_NAME;

   deZeddify(globalData);
   if (isValidFN(globalData))
   {
      ret = 0x40 | f_unlink((const XCHAR*)&globalData[0]);
   }

   LATD = ret;
}


// buffer of form "C", "C,C", "C,123"
//
void comParseBuffer(void)
{
   char* token;
   BYTE retcode = 0x40;

   // keep any raw keycodes that might be lost in a conversion
   //
   mem_cpy((void*)&globalData[128], (void*)&globalData[0], 128);

   deZeddify(globalData);

   if(!isalpha(globalData[0]))
   {
      retcode |= FR_INVALID_OBJECT;
      goto retok;
   }

   // 'A' = 3.
   globalData[0] = globalData[0]-'A'+3;
   token = strtokpgmram((char*)&globalData[1], (RFC)SEPARATOR);

   if (globalData[0] == 'V'-'A'+3)
   {
      // version string

      strcpypgm2ram((char*)&globalData[1], (RFC)VERSION);

      zeddify(&globalData[1]);
      globalData[0] = 1;
      goto retok;
   }
   else
   if (globalData[0] == 'D'-'A'+3)
   {
      // set current working directory

      if (!token)
      {
         globalData[32]='\\';
         globalData[33]=0;
         token = (char*)&globalData[32];
      }

      retcode |= f_chdir(token);
      goto retok;
   }
   else if (globalData[0] == 'M'-'A'+3)
   {
      // memory map control

      if (token)
      {
         if (*token == 'H')
         {
            // HI ram
            GO_HIGH;
         }
         else if (*token == 'L')
         {
            // LO ram
            GO_LOW;
         }
         else
         {
            retcode |= FR_INVALID_OBJECT;
            goto retok;
         }
      }
      else
      {
         if (L_LOW)
         {
            strcpypgm2ram((char*)&globalData[1], (RFC)EIGHT40);
         }
         else
         {
            strcpypgm2ram((char*)&globalData[1], (RFC)SIXTEEN48);
         }

         zeddify(&globalData[1]);
         globalData[0] = 1;
      }

      goto retok;
   }
   else if (globalData[0] == 'C'-'A'+3)
   {
      // config control

      if (token)
      {
         unsigned char n = *token - '0';
         if (n > 9) n -= 7;
         if (n > 15)
         {
            retcode |= FR_INVALID_OBJECT;
            goto retok;
         }
         ++token;
         configByte = n * 16;
         n = *token - '0';
         if (n > 9) n -= 7;
         if (n > 15)
         {
            retcode |= FR_INVALID_OBJECT;
            goto retok;
         }
         configByte += n;
         WriteEEPROM(0x04, configByte);
      }
      else
      {
         unsigned char* p = &globalData[0];
         *p = 1;
         ++p;
         *p = ((configByte & 0xf0) >> 4) + 0x1c;
         ++p;
         *p = (configByte & 15) + 0x1c;
         ++p;
         *p = 0xff;
         ++p;
         *p = configByte;
      }

      goto retok;
   }
   else if (globalData[0] == 'O'-'A'+3)
   {
      // overwrite control
      if (token)
      {
         unsigned char n = *token - '0';
         if (n > 2)
         {
            retcode |= FR_INVALID_OBJECT;
            goto retok;
         }
         fsConfig = n;
         WriteEEPROM(0x05, fsConfig);
      }
      else
      {
         unsigned char* p = &globalData[0];
         *p = 1;
         ++p;
         if ((fsConfig & 3) == 1)
         {
            strcpypgm2ram((char*)p, (RFC)"BAK");
         }
         else if ((fsConfig & 3) == 2)
         {
            strcpypgm2ram((char*)p, (RFC)"OVR");
         }
         else
         {
            strcpypgm2ram((char*)p, (RFC)"ERR");
         }
         zeddify(p);
         p+= 3;
         *p = 0xff;
      }

      goto retok;
   }
   else if (globalData[0] == 'J'-'A'+3)
   {
      // joystick mapping control

      if (token)
      {
         token += 128;
         mapJS(0, *token);
         ++token;
         mapJS(1, *token);
         ++token;
         mapJS(2, *token);
         ++token;
         mapJS(3, *token);
         ++token;
         mapJS(4, *token & 0x3f); // might have top bit set being last char in string
         // might not. there might not be 4 chars there.
      }
      else
      {
         retcode |= FR_INVALID_OBJECT;
      }

      goto retok;
   }


   // generic command follows? e.g.:

   // R(amtop),nnnn

   {
      BYTE n = 0;

      token = strtokpgmram((char*)&globalData[0], (RFC)SEPARATOR);
      if (token)
      {
         // parse optional parameters
         //
         while ((token = strtokpgmram((char*)NULL, (RFC)SEPARATOR)) != NULL)
         {
            start = atoi(token);
            globalData[128+n] = start & 255;
            globalData[129+n] = start / 256;
            n += 2;
         }

         mem_cpy((void*)&globalData[1], (void*)&globalData[128], n);

         // values have been decoded to the buffer. parameterised commands may now execute
   /*
         // to ensure that a paramterised command has its parameters, put it inside this clause
         //
         if (globalData[0] == 'B'-'A'+3)
         {
            // set bank

            if (globalData[1])
            {
               GO_BANK1;
            }
            else
            {
               GO_BANK0;
            }
         }
         if (globalData[0] == 'E'-'A'+3)
         {
            retcode |= globalData[1];
         }
   */
      }

      if (n == 0 && globalData[0] == 'R'-'A'+3)
      {
         retcode |= FR_INVALID_OBJECT;
      }
   }

retok:
   LATD = retcode;
}
