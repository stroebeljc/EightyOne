// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_SOUND_DIGITALK_H
#define MAME_SOUND_DIGITALK_H

#pragma once

#include <stdlib.h>
#include <cstring>

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> digitalker_device

class digitalker_device
{
public:
	digitalker_device();

	void digitalker_start_command(int cmd);
	int digitalker_0_intr_r();
        int single_sample(void);
        void LoadRom(char* filePath);

	// device-level overrides
	void device_start();

private:
	void digitalker_write(int *adr, int vol, int dac);
	int digitalker_pitch_next(int val, int prev, int step);
	void digitalker_set_intr(int intr);
	void digitalker_step_mode_0();
	void digitalker_step_mode_1();
	void digitalker_step_mode_2();
	void digitalker_step_mode_3();
	void digitalker_step();
	int digitalker_intr_r();

	// Port/lines state
	int m_data;
	int m_intr;

	// Current decoding state
	int m_bpos;
	int m_apos;

	int m_mode;
	int m_cur_segment;
	int m_cur_repeat;
	int m_segments;
	int m_repeats;

	int m_prev_pitch;
	int m_pitch;
	int m_pitch_pos;

	int m_stop_after;
	int m_cur_dac;
	int m_cur_bits;

	// Zero-range size
	unsigned int m_zero_count; // 0 for done

	// Waveform and current index in it
	int m_dac_index; // 128 for done
	int m_dac[128];

        unsigned char m_rom[16384];
};

#endif // MAME_SOUND_DIGITALK_H
