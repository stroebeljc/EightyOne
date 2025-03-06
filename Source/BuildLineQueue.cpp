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

 //---------------------------------------------------------------------------
//#include "zx81.h"
//#include "zx81config.h"
#include "BuildLineQueue.h"


BuildLineQueue::BuildLineQueue(void)
{
        m_ThreadHandle=NULL;
        m_hWnd=NULL;
        m_FrameReadyEvent=NULL;
        m_Head=m_Tail=0;
}

void BuildLineQueue::Initialise(HWND hWnd, HANDLE frameReadyEvent)
{
        if (frameReadyEvent) m_FrameReadyEvent=frameReadyEvent;
        if (hWnd) m_hWnd=hWnd;
        if (m_FrameReadyEvent && m_hWnd)
        {
                m_ThreadHandle = CreateThread(0, 0, &CallThread, this, 0, &m_ThreadID);
                m_mutex = CreateMutex(NULL, FALSE, NULL);
        }
}

void BuildLineQueue::End(void)
{
        if (m_ThreadHandle) TerminateThread(m_ThreadHandle,0);
        m_ThreadHandle=NULL;
}

void BuildLineQueue::Push(SCANLINE newLine)
{
        WaitForSingleObject(m_mutex, INFINITE);
        //if (newLine==NULL) return;
        memcpy(&m_VideoLines[m_Head], &newLine, sizeof(SCANLINE));
        m_Head++;
        if (m_Head>=BUILDLINEQUEUESIZE) m_Head=0;
        //SetEvent(m_FrameReadyEvent);
        ReleaseMutex(m_mutex);
}

bool BuildLineQueue::Pop(SCANLINE &videoLine)
{
        bool retVal = false;
        WaitForSingleObject(m_mutex, INFINITE);
        //if ((videoLine==NULL) || (m_Tail==m_Head)) return false;
        if (m_Tail!=m_Head)
        {
                memcpy(&videoLine, &m_VideoLines[m_Tail], sizeof(SCANLINE));
                m_Tail++;
                if (m_Tail>=BUILDLINEQUEUESIZE) m_Tail=0;
                retVal = true;
        }
        ReleaseMutex(m_mutex);
        return retVal;
}

// Helper function to get around not being able to
// use class functions as windows callback directly
DWORD WINAPI BuildLineQueue::CallThread(LPVOID Param)
{
        BuildLineQueue *instance = (BuildLineQueue *)Param;

        if(!instance) return -1;

        instance->ThreadFN();
        return(0);
}

// Audio thread function - called every frame
// copy a frame size chunk of audio data from the audio
// queue into the dx sound portion of buffer that isn't
// being used
void BuildLineQueue::ThreadFN()
{
        while(1)
        {
                WaitForSingleObject(m_FrameReadyEvent, INFINITE);

                SetLastError(0);
                SendMessage( m_hWnd, WM_USER, NULL, NULL);
                if (GetLastError())
                {
                        char buf[256];
                        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                buf, sizeof(buf), NULL);

                        MessageBox(NULL, buf,"Message Sending Error",2);
                }

                ResetEvent(m_FrameReadyEvent);
        }
}

