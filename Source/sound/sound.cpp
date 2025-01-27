/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2001 Russell Marks.
 * Modifications/Changes (C) 2021 Michael D Wynne, EightyOne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * sound.c - sound support, based on the beeper/AY code I
 *           wrote for Fuse.
 */

/* ZX81 sound routines seem to be based on using VSYNC; in Basic
 * you might do this by alternating FAST/SLOW mode quickly,
 * for example. So that's what's emulated here. Obviously this
 * won't be perfect, not least as it won't be as noisy... :-)
 */

/* some AY details (volume levels, white noise RNG algorithm) based on
 * info from MAME's ay8910.c - MAME's licence explicitly permits free
 * use of info (even encourages it).
 */

/* NB: I know some of this stuff looks fairly CPU-hogging.
 * For example, the AY code tracks changes with sub-frame timing
 * in a rather hairy way, and there's subsampling here and there.
 * But if you measure the CPU use, it doesn't actually seem
 * very high at all. And I speak as a Cyrix owner. :-)
 *
 * (I based that on testing in Fuse, but I doubt it's that much
 * worse in z81. Though in both, the AY code does cause cache oddities
 * on my machine, so I get the bizarre situation of z81 jumping
 * between <=2% CPU use and *30*% CPU use pretty much at random...)
 */

#pragma warn -8071

#include "sound\sound.h"
#include "sound\soundop.h"
#include "sound\midi.h"
#include "zx81config.h"
#include "sp0256drv.h"

CSound Sound;

// Initialise - may be called several time with different data from different
// sections of the progrm.  eg, first time it's called we provide the window
// handle, next time we're called we get the fps of the emulated machine
// third time we get the sample rate requested.
// Each time we're called we store the provided information, and only when everything
// needed has been provided do we actually do anything.

int CSound::Initialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels)
{
        // If a parameter is not NULL, store the data

        if (hWnd) m_hWnd=hWnd;
        if (FPS) m_FPS=FPS;
        if (BitsPerSample) m_BitsPerSample=BitsPerSample;
        if (SampleRate) m_SampleRate=SampleRate;
        if (Channels) m_Channels=Channels;

        // If anything is still missing, return

        if (!m_hWnd || !m_FPS || !m_BitsPerSample || !m_SampleRate || !m_Channels) return(0);

        // Otherwise, we're good to go, so configure the sound.

        m_BytesPerSample=m_BitsPerSample/8;
        m_SamplesPerTState=m_SampleRate/(double)machine.clockspeed;
        EnvHeld=0;
        EnvAlternating=0;
        BeeperLastSubpos=0;

        // Start by initialsing DirectSound

        int r = DXSound.Initialise(m_hWnd, m_FPS, m_BitsPerSample, m_SampleRate, m_Channels);
        if (r)
        {
                return(r);
        }

        // get a 1 frame size buffer

        FrameSize=m_SampleRate/m_FPS;

        Buffer = new short[FrameSize*m_Channels];

        if(Buffer==NULL)
        {
                DXSound.End();
                return(-1); // Oh dear, malloc failed
        }

        OldVal=OldValOrig=0;
        OldPos=-1;
        FillPos=0;

        BeeperTick=0;
        BeeperTickIncr=(1<<24)/m_SampleRate;

        InitDevices();  // initialise sound generating devices

        sp0256_AL2.SetSamplingFreq(m_SampleRate);

        DXSound.Play();
        return(0);
}

// Reinitialise gets called if something changes... we lose the window handle,
// The user changes the sample rate - that kind of thing.

int CSound::ReInitialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels)
{
        if (!hWnd) hWnd = m_hWnd;
        if (!FPS) FPS = m_FPS;
        if (!BitsPerSample) BitsPerSample = m_BitsPerSample;
        if (!SampleRate) SampleRate = m_SampleRate;
        if (!Channels) Channels = m_Channels;

        End();
        return Initialise(hWnd,FPS,BitsPerSample,SampleRate,Channels);
}

// End() - free the sound buffer and tell DirectSould it's all over.
void CSound::End(void)
{
        DXSound.End();
        if(Buffer) delete []Buffer;
}

void CSound::InitDevices()
{
        AYInit();
        SpecDrumInit();
}

// Initialise the AY Sound chip
void CSound::AYInit(void)
{
        int f,clock;
        double v;

        // logarithmic volume levels, 3dB per step
        v=AMPL_AY_TONE;
        for(f=15;f>0;f--)
        {
                AYToneLevels[f]=(unsigned char)(v+0.5);
                // 10^3/20 = 3dB
                v/=1.4125375446;
        }
        AYToneLevels[0]=0;

        AYNoiseTick=AYNoisePeriod=0;
        AYEnvTick=AYEnvPeriod=0;

        for(f=0;f<3;f++)
                AYToneTick[f]=AYTonePeriod[f]=0;

        switch(machine.aytype)
        {
        case AY_TYPE_QUICKSILVA: clock=AY_CLOCK_QUICKSILVA; break;
        case AY_TYPE_ZONX:
                if (emulator.machine == MACHINESPECTRUM)
                {
                        clock=(spectrum.model >= SPECCY128 ? AY_CLOCK_ZONX_SPEC128 : AY_CLOCK_ZONX_SPEC48); break;
                }
                else
                {
                        clock=AY_CLOCK_ZONX_ZX81;
                }
                break;
        case AY_TYPE_FULLER: clock=AY_CLOCK_FULLER; break;
        case AY_TYPE_ACE_USER: clock=AY_CLOCK_ACE_USER; break;
        case AY_TYPE_SINCLAIR: clock=(spectrum.model >= SPECCY128 ? AY_CLOCK_SINCLAIR_128K : AY_CLOCK_SINCLAIR_48K); break;
        case AY_TYPE_TS2068: clock=AY_CLOCK_TS2068; break;
        case AY_TYPE_TC2068: clock=AY_CLOCK_TC2068; break;
        case AY_TYPE_BOLDFIELD: clock=AY_CLOCK_BOLDFIELD; break;
        case AY_TYPE_DKTRONICS: clock=(spectrum.model >= SPECCY128 ? AY_CLOCK_DKTRONICS_128K : AY_CLOCK_DKTRONICS_48K); break;
        default:
                //fprintf(stderr,"AY type not specified - can't happen!\n");
                //sound_ay=0;
                return;
        }

        AYTickIncr=(int)(65536.*clock/m_SampleRate);
        AYChangeCount=0;
        AYReset();
}

// not great having this as a macro to inline it, but it's only
// a fairly short routine, and it saves messing about.
// (XXX ummm, possibly not so true any more :-))

#define AY_GET_SUBVAL(tick,period) \
  (level*2*(tick-period)/AYTickIncr)

#define AY_OVERLAY_TONE(val,chan,level) { \
  was_high=0;								\
  if(level)								\
    {									\
    if(AYToneTick[chan]>=AYTonePeriod[chan])			\
      { (val)+=(level)*256; was_high=1; }					\
    else								\
      (val)-=(level)*256;						\
    }									\
  									\
  AYToneTick[chan]+=AYTickIncr;					\
  if(level && !was_high && AYToneTick[chan]>=AYTonePeriod[chan])	\
    (val)+=AY_GET_SUBVAL(AYToneTick[chan],AYTonePeriod[chan])*256;	\
  									\
  if(AYToneTick[chan]>=AYTonePeriod[chan]*2)			\
    {									\
    AYToneTick[chan]-=AYTonePeriod[chan]*2;				\
    /* sanity check needed to avoid making samples sound terrible */ 	\
    if(level && AYToneTick[chan]<AYTonePeriod[chan]) 		\
      (val)-=AY_GET_SUBVAL(AYToneTick[chan],0)*256;			\
    }                                                           \
  }


void CSound::AYOverlay(void)
{

        static int rng=1;
        static int noise_toggle=1;
        static int env_level=0;
        int tone_level[3];
        int mixer,envshape;
        int f,g,level;
        int v=0;
        struct AYChangeTag *change_ptr;
        int changes_left;
        int reg,r;
        int was_high;
        unsigned int FineValue, CoarseValue, TonePeriod;

        change_ptr=AYChange;
        changes_left=AYChangeCount;
        bool stereo = (m_Channels==2)&&ACBMix;

        // If no AY chip, don't produce any AY sound (!)
        //if(!sound_ay) return;

        for(f=0;f<FrameSize;f++)
        {
                // update ay registers. All this sub-frame change stuff
                // is pretty hairy, but how else would you handle the
                // samples in Robocop? :-) It also clears up some other
                // glitches.
                //
                // Ok, maybe that's no big deal on the ZX81, but even so. :-)
                // (Though, due to tstate-changing games in z80.c, we can
                // rarely `lose' one this way - hence "f==.." bit below
                // to catch any that slip through.)


                while(changes_left && (f>=change_ptr->ofs || f==FrameSize-1))
                {
                        reg=change_ptr->reg;
                        AYRegisters[reg]=change_ptr->val;
                        change_ptr++;
                        changes_left--;

                        // fix things as needed for some register changes
                        switch(reg)
                        {

                        case 0: case 1: case 2: case 3: case 4: case 5:
                                FineValue   = AYRegisters[reg & ~1];
                                CoarseValue = AYRegisters[reg |  1] & 15;
                                TonePeriod = (8 * (FineValue | (CoarseValue << 8))) << 16;

                                r = reg >> 1;
                                AYTonePeriod[r] = TonePeriod;

                                // important to get this right, otherwise e.g. Ghouls 'n' Ghosts
                                // has really scratchy, horrible-sounding vibrato.

                                if(AYTonePeriod[r] && AYToneTick[r]>=AYTonePeriod[r]*2)
                                        AYToneTick[r]%=AYTonePeriod[r]*2;
                                break;
                        case 6:
                                AYNoiseTick=0;
                                AYNoisePeriod=(16*(AYRegisters[reg]&31))<<16;
                                break;
                        case 11: case 12:
                                // this one *isn't* fixed-point
                                AYEnvPeriod=AYRegisters[11]|(AYRegisters[12]<<8);
                                break;

                        case 13:
                                AYEnvTick=AYEnvSubCycles=0;
                                EnvHeld=EnvAlternating=0;
                                env_level=0;
                                break;
                        }

                }

                // the tone level if no enveloping is being used
                for(g=0;g<3;g++)
                        tone_level[g]=AYToneLevels[AYRegisters[8+g]&15];

                // envelope
                envshape=AYRegisters[13];
                if(AYEnvPeriod)
                {
                        if(!EnvHeld)
                        {
                                v=((int)AYEnvTick*15)/AYEnvPeriod;
                                if(v<0) v=0;
                                if(v>15) v=15;
                                if((envshape&4)==0) v=15-v;
                                if(EnvAlternating) v=15-v;
                                env_level=AYToneLevels[v];
                        }
                }

                for(g=0;g<3;g++)
                        if(AYRegisters[8+g]&16)
                                tone_level[g]=env_level;

                if(AYEnvPeriod)
                {
                        // envelope gets incr'd every 256 AY cycles
                        AYEnvSubCycles+=AYTickIncr;
                        if(AYEnvSubCycles>=(256<<16))
                        {
                                AYEnvSubCycles-=(256<<16);

                                AYEnvTick++;
                                if(AYEnvTick>=AYEnvPeriod)
                                {
                                        AYEnvTick-=AYEnvPeriod;
                                        if(!EnvHeld && ((envshape&1) || (envshape&8)==0))
                                        {
                                                EnvHeld=1;
                                                if((envshape&2) || (envshape&0xc)==4)
                                                        env_level=AYToneLevels[15-v];
                                        }
                                        if(!EnvHeld && (envshape&2))
                                                EnvAlternating=!EnvAlternating;
                                }
                        }
                }

                // generate tone
                // channel C first to make ACB easier
                int ch1=0;
                int ch2=0;
                mixer=AYRegisters[7];
                if((mixer&4)==0)
                {
                        // channel C
                        int tempval=0;
                        level=(tone_level[2]*VolumeLevel[2])/AMPL_AY_TONE;
                        AY_OVERLAY_TONE(tempval,2,level);
                        if(stereo)
                        {
                                // chan c shouldn't be full vol on both channels
                                int atv = tempval * 3/4;
                                ch1+=atv;
                                ch2+=atv;
                        }
                        else
                                ch1+=tempval;
                }
                if((mixer&1)==0)
                {
                        // channel A
                        level=(tone_level[0]*VolumeLevel[0])/AMPL_AY_TONE;
                        AY_OVERLAY_TONE(ch1,0,level);
                }
                if((mixer&2)==0)
                {
                        // channel B
                        level=(tone_level[1]*VolumeLevel[1])/AMPL_AY_TONE;
                        if (stereo)
                        {
                                AY_OVERLAY_TONE(ch2,1,level);
                        }
                        else
                        {
                                AY_OVERLAY_TONE(ch1,1,level);
                        }
                }

                // generate noise
                if((mixer&0x20)==0)
                {
                        // channel C
                        level=noise_toggle?tone_level[2]:0;
                        level=(256*level*VolumeLevel[2])/AMPL_AY_TONE;
                        if(stereo)
                        {
                                // chan c shouldn't be full vol on both channels
                                int atv = level * 3/4;
                                ch1+=atv;
                                ch2+=atv;
                        }
                        else
                                ch1+=level;
                }
                if((mixer&0x08)==0)
                {
                        // channel A
                        level=noise_toggle?tone_level[0]:0;
                        level=(256*level*VolumeLevel[0])/AMPL_AY_TONE;
                        ch1+=level;
                }
                if((mixer&0x10)==0)
                {
                        // channel B
                        level=noise_toggle?tone_level[1]:0;
                        level=(256*level*VolumeLevel[1])/AMPL_AY_TONE;
                        if (stereo)
                        {
                                ch2+=level;
                        }
                        else
                                ch1+=level;
                }

                if(!stereo)
                        ch2=ch1;

                Buffer[f*m_Channels]+=ch1;
                if (m_Channels==2)
                        Buffer[f*m_Channels+1]+=ch2;

                // update noise RNG/filter
                AYNoiseTick+=AYTickIncr;
                if(AYNoiseTick>=AYNoisePeriod)
                {
                        if((rng&1)^((rng&2)?1:0))
                                noise_toggle=!noise_toggle;

                        // rng is 17-bit shift reg, bit 0 is output.
                        // input is bit 0 xor bit 2.

                        rng|=((rng&1)^((rng&4)?1:0))?0x20000:0;
                        rng>>=1;

                        AYNoiseTick-=AYNoisePeriod;
                }
        }

        AYChangeCount=0;
}

// don't make the change immediately; record it for later,
// to be made by sound_frame() (via sound_ay_overlay()).

void CSound::AYWrite(int reg, int val, int frametstates)
{
        AYRegisterStore[reg]=(unsigned char)val;

        // accept r15, in case of the two-I/O-port 8910
        if(reg>=16) return;
        if (reg==14) Midi.WriteBit(val);

        if(AYChangeCount<AY_CHANGE_MAX)
        {
                AYChange[AYChangeCount].ofs=(unsigned short)(frametstates*m_SamplesPerTState);
                AYChange[AYChangeCount].reg=(unsigned char)reg;
                AYChange[AYChangeCount].val=(unsigned char)val;
                AYChangeCount++;
        }
}

int CSound::AYRead(int reg)
{

        return(AYRegisterStore[reg]);
}

// no need to call this initially, but should be called on reset otherwise.

void CSound::AYReset(void)
{
        int f;

        for(f=0;f<16;f++)
                AYWrite(f,0,0);

        AYOverlay();
}

void CSound::SpeechOverlay(void)
{
        for(int f=0;f<FrameSize;f++)
        {
                int temp = sp0256_AL2.GetNextSample();
                temp = (temp*VolumeLevel[4])/AMPL_SPEECH;
                Buffer[f*m_Channels]+=temp;
                if(m_Channels == 2)
                {
                        Buffer[f*m_Channels+1]+=temp;
                }
        }
}

void CSound::SpecDrumInit(void)
{
        SpecDrumChangeCount=0;
        SpecDrumLevel=0;
}

void CSound::SpecDrumWrite(BYTE data, int frametstates)
{
        if(SpecDrumChangeCount<SPECDRUM_BUFFSIZE)
        {
                SpecDrumChange[SpecDrumChangeCount].ofs=(unsigned short)(frametstates*m_SamplesPerTState);
                SpecDrumChange[SpecDrumChangeCount].val=data;
                SpecDrumChangeCount++;
        }
}

void CSound::SpecDrumOverlay(void)
{
        int f;
        struct SpecDrumChangeTag *change_ptr;
        int changes_left;

        change_ptr=SpecDrumChange;
        changes_left=SpecDrumChangeCount;

        for(f=0;f<FrameSize;f++)
        {
                while(changes_left && (f>=change_ptr->ofs || f==FrameSize-1))
                {
                        SpecDrumLevel=256*(change_ptr->val-128);
                        change_ptr++;
                        changes_left--;
                }

                // generate sound
                int level=(SpecDrumLevel*VolumeLevel[5])/AMPL_SPECDRUM;

                Buffer[f*m_Channels]+=level;
                if (m_Channels==2)
                        Buffer[f*m_Channels+1]+=level;
        }

        SpecDrumChangeCount=0;
}

// XXX currently using speccy beeper code verbatim for VSYNC.
// Not sure how plausible this is, but for now it'll do.
// It does *sound* pretty plausible.


// it should go without saying that the beeper was hardly capable of
// generating perfect square waves. :-) What seems to have happened is
// that after the `click' in the intended direction away from the rest
// (zero) position, it faded out gradually over about 1/50th of a second
// back down to zero - the bulk of the fade being over after about
// a 1/500th.

// The true behaviour is awkward to model accurately, but a compromise
// emulation (doing a linear fadeout over 1/150th) seems to work quite
// well and IMHO sounds a little more like a real speccy than most
// emulations. It also has the considerable advantage of having a zero
// rest position, which I'm a lot happier with. :-)


#define BEEPER_FADEOUT	(((1<<24)/150)/AMPL_BEEPER)

#define BEEPER_OLDVAL_ADJUST \
  BeeperTick+=BeeperTickIncr;	\
  if(BeeperTick>=BEEPER_FADEOUT)	\
    {					\
    BeeperTick-=BEEPER_FADEOUT;	\
    if(OldVal>0)		\
      OldVal--;			\
    else				\
      if(OldVal<0)		\
        OldVal++;			\
    }


void CSound::Frame(void)
{                  
        int f;

        for(f=FillPos;f<FrameSize;f++)
        {
                BEEPER_OLDVAL_ADJUST;
                int tempval=(OldVal*256*VolumeLevel[3])/AMPL_BEEPER;
                Buffer[f*m_Channels]=tempval;

                if(m_Channels == 2)
                {
                        Buffer[f*m_Channels+1]=tempval;
                }
        }

        if (machine.aytype) AYOverlay();

        if (spectrum.specdrum) SpecDrumOverlay();

        if (machine.speech) SpeechOverlay();

        DXSound.Frame((unsigned char *)Buffer, FrameSize*m_Channels*m_BytesPerSample);
        SoundOutput->UpdateImage(Buffer,m_Channels,FrameSize);

        OldPos=-1;
        FillPos=0;
}

void CSound::Beeper(int on, int frametstates)
{                                     
        int newpos,subpos;
        int val,subval;
        int f;

        // if(!sound_enabled) return;

        val=AMPL_BEEPER*(on?1:-1);

        if(val==OldValOrig) return;

        // XXX a lookup table might help here...
        newpos=(frametstates*FrameSize)/machine.tperframe;
        subpos=(frametstates*FrameSize*AMPL_BEEPER)/machine.tperframe-AMPL_BEEPER*newpos;

        // if we already wrote here, adjust the level.

        if(newpos==OldPos)
        {
                // adjust it as if the rest of the sample period were all in
                // the new state. (Often it will be, but if not, we'll fix
                // it later by doing this again.)

                if(on)
                        BeeperLastSubpos+=AMPL_BEEPER-subpos;
                else
                        BeeperLastSubpos-=AMPL_BEEPER-subpos;
        }
        else
                BeeperLastSubpos=(on?AMPL_BEEPER-subpos:subpos);

        subval=0-AMPL_BEEPER+BeeperLastSubpos;

        if(newpos>=0)
        {
                //  fill gap from previous position
                for(f=FillPos;f<newpos && f<FrameSize;f++)
                {
                        BEEPER_OLDVAL_ADJUST;
                        int tempval=(OldVal*256*VolumeLevel[3])/AMPL_BEEPER;
                        Buffer[f*m_Channels]=tempval;

                        if(m_Channels==2)
                        {
                                Buffer[f*m_Channels+1]=tempval;
                        }
                }

                if(newpos<FrameSize)
                {
                        // newpos may be less than FillPos, so...
                        // limit subval in case of faded beeper level,
                        // to avoid slight spikes on ordinary tones.

                        if((OldVal<0 && subval<OldVal) ||
                                (OldVal>=0 && subval>OldVal))
                                        subval=OldVal;

                        // write subsample value
                        int tempval=(subval*256*VolumeLevel[3])/AMPL_BEEPER;
                        Buffer[newpos*m_Channels]=tempval;
                        if(m_Channels==2)
                        {
                                Buffer[newpos*m_Channels+1]=tempval;
                        }
                }
        }

        OldPos=newpos;
        FillPos=newpos+1;
        OldVal=OldValOrig=val;
}



