//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "kbstatus.h"
#include "zx81.h"
#include "zx81config.h"

int PCShift=1;
unsigned int PCALT=0;

struct kb
{
        BYTE Shift;
        WORD WinKey;
        BYTE Addr1, Data1, Addr2, Data2;
};

struct kb *KeyMap;

struct kb KBZX81[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD4, 255,255 },
        { 0, 'C' , kbA8, kbD3, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD2, 255,255 },
        { 0, 'N' , kbA15, kbD3, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA8, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD2, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD1, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 1, 186, kbA8, kbD2, kbA8, kbD0 },
        { 2, 186, kbA8, kbD1, 255,255 },
        { 1, 189, kbA14, kbD3, kbA8, kbD0 },

        { 1, 187, kbA14, kbD1, kbA8, kbD0 },
        { 2, 187, kbA14, kbD2, kbA8, kbD0 },

        { 1, 188, kbA15, kbD1, kbA8, kbD0 },
        { 2, 188, kbA15, kbD3, kbA8, kbD0 },

        { 1, 190 , kbA15, kbD1, 255,255 },
        { 2, 190 , kbA15, kbD2, kbA8, kbD0 },
        { 1, 191 , kbA8, kbD4, kbA8, kbD0 },
        { 2, 191 , kbA8, kbD3, kbA8, kbD0 },
        //{ 2, 188, kbA15, kbD3, kbA8, kbD0 },
        //{ 2, 190, kbA15, kbD2, kbA8, kbD0 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },

        { 0, VK_CONTROL , kbA14, kbD0, kbA8 , kbD0 },

        {0, 0, 0, 0, 0, 0 }
};

struct kb KBACE[]=
{
        { 0, VK_SHIFT , kbA8, kbD0, 255,255 },
        { 0, VK_CONTROL , kbA8, kbD1, 255,255 },

        { 0, VK_RETURN , kbA14, kbD0, 255,255 },
        { 0, VK_SPACE , kbA15, kbD0, 255,255 },

        { 0, 'A' , kbA9, kbD0, 255,255 },
        { 0, 'B' , kbA15, kbD3, 255,255 },
        { 0, 'C' , kbA8, kbD4, 255,255 },
        { 0, 'D' , kbA9, kbD2, 255,255 },
        { 0, 'E' , kbA10, kbD2, 255,255 },
        { 0, 'F' , kbA9, kbD3, 255,255 },
        { 0, 'G' , kbA9, kbD4, 255,255 },
        { 0, 'H' , kbA14, kbD4, 255,255 },
        { 0, 'I' , kbA13, kbD2, 255,255 },
        { 0, 'J' , kbA14, kbD3, 255,255 },
        { 0, 'K' , kbA14, kbD2, 255,255 },
        { 0, 'L' , kbA14, kbD1, 255,255 },
        { 0, 'M' , kbA15, kbD1, 255,255 },
        { 0, 'N' , kbA15, kbD2, 255,255 },
        { 0, 'O' , kbA13, kbD1, 255,255 },
        { 0, 'P' , kbA13, kbD0, 255,255 },
        { 0, 'Q' , kbA10, kbD0, 255,255 },
        { 0, 'R' , kbA10, kbD3, 255,255 },
        { 0, 'S' , kbA9, kbD1, 255,255 },
        { 0, 'T' , kbA10, kbD4, 255,255 },
        { 0, 'U' , kbA13, kbD3, 255,255 },
        { 0, 'V' , kbA15, kbD4, 255,255 },
        { 0, 'W' , kbA10, kbD1, 255,255 },
        { 0, 'X' , kbA8, kbD3, 255,255 },
        { 0, 'Y' , kbA13, kbD4, 255,255 },
        { 0, 'Z' , kbA8, kbD2, 255,255 },

        { 0, '1' , kbA11, kbD0, 255,255 },
        { 0, '2' , kbA11, kbD1, 255,255 },
        { 0, '3' , kbA11, kbD2, 255,255 },
        { 0, '4' , kbA11, kbD3, 255,255 },
        { 0, '5' , kbA11, kbD4, 255,255 },
        { 0, '6' , kbA12, kbD4, 255,255 },
        { 0, '7' , kbA12, kbD3, 255,255 },
        { 0, '8' , kbA12, kbD2, 255,255 },
        { 0, '9' , kbA12, kbD1, 255,255 },
        { 0, '0' , kbA12, kbD0, 255,255 },

        { 1, 186, kbA13, kbD1, kbA8, kbD1 },
        { 2, 186, kbA8, kbD2, kbA8, kbD1 },
        //{ 1, 192, kbA13, kbD1, kbA8, kbD1 },
        { 2, 192, kbA11, kbD1, kbA8, kbD1 },
        { 1, 222, kbA11, kbD2, kbA8, kbD1 },
        { 2, 222, kbA9, kbD0, kbA8, kbD1 },
        { 1, 219, kbA13, kbD4, kbA8, kbD1 },
        { 1, 221, kbA13, kbD3, kbA8, kbD1 },
        { 2, 219, kbA9, kbD3, kbA8, kbD1 },
        { 2, 221, kbA9, kbD4, kbA8, kbD1 },
        { 1, 189, kbA14, kbD3, kbA8, kbD1 },
        { 2, 189, kbA12, kbD0, kbA8, kbD1 },
        { 1, 187, kbA14, kbD1, kbA8, kbD1 },
        { 2, 187, kbA14, kbD2, kbA8, kbD1 },
        { 1, 189, kbA14, kbD3, kbA8, kbD1 },
        { 2, 189, kbA12, kbD0, kbA8, kbD1 },
        { 1, 188, kbA15, kbD2, kbA8, kbD1 },
        { 1, 190, kbA15, kbD1, kbA8, kbD1 },
        { 2, 188, kbA10, kbD3, kbA8, kbD1 },
        { 2, 190, kbA10, kbD4, kbA8, kbD1 },
        { 1, 191, kbA15, kbD4, kbA8, kbD1 },
        { 2, 191, kbA8, kbD4, kbA8, kbD1 },
        { 1, 220, kbA9, kbD2, kbA8, kbD1 },
        { 2, 220, kbA9, kbD1, kbA8, kbD1 },

        { 0, VK_BACK , kbA12, kbD0, kbA8, kbD0 },
        { 0, VK_LEFT , kbA11, kbD4, kbA8, kbD0 },
        { 0, VK_DOWN , kbA12, kbD4, kbA8, kbD0 },
        { 0, VK_UP , kbA12, kbD3, kbA8, kbD0 },
        { 0, VK_RIGHT , kbA12, kbD2, kbA8, kbD0 },



        {0, 0, 0, 0, 0, 0 }
};

//---------------------------------------------------------------------------

#pragma package(smart_init)

void PCKbInit(void)
{
        int i;

        for(i=0;i<8;i++) ZXKeyboard[i]=0;

        if (zx81.machine==MACHINEACE) KeyMap=KBACE;
        else KeyMap=KBZX81;
}


int PCFindKey(Word key)
{
        int i=0;

        while (KeyMap[i].WinKey)
        {
                if (KeyMap[i].WinKey == key) return(i);
                i++;
        }

        return(-1);
}

void PCSetKey(WORD dest, int source, int shift)
{
        int d;

        d=PCFindKey(dest);

        if (d!=-1)
        {
                KeyMap[d].Addr1 = KeyMap[source].Addr1;
                KeyMap[d].Data1 = KeyMap[source].Data1;

                if (shift)
                {
                        KeyMap[d].Addr2 = kbA8;
                        KeyMap[d].Data2 = kbD0;
                }
                else
                {
                        KeyMap[d].Addr2 = 255;
                        KeyMap[d].Data2 = 255;
                }
        }
}

void PCKeySetCTRL(char key)
{
        int Kctrl;


        if (key=='0')
        {
                Kctrl=PCFindKey('0');
                PCSetKey(VK_CONTROL,Kctrl,0);
        }
        else
        {
                Kctrl=PCFindKey(VK_RETURN);
                PCSetKey(VK_CONTROL,Kctrl,1);
        }
}
void PCKeySetCursor(char left, char down, char up, char right, int shift)
{
        int Kleft,Kdown, Kright, Kup;

        Kleft=PCFindKey(left);
        Kdown=PCFindKey(down);
        Kup=PCFindKey(up);
        Kright=PCFindKey(right);

        PCSetKey(VK_LEFT,Kleft,shift);
        PCSetKey(VK_DOWN,Kdown,shift);
        PCSetKey(VK_UP,Kup,shift);
        PCSetKey(VK_RIGHT,Kright,shift);
}


void PCKeyDown(WORD key)
{
        int i=0;

        if (key==VK_SHIFT) PCShift=2;

        PCALT=(GetKeyState(VK_MENU)&128);
        if (PCALT) return;
        while (KeyMap[i].WinKey)
        {
                if ((KeyMap[i].WinKey == key) &&
                        ((KeyMap[i].Shift==PCShift) || (KeyMap[i].Shift==0)))
                {
                        ZXKeyboard[KeyMap[i].Addr1] |= KeyMap[i].Data1;
                        if (KeyMap[i].Addr2!=255)
                                ZXKeyboard[KeyMap[i].Addr2] |= KeyMap[i].Data2;
                        return;
                }
                i++;
        }
}

void PCKeyUp(WORD key)
{
        int i=0;

        if (key==VK_SHIFT) PCShift=1;

        while (KeyMap[i].WinKey)
        {
                if (KeyMap[i].WinKey == key &&
                        ((KeyMap[i].Shift==PCShift) || (KeyMap[i].Shift==0)))
                {
                        ZXKeyboard[KeyMap[i].Addr1] &= ~KeyMap[i].Data1;
                        if (KeyMap[i].Addr2!=255)
                                ZXKeyboard[KeyMap[i].Addr2] &= ~KeyMap[i].Data2;
                        return;
                }
                i++;
        }
}

void PCAllKeysUp()
{
        int i;
        for(i=0; i<8; i++) ZXKeyboard[i]=0;
}

