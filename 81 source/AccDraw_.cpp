//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AccDraw_.h"
#include "main_.h"
#include "accurate.h"
#include "zx81.h"
#include "zx81config.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define DEBUG 0

#define HSYNC_TOLLERANCE 400
#define VSYNC_TOLLERANCE 250
#define HSYNC_MINLEN 16
#define VSYNC_MINLEN 350

//#define HSYNC_TOLLERANCE 400
//#define VSYNC_TOLLERANCE 100
//#define HSYNC_MINLEN 16
//#define VSYNC_MINLEN 350


#if DEBUG==1
#define WINDOWRIGHT 520
#define WINDOWBOTTOM 380
#define WINDOWLEFT 0
#define WINDOWTOP 0
#else
#define WINDOWRIGHT 360
#define WINDOWBOTTOM 294
#define WINDOWLEFT 0
#define WINDOWTOP 14
#endif

extern int borrow;
extern int hsync_counter;
extern int rowcounter;

int RasterX=0, RasterY=0;
int TVW, TVH, TVP;

BYTE *dest=NULL, *buffer=NULL;

TDBitCanvas *Frame1=NULL, *Frame2=NULL,
                *CurrentFrame=NULL, *PrevFrame=NULL, *IntFrame=NULL;

void DoInterlace(void)
{
        int x,y;
        BYTE *New, *Old, *Output;
        BYTE c1,c2;

        New=buffer + WINDOWTOP*TVP;
        Old=(BYTE *)PrevFrame->Lock(FullRect);
        Output=(BYTE *)IntFrame->Lock(FullRect);

        Old += WINDOWTOP*TVP;
        Output += WINDOWTOP*TVP;

        for(y=WINDOWTOP; y<WINDOWBOTTOM; y++)
        {
                for(x=WINDOWLEFT; x<WINDOWRIGHT; x++)
                {
                        c1=New[x]; c2=Old[x];
                        Output[x]=c1;
                        if (c1 != c2) Output[x]=248;
                }

                New += TVP;
                Old += TVP;
                Output += TVP;
        }

        PrevFrame->Unlock(NULL);
        IntFrame->Unlock(NULL);
}


void AccurateUpdateFrame(bool flip)
{
        TRect rc;
        //if (!flip) return;

#if DEBUG==1
#else
        if (Form1->Interlaced->Checked  && flip) DoInterlace();
#endif
        rc.Right=WINDOWRIGHT;
        rc.Bottom=WINDOWBOTTOM;
        rc.Left=WINDOWLEFT;
        rc.Top=WINDOWTOP;

//        Form1->DImage1->Canvas->Unlock(NULL);
//        Form1->DFlipSurface->Canvas[0]->CopyRect(NULL,Form1->DImage1->Canvas,&rc);
//        Form1->DFlipSurface->RepaintTopSurface(NULL);
//        buffer=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);

        CurrentFrame->Unlock(NULL);

#if DEBUG==0
        if (Form1->Interlaced->Checked)
                Form1->DFlipSurface->Canvas[0]->CopyRect(NULL,IntFrame,&rc);
        else
#endif
                Form1->DFlipSurface->Canvas[0]->CopyRect(NULL,CurrentFrame,&rc);

        Form1->DFlipSurface->RepaintTopSurface(NULL);

#if DEBUG==1
#else
        if (flip)
        {
                if (CurrentFrame==Frame1)
                {
                        CurrentFrame->Unlock(NULL);
                        CurrentFrame=Frame2;
                        PrevFrame=Frame1;
                }
                else
                {
                        CurrentFrame->Unlock(NULL);
                        CurrentFrame=Frame1;
                        PrevFrame=Frame2;
                }
        }
#endif
        buffer=(BYTE *)CurrentFrame->Lock(FullRect);

//#if DEBUG==1
//        int i,j;
//        dest=buffer;
//        for(i=0;i<TVH;i++)
//        {
//                for(j=0;j<TVW;j++) dest[j]=0;
//                dest += TVP;
//        }
//#endif
}

int AccurateDraw()
{
        int i;
        BYTE source;

        //int scancount=0;

//        int H=Form1->DImage1->Canvas->Height;
//        int W=Form1->DImage1->Canvas->Width;
//        int P=Form1->DImage1->Canvas->Pitch;

        dest = buffer + RasterY*TVP;// + RasterX;

        for(i=0; i<scanline_len; i++)
        {

                dest[RasterX]= scanline[i];//? 0:255;

                if (++RasterX > 518)
                {
                        RasterX=0;
                        RasterY++;
                        dest += TVP;
                        //scancount++;
                }

                if (RasterY>379)
                {
                        AccurateUpdateFrame(true);

                        //buffer=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);

                        RasterX=0;
                        RasterY=0;
                        dest=buffer;
                }

        }
        //if (sync_len<HSYNC_MINLEN) sync_valid=0;
        if (sync_valid)
        {
                if (RasterX>HSYNC_TOLLERANCE)
                {
                        RasterX=0;
                        RasterY++;
                        dest += TVP;
                        //scancount++;

                        //if (RasterY<2) rowcounter=7;
                }
                if (RasterY>360 || (sync_len>VSYNC_MINLEN && RasterY>VSYNC_TOLLERANCE))
                {
                        AccurateUpdateFrame(true);
                        RasterX=RasterY=0;
                        dest=buffer;
                }

                sync_len=0;
                sync_valid=-1;
        }
        //AccurateUpdateFrame(buffer, W, H, P);

        if (zx81.single_step)
        {
                dest[RasterX]=15;
                dest[RasterX+1]=15;
                dest[RasterX+2]=15;
                AccurateUpdateFrame(false);
        }

        return(0);
}

void AccurateInit(void)
{
        Form1->Accurate1->Checked=true;
        Form1->Fast1->Checked=false;
        Form1->DImage1->Width=520;
        Form1->DImage1->Height=380;
        Form1->DImage2->Width=520;
        Form1->DImage2->Height=380;
        Form1->DImage3->Width=520;
        Form1->DImage3->Height=380;
#if DEBUG==1
        Form1->BaseWidth=520;
        Form1->BaseHeight=380;
        Form1->ClientWidth=520;
        Form1->ClientHeight=380;
        Form1->DFlipSurface->Width=520;
        Form1->DFlipSurface->Height=380;
#else
        Form1->BaseWidth=360;
        Form1->BaseHeight=280;
        Form1->ClientWidth=360;
        Form1->ClientHeight=280;
        Form1->DFlipSurface->Width=360;
        Form1->DFlipSurface->Height=280;
#endif
        zx81.fastdirty=0;
        zx81_init();

        Frame1=Form1->DImage1->Canvas;
        Frame2=Form1->DImage2->Canvas;
        IntFrame=Form1->DImage3->Canvas;
        CurrentFrame = Frame2;
        PrevFrame=Frame1;

        TVH=CurrentFrame->Height;
        TVW=CurrentFrame->Width;
        TVP=CurrentFrame->Pitch;

        Frame1->Unlock(NULL);
        Frame2->Unlock(NULL);

        // buffer=(BYTE *)Form1->DImage1->Canvas->Lock(FullRect);
        buffer=(BYTE *)CurrentFrame->Lock(FullRect);
}
