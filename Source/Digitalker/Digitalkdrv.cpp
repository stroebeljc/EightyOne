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

void Digitalk::LoadRom(char* filePath)
{
        digiTalk.LoadRom(filePath);
}


void Digitalk::Write(unsigned char Data)
{
        digiTalk.digitalker_start_command(Data&0x3F);
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
        m_sample_count += 1.0;

        while (m_sample_count >= m_scaler)
        {
                m_sample_count -= m_scaler;
                int sample = digiTalk.single_sample();
                m_lastsample += 0.025*(sample - m_lastsample);
        }

        return m_lastsample;
}

void Digitalk::Reset()
{
        digiTalk.device_start();
}
