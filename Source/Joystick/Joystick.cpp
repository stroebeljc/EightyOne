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

#include <windows.h>
#include "utils.h"
#include "Joystick.h"
#include "zx81config.h"

KeyInfo JoystickLeft1  = {'O'};
KeyInfo JoystickRight1 = {'P'};
KeyInfo JoystickUp1    = {'Q'};
KeyInfo JoystickDown1  = {'A'};
KeyInfo JoystickFire1  = {'M'};

KeyInfo JoystickLeft2;
KeyInfo JoystickRight2;
KeyInfo JoystickUp2;
KeyInfo JoystickDown2;
KeyInfo JoystickFire2;

bool controllerPresent[16];

static DWORD xPosMinTrip[16];
static DWORD xPosMaxTrip[16];
static DWORD yPosMinTrip[16];
static DWORD yPosMaxTrip[16];
static DWORD zPosMinTrip[16];
static DWORD zPosMaxTrip[16];
static DWORD rPosMinTrip[16];
static DWORD rPosMaxTrip[16];
static JOYINFOEX JoyInfo[16];

static const int JoystickReadsOn = 5;
static const int JoystickReadsOff = 5;
static const int JoystickTotalReads = JoystickReadsOn + JoystickReadsOff;

static int joystick1Id;
static int joystick2Id;
static int joystick1AutoFireReadCount;
static int joystick2AutoFireReadCount;

inline bool NumLockOn()
{
        return ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0x0001);
}

void SetJoystick1Controller(int controllerIndex)
{
        joystick1Id = controllerIndex;
}

void SetJoystick2Controller(int controllerIndex)
{
        joystick2Id = controllerIndex;
}

void InitialiseJoysticks()
{
        JOYCAPS caps;
        DWORD xTripOffset;
        DWORD yTripOffset;
        DWORD zTripOffset;
        DWORD rTripOffset;

        for (int i = 0; i < 16; i++)
        {
                if (joyGetDevCaps(i, &caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
                {
                        controllerPresent[i] = true;

                        xTripOffset    = (caps.wXmax - caps.wXmin) / 4;
                        xPosMinTrip[i] = caps.wXmin + xTripOffset;
                        xPosMaxTrip[i] = caps.wXmax - xTripOffset;

                        yTripOffset    = (caps.wYmax - caps.wYmin) / 4;
                        yPosMinTrip[i] = caps.wYmin + yTripOffset;
                        yPosMaxTrip[i] = caps.wYmax - yTripOffset;

                        zTripOffset    = (caps.wZmax - caps.wZmin) / 4;
                        zPosMinTrip[i] = caps.wZmin + zTripOffset;
                        zPosMaxTrip[i] = caps.wZmax - zTripOffset;

                        rTripOffset    = (caps.wRmax - caps.wRmin) / 4;
                        rPosMinTrip[i] = caps.wRmin + rTripOffset;
                        rPosMaxTrip[i] = caps.wRmax - rTripOffset;
                }
                else
                {
                        controllerPresent[i] = false;
                }

                JoyInfo[i].dwSize = sizeof(JoyInfo[i]);
        }

        joystick1AutoFireReadCount = 0;
        joystick2AutoFireReadCount = JoystickReadsOn; //Ensure the two reads do not collide to allow reading from BASIC
}

BYTE ReadJoystick1()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        if (joystick1Id >= 0 && controllerPresent[joystick1Id])
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwRpos <= rPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwRpos >= rPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                        if (JoyInfo[joystick1Id].dwZpos <= zPosMinTrip[joystick1Id]) result &= JoystickLeft1.Data;
                        if (JoyInfo[joystick1Id].dwZpos >= zPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;

                        if (JoyInfo[joystick1Id].dwYpos <= yPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwYpos >= yPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                        if (JoyInfo[joystick1Id].dwXpos <= xPosMinTrip[joystick1Id]) result &= JoystickLeft1.Data;
                        if (JoyInfo[joystick1Id].dwXpos >= xPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;

                        if ((JoyInfo[joystick1Id].dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
                }
        }
        
        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;
                if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;
                if (IsKeyPressed(VK_NUMPAD4)) result &= JoystickLeft1.Data;
                if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;
                if (IsKeyPressed(VK_NUMPAD0) || readFireButton) result &= JoystickFire1.Data;
        }

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick2()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick2AutoFireEnabled && (joystick2AutoFireReadCount < JoystickReadsOn));

        if (joystick2Id >= 0 && controllerPresent[joystick2Id])
        {
                JoyInfo[joystick2Id].dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

                if (controllerPresent[joystick2Id] && joyGetPosEx(joystick2Id, &JoyInfo[joystick2Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick2Id].dwRpos <= rPosMinTrip[joystick2Id]) result &= JoystickUp2.Data;
                        if (JoyInfo[joystick2Id].dwRpos >= rPosMaxTrip[joystick2Id]) result &= JoystickDown2.Data;
                        if (JoyInfo[joystick2Id].dwZpos <= zPosMinTrip[joystick2Id]) result &= JoystickLeft2.Data;
                        if (JoyInfo[joystick2Id].dwZpos >= zPosMaxTrip[joystick2Id]) result &= JoystickRight2.Data;

                        if (JoyInfo[joystick2Id].dwYpos <= yPosMinTrip[joystick2Id]) result &= JoystickUp2.Data;
                        if (JoyInfo[joystick2Id].dwYpos >= yPosMaxTrip[joystick2Id]) result &= JoystickDown2.Data;
                        if (JoyInfo[joystick2Id].dwXpos <= xPosMinTrip[joystick2Id]) result &= JoystickLeft2.Data;
                        if (JoyInfo[joystick2Id].dwXpos >= xPosMaxTrip[joystick2Id]) result &= JoystickRight2.Data;

                        if ((JoyInfo[joystick2Id].dwButtons & 0x03FF) || readFireButton) result &= JoystickFire2.Data;
                }
        }

        if (!machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp2.Data;
                if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown2.Data;
                if (IsKeyPressed(VK_NUMPAD4)) result &= JoystickLeft2.Data;
                if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight2.Data;
                if (IsKeyPressed(VK_NUMPAD0) || readFireButton) result &= JoystickFire2.Data;
        }

        joystick2AutoFireReadCount++;
        if (joystick2AutoFireReadCount == JoystickTotalReads) joystick2AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick1_Left()
{
        BYTE result = 0xFF;

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNX | JOY_RETURNZ;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwZpos <= zPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwXpos <= xPosMinTrip[joystick1Id]) result &= JoystickLeft1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD4)) result &= JoystickLeft1.Data;
        }

        return result;
}

BYTE ReadJoystick1_Right()
{
        BYTE result = 0xFF;

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNX | JOY_RETURNZ;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwZpos >= zPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;
                        if (JoyInfo[joystick1Id].dwXpos >= xPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;
        }

        return result;
}

BYTE ReadJoystick1_Up()
{
        BYTE result = 0xFF;

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNY | JOY_RETURNR;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwRpos <= rPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwYpos <= yPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;
        }

        return result;
}

BYTE ReadJoystick1_Down()
{
        BYTE result = 0xFF;

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNY | JOY_RETURNR;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwRpos >= rPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                        if (JoyInfo[joystick1Id].dwYpos >= yPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;
        }

        return result;
}

BYTE ReadJoystick1_Fire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNBUTTONS;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if ((JoyInfo[joystick1Id].dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD0) || readFireButton) result &= JoystickFire1.Data;
        }

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick1_RightUpDownFire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        if (joystick1Id >= 0)
        {
                JoyInfo[joystick1Id].dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNR | JOY_RETURNZ;

                if (controllerPresent[joystick1Id] && joyGetPosEx(joystick1Id, &JoyInfo[joystick1Id]) == JOYERR_NOERROR)
                {
                        if (JoyInfo[joystick1Id].dwRpos <= rPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwRpos >= rPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                        if (JoyInfo[joystick1Id].dwZpos >= zPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;

                        if (JoyInfo[joystick1Id].dwYpos <= yPosMinTrip[joystick1Id]) result &= JoystickUp1.Data;
                        if (JoyInfo[joystick1Id].dwYpos >= yPosMaxTrip[joystick1Id]) result &= JoystickDown1.Data;
                        if (JoyInfo[joystick1Id].dwXpos >= xPosMaxTrip[joystick1Id]) result &= JoystickRight1.Data;

                        if ((JoyInfo[joystick1Id].dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
                }
        }

        if (machine.joystick1Connected && emulator.UseNumericPadForJoystick && NumLockOn())
        {
                if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;
                if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;
                if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;
                if ((IsKeyPressed(VK_NUMPAD0)) || readFireButton) result &= JoystickFire1.Data;
        }

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

