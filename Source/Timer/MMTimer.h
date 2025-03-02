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

#ifndef MMTimerH
#define MMTimerH
//---------------------------------------------------------------------------
#include <vcl\SysUtils.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Forms.hpp>
#include <windows.h>
#include <mmsystem.h>
//---------------------------------------------------------------------------
typedef enum TimerEventType{etOneShot,etPeriodic}TTimerEventType;
class PACKAGE TMMTimer : public TComponent
{
private:
	TNotifyEvent 	FOnMMTimer;
   MMRESULT			FTimerID;
   int	FInterval;
   int	FResolution;
   bool				FEnabled;
   TTimerEventType FType;
   MMRESULT __fastcall StartTimer();
   void __fastcall SetInterval(int Value);
   void __fastcall SetResolution(int Value);
   void __fastcall SetType(TTimerEventType Value);
   static void __stdcall MMTimerCallbackProc(UINT TimerId,UINT uMsg,DWORD dwUser,
  												DWORD dw1,DWORD dw2);
protected:
public:
        __fastcall TMMTimer(TComponent* Owner);
   __fastcall ~TMMTimer();
   
__published:
   __property TNotifyEvent OnMMTimer={read=FOnMMTimer,write=FOnMMTimer};
   __property int Interval={read=FInterval,write=SetInterval};
   __property int Resolution={read=FResolution,write=SetResolution};
   __property bool Enabled={read=FEnabled,write=FEnabled};
   __property TTimerEventType Type={read=FType,write=SetType};
};
//---------------------------------------------------------------------------
#endif
