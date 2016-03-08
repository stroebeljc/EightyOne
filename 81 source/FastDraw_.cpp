//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FastDraw_.h"
#include "main_.h"
#include "zx81.h"
#include "fastdirty.h"
#include "zx81config.h"
#include "z80.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

static bool IRegChanged=1;

void EraseScreen(BYTE *dest,int W, int H, int P)
{
        int i,j;

        BYTE colour = zx81_mode ? 255:7;

        for(i=0;i<H;i++)
        {
                for(j=0;j<W;j++) dest[j]=colour;
                dest += P;
        }
}

void OutputByte(BYTE *dest, int P, int x, int y, int c, int inverse)
{
        int i,j,k;

        BYTE source;

        dest = dest + y*P + x;

        k=128;
        source=memory[c++];
        if (inverse) source = ~source;

        for(j=0;j<8;j++)
        {
                dest[j]= (source&k) ? 0:255;
                k=k>>1;
        }
}

void OutputChar(BYTE *dest, int P, int x, int y, int c, int inverse)
{
        int i,j,k;

        BYTE source;

        dest = dest + y*P + x;

        for (i=0;i<8;i++)
        {
                k=128;
                source=memory[c++];
                if (inverse) source = ~source;

                for(j=0;j<8;j++)
                {
                        dest[j]= (source&k) ? 0:255;
                        k=k>>1;
                }

                dest += P;
        }
}

int GetPseudoAddress(void)
{
        int Addr, Addr2, c;
        BYTE b;

        for (Addr = 26432; Addr>=8192; Addr--)
        {
                c=0;
                Addr2=Addr;
                b=memory[Addr];
                while((b&64) && (memory[Addr2]==b))
                {
                        Addr2+=33;
                        c++;
                }

                if (c>=191) return(Addr);
        }

        return(0);
}

void FastDirtyDrawTrueHiRes(BYTE *dest,int W, int H, int P)
{
        int addr;
        int x,y,i,pixel;

        IRegChanged=1;

        addr = 8192;//z80.i << 8;

        for(y=0; y<192; y++)
        {
                pixel=0;
                for(x=0;x<32;x++)
                {
                        for(i=7;i>=0;i--)
                                dest[pixel++] = (memory[addr]&(1<<i))?255:0;
                        addr++;
                }

                dest+=P;
        }



}

void FastDirtyDrawPseudoHiRes(BYTE *dest,int W, int H, int P)
{
        static int DAddr=0, dfile;

        if (IRegChanged)
        {
                DAddr=GetPseudoAddress();
                IRegChanged=0;
        }

        if (!DAddr)
        {
                FastDirtyDrawTrueHiRes(dest, W, H, P);
                return;
        }

        int bitmap;
        int x=0, y=0;
        int inverse;

        BYTE c;

        bitmap=get_i_reg()*256;
        dfile=DAddr;

        do
        {
                c=memory[dfile++];
                if (c&64) { x=0; y++; }
                else
                {
                        OutputByte(dest,P,x,y,bitmap+(c&127)*8,c&128);
                        x += 8;
                        if (x>248) x=0;
                }
        } while (y<192);
}

void FastDirtyDrawLowRes(BYTE *dest,int W, int H, int P)
{
        int dfile,bitmap;
        int x=0, y=0, eol=0;
        int inverse;

        BYTE c;

        IRegChanged=1;
        dfile=1+memory[16396]+256*memory[16397];
        bitmap=get_i_reg()*256;

        for(y=0;y<24;y++)
        {
                eol=0;
                for(x=0;x<33;x++)
                {
                        if (eol) c=0;
                        else c=memory[dfile++];

                        if (c==118)
                        {
                                eol=1;
                                c=0;
                        }
                        OutputChar(dest,P,x*8,y*8,bitmap+(c&127)*8,c&128);
                }

                if (!eol) dfile++;
        }
/*        do
        {
                c=memory[dfile++];
                if (c==118) { x=0; y+=8; }
                else
                {
                        if (c&64) c=15 | c&128;;
                        OutputChar(dest,P,x,y,bitmap+(c&127)*8,c&128);
                        x += 8;
                        if (x>248) x=0;
                }
        } while (y<192);
*/
}



void FastDraw(void)
{
        Form1->DImage1->Canvas->Unlock(NULL);
        BYTE *dest=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);
        if (dest==NULL) return;

        int H=Form1->DImage1->Canvas->Height;
        int W=Form1->DImage1->Canvas->Width;
        int P=Form1->DImage1->Canvas->Pitch;

        if (zx81_mode || inputwait)
        {
                if (z80.i == 0x1e && z80.ix.w<8192) FastDirtyDrawLowRes(dest, W, H, P);
                else if (z80.i<32) FastDirtyDrawPseudoHiRes(dest, W, H, P);
        }
        else    EraseScreen(dest, W, H, P);

        Form1->DImage1->Canvas->Unlock(NULL);

        TRect rc;
        rc.Left=0;
        rc.Top=0;
        rc.Right=256;
        rc.Bottom=192;
        Form1->DFlipSurface->Canvas[0]->CopyRect(NULL,Form1->DImage1->Canvas,&rc);
        Form1->DFlipSurface->RepaintTopSurface(NULL);
}

void AceFastDraw(void)
{
        int dfile,bitmap;
        int x=0, y=0;
        int inverse;

        BYTE c;

        BYTE *dest=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);
        if (!dest)
        {
                Form1->DImage1->Canvas->Unlock(NULL);
                dest=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);
                if (!dest) return;
        }

        int H=Form1->DImage1->Canvas->Height;
        int W=Form1->DImage1->Canvas->Width;
        int P=Form1->DImage1->Canvas->Pitch;

        EraseScreen(dest, W, H, P);

        dfile=0x2400;
        bitmap=0x2c00;

        for(y=0;y<24;y++)
                for(x=0;x<32;x++)
                {
                        c=memory[dfile++];
                        OutputChar(dest,P,x*8,y*8,bitmap+(c&127)*8,~c&128);
                }

        Form1->DImage1->Canvas->Unlock(NULL);

        TRect rc;
        rc.Left=0;
        rc.Top=0;
        rc.Right=256;
        rc.Bottom=192;
        Form1->DFlipSurface->Canvas[0]->CopyRect(NULL,Form1->DImage1->Canvas,&rc);
        Form1->DFlipSurface->RepaintTopSurface(NULL);
}

void FastInit(void)
{
        Form1->Accurate1->Checked=false;
        Form1->Fast1->Checked=true;
        Form1->BaseWidth=256;
        Form1->BaseHeight=192;

        zx81.fastdirty=1;
        zx81_init();

}

