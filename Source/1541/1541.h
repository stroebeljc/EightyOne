/*
 *	The C64 emulator
 *
 *	Copyright 1992-96 by ALE.
 *	written by Andreas Arens.
 *
 *	DOS 2.6 single drive file io emulation
 *------------------------------------------------------------------------------
 * $Id$
 * $Log$
 *------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------*/

extern void Cleanup(void);
extern void LedOn(void);
extern void LedOff(void);
extern void LedFlash(void);
extern void VicMessage(const char*, int);

/*----------------------------------------------------------------------------*/

#define PACKED(x) x

struct BAM_track {
    unsigned char 	PACKED(freecnt);
    unsigned char 	PACKED(bitmap[3]);
};

struct TrackSector {
    unsigned char	PACKED(track);
    unsigned char	PACKED(sector);
};

struct BAM {
    struct TrackSector	PACKED(dirstart);
    unsigned char 	PACKED(format);
    unsigned char 	PACKED(notused1[1]);
    struct BAM_track 	PACKED(bam[35]);
    unsigned char 	PACKED(diskname[16]);
    unsigned char 	PACKED(shiftedspaces[2]);
    unsigned char 	PACKED(diskid[2]);
    unsigned char 	PACKED(shiftedspace[1]);
    unsigned char 	PACKED(dosversion[2]);
    unsigned char 	PACKED(shiftedspaces2[4]);
    unsigned char 	PACKED(notused2[85]);
};

struct FileEntry {				/* repeated 8 times per sector */
    struct TrackSector	PACKED(dirnext);	/* chain: only valid for first entry, 0/FF for last */
    unsigned char 	PACKED(type);		/* file type */
    struct TrackSector	PACKED(datastart);	/* first sector of file data */
    unsigned char 	PACKED(name[16]);	/* file name padded with 0xA0 */
    struct TrackSector	PACKED(sidestart);	/* rel file: location of first side sector */
    unsigned char 	PACKED(recordsize);
    unsigned char 	PACKED(notused[6]);
    unsigned char 	PACKED(size[2]);
};

struct DataBlock {
    struct TrackSector	PACKED(datanext);	/* or 0/N-1 dat bytes for last sector */
    unsigned char 	PACKED(data[254]);
};

struct SideSector {
    struct TrackSector	PACKED(sidenext);	/* or 0/FF for last side sector */
    unsigned char	PACKED(sidenumber);	/* number of this side sector */
    unsigned char 	PACKED(recordsize);
    struct TrackSector	PACKED(sidesectors[6]);	/* other side sectors */
    struct TrackSector	PACKED(datasectors[120]);
};

struct c4entry {
    char		PACKED(name[17]);
    union {
	char		PACKED(osname[17]);
	struct {
	    unsigned char	PACKED(pad[2]);	/* osname[0] = 0 in d64 mode! */
	    unsigned char	PACKED(starttrack);
	    unsigned char	PACKED(startsector);
	    unsigned char	PACKED(sidetrack);
	    unsigned char	PACKED(sidesector);
	} d64;
    } u;
    unsigned char	PACKED(mode);
    unsigned char	PACKED(flags);
    unsigned long	PACKED(size);
    unsigned char	PACKED(recordsize);
    unsigned char	PACKED(pad);
};

#define FLG_PC64	1
#define FLG_LOCK	2	/* Write protect with '<' */
#define FLG_UNCLOSED	4	/* D64: file with '*' */
#define FLG_REL		8

struct pc64entry {
    char   		PACKED(detect[8]);
    char   		PACKED(name[17]);
    unsigned char	PACKED(recordsize);
};

#define PC64ID "C64File"

#ifdef M_UNIX
#pragma pack()
#endif

/* Known D64 file sizes */
#define D64_35_SIZE		(683 * 256)	/* 35 tracks */
#define D64_35_EINFO_SIZE	(683 * 257)	/* + one byte error code per track */
#define D64_40_SIZE		(768 * 256)	/* 40 tracks */
#define D64_40_EINFO_SIZE	(768 * 257)
#define D64_42_SIZE		(802 * 256)	/* 42 tracks */
#define D64_42_EINFO_SIZE	(802 * 257)

/* D64 image block flags */
#define IF_USED 1
#define IF_DIRTY 2

/* emu_mode types */
#define EMU_MODE_FILESYS 0
#define EMU_MODE_DIRFILE 1
#define EMU_MODE_D64     2

#define DEF_UNIT 8      /* iec unit */

#ifndef __GO32__
#if (defined(SYSV) || defined(M_UNIX)) && !defined(__linux__)
#define DEF_PRINTCMD    "lp -s -R %s"
#else
#define DEF_PRINTCMD    "lpr -s -r %s"
#endif
#define DEF_PRINTER 4   /* iec unit */
#define SET_PRINTFILE   "/usr/tmp/c64XXXXXX.pr"
#define DEF_PRINTFILE   PrintFile
#endif

#ifdef DIRFILE
#define DIRFILENAME "dirfile.c64"
#endif

#define NCHAN    16     /* # of chanels */
#define CMD_CHAN 15

/* global flags */
#define F_OPEN            1
#define F_FILEVALID       2
#define F_RELATIVE        4
#define F_ERRORMSG        8
#define F_INLISTEN       16
#define F_INNAMELISTEN   32
#define F_INTALK         64
#define F_INCLOSE       128
#define F_WRITE         0x100
#define F_READ          0x200
#define F_OVERWRITE     0x400
#define F_CHANOPEN      0x800

#define F_PR_LISTEN     0x1000
#define F_PR_WRITE      0x2000
#define F_PR_CLOSE      0x4000

#define MODE_R 'R'	/* OPEN modes */
#define MODE_W 'W'
#define MODE_A 'A'
#define MODE_M 'M'      /* open unclosed */
#define MODE_NOTHING	0

#define TYPE_NOTHING	0		/* this MUST be 0 */
#define TYPE_DIR	1
#define TYPE_BIG	2

#define TYPE_P 'P'	/* FILE types */
#define TYPE_S 'S'
#define TYPE_U 'U'
#define TYPE_D 'D'      /* del */
#define TYPE_L 'L'      /* rel */

/* unix stat to file type mapping */
#define STAT_MASK (S_ISUID | S_ISGID)
#define STAT_PRG 0
#define STAT_SEQ (S_ISUID)
#define STAT_USR (S_ISGID)
#define STAT_REL (S_ISUID|S_ISGID)
