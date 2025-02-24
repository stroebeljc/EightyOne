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

#ifndef _IECBUS_H
#define _IECBUS_H

#ifdef __cplusplus
extern "C" {
#endif
extern void IECAssertReset(int DeviceNo);
extern void IECAssertATN(int DeviceNo);
extern void IECAssertClock(int DeviceNo);
extern void IECAssertData(int DeviceNo);

extern void IECReleaseReset(int DeviceNo);
extern void IECReleaseATN(int DeviceNo);
extern void IECReleaseClock(int DeviceNo);
extern void IECReleaseData(int DeviceNo);

extern int IECIsReset(void);
extern int IECIsATN(void);
extern int IECIsClock(void);
extern int IECIsData(void);

extern void IECReset(void);
extern void IECClockTick(int ts);
#ifdef __cplusplus
}
#endif

#endif

