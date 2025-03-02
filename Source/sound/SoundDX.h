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
 
// sound support based on the beeper/AY code I (Mike) wrote for Fuse.

#ifndef _DSND_H
#define _DSND_H

#include <windows.h>
#include <mmreg.h>
#include <dsound.h>

class CDSnd
{
public:
        CDSnd();
	int Initialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels);
        int Play();
	int End();
        void Frame(unsigned char *data, unsigned int len);

private:
        void ThreadFN(void);
        static DWORD WINAPI CallThread(LPVOID Param);

        HWND m_hWnd;
        int m_BitsPerSample;
        int m_SampleRate;
        int m_Channels;
        int m_FPS;

        DWORD m_ThreadID;
        HANDLE m_ThreadHandle;

        // DirectSound
	WAVEFORMATEX m_WFE;
	LPDIRECTSOUND m_lpDS;
	LPDIRECTSOUNDBUFFER m_lpDSB;
	HANDLE m_pHEvent[2];
        DWORD m_DXBufLen;

	// Audio Buffer
	// LPGETAUDIOSAMPLES_PROGRESS m_lpGETAUDIOSAMPLES;
	LPBYTE m_lpAudioQueue;
        DWORD m_QueueLen;
        DWORD m_QueueSize;
        DWORD m_QueueStart;
};

#endif

