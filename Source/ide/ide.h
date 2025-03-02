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

#ifndef IDE_H
#define IDE_H

#ifdef __cplusplus
#include <vcl4.h>
#include <winioctl.h>
#include <stdio.h>


#define ACCESS_NONE 0
#define ACCESS_HDF  1
#define ACCESS_PHY  2

#define ATA_REG_DATA 0
#define ATA_REG_ERROR 1
#define ATA_REG_FEATURE 1
#define ATA_REG_SEC_CNT 2
#define ATA_REG_SEC 3
#define ATA_REG_CYL_LOW 4
#define ATA_REG_CYL_HIGH 5
#define ATA_REG_HEAD 6
#define ATA_REG_STATUS 7
#define ATA_REG_CMD 7
#define ATA_REG_HIGH 8

#define READ_SECTOR 0x20
#define READ_SECTOR_NV 0x21
#define WRITE_SECTOR 0x30
#define WRITE_SECTOR_NV 0x31
#define ID_DRIVE 0xec
#define ID_DRIVE_PACKET 0xa1
#define DRIVE_DIAGNOSTICS 0x90
#define SET_FEATURE     0xEF
#define INIT_PARAMS     0x91

#define ATA_READY 0
#define ATA_PIO_OUT 1
#define ATA_PIO_IN 2

#define ATA_ERR 0x01
#define ATA_DRQ 0x08
#define ATA_DRDY 0x40
#define ATA_BUSY 0x80

#define ATA_HEAD_HEAD 0x0f
#define ATA_HEAD_DEVICE 0x10
#define ATA_HEAD_LBA 0x40

#define ATA_ERR_OK 0x00
#define ATA_ERR_ABRT 0x04
#define ATA_ERR_IDNF 0x10
#define ATA_ERR_UNC 0x40

typedef struct DriveStruct
{
        int Drive;
        int Heads;
        int Cylinders;
        int Sectors;
        unsigned long Size;
        int Type;
        char Path[32];
        int ReadOnly;
} PHYSDRIVE;

typedef struct
{
	BYTE sig[0x06];
	BYTE id;
	BYTE revision;
	BYTE flags;
	BYTE data_low;
	BYTE data_high;
	BYTE reserved[0x0b];
} HDF_HEADER;

typedef struct
{
        char cookie[8];
        int features;
        int version;
        int offset1, offset2;
        int timestamp;
        int creator_app;
        int creator_ver;
        int creator_os;
        long origsz1, origsz2;
        long cursz1, cursz2;
        char cyl_h, cyl_l;
        char heads;
        char sectors;
        int disk_type;
        int checksum;
        int uid1, uid2, uid3, uid4;
        char state;
        char reserved[427];
} VHD_HEADER;

typedef struct
{
	char filename[256];
	FILE *f;
        HANDLE h;
	int data;
        int AccessMode;
	unsigned int sector_size;
	HDF_HEADER hdf;
        VHD_HEADER vhd;

        BYTE drive_id[512];
        int id_len;
	int cylinders;
	int heads;
	int sectors;
	BYTE error;
	BYTE status;
        unsigned long size;

        int read_only;
} ATA_DRIVE;

typedef struct
{
	ATA_DRIVE drive[2];

	BYTE feature;
	BYTE sector_count;
	BYTE cylinder_low;
	BYTE cylinder_high;
	BYTE head;
        BYTE sector;

        BYTE multi_sector;

        int DataHighByte;

	int action;
	int cur_drive;
	int data_counter;
        int sector_no;

        int mode;

	BYTE buffer[512];
} ATA_CHANNEL;

extern PHYSDRIVE PhysDrives[];
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ATA_MODE_8BIT 0
#define ATA_MODE_16BIT 1
#define ATA_MODE_16BIT_SWAPPED 2
#define ATA_MODE_16BIT_WRSWAP 3

void ATA_Init(void);
char *ATA_GetHDF(int drive);
int ATA_LoadHDF(int drive, char *FileName);
void ATA_EjectHDF(int drive);
void ATA_Reset(void);
int ATA_ReadRegister(int Register);
void ATA_WriteRegister(int Register, int Data);
void ATA_SetMode(int mode);
void DetectPhysDrives(void);
void ATA_GetCHS(int Drive, int *c, int *h, int *s, unsigned long *sectors);
void ATA_SetCHS(int Drive, int c, int h, int s);
int ATA_GetReadOnly(int drive);
void ATA_SetReadOnly(int drive, int read_only);

#ifdef __cplusplus
}
#endif
#endif

