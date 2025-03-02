/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
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
 */

#ifndef DIGITALKDRV_H_
#define DIGITALKDRV_H_

#include "Digitalk.h"


class Digitalk
{
public:
        Digitalk();
        void Write1(unsigned char Data);
        void Write2(unsigned char Data);
        bool Busy(void);
        int GetNextSample(void);
        void Init(const char *romBasePath);
        void Reset();
        void SetSamplingFreq(int freq);

private:
        double m_scaler;
        double m_sample_count;
        double m_lastsample;
        digitalker_device digiTalk;
        unsigned char rombank1[0x4000],rombank2[0x4000];
};


extern Digitalk Digitalker;

#endif


 