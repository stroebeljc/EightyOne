#ifndef DIGITALKDRV_H_
#define DIGITALKDRV_H_

#include "Digitalk.h"


class Digitalk
{
public:
        Digitalk();
        void Write(unsigned char Data);
        bool Busy(void);
        int GetNextSample(void);
        void Reset();
        void SetSamplingFreq(int freq);
        void LoadRom(char* filePath);

private:
        double m_scaler;
        double m_sample_count;
        double m_lastsample;
        digitalker_device digiTalk;
};


extern Digitalk Digitalker;

#endif


 