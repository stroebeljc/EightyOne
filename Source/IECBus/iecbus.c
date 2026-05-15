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
#include "zx81config.h"
#include "1541.h"
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

void DeviceTick(void);
void DeviceTurnAround(int DeviceNo);
void DeviceUnTurnAround(int DeviceNo);
void DeviceTalk(int DeviceNo);
int DeviceListen(int DeviceNo);
int SwitchDisk(int device);

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

#define READDATA        0x11
#define WRITEDATA       0x12

#define DISKDRIVES      2
#define BASEDEVICE      0x0A

char SendBuffer[65536], *SendBuf;
int SendBufLen;

static int FlashCounter=-1;
static unsigned int TimeOut=0;
static int ListenState=IDLE;
static int TalkState=IDLE;
static int ProtocolState=IDLE;
static int ActiveDevice=BASEDEVICE;
static char ImagePath[DISKDRIVES][512];
static int LastDevice=-1;

void Cleanup(void) {}
void LedOn(void) { machine.drivebusy = 1; }
void LedOff(void)
{
        FlashCounter=-1;
        machine.drivebusy = 0;
}
void LedFlash(void) {
        if (FlashCounter<0)
        {
                FlashCounter=0;
                machine.drivebusy = 1;
        }
}
void VicMessage(const char* message, int size) {}

void DeviceTick(void)
{
        int Byte, i;

        switch (ProtocolState)
        {
        case IDLE:
                if (!IECIsATN())
                {
                        LedOff();
                        IECReleaseData(ActiveDevice);
                        TalkState=ListenState=IDLE;
                        break;
                }
                Byte=DeviceListen(ActiveDevice);
                if (Byte<0) break;

                IECReleaseData(ActiveDevice);
                for (i=0; i<DISKDRIVES; i++)
                {
                        if ((Byte&0x1F)==(BASEDEVICE+i))
                        {
                                ActiveDevice=(Byte&0x1F);
                                if (!SwitchDisk(ActiveDevice)) break;
                                IECAssertData(ActiveDevice);
                                if (Byte==TALK+BASEDEVICE+i)
                                {
                                        ProtocolState=TALK;
                                        IEC_Talk(ActiveDevice);
                                }
                                if (Byte==LISTEN+BASEDEVICE+i)
                                {
                                        ProtocolState=LISTEN;
                                        IEC_Listen(ActiveDevice);
                                }
                        }
                }
                break;

        case TALK:
                Byte=DeviceListen(ActiveDevice);
                if (!IECIsATN() || Byte<0) break;
                if (Byte==UNTALK)
                {
                        ProtocolState=IDLE;
                        IECReleaseData(ActiveDevice);
                        IEC_Untalk();
                        break;
                }
                else if ((Byte&0xF0)==OPEN) ProtocolState=READDATA;
                else if ((Byte&0xF0)==DATA) ProtocolState=WRITEDATA;
                IEC_SEC_Talk(Byte);

                SendBuf=SendBuffer;
                SendBufLen=0;
                while (!IEC_GetStatus())
                {
                        *(SendBuf++)=(char)IEC_Read();
                        SendBufLen++;
                }
                SendBuf=SendBuffer;
                DeviceTurnAround(ActiveDevice);
                break;

        case LISTEN:
                Byte=DeviceListen(ActiveDevice);
                if (!IECIsATN() || Byte<0) break;
                if (Byte==UNLISTEN)
                {
                        ProtocolState=IDLE;
                        IECReleaseData(ActiveDevice);
                        IEC_Unlisten();
                        break;
                }
                else if ((Byte&0xF0)==OPEN) ProtocolState=READDATA;
                else if ((Byte&0xF0)==DATA) ProtocolState=READDATA;
                IEC_SEC_Listen(Byte);
                break;

        case READDATA:
                Byte=DeviceListen(ActiveDevice);
                if (Byte<0) break;
                LedFlash();
                if (IECIsATN() && Byte==UNLISTEN)
                {
                        ProtocolState=IDLE;
                        IECReleaseData(ActiveDevice);
                        IEC_Unlisten();
                        break;
                }
                IEC_Write(Byte);
                break;

        case WRITEDATA:
                if (IECIsATN()) break;
                LedFlash();
                if (SendBufLen || TalkState!=IDLE) DeviceTalk(ActiveDevice);
                else
                {
                        ProtocolState=IDLE;
                        IECReleaseData(ActiveDevice);
                        IEC_Untalk();
                }
                break;
        }
}


// ***************************************************************

void DeviceTalk(int DeviceNo)
{
        static int BitCount;

        switch(TalkState)
        {
        case IDLE:
                if (IECIsData())
                {
                        if (TimeOut>220)
                        {
                                IECReleaseClock(DeviceNo);
                                TalkState=READYTOSEND;
                                TimeOut=0;
                        }
                }
                break;

        case READYTOSEND:
                if (!IECIsData())
                {
                        TalkState=READYFORDATA;
                }
                break;

        case READYFORDATA:
                if (SendBufLen==1)
                {
                        if (IECIsData()) TalkState=EOI;
                }
                else
                {
                        TimeOut=0;
                        IECAssertClock(DeviceNo);
                        TalkState=GETTINGBITS;
                }
                break;

        case EOI:
                if (!IECIsData())
                {
                        TimeOut=0;
                        IECAssertClock(DeviceNo);
                        TalkState=GETTINGBITS;
                }
                break;

        case GETTINGBITS:
                if (TimeOut>200)
                {
                        TimeOut=0;

                        switch(IECIsClock())
                        {
                        case true:
                                BitCount++;
                                IECReleaseClock(DeviceNo);
                                if (!((*SendBuf)&1)) IECAssertData(DeviceNo);
                                else IECReleaseData(DeviceNo);
                                *SendBuf >>= 1;
                                break;
                        case false:
                                IECReleaseData(DeviceNo);
                                IECAssertClock(DeviceNo);
                                if (BitCount==8)
                                {
                                        BitCount=0;
                                        SendBufLen--;
                                        SendBuf++;
                                        TalkState=FRAMEACK;
                                }
                        }
                }
                break;

        case FRAMEACK:
                if (IECIsData())
                {
                        if (!SendBufLen)
                        {
                                DeviceUnTurnAround(DeviceNo);
                        }
                        TalkState=IDLE;
                }
        }
}

// ***************************************************************

int DeviceListen(int DeviceNo)
{
        static int LastByte;
        static int LastClock;
        static int BitCount, BitValue;

        switch(ListenState)
        {
        case IDLE:
                if (IECIsClock())
                {
                        IECAssertData(DeviceNo);
                        ListenState=READYTOSEND;
                }
                break;

        case READYTOSEND:
                if (!IECIsClock())
                {
                        IECReleaseData(DeviceNo);
                        ListenState=READYFORDATA;
                        TimeOut=0;
                        LastByte=false;
                }
                break;

        case READYFORDATA:
                if (IECIsClock())
                {
                        ListenState=GETTINGBITS;
                        BitCount=0;
                        BitValue=0;
                        LastClock=IECIsClock();
                }
                else
                {
                        if ((!LastByte) && (TimeOut>160))
                        {
                                LastByte=true;
                                IECAssertData(DeviceNo);
                                ListenState=EOI;
                                TimeOut=0;
                        }
                }
                break;

        case EOI:
                if (TimeOut>200)
                {
                        IECReleaseData(DeviceNo);
                        ListenState=READYFORDATA;
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
                                ListenState=FRAMEACK;
                                TimeOut=0;
                        }
                }
                LastClock=IECIsClock();
                break;

        case FRAMEACK:
                if (IECIsClock())
                {
                        IECAssertData(DeviceNo);
                        if (TimeOut>200)
                        {
                                TimeOut=0;
                                ListenState=IDLE;
                                return(BitValue);
                        }
                }
                break;
        }

        return(-1);
}

// ***************************************************************

void DeviceTurnAround(int DeviceNo)
{
        IECAssertClock(DeviceNo);
        IECReleaseData(DeviceNo);
}

void DeviceUnTurnAround(int DeviceNo)
{
        IECAssertData(DeviceNo);
        IECReleaseClock(DeviceNo);
}


// ***************************************************************

void IECReset(void)
{
        Reset = 0;
        ATN = 0;
        Clock = 0;
        Data = 0;
        ProtocolState=IDLE;
        ActiveDevice=BASEDEVICE;
        TimeOut=0;
        SendBufLen=0;
        ListenState=IDLE;
        TalkState=IDLE;
        memset(ImagePath,0,sizeof(ImagePath));

        IECPerformCommand("U:",2); // reset
}

int SwitchDisk(int device)
{
        char commandString[512];

        if (device==LastDevice) return 1; // no switch needed

        if (!ImagePath[device-BASEDEVICE][0]) return 0; // no file to load, can't switch

        LastDevice=device;

        // choose drive
        strcpy(commandString, "U0>x");
        commandString[3]=(char)device;
        IECPerformCommand(commandString,4);

        // set image location
        strcpy(commandString, "G:");
        strcat(commandString, ImagePath[device-BASEDEVICE]);
        IECPerformCommand(commandString,strlen(commandString));
        return 1;
}

void IECLoadDisk(int drive, char *filename)
{
        if (drive<0 || drive>1) drive = 0;

        strcpy(ImagePath[drive], filename);
        if ((drive+BASEDEVICE)==LastDevice) LastDevice=-1; // force a switch
}

void IECEjectDisk(int drive)
{
        if (drive<0 || drive>1) drive = 0;

        ImagePath[drive][0]=NULL;
        if ((drive+BASEDEVICE)==LastDevice)
        {
                LastDevice=-1; // force a switch
                IECPerformCommand("U:",2); // reset
        }
}

void IECClockTick(int ts)
{
        TimeOut += ts;
        DeviceTick();
        if (FlashCounter>=0)
        {
                FlashCounter += ts;
                if (FlashCounter > 600000)
                {
                        machine.drivebusy=!machine.drivebusy;
                        FlashCounter=0;
                }
        }
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

