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

#include "iecbus.h"
#include <string.h>

#define SET(Line, Device)  (Line) = ((Line) | (1<<(Device)))
#define RESET(Line, Device)  (Line) = ((Line) & ~(1<<(Device)))
#define CHECK(Line) ((Line)!=0)

#ifndef true
#define false 0
#define true (!false)
#endif

static unsigned int Reset = 0;
static unsigned int ATN = 0;
static unsigned int Clock = 0;
static unsigned int Data = 0;
static unsigned int TimeOut = 0;

void Device8Tick(void);
void Device8TurnAround(void);
void Device8UnTurnAround(void);
void Device8Talk(void);
void Received1541(int Byte);
int Device8Listen(void);

char SendBuffer[65536], *SendBuf;
int SendBufLen=0;

#define IDLE            0
#define READYTOSEND     1
#define READYFORDATA    2
#define EOI             3
#define GETTINGBITS     4
#define FRAMEACK        5

#define TALK            0x40
#define UNTALK          0x5f
#define LISTEN          0x20
#define UNLISTEN        0x3f
#define OPEN            0xf0
#define CLOSE           0xe0
#define DATA            0x60

void Device8Tick(void)
{
        int Byte;

        if (SendBufLen) Device8Talk();
        else
        {
                Byte=Device8Listen();
                if (Byte!=-1) Received1541(Byte);
        }
}

void Received1541(int Byte)
{
        static int State=IDLE;

        switch(State)
        {
        case IDLE:
        {
                switch(Byte)
                {
                case 0x48:      State=TALK;
                                break;

                default:        State=IDLE;
                                break;

                }
                break;
        }

        case TALK:
        {
                if (Byte==0x6f)
                {
                        strcpy(SendBuffer,"EightyOne 0.50");
                        SendBuf=SendBuffer;
                        SendBufLen=strlen(SendBuf);
                        Device8TurnAround();
                }
        }


        default:
                State=IDLE;
        }

}


// ***************************************************************

void Device8Talk(void)
{
        static int State=IDLE;
        static int BitCount;

        switch(State)
        {
        case IDLE:
                if (IECIsData())
                {
                        if (TimeOut>220)
                        {
                                IECReleaseClock(8);
                                State=READYTOSEND;
                                TimeOut=0;
                        }
                }
                break;

        case READYTOSEND:
                if (!IECIsData())
                {
                        State=READYFORDATA;
                }
                break;

        case READYFORDATA:
                if (SendBufLen==1)
                {
                        if (IECIsData()) State=EOI;
                }
                else
                {
                        TimeOut=0;
                        IECAssertClock(8);
                        State=GETTINGBITS;
                }
                break;

        case EOI:
                TimeOut=0;
                IECAssertClock(8);
                if (!IECIsData()) State=GETTINGBITS;
                break;

        case GETTINGBITS:
                if (TimeOut>200)
                {
                        TimeOut=0;

                        switch(IECIsClock())
                        {
                        case true:
                                BitCount++;
                                IECReleaseClock(8);
                                if (BitCount==8)
                                {
                                        BitCount=0;
                                        SendBufLen--;
                                        SendBuf++;
                                        State=FRAMEACK;
                                }
                                else
                                {
                                        if ((*SendBuf)&1) IECAssertData(8);
                                        else IECReleaseData(8);
                                        *SendBuf >>= 1;
                                }
                                break;
                        case false:
                                IECReleaseData(8);
                                IECAssertClock(8);
                        }
                }
                break;

        case FRAMEACK:
                if (IECIsData())
                {
                        if (!SendBufLen) Device8UnTurnAround();
                        State=IDLE;
                }
        }
}

// ***************************************************************

int Device8Listen(void)
{
        static int State=IDLE, LastByte=false;
        static int LastClock=true;
        static int BitCount, BitValue;

        switch(State)
        {
        case IDLE:
                if (IECIsClock())
                {
                        IECAssertData(8);
                        State=READYTOSEND;
                }
                break;

        case READYTOSEND:
                if (!IECIsClock())
                {
                        IECReleaseData(8);
                        State=READYFORDATA;
                        TimeOut=0;
                        LastByte=false;
                }
                break;

        case READYFORDATA:
                if (IECIsClock())
                {
                        State=GETTINGBITS;
                        BitCount=0;
                        BitValue=0;
                        LastClock=IECIsClock();
                }
                else
                {
                        if ((!LastByte) && (TimeOut>160))
                        {
                                LastByte=true;
                                IECAssertData(8);
                                State=EOI;
                                TimeOut=0;
                        }
                }
                break;

        case EOI:
                if (TimeOut>200)
                {
                        IECReleaseData(8);
                        State=READYFORDATA;
                        TimeOut=0;
                }
                break;

        case GETTINGBITS:
                if (LastClock && !IECIsClock())
                {
                        BitCount++;
                        BitValue >>= 1;
                        if (!IECIsData()) BitValue |= 128;

                        if (BitCount==8)
                        {
                                BitCount=0;
                                State=FRAMEACK;
                                TimeOut=0;
                        }
                }
                LastClock=IECIsClock();
                break;

        case FRAMEACK:
                if (IECIsClock())
                {
                        IECAssertData(8);
                        if (TimeOut>200)
                        {
                                TimeOut=0;
                                State=IDLE;
                                return(BitValue);
                        }
                }
                break;
        }

        return(-1);
}

// ***************************************************************

void Device8TurnAround(void)
{
        IECAssertClock(8);
        IECReleaseData(8);
}

void Device8UnTurnAround(void)
{
        IECAssertData(8);
        IECReleaseClock(8);
}


// ***************************************************************

void IECReset(void)
{
        Reset = 0;
        ATN = 0;
        Clock = 0;
        Data = 0;
        TimeOut=0;
}

void IECClockTick(int ts)
{
        TimeOut += ts;

        Device8Tick();
}

// ***************************************************************

void IECAssertReset(int DeviceNo)
{
        SET(Reset, DeviceNo);
}

void IECAssertATN(int DeviceNo)
{
        SET(ATN, DeviceNo);
}

void IECAssertClock(int DeviceNo)
{
        SET(Clock, DeviceNo);
}
void IECAssertData(int DeviceNo)
{
        SET(Data, DeviceNo);
}

// ***************************************************************

void IECReleaseReset(int DeviceNo)
{
        RESET(Reset, DeviceNo);
}

void IECReleaseATN(int DeviceNo)
{
        RESET(ATN, DeviceNo);
}

void IECReleaseClock(int DeviceNo)
{
        RESET(Clock, DeviceNo);
}

void IECReleaseData(int DeviceNo)
{
        RESET(Data, DeviceNo);
}

// ***************************************************************

int IECIsReset(void)
{
        return(CHECK(Reset));
}

int IECIsATN(void)
{
        return(CHECK(ATN));
}

int IECIsClock(void)
{
        return(CHECK(Clock));
}

int IECIsData(void)
{
        return(CHECK(Data));
}

// ***************************************************************

