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
 * sound.h - sound support, based on the beeper/AY code I
 *           wrote for Fuse.
 */

#ifndef _SOUND_H
#define _SOUND_H

#include "sound\SoundDX.h"

#define AY_TYPE_NONE                    0
#define AY_TYPE_QUICKSILVA              1
#define AY_TYPE_ZONX                    2
#define AY_TYPE_FULLER                  3
#define AY_TYPE_ACE_USER                4
#define AY_TYPE_SINCLAIR                5
#define AY_TYPE_TS2068                  6
#define AY_TYPE_TC2068                  7
#define AY_TYPE_BOLDFIELD               8
#define AY_TYPE_DKTRONICS               9

#define AY_CLOCK_QUICKSILVA	        (3250000/4)
#define AY_CLOCK_ZONX_ZX81              1625000
#define AY_CLOCK_ZONX_SPEC48            1750000
#define AY_CLOCK_ZONX_SPEC128           1773400
#define AY_CLOCK_FULLER                 1638190
#define AY_CLOCK_ACE_USER               1625000
#define AY_CLOCK_SINCLAIR_128K          1773400
#define AY_CLOCK_SINCLAIR_48K           1750000
#define AY_CLOCK_TS2068                 1764000
#define AY_CLOCK_TC2068                 1750000
#define AY_CLOCK_BOLDFIELD              1773400
#define AY_CLOCK_DKTRONICS_128K         1773400
#define AY_CLOCK_DKTRONICS_48K          1750000

#define SPEECH_TYPE_NONE                0
#define SPEECH_TYPE_USPEECH             1
#define SPEECH_TYPE_PARROT              2
#define SPEECH_TYPE_SWEETTALKER         3
#define SPEECH_TYPE_MAGECO              4
#define SPEECH_TYPE_TALKBACK            5
#define SPEECH_TYPE_DKTRONICS           6
#define SPEECH_TYPE_ORATOR              7
#define SPEECH_TYPE_DIGITALKER          8

// assume all three tone channels together match the beeper volume.
// Must be <=127 for all channels; 4 x 31 = 124.

#define AMPL_BEEPER		31
#define AMPL_AY_TONE		31	/* three of these */
#define AMPL_SPECDRUM		31
#define AMPL_SPEECH             31

// max. number of sub-frame AY port writes allowed;
// given the number of port writes theoretically possible in a
// 50th I think this should be plenty.

#define AY_CHANGE_MAX		8000

// Measured an approximate sampling rate from the SpecDrum software of 22kHz.
// At 50fps this is about 440 samples, which I doubled and rounded up to get this. 
#define SPECDRUM_BUFFSIZE       1000


struct AYChangeTag
{
        unsigned short ofs;
        unsigned char reg,val;
};

struct SpecDrumChangeTag
{
        unsigned short ofs;
        unsigned char val;
};


class CSound
{
public:
	void InitDevices(void);
        int Initialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels);
        int ReInitialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels);

	void End(void);
	void AYWrite(int reg,int val, int frametstates);
	int AYRead(int reg);
	void AYReset(void);
        void SpecDrumWrite(BYTE data, int frametstates);
	void Frame(bool pause);
	void Beeper(int on, int frametstates);

        int VolumeLevel[6];
        bool ACBMix;

private:
        CDSnd DXSound;

        HWND m_hWnd;
        int m_BitsPerSample;
        int m_BytesPerSample;
        int m_SampleRate;
        int m_Channels;
        int m_FPS;
        double m_SamplesPerTState;

        void AYInit(void);
        void AYOverlay(void);
        void SpeechOverlay(void);
        void SpecDrumInit(void);
        void SpecDrumOverlay(void);
        void DigiTalkOverlay(void);

        int SelectAYReg;
        int FrameSize;
        int FramesPerSecond;

        unsigned char AYToneLevels[16];
        short *Buffer;
        int OldPos,FillPos,OldVal,OldValOrig;

	// timer used for fadeout after beeper-toggle;
	// fixed-point with low 24 bits as fractional part.

        unsigned int BeeperTick,BeeperTickIncr;

	// tick/incr/periods are all fixed-point with low 16 bits as
	// fractional part, except ay_env_{tick,period} which count as the chip does.

        unsigned int AYToneTick[3],AYNoiseTick;
        unsigned int AYEnvTick,AYEnvSubCycles;
        unsigned int AYTickIncr;
        unsigned int AYTonePeriod[3],AYNoisePeriod,AYEnvPeriod;

        int EnvHeld,EnvAlternating;
        int BeeperLastSubpos;

	// AY registers
	// we have 16 so we can fake an 8910 if needed

        unsigned char AYRegisters[16];
        unsigned char AYRegisterStore[16];
        struct AYChangeTag AYChange[AY_CHANGE_MAX];
        int AYChangeCount;

        // SpecDrum buffer
        struct SpecDrumChangeTag SpecDrumChange[SPECDRUM_BUFFSIZE];
        int SpecDrumChangeCount;
        int SpecDrumLevel;

};

extern CSound Sound;

#endif
