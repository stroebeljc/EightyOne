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

extern bool controllerPresent[16];

extern KeyInfo JoystickLeft1;
extern KeyInfo JoystickRight1;
extern KeyInfo JoystickUp1;
extern KeyInfo JoystickDown1;
extern KeyInfo JoystickFire1;

extern KeyInfo JoystickLeft2;
extern KeyInfo JoystickRight2;
extern KeyInfo JoystickUp2;
extern KeyInfo JoystickDown2;
extern KeyInfo JoystickFire2;

extern int NumberOfJoysticks();
extern void InitialiseJoysticks();
extern BYTE ReadJoystick1();
extern BYTE ReadJoystick2();
extern BYTE ReadJoystick1_Left();
extern BYTE ReadJoystick1_Right();
extern BYTE ReadJoystick1_Up();
extern BYTE ReadJoystick1_Down();
extern BYTE ReadJoystick1_Fire();
extern BYTE ReadJoystick1_RightUpDownFire();
extern void SetJoystick1Controller(int controllerIndex);
extern void SetJoystick2Controller(int controllerIndex);

#endif