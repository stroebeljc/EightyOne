#include <windows.h>
#include "Joystick.h"
#include "zx81config.h"

KeyInfo JoystickLeft  = {'O'};
KeyInfo JoystickRight = {'P'};
KeyInfo JoystickUp    = {'Q'};
KeyInfo JoystickDown  = {'A'};
KeyInfo JoystickFire  = {'M'};

KeyInfo JoystickLeft2;
KeyInfo JoystickRight2;
KeyInfo JoystickUp2;
KeyInfo JoystickDown2;
KeyInfo JoystickFire2;

static DWORD xPosMinTrip;
static DWORD xPosMaxTrip;
static DWORD yPosMinTrip;
static DWORD yPosMaxTrip;
static DWORD zPosMinTrip;
static DWORD zPosMaxTrip;
static DWORD rPosMinTrip;
static DWORD rPosMaxTrip;
static JOYINFOEX joyInfo;

static DWORD xPosMinTrip2;
static DWORD xPosMaxTrip2;
static DWORD yPosMinTrip2;
static DWORD yPosMaxTrip2;
static DWORD zPosMinTrip2;
static DWORD zPosMaxTrip2;
static DWORD rPosMinTrip2;
static DWORD rPosMaxTrip2;
static JOYINFOEX joyInfo2;

static int joystick1AutoFireReadCount;
static int joystick2AutoFireReadCount;

static const int JoystickReadsOn = 5;
static const int JoystickReadsOff = 5;
static const int JoystickTotalReads = JoystickReadsOn + JoystickReadsOff;

int NumberOfJoysticks()
{
        joyInfo2.dwFlags = JOY_RETURNBUTTONS;

        if (joyGetPosEx(JOYSTICKID2, &joyInfo2) == JOYERR_NOERROR)
        {
                return 2;
        }

        joyInfo.dwFlags = JOY_RETURNBUTTONS;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                return 1;
        }

        return 0;
}

void InitialiseJoysticks()
{
        JOYCAPS caps;
        JOYCAPS caps2;
        DWORD xTripOffset;
        DWORD yTripOffset;
        DWORD zTripOffset;
        DWORD rTripOffset;

        if (joyGetDevCaps(JOYSTICKID1, &caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
        {
                xTripOffset = (caps.wXmax - caps.wXmin) / 4;
                xPosMinTrip = caps.wXmin + xTripOffset;
                xPosMaxTrip = caps.wXmax - xTripOffset;

                yTripOffset = (caps.wYmax - caps.wYmin) / 4;
                yPosMinTrip = caps.wYmin + yTripOffset;
                yPosMaxTrip = caps.wYmax - yTripOffset;

                zTripOffset = (caps.wZmax - caps.wZmin) / 4;
                zPosMinTrip = caps.wZmin + zTripOffset;
                zPosMaxTrip = caps.wZmax - zTripOffset;

                rTripOffset = (caps.wRmax - caps.wRmin) / 4;
                rPosMinTrip = caps.wRmin + rTripOffset;
                rPosMaxTrip = caps.wRmax - rTripOffset;
        }

        if (joyGetDevCaps(JOYSTICKID2, &caps2, sizeof(JOYCAPS)) == JOYERR_NOERROR)
        {
                xTripOffset = (caps2.wXmax - caps2.wXmin) / 4;
                xPosMinTrip2 = caps2.wXmin + xTripOffset;
                xPosMaxTrip2 = caps2.wXmax - xTripOffset;

                yTripOffset = (caps2.wYmax - caps2.wYmin) / 4;
                yPosMinTrip2 = caps2.wYmin + yTripOffset;
                yPosMaxTrip2 = caps2.wYmax - yTripOffset;

                zTripOffset = (caps2.wZmax - caps2.wZmin) / 4;
                zPosMinTrip2 = caps2.wZmin + zTripOffset;
                zPosMaxTrip2 = caps2.wZmax - zTripOffset;

                rTripOffset = (caps2.wRmax - caps2.wRmin) / 4;
                rPosMinTrip2 = caps2.wRmin + rTripOffset;
                rPosMaxTrip2 = caps2.wRmax - rTripOffset;
        }

        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo2.dwSize = sizeof(joyInfo2);

        joystick1AutoFireReadCount = 0;
        joystick2AutoFireReadCount = JoystickReadsOn; //Ensure the two reads do not collide to allow reading from BASIC
}

BYTE ReadJoystick()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        joyInfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwRpos <= rPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwRpos >= rPosMaxTrip) result &= JoystickDown.Data;
                if (joyInfo.dwZpos <= zPosMinTrip) result &= JoystickLeft.Data;
                if (joyInfo.dwZpos >= zPosMaxTrip) result &= JoystickRight.Data;

                if (joyInfo.dwYpos <= yPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwYpos >= yPosMaxTrip) result &= JoystickDown.Data;
                if (joyInfo.dwXpos <= xPosMinTrip) result &= JoystickLeft.Data;
                if (joyInfo.dwXpos >= xPosMaxTrip) result &= JoystickRight.Data;

                if ((joyInfo.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire.Data;
        }

        if (GetKeyState(VK_NUMPAD8) & 0x8000) result &= JoystickUp.Data;
        if (GetKeyState(VK_NUMPAD2) & 0x8000) result &= JoystickDown.Data;
        if (GetKeyState(VK_NUMPAD4) & 0x8000) result &= JoystickLeft.Data;
        if (GetKeyState(VK_NUMPAD6) & 0x8000) result &= JoystickRight.Data;
        if ((GetKeyState(VK_NUMPAD0) & 0x8000) || readFireButton) result &= JoystickFire.Data;

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick2()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick2AutoFireEnabled && (joystick2AutoFireReadCount < JoystickReadsOn));

        joyInfo2.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

        if (joyGetPosEx(JOYSTICKID2, &joyInfo2) == JOYERR_NOERROR)
        {
                if (joyInfo2.dwRpos <= rPosMinTrip) result &= JoystickUp2.Data;
                if (joyInfo2.dwRpos >= rPosMaxTrip) result &= JoystickDown2.Data;
                if (joyInfo2.dwZpos <= zPosMinTrip) result &= JoystickLeft2.Data;
                if (joyInfo2.dwZpos >= zPosMaxTrip) result &= JoystickRight2.Data;

                if (joyInfo2.dwYpos <= yPosMinTrip) result &= JoystickUp2.Data;
                if (joyInfo2.dwYpos >= yPosMaxTrip) result &= JoystickDown2.Data;
                if (joyInfo2.dwXpos <= xPosMinTrip) result &= JoystickLeft2.Data;
                if (joyInfo2.dwXpos >= xPosMaxTrip) result &= JoystickRight2.Data;

                if ((joyInfo2.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire2.Data;
        }

        joystick2AutoFireReadCount++;
        if (joystick2AutoFireReadCount == JoystickTotalReads) joystick2AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick_Left()
{
        BYTE result = 0xFF;

        joyInfo.dwFlags = JOY_RETURNX | JOY_RETURNZ;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwZpos <= zPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwXpos <= xPosMinTrip) result &= JoystickLeft.Data;
        }

        if (GetKeyState(VK_NUMPAD4) & 0x8000) result &= JoystickLeft.Data;

        return result;
}

BYTE ReadJoystick_Right()
{
        BYTE result = 0xFF;

        joyInfo.dwFlags = JOY_RETURNX | JOY_RETURNZ;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwZpos >= zPosMaxTrip) result &= JoystickRight.Data;
                if (joyInfo.dwXpos >= xPosMaxTrip) result &= JoystickRight.Data;
        }

        if (GetKeyState(VK_NUMPAD6) & 0x8000) result &= JoystickRight.Data;

        return result;
}

BYTE ReadJoystick_Up()
{
        BYTE result = 0xFF;

        joyInfo.dwFlags = JOY_RETURNY | JOY_RETURNR;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwRpos <= rPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwYpos <= yPosMinTrip) result &= JoystickUp.Data;
        }

        if (GetKeyState(VK_NUMPAD8) & 0x8000) result &= JoystickUp.Data;

        return result;
}

BYTE ReadJoystick_Down()
{
        BYTE result = 0xFF;

        joyInfo.dwFlags = JOY_RETURNY | JOY_RETURNR;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwRpos >= rPosMaxTrip) result &= JoystickDown.Data;
                if (joyInfo.dwYpos >= yPosMaxTrip) result &= JoystickDown.Data;
        }

        if (GetKeyState(VK_NUMPAD2) & 0x8000) result &= JoystickDown.Data;

        return result;
}

BYTE ReadJoystick_Fire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        joyInfo.dwFlags = JOY_RETURNBUTTONS;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if ((joyInfo.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire.Data;
        }

        if ((GetKeyState(VK_NUMPAD0) & 0x8000) || readFireButton) result &= JoystickFire.Data;

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick_RightUpDownFire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        joyInfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNR | JOY_RETURNZ;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
        {
                if (joyInfo.dwRpos <= rPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwRpos >= rPosMaxTrip) result &= JoystickDown.Data;
                if (joyInfo.dwZpos >= zPosMaxTrip) result &= JoystickRight.Data;

                if (joyInfo.dwYpos <= yPosMinTrip) result &= JoystickUp.Data;
                if (joyInfo.dwYpos >= yPosMaxTrip) result &= JoystickDown.Data;
                if (joyInfo.dwXpos >= xPosMaxTrip) result &= JoystickRight.Data;

                if ((joyInfo.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire.Data;
        }

        if (GetKeyState(VK_NUMPAD8) & 0x8000) result &= JoystickUp.Data;
        if (GetKeyState(VK_NUMPAD2) & 0x8000) result &= JoystickDown.Data;
        if (GetKeyState(VK_NUMPAD6) & 0x8000) result &= JoystickRight.Data;
        if ((GetKeyState(VK_NUMPAD0) & 0x8000) || readFireButton) result &= JoystickFire.Data;

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

