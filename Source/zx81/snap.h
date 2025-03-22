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


#ifndef SNAP_H
#define SNAP_H

#ifdef __cplusplus
extern "C"
{
#endif

int load_snap(char *filename);
int save_snap(char *filename);
int load_file(char *filename);
int memory_load(char *filename, int address, int length, int secondbank = 0);
int memory_device_rom_load(char *filename, int address, int length);

#ifdef __cplusplus
}
#endif

#endif

