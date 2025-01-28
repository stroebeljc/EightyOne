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


 