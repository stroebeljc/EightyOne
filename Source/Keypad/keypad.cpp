/* EightyOne - a Sinclair emulators.
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
 *
 */

#include <windows.h>
#include "zx81config.h"
#include "keypad.h"
#include "kbstatus.h"

CKeypad Keypad;

const int CKeypad::KeypadSocketOutLineLevelLow  = 0x00;
const int CKeypad::KeypadSocketOutLineLevelHigh = 0x01;
const int CKeypad::KeypadSocketInLineLevelLow  = 0x00;
const int CKeypad::KeypadSocketInLineLevelHigh = 0x20;

CKeypad::CKeypad()
{
        Reset();
}

int stateTimeoutTCylesLast;

void CKeypad::Reset()
{
        state = AwaitingPollRequest;
        substate = AwaitingReadBitSync;
        keypadSocketIn = KeypadSocketInLineLevelHigh;
        stateTimeoutTCyles = 0;

        for (int i = 0; i < 5; i++)
        {
                keyRowBits[i] = 0xF;
        }
}

void CKeypad::Write(int registerByte)
{
        keypadSocketOut = (registerByte & 0x01);

        switch (state)
        {
        case AwaitingPollRequest:
                if (keypadSocketOut == KeypadSocketOutLineLevelLow)
                {
                        keypadSocketIn = KeypadSocketInLineLevelLow;
                        state = AwaitingPollRequestEnd;
                        stateTimeoutTCyles = 5000;
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingPollRequestEnd:
                if (keypadSocketOut == KeypadSocketOutLineLevelHigh)
                {
                        keypadSocketIn = KeypadSocketInLineLevelHigh;
                        state = AwaitingScanRequest;
                        stateTimeoutTCyles = 5000;
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingScanRequest:
                if (keypadSocketOut == KeypadSocketOutLineLevelLow)
                {
                        state = AwaitingReadKeypadID;
                        substate = AwaitingReadBitSync;
                        stateTimeoutTCyles = 5500;
                        keyBitsToSend = 0x4;
                        bitsToSend = 4;
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingReadKeypadID:
                HandleRowRead();
                break;

        case AwaitingReadRowFlag:
                HandleRowRead();
                break;

        case AwaitingReadRowBits:
                HandleRowRead();
                break;
        }
}

void CKeypad::HandleRowRead()
{
        switch (substate)
        {
        case AwaitingReadBitSync:
                if (keypadSocketOut == KeypadSocketOutLineLevelHigh)
                {
                        keypadSocketIn = KeypadSocketInLineLevelLow;
                        stateTimeoutTCyles = 19000;
                        substate = AwaitingReadBitSyncEnd;

                        if (state == AwaitingReadRowFlag)
                        {
                                int keyBits = FetchRowKeys();

                                if (keyBits != keyRowBits[row])
                                {
                                        keyRowBits[row] = keyBits;
                                        keyBitsToSend = (keyRowBits[row] << 1) | 1;
                                        bitsToSend = 5;
                                }
                                else
                                {
                                        keyBitsToSend = 0;
                                        bitsToSend = 1;
                                }
                        }
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingReadBitSyncEnd:
                if (keypadSocketOut == KeypadSocketOutLineLevelLow)
                {
                        keypadSocketIn = (keyBitsToSend & 0x01) ? KeypadSocketInLineLevelHigh: KeypadSocketInLineLevelLow;
                        keyBitsToSend >>= 1;

                        substate = AwaitingReadBit;
                        stateTimeoutTCyles = 1500;
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingReadBit:
                if (keypadSocketOut == KeypadSocketOutLineLevelHigh)
                {
                        keypadSocketIn = KeypadSocketInLineLevelHigh;
                        substate = AwaitingReadBitEnd;
                        stateTimeoutTCyles = 1500;
                }
                else
                {
                        Reset();
                }
                break;

        case AwaitingReadBitEnd:
                if (keypadSocketOut == KeypadSocketOutLineLevelLow)
                {
                        bitsToSend--;
                        
                        if (bitsToSend > 0)
                        {
                                if (state == AwaitingReadRowFlag)
                                {
                                        state = AwaitingReadRowBits;
                                }

                                substate = AwaitingReadBitSync;
                                stateTimeoutTCyles = 2000;
                        }
                        else
                        {
                                if (state == AwaitingReadKeypadID)
                                {
                                        state = AwaitingReadRowFlag;
                                        substate = AwaitingReadBitSync;
                                        stateTimeoutTCyles = 2000;
                                        row = 0;
                                }
                                else
                                {
                                        row++;
                                        if (row == 5)
                                        {
                                                row = 0;
                                                stateTimeoutTCyles = 0;
                                        }
                                        else
                                        {
                                                stateTimeoutTCyles = 5000;
                                        }
                                        state = AwaitingReadRowFlag;
                                        substate = AwaitingReadBitSync;
                                }
                        }  
                }
                else
                {
                        Reset();
                }
                break;
        }
}

int CKeypad::Read()
{
        return keypadSocketIn;
}

int CKeypad::FetchRowKeys()
{
        int keyData;

        if ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) == 0x8000)
        {
                keyData = FetchRowKeys_Keyboard();
        }
//      else
//      {
//              keyData = FetchRowKeys_NumericPad();
//      }

        return keyData;
}

inline int CKeypad::FetchRowKeys_Keyboard()
{
        int keyData = 0x00;

        switch (row)
        {
        default:
        case 0: // Row 5
                if (GetAsyncKeyState('0') & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000) keyData |= 0x02;
                break;

        case 1: // Row 4
                if (GetAsyncKeyState('1') & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState('2') & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState('3') & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_RETURN) & 0x8000) keyData |= 0x01;
                break;

        case 2: // Row 1
                if (GetAsyncKeyState(VK_OEM_2) & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_OEM_7) & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState(VK_OEM_4) & 0x8000)  keyData |= 0x02;
                if (GetAsyncKeyState(VK_OEM_6) & 0x8000)  keyData |= 0x01;
                break;

        case 3: // Row 2
                if (GetAsyncKeyState('7') & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState('8') & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState('9') & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) keyData |= 0x01;
                break;

        case 4: // Row 3
                if (GetAsyncKeyState('4') & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState('5') & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState('6') & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) keyData |= 0x01;
                break;
        }

        return keyData;
}
/*
inline int CKeypad::FetchRowKeys_NumericPad()
{
        int keyData = 0x00;

        switch (row)
        {
        default:
        case 0: // Row 5
                if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_DECIMAL) & 0x8000) keyData |= 0x02;
                break;

        case 1: // Row 4
                if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_RETURN)  & 0x8000) keyData |= 0x01;
                break;

        case 2: // Row 1
                if (GetAsyncKeyState(VK_DIVIDE)   & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState(VK_NUMLOCK)  & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_LAUNCH_APP1) & 0x8000) keyData |= 0x01;
                break;

        case 3: // Row 2
                if (GetAsyncKeyState(VK_NUMPAD7)  & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_NUMPAD8)  & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState(VK_NUMPAD9)  & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000) keyData |= 0x01;
                break;

        case 4: // Row 3
                if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) keyData |= 0x08;
                if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) keyData |= 0x04;
                if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000) keyData |= 0x02;
                if (GetAsyncKeyState(VK_ADD)     & 0x8000) keyData |= 0x01;
                break;
        }

        return keyData;
}
*/
void CKeypad::ClockTick(int ts)
{
        if (stateTimeoutTCyles > 0)
        {
                stateTimeoutTCyles -= ts;
                
                if (stateTimeoutTCyles <= 0)
                {
                        Reset();
                }
        }
}

