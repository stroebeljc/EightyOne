#include <windows.h>

static DWORD xPosMinTrip;
static DWORD xPosMaxTrip;
static DWORD yPosMinTrip;
static DWORD yPosMaxTrip;

void initJoy(void)
{
   // don't bother with error checking;
   // if this fails the joyGetPosEx call will fail too.

   JOYCAPS caps;
   joyGetDevCaps(JOYSTICKID1, &caps, sizeof(JOYCAPS));

   DWORD xTripOffset = (caps.wXmax - caps.wXmin) / 4;
   DWORD yTripOffset = (caps.wYmax - caps.wYmin) / 4;

   xPosMinTrip = caps.wXmin + xTripOffset;
   xPosMaxTrip = caps.wXmax - xTripOffset;

   yPosMinTrip = caps.wYmin + yTripOffset;
   yPosMaxTrip = caps.wYmax - yTripOffset;
}

unsigned char readJoy(void)
{
   JOYINFOEX joyInfo;
   joyInfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;
   joyInfo.dwSize = sizeof(joyInfo);

   unsigned char joypos = 0xfe;

   if (JOYERR_NOERROR == joyGetPosEx(JOYSTICKID1, &joyInfo))
   {
      if (joyInfo.dwYpos <= yPosMinTrip) joypos &= 0x7f;     // up
      if (joyInfo.dwYpos >= yPosMaxTrip) joypos &= 0xbf;     // down
      if (joyInfo.dwXpos <= xPosMinTrip) joypos &= 0xdf;     // left
      if (joyInfo.dwXpos >= xPosMaxTrip) joypos &= 0xef;     // right
      if (joyInfo.dwButtons != 0)        joypos &= 0xf7;     // fire
   }

   if (GetKeyState(VK_NUMPAD8) & 0x8000)
   {
      joypos &= 0x7f;
   }
   else if (GetKeyState(VK_NUMPAD2) & 0x8000)
   {
      joypos &= 0xbf;
   }

   if (GetKeyState(VK_NUMPAD4) & 0x8000)
   {
      joypos &= 0xdf;
   }
   else if (GetKeyState(VK_NUMPAD6) & 0x8000)
   {
      joypos &= 0xef;
   }

   if (GetKeyState(VK_NUMPAD0) & 0x8000)
   {
      joypos &= 0xf7;
   }

   return joypos;
}
