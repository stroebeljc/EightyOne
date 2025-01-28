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
                fread((unsigned char*)rombank1, 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR2.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread((unsigned char*)&rombank1[0x2000], 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR5.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread((unsigned char*)rombank2, 1, 0x2000, f);
                fclose(f);
        }

        strcpy(romFilePath,romBasePath);
        strcat(romFilePath,"SSR6.bin");
        f = fopen(romFilePath, "rb");
        if (f)
        {
                fread((unsigned char*)&rombank2[0x2000], 1, 0x2000, f);
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
