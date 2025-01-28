// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_SOUND_DIGITALK_H
#define MAME_SOUND_DIGITALK_H

#pragma once

#include <stdlib.h>
#include <cstring>
#include "types.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> digitalker_device

class digitalker_device
{
public:
	digitalker_device();

	void digitalker_start_command(uint8_t cmd);
	int digitalker_0_intr_r();
        int single_sample(void);
        void setROM(uint8_t *romPtr);

	// device-level overrides
	void device_start();

private:
	void digitalker_write(uint8_t *adr, uint8_t vol, int8_t dac);
	uint8_t digitalker_pitch_next(uint8_t val, uint8_t prev, int step);
	void digitalker_set_intr(uint8_t intr);
	void digitalker_step_mode_0();
	void digitalker_step_mode_1();
	void digitalker_step_mode_2();
	void digitalker_step_mode_3();
	void digitalker_step();
	int digitalker_intr_r();

	// Port/lines state
	uint8_t m_data;
	uint8_t m_intr;

	// Current decoding state
	uint16_t m_bpos;
	uint16_t m_apos;

	uint8_t m_mode;
	uint8_t m_cur_segment;
	uint8_t m_cur_repeat;
	uint8_t m_segments;
	uint8_t m_repeats;

	uint8_t m_prev_pitch;
	uint8_t m_pitch;
	uint8_t m_pitch_pos;

	uint8_t m_stop_after;
	uint8_t m_cur_dac;
	uint8_t m_cur_bits;

	// Zero-range size
	uint32_t m_zero_count; // 0 for done

	// Waveform and current index in it
	uint8_t m_dac_index; // 128 for done
	int16_t m_dac[128];

        uint8_t *m_rom;
};

#endif // MAME_SOUND_DIGITALK_H
