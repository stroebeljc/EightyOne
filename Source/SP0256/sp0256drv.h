#ifndef SP0256DRV_H_
#define SP0256DRV_H_

#include "sp0256.h"


enum model_t
{
	_012, _AL2
};


class SP0256
{
public:
        SP0256(model_t model);
        void Write(unsigned char Data);
        bool Busy(void);
        int16_t GetNextSample(void);
        void Reset();
        void SetSamplingFreq(int freq);

private:
        double m_scaler;
        double m_sample_count;
        int16_t m_lastsample;
};


extern SP0256 sp0256_AL2;
extern SP0256 sp0256_ivoice;

#endif


 