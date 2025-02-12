#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <vcl4.h>

#include "kbstatus.h"

#define JOYSTICK_NONE           0
#define JOYSTICK_KEMPSTON       1
#define JOYSTICK_FULLER         2
#define JOYSTICK_CURSOR         3
#define JOYSTICK_INTERFACE2     4
#define JOYSTICK_ZXPAND         5
#define JOYSTICK_PROGRAMMABLE   6
#define JOYSTICK_BOLDFIELD      7
#define JOYSTICK_TIMEX          8

extern KeyInfo JoystickLeft;
extern KeyInfo JoystickRight;
extern KeyInfo JoystickUp;
extern KeyInfo JoystickDown;
extern KeyInfo JoystickFire;

extern KeyInfo JoystickLeft2;
extern KeyInfo JoystickRight2;
extern KeyInfo JoystickUp2;
extern KeyInfo JoystickDown2;
extern KeyInfo JoystickFire2;

extern int NumberOfJoysticks();
extern void InitialiseJoysticks();
extern BYTE ReadJoystick();
extern BYTE ReadJoystick2();
extern BYTE ReadJoystick_Left();
extern BYTE ReadJoystick_Right();
extern BYTE ReadJoystick_Up();
extern BYTE ReadJoystick_Down();
extern BYTE ReadJoystick_Fire();
extern BYTE ReadJoystick_RightUpDownFire();

#endif