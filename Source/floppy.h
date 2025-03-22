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

#include "765.h"

#ifdef __cplusplus
extern "C" {
#endif

extern FDC_PTR p3_fdc;
extern FDRV_PTR p3_drive_a;
extern FDRV_PTR p3_drive_b;
extern FDRV_PTR p3_drive_null;

void floppy_init(void);
void floppy_eject(int drive);
void floppy_setimage(int drive, char *filename, int readonly);
void floppy_ClockTick(int ts);
void floppy_shutdown(void);

BYTE floppy_get_state(void);

void floppy_set_motor(BYTE Data);
void floppy_write_cmdreg(BYTE Data);
void floppy_write_trackreg(BYTE Data);
void floppy_write_secreg(BYTE Data);
void floppy_write_datareg(BYTE Data);
BYTE floppy_read_datareg(void);
BYTE floppy_read_statusreg(void);
BYTE floppy_read_trackreg(void);
BYTE floppy_read_secreg(void);

BYTE OpusD6821Access(BYTE reg, BYTE Data, BYTE Dir);

#ifdef __cplusplus
}
#endif
