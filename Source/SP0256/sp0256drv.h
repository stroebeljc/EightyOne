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


 