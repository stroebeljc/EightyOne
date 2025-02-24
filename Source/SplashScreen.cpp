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

#include <vcl4.h>
#pragma hdrstop

#include "SplashScreen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplash *Splash;
//---------------------------------------------------------------------------
__fastcall TSplash::TSplash(TComponent* Owner)
        : TForm(Owner)
{
        Left = (Screen->Width - Width) /2;
        Top = (Screen->Height - Height) /2;
        Progress->Position=0;
        SetWindowPos(Handle, HWND_TOPMOST,Left,Top,Width,Height,NULL);
}
//---------------------------------------------------------------------------
void TSplash::SetProgress(int Max, AnsiString text)
{
        Progress->Max=Max;
        Action->Caption="Initialising "+text;
}

void TSplash::IncProgress(AnsiString text)
{
        Action->Caption="Initialising "+text+"...";
        Progress->Position ++;
        Application->ProcessMessages();
}






