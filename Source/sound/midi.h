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

#ifndef MIDI_H
#define MIDI_H

class CMidi
{
public:
        CMidi();
        void WriteBit(int Bit);
        void ClockTick(int ts);
        void Write(int Byte);
        void Start(void);
        void Stop(void);
        int Device;

private:
        int MidiSerialCount;
        int MidiByte;
        unsigned char MidiBuffer[1024];
        int MidiBufferLen;
        void *outHandle;     
};

extern CMidi Midi;


#endif
