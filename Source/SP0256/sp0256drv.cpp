#include "sp0256drv.h"

#include "sp0256_al2.h"	// SP0256-AL2 "Narrator"
#include "sp0256_012.h"	// SP0256-012 "Intellivoice"

SP0256 sp0256_AL2(_AL2);
//SP0256 sp0256_ivoice(_012);

SP0256::SP0256(model_t model)
{
        m_sample_count = 0.0;
        m_lastsample = 0;
        m_samplefreq = 22000;

        if (model==_012)
	        sp0256_setLabels( sp0256_012::nlabels, sp0256_012::labels );
        else
	        sp0256_setLabels( sp0256_al2::nlabels, sp0256_al2::labels );

        if (model==_012)
                ivoice_init( sp0256_012::mask );
        else
                ivoice_init( sp0256_al2::mask );
}

void SP0256::Write(unsigned char Data)
{
        sp0256_sendCommand( uint32_t( Data&0x3F ) );
}

bool SP0256::Busy(void)
{
        unsigned int status = sp0256_getStatus();

        return status == 0;
}

void SP0256::SetSamplingFreq(int freq)
{
        m_samplefreq = freq;
}

char SP0256::GetNextSample(void)
{
        const int xtal = 3120000;
        const double freq = xtal/2/156;

        double scaler = m_samplefreq/freq;
        m_sample_count += 1.0;

        while (m_sample_count >= scaler)
        {
                m_sample_count -= scaler;
                int sample = sp0256_getNextSample();
       	        sample >>= 8;
                m_lastsample = (char)(sample & 0xFF);
        }

        return m_lastsample;
}

void SP0256::Reset()
{
        sp0256_reset();
}
