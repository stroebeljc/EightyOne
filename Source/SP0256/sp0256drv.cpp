#include "sp0256drv.h"

#include "sp0256_al2.h"	// SP0256-AL2 "Narrator"
#include "sp0256_012.h"	// SP0256-012 "Intellivoice"

static const int c_xtal = 3120000;
static const double c_freq = c_xtal/2/156;

SP0256 sp0256_AL2(_AL2);
//SP0256 sp0256_ivoice(_012);

SP0256::SP0256(model_t model) :
        m_scaler(1.0),
        m_sample_count(0.0),
        m_lastsample(0)
{
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
        if (freq > 0)
                m_scaler = freq/c_freq;
}

int16_t SP0256::GetNextSample(void)
{
        m_sample_count += 1.0;

        while (m_sample_count >= m_scaler)
        {
                m_sample_count -= m_scaler;
                m_lastsample = sp0256_getNextSample();
        }

        return m_lastsample;
}

void SP0256::Reset()
{
        sp0256_reset();
}
