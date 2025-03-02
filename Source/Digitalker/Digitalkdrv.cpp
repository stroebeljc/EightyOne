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

#include <string.h>
#include <stdio.h>
#include "Digitalkdrv.h"

static const int c_xtal = 4000000;
static const double c_freq = c_xtal/4;

Digitalk Digitalker;

Digitalk::Digitalk() :
        m_scaler(1.0),
        m_sample_count(0.0),
        m_lastsample(0)
{
}

void Digitalk::Init(const char* romBasePath)
{
        char romFilePath[256];

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR1.bin");
        FILE* f = fopen(romFilePath, "rb");
        if (f)
        {
                fread(rombank1, 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR2.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread(&rombank1[0x2000], 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR5.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread(rombank2, 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR6.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread(&rombank2[0x2000], 1, 0x2000, f);
                fclose(f);
        }
}


void Digitalk::Write1(unsigned char Data)
{
        digiTalk.setROM(rombank1);
        digiTalk.digitalker_start_command(Data);
}

void Digitalk::Write2(unsigned char Data)
{
        digiTalk.setROM(rombank2);
        digiTalk.digitalker_start_command(Data);
}

bool Digitalk::Busy(void)
{
        int status = digiTalk.digitalker_0_intr_r();

        return status == 0;
}

void Digitalk::SetSamplingFreq(int freq)
{
        if (freq > 0)
                m_scaler = freq/c_freq;
}

int Digitalk::GetNextSample(void)
{
        double beta = m_scaler>1.0?1.0:m_scaler;
        m_sample_count += 1.0;

        while (m_sample_count >= m_scaler)
        {
                m_sample_count -= m_scaler;
                int sample = digiTalk.single_sample();
                m_lastsample += beta*(sample - m_lastsample);
        }

        return m_lastsample;
}

void Digitalk::Reset()
{
        digiTalk.device_start();
}
