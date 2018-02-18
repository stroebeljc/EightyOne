/*
 *	The C64 emulator
 *
 *	Copyright 1992-96 by ALE.
 *	written by Andreas Arens.
 *
 *	DOS 2.6 single drive file io emulation
 *------------------------------------------------------------------------------
 * $Id: 1541.c,v 1.23 1996/09/27 23:40:07 ari Exp root $
 * $Log: 1541.c,v $
 * Revision 1.23  1996/09/27 23:40:07  ari
 * Support for filenames starting with '@'. Formating D64 added.
 *
 * Revision 1.22  1996/08/12 01:10:31  ari
 * Write protect and block-write support added.
 *
 * Revision 1.21  1996/07/06 22:54:31  ari
 * Moved structures into own headerfile, D64 block-write support.
 *
 * Revision 1.20  1996/07/05 01:40:28  ari
 * Much rewritten, more D64 support.
 *
 * Revision 1.19  1996/06/20 21:01:23  ari
 * started implement D64 support, Changed IEC_STATUS.
 *
 * Revision 1.18  1996/06/16 17:09:35  ari
 * Full x00 support, bug fixes.
 *
 * Revision 1.17  1995/06/01  12:07:28  ari
 * fixed treatment of channel 0 and 1 for non-prog files
 *
 * Revision 1.16  1995/05/03  21:22:17  ari
 * allowed explict types on channel 0 and 1
 *
 * Revision 1.15  1995/02/13  11:27:22  ari
 * second linux port ( by ari ) integration
 *
 * Revision 1.14  1994/07/16  13:58:42  ari
 * new go32: avoid duplicate define.
 *
 * Revision 1.13  1993/12/30  11:48:12  ari
 * go32 changes
 *
 * Revision 1.12  1993/09/17  11:28:20  ari
 * full file type support for non-dirfile unix filesystems
 *
 * Revision 1.11  1993/09/07  21:41:25  ari
 * fixed rename, added multifile scratch with ',', added copy
 *
 * Revision 1.10  1993/09/04  14:25:42  ari
 * support for dirfiles ( required for msdos )
 *
 * Revision 1.9  1993/08/31  20:30:03  ari
 * go32 initial support
 *
 * Revision 1.8  1993/06/13  11:52:59  ari
 * enhanced filename processing, unique printfile name generation
 *
 * Revision 1.7  1993/05/08  21:46:37  ari
 * implemented scratch
 *
 * Revision 1.6  1993/01/05  12:41:24  ari
 * added SCCS string
 *
 * Revision 1.4  1992/07/28  19:43:33  ari
 * Warnings removed.
 *
 * Revision 1.3  1992/07/20  04:15:46  ari
 * Unused variables removed.
 *
 * Revision 1.2  1992/07/13  04:36:26  ari
 * IEC-OPEN "*" added.
 *
 * Revision 1.1  1992/07/11  21:52:24  ari
 * Initial revision
 *
 *------------------------------------------------------------------------------
 */

#ifdef ENABLE1541

#include "config.h"
#include "md.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//#   include <sys/file.h>
#   ifndef S_ISREG
#	define S_ISREG(x) (x & S_IFREG)
#   endif
#   define S_ISUID	1
#   define S_ISGID	2
#   include <dos.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <io.h>
//#include <unistd.h>

#include "1541.h"

#define R_OK 4
#define W_OK 2

/*----------------------------------------------------------------------------*/

#define noDEBUG_BLOCK_CMDS

/*----------------------------------------------------------------------------*/

static struct c4entry * c4dhook = NULL;
static int nc4d = 0;
static int dirgetidx = -1;
static int findfileidx = -1;
static char dirgetbuf[32];
static int dirisopen = 0;
static int dirlen;
static int dirpos;
static int dirnmatch = 0;
static char dirmatch[32];
static char dirintro[] = "\001\010\001\001\000\000\022\"";
			/*      ^-- 004, but helps ,8,1 kernel */

#ifdef DIRFILE
static int nc4wr = 0;
static struct c4entry c4wrhook[16];
static char dirfilebuf[256];
#endif

char *FloppyImage = NULL;
int ImageRO = 0;

static int ImageFD = -1;
static unsigned char *ImageData = NULL;
static unsigned char *ImageErrInfo;
static int ImageDataSize = 0;
static unsigned char ImageFlags[802];
static int d64_has_errinfo = 0;
static int d64_tracks = 0;
static struct BAM *d64_BAM;

static int emu_mode = 0;
int IEC_Status;				/* status of last TALK,LISTEN,... */

#define IEC_SetStatus(x)	(IEC_Status = x) 

void Init_IECDos();

#ifdef PRINTER_SUPPORT
void ClosePrinter();
static char PrintFile[32];
static int printer_unit;
static int printer_cmd;
static int printer_open = 0;
static int printer_fd;
char *printer_printcmd = DEF_PRINTCMD;
#endif

static int globflags;		/* bus flags */
static int aktunit;		/* c64 */

#ifndef __GO32__
static mode_t UMask;
#endif

static int unit;
static int channel;
static char filenamebuf[NCHAN][64];    /* buffer for filenames */
static int filenamelen[NCHAN];
static char unixname[NCHAN][32];
static long filelen[NCHAN];
static long filepos[NCHAN];
static int chfd[NCHAN];
static int fmod[NCHAN];
static int flags[NCHAN];
static char chanbuf[NCHAN][256];
static int chanbufp[NCHAN];
static long chanpos[NCHAN];
static int errorcode;
static int errortrack;
static int errorsector;
char errorbuf[64];

static int ftype;
static int typemode;           /* after '=' */
static int openmode;
static int overwrite;          /* starts with '@' */
static int dir;                /* starts with '$' */
static int chanopen;
static char nbuf[34];
static struct pc64entry pc64header;

#ifdef __GO32__
static char *emulver = "@(#) ALE DOSEMUL V1.9.5 MSDOS";
#else
static char *emulver = "@(#) ALE DOSEMUL V1.9.5 UNIX";
#endif
static char *errors[] = {
    "OK",
    "FILES SCRATCHED",
    "",
    "UNIMPLEMENTED FEATURE",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "READ ERROR",
    "READ ERROR",
    "READ ERROR",
    "READ ERROR",
    "READ ERROR",
    "WRITE ERROR",
    "WRITE PROTECT ON",
    "READ ERROR",
    "WRITE ERROR",
    "DISK ID MISMATCH",
    "SYNTAX ERROR",
    "SYNTAX ERROR",
    "SYNTAX ERROR",
    "SYNTAX ERROR",
    "SYNTAX ERROR",
    "",
    "",
    "",
    "",
    "FILE NOT FOUND",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "RECORD NOT PRESENT",
    "OVERFLOW IN RECORD",
    "FILE TO LARGE",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "WRITE FILE OPEN",
    "FILE NOT OPEN",
    "FILE NOT FOUND",
    "FILE EXISTS",
    "FILE TYPE MISMATCH",
    "NO BLOCK",
    "ILLEGAL TRACK OR SECTOR",
    "ILLEGAL SYSTEM T OR S",
    "",
    "",
    "NO CHANNEL",
    "DIR ERROR",
    "DISK FULL",
    "",
    "DRIVE NOT READY",
    "FORMAT SPEED ERROR"
};

static int ZBR_ns[43] = {
     0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    19, 19, 19, 19, 19, 19, 19,
    18, 18, 18, 18, 18, 18,
    17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17
};

static int ZBR_st[43] = {
      0,
      0,  21,  42,  63,  84, 105, 126, 147, 168, 189, 210, 231, 252, 273, 294, 315, 336,
    357, 376, 395, 414, 433, 452, 471,
    490, 508, 526, 544, 562, 580,
    598, 615, 632, 649, 666,
    683, 700, 717, 734, 751, 768, 785
};

static int D64EIMap[] = {
     0,
     0,	/*  1: OK */
    20,	/*  2: block header not found */
    21,	/*  3: sync not found */
    22,	/*  4: data block not found */
    23,	/*  5: data checksum error */
     0,	/*  6: unused */
    25,	/*  7: verify failed */
    26,	/*  8: write protect on */
    27,	/*  9: header checksum error */
     0,	/* 10: unused */
    29,	/* 11: ID mismatch */
     0,	/* 12: unused */
     0,	/* 13: unused */
     0,	/* 14: unused */
    74,	/* 15: drive not ready (or present!) */
};

static int ErrInfo2Error(int ei) {
    return D64EIMap[ei & 15];
}

/*
 *	Print an unimplemented command.
 */
static void NotImplemented(CONST unsigned char* cmd)
{
#ifndef __GO32__
    printf("1541: not implemented '");
    while (*cmd) {
	if (' ' <= *cmd && *cmd < 127)
	    printf("%c", *cmd);
	else
	    printf("\\%02x", *cmd);
	++cmd;
    }
    printf("'\n");
#endif
}

#ifdef __GO32__
int freeblks(char *n)
{
    union REGS in, out;
    unsigned long bytes = 0;

    memset(&in, 0, sizeof(in));
    in.h.ah = 0x36;
    intdos(&in, &out);
    if (out.x.ax != 0xffff)
	bytes = out.x.bx * out.x.ax * out.x.cx;
    return (int)(bytes / 254);
}

char *systemname(void)
{
    return "dos";
}

#else

int freeblks(char *n)
{
//    struct statfs sfs;
//
//#ifdef __linux__
//    if (!statfs(n,&sfs))
//	return sfs.f_bsize * sfs.f_bavail / 256;
//#else
//    if (!statfs(n,&sfs,sizeof(sfs),0)) {
//	/* sfs.f_bsize is correctly set to 1024, but
//	   sfs.f_bfree returns the number of free 512 byte blocks... */
//	return sfs.f_bfree * 512 / 254;
//    }
//#endif
    return 0;
}

char *systemname(void)
{
//    static struct utsname uts;
//
//    if (uname(&uts) == -1)
//	strcpy(uts.sysname,"unix");
//    return uts.sysname;
}
#endif

static char asctocbm(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
	return c & ~0x20;
    if (c >= 'A' && c <= 'Z')
	return c | 0x80;
    return c;
}

static char cbmtoasc(unsigned char c)
{
    if (c >= ('A' | 0x80) && c <= ('Z' | 0x80))
	return c & 0x7f;
    if (c >= 'A' && c <= 'Z')
	return c | 0x20;
    return c;
}

static char *strcbm2asc(char *s)
{
    char *s2 = s;

    while (*s2)
	*s2++ = cbmtoasc(*s2);
    return s;
}

static char *strasc2cbm(char *s)
{
    char *s2 = s;

    while (*s2)
	*s2++ = asctocbm(*s2);
    return s;
}


#ifdef ALWAYS_SAVE_P00
/*
 *	Convert CBM name to P00 name.
 *	(16 character C64 character set to 8 characters limited ASCII set).
 *	by johns
 */
static char *cbm2P00name(const char *cbm)
{
    static char p00[16];
    char buf[16];
    char *dst;
    int i, l;
    unsigned char uc;

    dst = buf;
    /*
    **	1) Convert to lowercase, underscore and numbers only.
    */
    do {
	switch (*cbm) {
	    case '\0':
		break;
	    case 'a' ... 'z':
	    case '0' ... '9':		/* lowercase and numbers ok */
		*dst++ = *cbm++;
		break;
	    case 'A' ... 'Z':		/* uppercase -> lowercase */
		*dst++ = *cbm++ + ' ';
		break;
	    case '-':
	    case ' ':			/* spaces -> underscore */
		*dst++ = '_';
		/* fall through */
	    default:
	    	cbm++;
		break;
	}
    } while (*cbm);

    l = dst - buf;
    if (l > 8) {
	/*
	 *	2) Reduce to 8 characters. (remove from end)
	 */
	memset(buf + l, 0, 16 - l);

	/*
	 *	2a) Remove whitspace.
	 */
	for (i = 15; i >= 0 && l > 8; --i) {
	    if (buf[i] == '_') {
		buf[i] = '\0';
		--l;
	    }
	}
	
	/*
	 *	2b) Remove vocals.
	 */
	for (i = 15; i >= 0 && l > 8; --i) {
	    uc = (unsigned char)buf[i] & (unsigned char)0xDF;
	    if (uc == 'a' || uc == 'e' || uc == 'i' || uc == 'o' || uc == 'u') {
		buf[i] = '\0';
		--l;
	    }
	}
	
	/*
	 *	2c) Remove characters.
	 */
	for (i = 15; i >= 0 && l > 8; --i) {
	    if ((buf[i] >= 'a' && buf[i] <= 'z') || (buf[i] >= 'A' && buf[i] <= 'Z')) {
		buf[i] = '\0';
		--l;
	    }
	}

	/*
	 *	2d) Remove digits.
	 */
	for (i = 15; i >= 0 && l > 8; --i) {
	    if (buf[i] >= '0' && buf[i] <= '9') {
		buf[i] = '\0';
		--l;
	    }
	}

	/*
	 *	2e) Pack the name.
	 */
	dst = buf;
	for (i = 0; i < 16; ++i) {
	    if (buf[i])
		*dst++ = buf[i];
	}
	if (dst - buf != l) {
	    Cleanup();
	    abort();
	}
    }
    if (l == 0) {
	/*
	 *	Empty name.
	 */
	p00[0] = '_';
	p00[1] = '\0';
    } else {
	memcpy(p00, buf, l);
	p00[l] = '\0';
    }
    return p00;
}

static char *MakeP00(char *nam)
{
    char *cp, *cp2;

    cp = cbm2P00name(nam);
    cp2 = cp + strlen(cp);
    *cp2++ = '.';
    *cp2++ = (unsigned char)ftype | (unsigned char)0x20;
    strcpy(cp2, "00");
    while (access(cp, 0) != -1) {
	cp2[1]++;
	if (cp2[1] == '9' + 1) {
	    cp2[1] = '0';
	    cp2[0]++;
	}
    }
    return cp;
}
#endif

static void ReWriteImage(void)
{
    int i;

    if (ImageRO || ImageFD < 0 || !ImageData)
	return;
    for (i = 0; i < 802; i++) {
	if ((ImageFlags[i] & IF_DIRTY)) {
	    if (i * 256 < ImageDataSize) {
		if (lseek(ImageFD, i * 256, SEEK_SET) >= 0)
		    write(ImageFD, ImageData + i * 256, 256);
	    }
	    ImageFlags[i] &= ~IF_DIRTY;
	}
    }
}

static int BAM_alloc(unsigned int track, unsigned int sector)
{
    struct BAM_track *trackmap;

    if (--track < 35)	/* no BAM support for tracks behind 35 */
    {
	trackmap = d64_BAM->bam + track;
	if ((trackmap->bitmap[sector >> 3] & ((unsigned char)1 << (sector & 7))) != 0) {
	    trackmap->bitmap[sector >> 3] &= (unsigned char)~((unsigned char)1 << (sector & 7));
	    trackmap->freecnt--;
	    if (!ImageRO)
		ImageFlags[ZBR_st[18]] |= IF_DIRTY;
	    return 1;
	}
    }
    return 0;
}

static int BAM_free(unsigned int track, unsigned int sector)
{
    struct BAM_track *trackmap;

    if (--track < 35)	/* no BAM support for tracks behind 35 */
    {
	trackmap = d64_BAM->bam + track;
	if ((trackmap->bitmap[sector >> 3] & ((unsigned char)1 << (sector & 7))) == 0) {
	    trackmap->bitmap[sector >> 3] |= (unsigned char)1 << (sector & 7);
	    trackmap->freecnt++;
	    if (!ImageRO)
		ImageFlags[ZBR_st[18]] |= IF_DIRTY;
	    return 1;
	}
    }
    return 0;
}

static int BAM_find_free_sector_on_track(unsigned int track, unsigned int sector)
{
    unsigned int shift, i, s;

    shift = 0;
    while (shift < 3) {
	for (i = 0; i < ZBR_ns[track]; i += 3) {
	    s = sector + i + shift;
	    if (s >= ZBR_ns[track])
		s -= ZBR_ns[track];
	    if (BAM_alloc(track, s))
		return s;
	}
	shift++;
    }
    return -1;
}

static int BAM_next_free_sector(unsigned int *track, unsigned int *sector)
{
    unsigned int t, s = -1;

    t = *track;
    if (t == 18) {		/* directory */
	s = BAM_find_free_sector_on_track(t, *sector);
	if (s == -1) {
	    *track = 0;
	    *sector = 0;
	    return 0;
	}
	*sector = s;
	return 1;
    } else if (t < 18) {	/* lower half */
	while (t > 0) {
	    s = BAM_find_free_sector_on_track(t, *sector);
	    if (s != -1)
		break;
	    *sector = 0;
	    t--;
	}
	if (s == -1) {
	    t = *track + 1;
	    while (t < 36) {
		if (t != 18)	/* FIXME: this is BETTER then original, which CAN return tracks from directory */
		    s = BAM_find_free_sector_on_track(t, *sector);
		if (s != -1)
		    break;
		t++;
	    }
	}
	if ( s == -1) {
	    *track = 0;
	    *sector = 0;
	    return 0;
	}
	*sector = s;
	*track = t;
	return 1;
    } else {
	while (t < 36) {
	    s = BAM_find_free_sector_on_track(t, *sector);
	    if (s != -1)
		break;
	    *sector = 0;
	    t++;
	}
	if (s == -1) {
	    t = *track - 1;
	    while (t > 0) {
		if (t != 18)	/* FIXME: this is BETTER then original, which CAN return tracks from directory */
		    s = BAM_find_free_sector_on_track(t, *sector);
		if (s != -1)
		    break;
		t--;
	    }
	}
	if ( s == -1) {
	    *track = 0;
	    return 0;
	}
	*sector = s;
	*track = t;
	return 1;
    }
}

/* 
 * Returns the absolute sector of of a given track/sector pair
 * This asumes standard 2A format ZBR
 */
int AbsoluteSector(unsigned int track, unsigned int sector)
{
    if (track == 0 || track > d64_tracks || sector >= ZBR_ns[track])
	return -1;
    return ZBR_st[track] + sector;
}

/*
 * Returns the pointer to a sector
 */
unsigned char *SectorPointer(unsigned int track, unsigned int sector)
{
    int i;

    i = AbsoluteSector(track, sector);
    if (i < 0)
	return NULL;
    return ImageData + i * 256;
}

/*
 * Returns the error to a sector
 */
int SectorError(unsigned int track, unsigned int sector)
{
    int i;

    i = AbsoluteSector(track, sector);
    if (i < 0)
	return 4;			/* NO BLOCK */
    if (d64_has_errinfo)
	return ImageErrInfo[i];
    return 1;
}

/*
 * Handle changed sector
 */
void SectorChanged(unsigned int track,unsigned int sector)
{
    int i;

    i = AbsoluteSector(track, sector);

    ImageFlags[i] |= IF_DIRTY;
    ReWriteImage();
}

/***** simple_match *****
 *
 * look for match 'string' 'pattern'
 * Pattern may be:  ?       matches a single char
 *                  *       matches any number of chars (even 0)
 * TRUE: if pattern matches string
 * FALSE: if not
 */

int simple_match(char *s, char *p)
{
    while (*p)
    {
	switch (*p)
	{
	    case '?':   if (*s++ == '\0')
			    return 0;
			p++;
			break;

	    case '*':   /* next 4 lines only for speed. only p++ necessary */
			while (*++p == '*')
			    ;
			if (*p == '\0')
			    return 1;

			while (!simple_match(s, p))
			    if (*s++ == '\0')
				return 0;
			return 1;

	    case '\\':  if (*++p == '\0')
			    break;

	    default:    if (((unsigned char)*s++ & (unsigned char)0xDF) != ((unsigned char)*p++ & (unsigned char)0xDF))
			    return 0;
	}
    }
    return *s ? 0 : 1;
}

static int stat_2_mode(struct stat *st)
{
    mode_t m;
    int blks;

    if (S_ISDIR(st->st_mode))
	return TYPE_DIR;
    blks = (st->st_size + 253) / 254;
    if (blks >= 0x10000)
	return TYPE_BIG;
    if (st->st_mode == S_IFREG)		/* '==', not '&' */
	return TYPE_D;
    m = st->st_mode & STAT_MASK;
    switch (m) {
	case STAT_PRG:
	    return TYPE_P;
	case STAT_SEQ:
	    return TYPE_S;
	case STAT_USR:
	    return TYPE_U;
	case STAT_REL:
	    return TYPE_L;
	default:
	    return TYPE_NOTHING;
    }
}

static int stat_2_lock(struct stat *st)
{
    return ((st->st_mode & S_IWUSR) == 0);
}

static int mode_2_stat(int m)
{
    switch (m) {
	case TYPE_P:
	    return STAT_PRG;
	case TYPE_S:
	    return STAT_SEQ;
	case TYPE_U:
	    return STAT_USR;
	case TYPE_L:
	    return STAT_REL;
	case TYPE_D:
	    return -2;
	default:
	    return -1;
    }
}

static int is_pc64(char *name, struct stat *st)
{
    int i, typ = TYPE_NOTHING;
    char *cp;

    if (S_ISDIR(st->st_mode))
	return TYPE_NOTHING;	/* not handled by us.. */
    cp = strchr(name,'.');
    if (!cp)
	return TYPE_NOTHING;
    switch((cp[1] & 0xDF)) {
	case 'P':
	    typ = TYPE_P;
	    break;
	case 'S':
	    typ = TYPE_S;
	    break;
	case 'U':
	    typ = TYPE_U;
	    break;
	case 'D':
	    typ = TYPE_D;
	    break;
	case 'L':
	    typ = TYPE_L;
	    break;
	default:
	    return TYPE_NOTHING;
    }
    if (cp[4] || cp[2] < '0' || cp[2] > '9' || cp[3] < '0' || cp[3] > '9')
    	return TYPE_NOTHING;
    i = open(name, O_RDONLY|BINARY_FILE);
    if (i == -1)
	return TYPE_NOTHING;
    if (read(i, &pc64header, sizeof(pc64header)) != sizeof(pc64header)) {
	close(i);
	return TYPE_NOTHING;
    }
    close(i);
    if (strcmp(pc64header.detect, PC64ID))
    	return TYPE_NOTHING;
    return typ;
}

#ifdef DIRFILE
static char * nextfreeosname(void)
{
    static char osfnbuf[24];
    int i;

    for (i=1;;i++)
    {
	sprintf(osfnbuf, "%08d.c64",i);
	if (access(osfnbuf, 0) == -1)
	    break;
    }
    return osfnbuf;
}

static int MakeDirEntry(char *n, int mode)
{
    if (nc4wr == 15)
	return -1;
    switch(mode) {
	case TYPE_P:
	case TYPE_S:
	case TYPE_U:
	case TYPE_D:
	/* FIXME: TYPE_L */
	    break;
	default:
	    return -1;
    }
    c4wrhook[nc4wr].mode = mode;
    strcpy(c4wrhook[nc4wr].name, n);
    strcpy(c4wrhook[nc4wr].u.osname, nextfreeosname());
    nc4wr++;
    return nc4wr - 1;
}

static void DeleteDirEntry(int i)
{
    if (i >= 0 && nc4wr > i) {
	if (i + 1 == nc4wr)
	    nc4wr--;
	else
	    c4wrhook[i] = c4wrhook[--nc4wr];
    }
}

static char * getdirfileentry(FILE *fp)
{
    int l, i;
    char *cp, *strtok();

    for (;;) {
	if (fgets(dirfilebuf,256,fp)) {
	    l = strlen(dirfilebuf);
	    while (l && (dirfilebuf[l-1] == '\r' || dirfilebuf[l-1] == '\n' || dirfilebuf[l-1] == ' ' || dirfilebuf[l-1] == '\t'))
		dirfilebuf[--l] = 0;
	    if (dirfilebuf[l-1] != '"')
		continue;
	    cp = dirfilebuf;
	    while (*cp && ( *cp == ' ' || *cp == '\t' ))
		cp++;
	    if (*cp == 0 || *cp == '#' )
		continue;
	    cp = strtok(cp," \t");
	    if (cp) {
		strcpy(dirfilebuf,cp);	/* may copy on itself (usual, worst case) */
		i = strlen(dirfilebuf);
		cp = strtok(NULL," \t");
		if (cp) {
		    dirfilebuf[i++] = ' ';
		    strcpy(dirfilebuf+i,cp);
		    cp = cp + strlen(cp)+1;
		    while (dirfilebuf+l-1 > cp && *cp != '"')
			cp++;
		    if (dirfilebuf+l-1 == cp)
			continue;
		    cp++;
		    if (dirfilebuf+l-1 == cp)
			continue;
		    cp[strlen(cp)-1] = 0;
		    return cp;
		} else
		    continue;
	    } else
		continue;
	} else
	    return NULL;
    }
}

static void rewrite_dir_file()			/* rewrite the directory after write operation */
{
    FILE *fp;
    int i;
    char *cp;

    if (emu_mode == EMU_MODE_DIRFILE) {
	fp = fopen(DIRFILENAME, "w");
	if (!fp)
	    return;
	for (i = 0; i < nc4d; i++) {
	    if (c4dhook[i].mode == TYPE_P)
		cp = "prg";
	    else if (c4dhook[i].mode == TYPE_S)
		cp = "seq";
	    else if (c4dhook[i].mode == TYPE_U)
		cp = "usr";
	    else if (c4dhook[i].mode == TYPE_D)
		cp = "del";
	    /* FIXME: TYPE_L */
	    else
		continue;
	    fprintf(fp, "%s %s \"%s\"\n", c4dhook[i].u.osname, cp, c4dhook[i].name);
	}
	if (nc4wr) {
	    for (i = 0; i < nc4wr; i++) {
		if (c4wrhook[i].mode == TYPE_P)
		    cp = "prg";
		else if (c4wrhook[i].mode == TYPE_S)
		    cp = "seq";
		else if (c4wrhook[i].mode == TYPE_U)
		    cp = "usr";
		else if (c4wrhook[i].mode == TYPE_D)
		    cp = "del";
		/* FIXME: TYPE_L */
		else
		    continue;
		fprintf(fp, "%s %s \"%s\"\n", c4wrhook[i].u.osname, cp, c4wrhook[i].name);
	    }
	    nc4wr = 0;
	}
	fclose(fp);
    }
}

static int read_dir_file(void)
{
    char *cp;
    FILE *fp;
    int cnt, i;
    struct stat st;

    fp = fopen(DIRFILENAME, "r");
    if (fp) {
	emu_mode = EMU_MODE_DIRFILE;
	cnt = 0;
	while (getdirfileentry(fp)) {
	    cp = strchr(dirfilebuf, ' ');
	    if (!cp)
		continue;
	    *cp++ = 0;
	    if (stat(dirfilebuf, &st))       /* something illegal.. */
		continue;
	    if (!strcmp(cp, "prg"))
		;
	    else if (!strcmp(cp, "seq"))
		;
	    else if (!strcmp(cp, "usr"))
		;
	    else if (!strcmp(cp, "del"))
		;
	    /* FIXME: TYPE_L */
	    else 
		continue;
	    cnt++;
	}
	c4dhook = (struct c4entry *)malloc((cnt + 1) * sizeof(struct c4entry));
	if (!c4dhook) {
	    Cleanup();
	    fprintf(stderr, "NO MEM!\n");
	    abort();
	}
	rewind(fp);
	i = 0;
	while ((cp = getdirfileentry(fp))) {
	    strncpy(c4dhook[i].name, cp, 16);
	    c4dhook[i].name[16] = 0;    /* trust noone ... */
	    cp = strchr(dirfilebuf, ' ');
	    *cp++ = 0;
	    if (stat(dirfilebuf, &st))       /* something illegal.. */
		continue;
	    strncpy(c4dhook[i].u.osname, dirfilebuf, 16);
	    c4dhook[i].u.osname[16] = 0;    /* trust noone ... */
	    c4dhook[i].size = st.st_size;
	    if (!strcmp(cp, "prg"))
		c4dhook[i].mode = TYPE_P;			/* 0 = big, 1 = dir */
	    else if (!strcmp(cp, "seq"))
		c4dhook[i].mode = TYPE_S;
	    else if (!strcmp(cp, "usr"))
		c4dhook[i].mode = TYPE_U;
	    else if (!strcmp(cp, "del"))
		c4dhook[i].mode = TYPE_D;
	    /* FIXME: TYPE_L */
	    else {
		Cleanup();
		abort();
	    }
	    c4dhook[i].size = st.st_size;
	    i++;
	}
	c4dhook[i].name[0] = 0;
	c4dhook[i].mode = 0;
	fclose(fp);
	nc4d = cnt;
	return 1;
    }
    return 0;
}
#endif


static int read_fs_dir(void)
{
    DIR *dirp;
    struct dirent *dp;
    int tmp;
    int cnt;
    struct stat st;
    int i;

    emu_mode = EMU_MODE_FILESYS;
    dirp = opendir(".");
    if (!dirp)
	return 0;

    cnt = 0;
    while ((dp = readdir(dirp)) != NULL) {
	if (!strcmp(dp->d_name, "."))
	    continue;
	if (!strcmp(dp->d_name, ".."))
	    continue;
	if (stat(dp->d_name, &st))       /* something illegal.. */
	    continue;
	if (!(S_ISREG(st.st_mode)) && !(S_ISDIR(st.st_mode))) 
	    continue;
	if (stat_2_mode(&st) == TYPE_NOTHING)
	    continue;
	cnt++;
    }
    c4dhook = (struct c4entry *)malloc((cnt + 1) * sizeof(struct c4entry));
    if (!c4dhook) {
	Cleanup();
	fprintf(stderr, "NO MEM!\n");
	abort();
    }
    rewinddir(dirp);
    i = 0;
    while (i < cnt && (dp = readdir(dirp)) != NULL) {
	if (!strcmp(dp->d_name, "."))
	    continue;
	if (!strcmp(dp->d_name, ".."))
	    continue;
	if (stat(dp->d_name, &st))       /* something illegal.. */
	    continue;
	if (!(S_ISREG(st.st_mode)) && !(S_ISDIR(st.st_mode))) 
	    continue;
	if ((tmp = is_pc64(dp->d_name, &st)) != TYPE_NOTHING) {
	    c4dhook[i].mode = tmp;
	    c4dhook[i].flags = FLG_PC64;
	    c4dhook[i].size = st.st_size - sizeof(struct pc64entry);
	    strcbm2asc(pc64header.name);
	    strcpy(c4dhook[i].name, pc64header.name);
	    strncpy(c4dhook[i].u.osname ,dp->d_name, 16);
	    c4dhook[i].u.osname[16] = 0;
	    if (tmp == TYPE_L) {
		c4dhook[i].recordsize = pc64header.recordsize;
		c4dhook[i].flags |= FLG_REL;
	    }
	} else {
	    if ((tmp = stat_2_mode(&st)) == TYPE_NOTHING)
		continue;
	    strncpy(c4dhook[i].name, dp->d_name, 16);
	    c4dhook[i].name[16] = 0;    /* trust noone ... */
	    strcpy(c4dhook[i].u.osname, c4dhook[i].name);	/* now always set the osname for PC64 support */
	    c4dhook[i].mode = tmp;
	    c4dhook[i].flags = 0;
	    c4dhook[i].size = st.st_size;
	    if ( c4dhook[i].mode == TYPE_L) {
		unsigned char rlbuf[4];
		tmp = open(dp->d_name, O_RDONLY|BINARY_FILE);
		if (tmp == -1)
		    continue;
		if (read(tmp, rlbuf, 4) != 4) {
		    close(tmp);
		    continue;
		}
		close(tmp);
		if (strncmp((char *)rlbuf, "r64", 3))
		    continue;
		if (rlbuf[3] == 0 || rlbuf[3] > 253)
		    continue;
		c4dhook[i].recordsize = rlbuf[3];
		c4dhook[i].flags |= FLG_REL;
	    }
	}
	if (stat_2_lock(&st)) c4dhook[i].flags |= FLG_LOCK;
	i++;
    }
    c4dhook[i].name[0] = 0;
    c4dhook[i].mode = 0;
    closedir(dirp);
    nc4d = cnt;
    return 1;
}

static void read_d64_dir(void)
{
    struct FileEntry *dir;
    int track, sector;
    int i, j, abssec, cnt;
    d64_BAM = (struct BAM *)(ImageData + AbsoluteSector(18, 0) * 256);

    cnt = 0;
    for (track = d64_BAM->dirstart.track, sector = d64_BAM->dirstart.sector;
		    (abssec = AbsoluteSector(track, sector)) != -1;
				 track = dir[0].dirnext.track, sector = dir[0].dirnext.sector) {
	if ((ImageFlags[abssec] && IF_USED))
	    break;
	ImageFlags[abssec] |= IF_USED;
	dir = (struct FileEntry *)(ImageData + abssec * 256);
	for (i = 0; i < 8; i++) {
	    if (dir[i].type != 0) {
		switch (dir[i].type & (unsigned char)0x8F) {
		    case 0x80:
		    case 0x81:
		    case 0x82:
		    case 0x83:
		    case 0x84:
			cnt++;
		    default:
			break;
		}
	    }
	}
    }
    c4dhook = (struct c4entry *)malloc((cnt + 1) * sizeof(struct c4entry));
    if (!c4dhook) {
	Cleanup();
	fprintf(stderr, "NO MEM!\n");
	abort();
    }
    for (i = 0; i < 802; i++)
	ImageFlags[i] &= ~IF_USED;
    cnt = 0;
    for (track = d64_BAM->dirstart.track, sector = d64_BAM->dirstart.sector;
		    (abssec = AbsoluteSector(track, sector)) != -1;
				 track = dir[0].dirnext.track, sector = dir[0].dirnext.sector) {
	if ((ImageFlags[abssec] && IF_USED))
	    break;
	ImageFlags[abssec] |= IF_USED;
	dir = (struct FileEntry *)(ImageData + abssec * 256);
	for (i = 0; i < 8; i++) {
	    if (dir[i].type != 0) {
		c4dhook[cnt].u.d64.starttrack = dir[i].datastart.track;
		c4dhook[cnt].u.d64.startsector = dir[i].datastart.sector;
		c4dhook[cnt].size = (dir[i].size[0] + dir[i].size[1] * 256) * 254;
		for (j = 0; j < 16 && dir[i].name[j] != (unsigned char)0xA0; j++)
		    c4dhook[cnt].name[j] = cbmtoasc(dir[i].name[j]);
		c4dhook[cnt].name[j] = 0;
		c4dhook[cnt].u.osname[0] = 0;
		c4dhook[cnt].flags = (dir[i].type & (unsigned char)0x40) ? FLG_LOCK : 0; 
		c4dhook[cnt].recordsize = 0;
		/* FIXME: FLG_UNCLOSED! */
		switch (dir[i].type & (unsigned char)0x8F) {
		    case 0x80:
			c4dhook[cnt++].mode = TYPE_D;
			break;
		    case 0x81:
			c4dhook[cnt++].mode = TYPE_S;
			break;
		    case 0x82:
			c4dhook[cnt++].mode = TYPE_P;
			break;
		    case 0x83:
			c4dhook[cnt++].mode = TYPE_U;
			break;
		    case 0x84:
			c4dhook[cnt].recordsize = dir[i].recordsize;
			c4dhook[cnt].u.d64.sidetrack = dir[i].sidestart.track;
			c4dhook[cnt].u.d64.sidesector = dir[i].sidestart.sector;
			c4dhook[cnt++].mode = TYPE_L;
			c4dhook[cnt].flags |= FLG_REL; 
			break;
		    default:
			break;
		}
	    }
	}
    }
    for (i = 0; i < 802; i++)
	ImageFlags[i] &= ~IF_USED;
    c4dhook[cnt].name[0] = 0;
    c4dhook[cnt].mode = 0;
    nc4d = cnt;
}

static int read_the_dir(void)
{
    if (c4dhook) {
	free(c4dhook);
	c4dhook = NULL;
    }
    nc4d = 0;
    dirgetidx = -1;

    if (emu_mode == EMU_MODE_D64) {
	read_d64_dir();
	return 1;
    }
#ifdef DIRFILE
    if (read_dir_file())
	return 1;
#endif
    emu_mode = EMU_MODE_FILESYS;
    return read_fs_dir();
}

static int ndirmatch(void)
{
    int i, fnd;

    fnd = 0;
    for (i = 0; i < nc4d; i++) {
	if (simple_match(c4dhook[i].name, dirmatch)) {
	    if (typemode && c4dhook[i].mode != typemode)
		continue;
	    fnd++;
	}
    }
    return fnd;
}

char *findfile(int ch, char *n)
{
    int i;

    if (n && *n) {
	for (i = 0; i < nc4d; i++) {
	    if (simple_match(c4dhook[i].name, n)) {
		if (c4dhook[i].mode != TYPE_D) {
		    findfileidx = i;
		    return strcpy(unixname[ch], c4dhook[i].u.osname);
		}
	    }
	}
    }
    findfileidx = -1;
    return NULL;
}

int dirgetfirst(char *n)
{
    strncpy(dirgetbuf, n, 32);
    dirgetbuf[31] = 0;
    for (dirgetidx = 0; dirgetidx < nc4d; dirgetidx++) {
	if (simple_match(c4dhook[dirgetidx].name, dirgetbuf))
	    break;
    }
    if (dirgetidx == nc4d)
    	dirgetidx = -1;
    return dirgetidx;
}

int dirgetnext(void)
{
    while (dirgetidx >= 0 && ++dirgetidx < nc4d) {
	if (simple_match(c4dhook[dirgetidx].name, dirgetbuf))
	    break;
    }
    if (dirgetidx == nc4d)
    	dirgetidx = -1;
    return dirgetidx;
}

static void setshort(unsigned char *p, int i)
{
    *p++ = i & 0xff;
    *p = i >> 8;
}

static void setheadline(unsigned char *bp)
{
    unsigned char *cp;
    int i;

    memcpy(bp, dirintro, 8);
    if (emu_mode == EMU_MODE_D64) {
	strcpy(bp + 8,"                \" ");
	cp = d64_BAM->diskname;
	i = 8;
	while (*cp != (unsigned char)0xA0 && i < 24)
	    bp[i++] = *cp++;
	memcpy(bp + 26, d64_BAM->diskid, 5);
	bp[31] = 0;
	for (i = 26; i < 31; i++)
	    if (bp[i] == (unsigned char)0xA0)
		bp[i] = ' ';
    } else {
	strcpy(bp + 8,"           (ALE)");
	cp = systemname();
	i = 8;
	while (*cp)
	    bp[i++] = asctocbm(*cp++);
	strcpy(bp + 24,"\" 64 2A");
    }
}

static void setstopline(char *bp)
{
    int blfree, t;

    setshort(bp, 257);
    if (emu_mode == EMU_MODE_D64) {
	for (blfree = t = 0; t < 35; t++)
	    if (t != d64_BAM->dirstart.track - 1)
		blfree += d64_BAM->bam[t].freecnt;
	setshort(bp + 2, blfree);
	strcpy(bp + 4, "BLOCKS FREE.             ");
    } else {
	blfree = freeblks(".");
	if (blfree >= 64000) {      /* kilo blocks... */
	    blfree /= 1024;
	    if (blfree >= 64000) {
		setshort(bp + 2, blfree / 1024);
		strcpy(bp + 4, "MEGA BLOCKS FREE.        ");
	    } else {
		setshort(bp + 2, blfree);
		strcpy(bp + 4, "KILO BLOCKS FREE.        ");
	    }
	} else {
	    setshort(bp + 2, blfree);
	    strcpy(bp + 4, "BLOCKS FREE.             ");
	}
    }
    setshort(bp + 30, 0);
}
 
static void setentry(char *cp) 
{
    int blks, i;
    char *np;
    int nspaces;
    int mode;
    struct c4entry *c4;

    i = dirisopen - 1;
    do {
	while (!simple_match(c4dhook[i].name, dirmatch))
	    i++;
    } while (typemode && c4dhook[i].mode != typemode);
    c4 = c4dhook + i;
    dirisopen = i + 2;
    mode = c4->mode;
    np = c4->name;
    setshort(cp, 257);   /* 0101 */
    if (mode == TYPE_BIG) {
	setshort(cp + 2, 0);
	nspaces = 3;
    } else {
	blks = (c4->size + 253) / 254;
	setshort(cp + 2, blks);
	if (blks < 10) nspaces = 3;
	else if (blks < 100) nspaces = 2;
	else if (blks < 1000) nspaces = 1;
	else nspaces = 0;
    }
    strcpy(cp + 4, "   \"" + 3 - nspaces);
    i = 5 + nspaces;
    while (*np)
	cp[i++] = asctocbm(*np++);
    cp[i++] = '"';
    while (i < (22 + nspaces)) {
	cp[i++] = ' ';
    }
    cp[i++] = ':';
    switch(mode) {
	case TYPE_BIG:
	    strcpy(cp + i, "BIG");
	    break;
	case TYPE_DIR:
	    strcpy(cp + i, "DIR");
	    break;
	case TYPE_P:
	    strcpy(cp + i, "PRG");
	    break;
	case TYPE_S:
	    strcpy(cp + i, "SEQ");
	    break;
	case TYPE_U:
	    strcpy(cp + i, "USR");
	    break;
	case TYPE_D:
	    strcpy(cp + i, "DEL");
	    break;
	case TYPE_L:
	    strcpy(cp + i, "REL");
	    break;
	default:
	    Cleanup();
	    abort();
    }
    i += 3;
    if ((c4->flags & FLG_LOCK))
	cp[i++] = '<';
    else
	cp[i++] = ' ';

    while (i < 31)
	cp[i++] = ' ';
    cp[i] = 0;
}


int open1541dir(char *n, int ch)
{
    if (dirisopen)
	return 1;
    if (n == NULL || *n == 0)
	strcpy(dirmatch, "*");
    else
	strncpy(dirmatch, n, 32);
    dirnmatch = ndirmatch();

    dirlen = dirnmatch * 32 + 64;
    dirisopen = 1;
    dirpos = 0;
    chfd[ch] = -4;      /* a directory */
    return 0;
}

void close1541dir(int ch)
{
    dirisopen = 0;
    chfd[ch] = -1;
}

int read1541dirblock(int ch)
{
    int nread;          /* max 8 per block */

    nread = 0;
    if (dirpos == 0) {
	setheadline(chanbuf[ch]);
	dirpos += 32;
	nread++;
    }
    while ((nread < 8) && (dirpos < (dirlen - 32))) {
	setentry(chanbuf[ch] + (nread * 32));
	dirpos += 32;
	nread++;
    }
    if ((nread < 8) && dirpos == (dirlen - 32)) {
	setstopline(chanbuf[ch] + (nread * 32));
	dirpos += 32;
	nread++;
    }
    return nread * 32;
}

void SetError(int e, int t, int s)
{
    LedOff();
    if (e) {
	if ((e != 73 && e != 1 && e != 65) || (e == 65 && t == 0)) {
	    globflags |= F_ERRORMSG;
	    LedFlash();
	}
    }
    errorcode = e;
    errortrack = t;
    errorsector = s;
    sprintf(errorbuf, "%02d,%s,%02d,%02d\r",
	e, errors[e], errortrack, errorsector);
#if defined(X11) || defined(SVGALIB)
    if (e && e != 73 && e != 1 && e != 65) {	/* NO ERROR, RESET, SCRATCH */
	printf("1541 Error: %s\n", errorbuf);
    } else if (e == 1 && t == 0) {
	printf("1541 Warning: %s\n", errorbuf);
    } else if (e == 65 && t == 0) {
	printf("1541 Error: %s\n", errorbuf);
    }
#endif
}

static int MountD64(void)
{
    char *cp;
    struct stat st;
    int i;

    if (ImageFD >= 0)
	close(ImageFD);
    if (ImageData) {
	free(ImageData);
	ImageData = NULL;
	ImageDataSize = 0;
    }
    ImageFD = -1;
    ImageRO = 0;
    d64_has_errinfo = 0;
    d64_tracks = 0;
    if (!FloppyImage || access(FloppyImage, R_OK) < 0 || stat(FloppyImage, &st))
	return 0;
    cp = strrchr(FloppyImage, '.');
    if (!cp || (strcmp(cp, ".d64") && strcmp(cp, ".D64")))
	return 0;
    if (!S_ISREG(st.st_mode))
	return 0;
    ImageDataSize = st.st_size;
    switch (ImageDataSize) {
	case D64_35_EINFO_SIZE:
	    d64_has_errinfo = 1;
	case D64_35_SIZE:
	    d64_tracks = 35;
	    break;
	case D64_40_EINFO_SIZE:
	    d64_has_errinfo = 1;
	case D64_40_SIZE:
	    d64_tracks = 40;
	    break;
	case D64_42_EINFO_SIZE:
	    d64_has_errinfo = 1;
	case D64_42_SIZE:
	    d64_tracks = 42;
	    break;
	default:
	    ImageDataSize = 0;
	    break;
    }
    if (!ImageDataSize)
	return 0;
    ImageData = malloc(ImageDataSize);
    if (!ImageData) {
	ImageDataSize = 0;
	return 0;
    }
    if (access(FloppyImage, W_OK) < 0)	/* don't optimize here! Under Unix eaccess()/open()
			       with saved ids may grant us more then we want the user to give! */
	ImageRO = 1;
    ImageFD = open(FloppyImage, (ImageRO ? O_RDONLY : O_RDWR)|BINARY_FILE);
    if (ImageFD < 0 || read(ImageFD, ImageData, ImageDataSize) != ImageDataSize) {
	free(ImageData);
	ImageData = NULL;
	ImageDataSize = 0;
	if (ImageFD >= 0)
	    close(ImageFD);
	ImageFD = -1;
	ImageRO = 0;
	return 0;
    }
    if (ImageRO) {
	close(ImageFD);
	ImageFD = -1;
    }
    if (d64_has_errinfo) {
	i = ImageDataSize / 257;
	ImageErrInfo = ImageData + i * 256;
    }
    d64_BAM = (struct BAM *)(ImageData + AbsoluteSector(18, 0) * 256);
    memset(ImageFlags, 0, 802);
    return 1;
}

int ParseFilename(char *name, int len, int getmode)
{
    char *cp2;
    char *cp = name;
    unsigned char c;
    int drive;

    ftype = TYPE_NOTHING;
    openmode = MODE_NOTHING;
    typemode = TYPE_NOTHING;
    chanopen = 0;
    nbuf[0] = 0;
    name[len] = 0;

   // printf("ParseFileName: '%s'", name);

    /* check filename for filetype, openmode,
       overwrite flag, or directory request. */
    if (*cp == '@') {
	overwrite = 1;
	dir = 0;
	cp++;
    } else {
	overwrite = 0;
	if (*cp == '$') {
	    dir = 1;
	    cp++;
	    cp2 = cp;
	    while (*cp >= '0' && *cp <= '9')
		cp++;
	    if (*cp == ':' && cp != cp2)        /* parse the drive */
		cp = cp2;
	} else {
	    dir = 0;
	    if (*cp == '#') {
		/* FIXME: we ignore all buffer number args here! */
		strcpy(nbuf,"#");
		chanopen = 1;
		return 0;
	    }
	}
    }
    cp2 = strchr(cp, ':');
    if (cp2) {
	*cp2++ = 0;
	drive = atoi(cp);
	if (drive < 0 || drive > 9) {
	    SetError(74, 0, 0);
	    return 1;
	}
	cp = cp2;
    } else if (overwrite) {
	overwrite = 0;		/* mimic 1541 behaviour: '@' is global overwrite only before ':'
				   or when explicitly checked by write commands, but can normally
				   used to load files */
	cp--;
    }
    if (getmode) {
	if ((cp2 = strchr(cp, '='))) {
	    *cp2++ = 0;
	    typemode = *cp2 & 0x7f;
	    switch (typemode) {
		case TYPE_P:
		case TYPE_S:
		case TYPE_U:
		case TYPE_D:
		case TYPE_L:
		    break;
		default:    /* including 0 */
		    SetError(33, 0, 0);
		    return 1;
	    }
	    if (cp2[1]) {
		SetError(33, 0, 0);
		return 1;
	    }
	}
	cp2 = strchr(cp, ',');
	if (cp2) {
	    *cp2++ = 0;
	    /* FIXME: insert drivedir... - ??? - */
	    if (dir || strlen(cp) > 30) {
		SetError(33, 0, 0);
		return 1;
	    }
	    ftype = cp2[0] & 0x7f;
	    switch (ftype) {
		case TYPE_P:
		case TYPE_S:
		case TYPE_U:
		case TYPE_D:
		case TYPE_L:
		    break;
		default:
		    ftype = TYPE_NOTHING;
		    break;
	    }
	    if (ftype != TYPE_NOTHING)
	    {
		cp2 = strchr(cp2, ',');
		if (!cp2) {
		    if (ftype == TYPE_L) {
			SetError(33, 0, 0);
			return 1;
		    }
		} else {
		    if (cp2[1])
			 openmode = ((unsigned char *)cp2)[1];

		    if (ftype != TYPE_L && openmode)
		    {
			openmode &= 0x7f;
			switch(openmode) {
			    case MODE_R:
			    case MODE_W:
			    case MODE_A:
			    case MODE_M:
				break;
			    default:
				openmode = MODE_NOTHING;
				break;
			}
		    } else {
			if (openmode == 0 || openmode > 253) {
			    SetError(33, 0, 0);
			    return 1;
			}
		    }
		}
	    }
	}
    }
    /* Now translate the filename ... */
    cp2 = nbuf;
    while ( (c = (unsigned char)(*cp++)) ) {
	if (c >= 'A' && c <= 'Z')
	    c |= 0x20;
	else if (c >= ('A'|0x80) && c <= ('Z'|0x80))
	    c &= 0x7f;
	*cp2++ = c;
	if (c == '*')   /* ignore anything after '*' */
	    break;
    }
    *cp2 = 0;
    
    // printf(" -> '%s' '%x' '%x' (%d)\n", nbuf, dir ? typemode : ftype, openmode, dir);

    return 0;
}

static int get_num_arg(char **cpp, int colon)
{
    char *cp = *cpp;
    int n = 0;

    if (!colon && *cp == ':')
	return -1;
    if (colon && *cp == ',')
	return -1;
    if (*cp == ':' || *cp == ',') {
	cp++;
	if (!*cp || (*cp != ' ' && *cp != 0x1d && (*cp < '0' || *cp > '9')))
	    return -1;
    }
    while (*cp && (*cp == ' ' || *cp == 0x1d))
	cp++;
    if (!*cp || *cp < '0' || *cp > '9')
	return -1;
    n = *cp++ - '0';
    while (*cp >= '0' && *cp <= '9') {
	n *= 10;
	n += *cp++ - '0';
    }
    *cpp = cp;
    return n;
} 

// D64 MOUNT/UMOUNT Support:
// g:dir	-> chdir() to dir
// g:file.d64	-> mount d64 file
// g:. 		-> unmount d64 file and use current dir again
int cmd_go(char *cmd)
{
    char *cp;
    struct stat st;
    static char FIbuf[128];

    cp = strchr(cmd, ':');
    if (!cp) {
	SetError(34, 0, 0);
	return 0;
    }
    cp++;
    if (!*cp) {
	SetError(30, 0, 0);
	return 0;
    }
    strcbm2asc(cp);
    if (stat(cp, &st)) {
	SetError(62, 0, 0);
	return 0;
    }
    if (S_ISDIR(st.st_mode)) {
	if (chdir(cp)) {
	    SetError(62, 0, 0);
	    return 0;
	}
	FloppyImage = NULL;
	emu_mode = EMU_MODE_FILESYS;
	read_the_dir();
	SetError(0, 0, 0);
	return 1;
    }
    FloppyImage = cp;
    if (MountD64()) {
	strncpy(FIbuf, cp, 127);
	FIbuf[127] = 0;
	FloppyImage = FIbuf;
	emu_mode = EMU_MODE_D64;
	read_the_dir();
	SetError(0, 0, 0);
	return 1;
    }
    emu_mode = EMU_MODE_FILESYS;
    FloppyImage = NULL;
    SetError(62, 0, 0);
    return 0;
}

static int cmd_scratch(char *cmd)
{
    char *cp, *cp2, *cp3;
    int i, idx, tpmod;

    cp = strchr(cmd, ':');
    if (!cp) {
	SetError(34, 0, 0);
	return 0;
    }
    cp++;
    if (!*cp) {
	SetError(30, 0, 0);
	return 0;
    }
    if (emu_mode == EMU_MODE_D64) {
	SetError(3, 0, 0);	/* unimplemented */
	return 0;
    }
    strcbm2asc(cp);
    for (i = 0; ;) {
	cp3 = strchr(cp, ',');
	if (cp3)
	    *cp3++ = 0;
	tpmod = TYPE_NOTHING;
	if ((cp2 = strchr(cp, '='))) {
	    *cp2++ = 0;
	    tpmod = *cp2 & 0x5f;
	    switch(tpmod) {
		    case TYPE_P:
		    case TYPE_S:
		    case TYPE_U:
		    case TYPE_D:
		    case TYPE_L:
			break;
		    default:
			SetError(34, 0, 0);
			return 0;
	    }
	}
	if (strlen(cp) > 16) 		/* no error here ... */
	    cp[16] = 0;
	idx = dirgetfirst(cp);
	while (idx >= 0) {
	    if (!tpmod || c4dhook[idx].mode == tpmod)
	    {
		if (!(c4dhook[idx].flags & FLG_LOCK))
		{
		    c4dhook[idx].mode = -1;
		    unlink(c4dhook[idx].u.osname);
		    i++;
		}
	    }
	    idx = dirgetnext();
	}
	if (cp3 == NULL || *cp3 == 0)
	    break;
	else
	    cp = cp3;
    }
#ifdef DIRFILE
    if (emu_mode == EMU_MODE_DIRFILE && i)
	rewrite_dir_file();
#endif
    SetError(1, i, 0);
    return 1;
}

static int cmd_rename(char *cmd)
{
    char *cp, *cp2, *cp3;
    int i, idx;

    cp = strchr(cmd, ':');
    if (!cp) {
	SetError(34, 0, 0);
	return 0;
    }
    cp++;
    if (!*cp) {
	SetError(30, 0, 0);
	return 0;
    }
    cp2 = strchr(cp, '=');
    if (!*cp2) {
	SetError(30, 0, 0);
	return 0;
    }
    *cp2++ = 0;
    cp3 = strchr(cp2, ':');
    if (cp3)
	cp2 = cp3 + 1;
    if (!*cp2) {			/* strange, but as the original */
	SetError(62, 0, 0);
	return 0;
    }
    strcbm2asc(cp);
    if (strlen(cp) > 16)
	cp[16] = 0;
    strcbm2asc(cp2);
    if (strlen(cp2) > 16)
	cp2[16] = 0;
    if (strchr(cp, '*') || strchr(cp, '?') || strchr(cp, ',')) {
	SetError(30, 0, 0);
	return 0;
    }
    if (strchr(cp2, '*') || strchr(cp2, '?') || strchr(cp2, ',')) {
	SetError(30, 0, 0);
	return 0;
    }
    idx = dirgetfirst(cp);
    if (idx >= 0) {
	SetError(63, 0, 0);
	return 0;
    }
    i = dirgetfirst(cp2);
    if (i < 0) {
	SetError(62, 0, 0);
	return 0;
    }
#ifdef DIRFILE
    if (emu_mode == EMU_MODE_DIRFILE) {
	strcpy(c4dhook[i].name, c4dhook[idx].name);
	rewrite_dir_file();
    } else
#endif
    {
	if (emu_mode == EMU_MODE_D64 || ((c4dhook[i].flags & FLG_PC64))) {
	    SetError(3, 0, 0);	/* unimplemented */
	    return 0;
	}
	rename(c4dhook[i].u.osname, cp);
    }
    SetError(0, 0, 0);
    return 1;
}

static int filecopy(char *osname, int srcidx, int append, int overwrite)
{
    char cpbuf[1024];
    int fdi, fdo, size;
    struct stat st;

    if ((c4dhook[srcidx].flags & FLG_PC64))
	return 3;					/* FIXME: unimplemented */

    if (emu_mode == EMU_MODE_D64)
	return 3;					/* FIXME: unimplemented */

    if (append && overwrite)
	return 2;
    if (stat(c4dhook[srcidx].u.osname, &st))
	return 1;
#ifndef __GO32__
    if (
#ifdef DIRFILE
	emu_mode == EMU_MODE_FILESYS &&
#endif
			   st.st_mode == S_IFREG) 	/* TYPE_D file open ... */
	chmod(c4dhook[srcidx].u.osname, 0600);
#endif
    fdi = open(c4dhook[srcidx].u.osname, O_RDONLY|BINARY_FILE);
#ifndef __GO32__
    if (
#ifdef DIRFILE
	emu_mode == EMU_MODE_FILESYS &&
#endif
			   st.st_mode == S_IFREG)
	chmod(c4dhook[srcidx].u.osname, 0);
#endif

    if (fdi < 0)
	return 1;
    if (fstat(fdi, &st)) {
	close(fdi);
	return 1;
    }
    fdo = open(osname, O_WRONLY|BINARY_FILE|O_CREAT|(append ? O_APPEND : 0)|(overwrite ? O_TRUNC : 0),
#ifdef __GO32__
    	0666
#else
	0666 & UMask
#endif
	);
    if (fdo < 0) {
	close(fdi);
	return 2;
    }
    while ((size = read(fdi, cpbuf, 1024)) > 0)		/* FIXME: check errors here */
	write(fdo, cpbuf, size);
    close(fdi);
    close(fdo);
#ifndef __GO32__
    chmod(osname, st.st_mode);				/* FIXME: check match with append */
#endif
    return 0;
}

static int cmd_concat(char *target, char *cmd)
{
    SetError(3, 0, 0);	/* unimplemented */
    return 0;
}

static int cmd_copy(char *cmd)
{
    char *cp, *cp2, *cp3;
    int i, idx, tpmod;

    cp = strchr(cmd, ':');
    if (!cp) {
	SetError(34, 0, 0);
	return 0;
    }
    cp++;
    if (!*cp) {
	SetError(30, 0, 0);
	return 0;
    }
    cp2 = strchr(cp, '=');
    if (!*cp2) {
	SetError(30, 0, 0);
	return 0;
    }
    *cp2++ = 0;
    if (!*cp2) {			/* strange, but as the original */
	SetError(62, 0, 0);
	return 0;
    }
    if (strlen(cp) > 16)
	cp[16] = 0;
    strcbm2asc(cp);
    if (strchr(cp, '*') || strchr(cp, '?') || strchr(cp, ',')) {
	SetError(30, 0, 0);
	return 0;
    }

    if (strchr(cp2, ','))	/* concat */
	return cmd_concat(cp, cp2);

    cp3 = strchr(cp2, ':');
    if (cp3)
	cp2 = cp3 + 1;
    if (!*cp2) {		/* strange, but as the original */
	SetError(62, 0, 0);
	return 0;
    }
    if (strlen(cp2) > 16)
	cp2[16] = 0;
    strcbm2asc(cp2);
    if (strchr(cp2, '*') || strchr(cp2, '?')) {
	SetError(30, 0, 0);
	return 0;
    }
    tpmod = i = -1;
    if (*cp == '@') {
	cp++;
	i = dirgetfirst(cp);
	if (i >= 0 && (c4dhook[i].flags & FLG_LOCK))
	{
	    SetError(26, 0, 0);	/* write protect */
	    return 0;
	}
    } else if (dirgetfirst(cp) >= 0) {
	SetError(63, 0, 0);
	return 0;
    }
    idx = dirgetfirst(cp2);
    if (idx < 0) {
	SetError(62, 0, 0);
	return 0;
    }
#ifdef DIRFILE
    if (emu_mode == EMU_MODE_DIRFILE) {
	if (i >= 0) {
	    cp = c4dhook[i].u.osname;
	} else {
	    tpmod = MakeDirEntry(cp, c4dhook[idx].mode);
	    if (tpmod == -1) {
		SetError(72, 0, 0);	/* disk full */
		return 0;
	    }
	    cp = c4wrhook[tpmod].u.osname;
	}
    }
#endif
    if ((idx = filecopy(cp, idx, 0, i >= 0))) {
#ifdef DIRFILE
	if (emu_mode == EMU_MODE_DIRFILE && tpmod != -1)
	    DeleteDirEntry(tpmod);
#endif
	switch (idx) {
	    case 1:
		SetError(62, 0, 0);
		return 0;
	    case 3:
		SetError(3, 0, 0);
		return 0;
	    case 2:
	    default:
		SetError(25, 0, 0);
		return 0;
	}
    }
#ifdef DIRFILE
    if (emu_mode == EMU_MODE_DIRFILE)
	rewrite_dir_file();
#endif
    return 1;
}

/* JOHNS: I need this global!! */
int cmd_new(char *cmd)
{
    char *cp, *cp2;
    unsigned char id[2];
    int i, s, has_id = 0;

    if (emu_mode == EMU_MODE_D64) {
	cp = strchr(cmd, ':');
	if (!cp) {
	    SetError(34, 0, 0);
	    return 0;
	}
	cp++;
	if (!*cp) {
	    SetError(30, 0, 0);
	    return 0;
	}
	cp2 = strchr(cmd, ',');
	if (cp2) {		/* low level format */
	    if (cp2 == cp || strlen(cp2) < 3) {
		SetError(34, 0, 0);
		return 0;
	    }
	    has_id = 1;
	    *cp2 = 0;
	    id[0] = cp2[1];
	    id[1] = cp2[2];
	} else {
	    cp2 = cp + strlen(cp);
	    id[0] = d64_BAM->diskid[0];
	    id[1] = d64_BAM->diskid[1];
	}
	if (ImageRO) {
	    SetError(26, 0, 0);	/* write protect */
	    return 0;
	}
	if (!has_id && d64_has_errinfo) {
	    i = AbsoluteSector(18, 0);
	    if (ImageErrInfo[i] != 1) {
		SetError(ErrInfo2Error(ImageErrInfo[i]), 0, 0);
		return 0;
	    }
	    if (ImageErrInfo[i + 1] != 1) {
		SetError(ErrInfo2Error(ImageErrInfo[i + 1]), 0, 0);
		return 0;
	    }
	}
	memset(d64_BAM, 0, 512);		/* BAM and first directory sector */
	((unsigned char *)d64_BAM)[257] = 1;	/* empty directory */

	d64_BAM->dirstart.track = 18;
	d64_BAM->dirstart.sector = 1;
	memset(d64_BAM->diskname, 0xA0, 27);	/* preset name data */
	i = 0;
	while (cp < cp2 && i < 16)
	    d64_BAM->diskname[i++] = *cp++;
	d64_BAM->diskid[0] = id[0];
	d64_BAM->diskid[1] = id[1];
	d64_BAM->dosversion[0] = '2';
	d64_BAM->dosversion[1] = d64_BAM->format = 'A';
	for (i = 1; i < 36; i++)
	    for (s = 0; s < ZBR_ns[i]; s++) {
		BAM_free(i, s);
		if (has_id && d64_has_errinfo)
		    ImageErrInfo[ZBR_st[i] + s] = 1;
	    }
	BAM_alloc(18, 0);
	BAM_alloc(18, 1);
	ImageFlags[ZBR_st[18] + 1] |= IF_DIRTY;		/* first directory block */
	ReWriteImage();
	if (has_id && d64_has_errinfo) {
	    i = ImageDataSize / 257;
	    if (lseek(ImageFD, i * 256, SEEK_SET) >= 0)
		write(ImageFD, ImageErrInfo, i);
	}
	return 1;
    } else {	/* not EMU_MODE_D64 */
	return 1;	/* just ok */
    }
}

static int cmd_block_read(char *arg, int bug)
{
    int i, chan, drv, t, s;

    while (*arg && *arg != ' ' && *arg != ':' && *arg != 0x1d)
	arg++;
    if (!*arg) {
	SetError(30, 0, 0);
	return 0;
    }
    chan = get_num_arg(&arg, 1);
    if (chan < 0 || chan >= NCHAN) {
	SetError(30, 0, 0);
	return 0;
    }
    drv = get_num_arg(&arg, 0);
    if (drv < 0) {
	SetError(30, 0, 0);
	return 0;
    }
    t = get_num_arg(&arg, 0);
    if (t < 0) {
	SetError(30, 0, 0);
	return 0;
    }
    s = get_num_arg(&arg, 0);
    if (s < 0) {
	s = 0;
	// SetError(30, 0, 0);
	// return 0;
    }
    if (chfd[chan] != -6) {
	SetError(70, 0, 0);
	return 0;
    }

#ifdef DEBUG_BLOCK_CMDS
    printf("B-R: %d %d %d %d\n", chan, drv, t, s);
#endif

    if (emu_mode == EMU_MODE_D64) {
	i = AbsoluteSector(t, s);
	if (i < 0) {
	    SetError(66, t, s);
	    return 0;
	}
	memcpy(chanbuf[chan], ImageData + i * 256, 256);
	if (d64_has_errinfo) {
	    i = ImageErrInfo[i];
	    if (i != 1) {
		SetError(ErrInfo2Error(i), 0, 0);
		chanpos[chan] = filelen[chan] = 0;
		return 0;
	    }
	}
	if (bug) {
	    filelen[chan] =  chanbuf[chan][0];
	    chanpos[chan] = 0;
	    chanbufp[chan] = 0;
	    return 1;
	}
    } else
	memset(chanbuf[chan], 0, 256);
    filelen[chan] =  256;
    chanpos[chan] = 0;
    chanbufp[chan] = 0;
    return 1;
}

static int cmd_block_write(char *arg, int bug)
{
    int i, chan, drv, t, s;

    while (*arg && *arg != ' ' && *arg != ':' && *arg != 0x1d)
	arg++;
    if (!*arg) {
	SetError(30, 0, 0);
	return 0;
    }
    chan = get_num_arg(&arg, 1);
    if (chan < 0 || chan >= NCHAN) {
	SetError(30, 0, 0);
	return 0;
    }
    drv = get_num_arg(&arg, 0);
    if (drv < 0) {
	SetError(30, 0, 0);
	return 0;
    }
    t = get_num_arg(&arg, 0);
    if (t < 0) {
	SetError(30, 0, 0);
	return 0;
    }
    s = get_num_arg(&arg, 0);
    if (s < 0) {
	s = 0;
	// SetError(30, 0, 0);
	// return 0;
    }
    if (chfd[chan] != -6) {
	SetError(70, 0, 0);
	return 0;
    }

#ifdef DEBUG_BLOCK_CMDS
    printf("B-W: %d %d %d %d\n", chan, drv, t, s);
#endif

    if (emu_mode == EMU_MODE_D64) {
	i = AbsoluteSector(t, s);
	if (i < 0) {
	    SetError(66, t, s);
	    return 0;
	}
	if (d64_has_errinfo) {
	    i = ImageErrInfo[i];
	    if (i != 1) {
		SetError(ErrInfo2Error(i), 0, 0);
		return 0;
	    }
	}
	if (ImageRO) {
	    SetError(26, 0, 0);	/* write protect */
	    return 0;
	}
	if (bug)
	    memcpy(ImageData + i * 256, chanbuf[chan], chanbuf[chan][0]);
	else
	    memcpy(ImageData + i * 256, chanbuf[chan], 256);
	ImageFlags[i] |= IF_DIRTY;
	ReWriteImage();
    }
    return 1;
}

static int cmd_block_pointer(char *arg)
{
    int chan, pos;

    while (*arg && *arg != ' ' && *arg != ':' && *arg != 0x1d)
	arg++;
    if (!*arg) {
	SetError(30, 0, 0);
	return 0;
    }
    chan = get_num_arg(&arg, 1);
    if (chan < 0 || chan >= NCHAN) {
	SetError(30, 0, 0);
	return 0;
    }
    pos = get_num_arg(&arg, 0);
    if (pos < 0 || pos > 255) {
	SetError(30, 0, 0);
	return 0;
    }
    if (chfd[chan] != -6) {
	SetError(70, 0, 0);
	return 0;
    }

#ifdef DEBUG_BLOCK_CMDS
    printf("B-P: %d %d\n", chan, pos);
#endif

    chanpos[chan] = pos;
    chanbufp[chan] = pos;
    return 1;
}

static void cmd_block_allocate(char *arg)
{
    int drv, t, t2, s, s2;

    while (*arg && *arg != ' ' && *arg != ':' && *arg != 0x1d)
	arg++;
    if (!*arg) {
	SetError(30, 0, 0);
	return;
    }
    drv = get_num_arg(&arg, 1);
    if (drv < 0) {
	SetError(30, 0, 0);
	return;
    }
    t = get_num_arg(&arg, 0);
    if (t < 0) {
	SetError(30, 0, 0);
	return;
    }
    s = get_num_arg(&arg, 0);
    if (s < 0) {
	SetError(30, 0, 0);
	return;
    }
    if (t < 1 || t > 35 || s >= ZBR_ns[t]) {
	SetError(66, t, s);
	return;
    }

#ifdef DEBUG_BLOCK_CMDS
    printf("B-A: %d %d %d\n", drv, t, s);
#endif

    if (emu_mode != EMU_MODE_D64) {
	SetError(0, 0, 0);	/* let it be ok */
	return;
    }

    if (ImageRO) {
	SetError(26, 0, 0);	/* write protect */
	return;
    }
    t2 = t;
    s2 = s;
    BAM_next_free_sector(&t2, &s2);
    if (t2 == t && s2 == s)
	SetError(0, 0, 0);
    else {
	SetError(65, t2, s2);
	if (t2 != 0)
	    BAM_free(t2, s2);	/* free it, since this was no real alloc */
    }
    ReWriteImage();	/* BAM */
}

static void cmd_block_free(char *arg)
{
    int drv, t, s;

    while (*arg && *arg != ' ' && *arg != ':' && *arg != 0x1d)
	arg++;
    if (!*arg) {
	SetError(30, 0, 0);
	return;
    }
    drv = get_num_arg(&arg, 1);
    if (drv < 0) {
	SetError(30, 0, 0);
	return;
    }
    t = get_num_arg(&arg, 0);
    if (t < 0) {
	SetError(30, 0, 0);
	return;
    }
    s = get_num_arg(&arg, 0);
    if (s < 0) {
	SetError(30, 0, 0);
	return;
    }
    if (t < 1 || t > 35 || s >= ZBR_ns[t]) {
	SetError(66, t, s);
	return;
    }

#ifdef DEBUG_BLOCK_CMDS
    printf("B-F: %d %d %d\n", drv, t, s);
#endif

    if (emu_mode != EMU_MODE_D64) {
	SetError(0, 0, 0);	/* let it be ok */
	return;
    }

    if (ImageRO) {
	SetError(26, 0, 0);	/* write protect */
	return;
    }
    if (t != 18)
	BAM_free(t, s);
    SetError(0, 0, 0);
    ReWriteImage();	/* BAM */
}

/* Execute a command channel string */
static void PerformCommand(char *cmd, int len)
{
    char *cp;

    if (len) {
	cmd[len] = 0;
	if (*cmd == 'U') {
	    if (cmd[1] == 'J' || cmd[1] == ':') {
		Init_IECDos();
		return;
	    }
	    if (cmd[1] == '9' || cmd[1] == 'I') {
		SetError(0, 0, 0);
		return;
	    }
	    if (cmd[1] == '1' || cmd[1] == 'A') {
		if (!cmd_block_read(cmd + 2, 0))
		    return;
		SetError(0, 0, 0);
		return;
	    }
	    if (cmd[1] == '2' || cmd[1] == 'B') {
		if (!cmd_block_write(cmd + 2, 0))
		    return;
		SetError(0, 0, 0);
		return;
	    }
	    if (cmd[1] == '0' && cmd[2] == '>' && cmd[3] < 31) {
		unit = cmd[3];
		SetError(0, 0, 0);
		return;
	    }
#ifdef PRINTER_SUPPORT
	    if (cmd[1] == 'P') {                                /* ALEEXTENSION: "up" Close Printer and send job */
		if (printer_open && printer_cmd == -1) 
		    ClosePrinter();
		SetError(0, 0, 0);
		return;
	    }
#endif
	    SetError(31, 0, 0);
	    return;
	}
	if (*cmd == 'I' || *cmd == 'V') {
	    SetError(0, 0, 0);
	    return;
	}
	if (*cmd == 'N') {      	/* new */
	    if (!cmd_new(cmd))
		return;
	    read_the_dir();
	    SetError(0, 0, 0);
	    return;
	}
	if (*cmd == 'G') {      	/* change directory ... */
	    cmd_go(cmd);
	    return;
	}
	if (*cmd == 'S') {      	/* scratch */
	    if (cmd_scratch(cmd))
		read_the_dir();
	    return;
	}
	if (*cmd == 'R') {      	/* rename */
	    if (!cmd_rename(cmd))
		return;
	    read_the_dir();
	    SetError(0, 0, 0);
	    return;
	}
	if (*cmd == 'C') {      	/* copy, concat */
	    if (!cmd_copy(cmd))
		return;
	    read_the_dir();
	    SetError(0, 0, 0);
	    return;
	}
	if (*cmd == 'P') {		/* goto record */
	    SetError(3, 0, 0);
	    return;
	}
	if (cmd[0] == 'B') {		/* block operation */
	    cp = strchr(cmd, '-');
	    if (!cp) {
		SetError(31, 0, 0);
		return;
	    }
	    cp++;
	    switch(*cp) {
		case 'R':
		    if (cmd_block_read(cp + 1, 1))
			SetError(0, 0, 0);
		    break;
		case 'W':
		    if (cmd_block_write(cp + 1, 1))
			SetError(0, 0, 0);
		    break;
		case 'P':
		    if (cmd_block_pointer(cp + 1))
			SetError(0, 0, 0);
		    break;
		case 'A':
		    cmd_block_allocate(cp + 1);
		    break;
		case 'F':
		    cmd_block_free(cp + 1);
		    break;
		case 'E':
		    VicMessage("FAST-LOADER detected",60*5);
		    NotImplemented(cmd);
		    SetError(3, 0, 0);
		    break;
		default:
		    SetError(31, 0, 0);
		    break;
	    }
	    return;
	}
	if (cmd[0] == 'M') {		/* memory operation */
	    VicMessage("FAST-LOADER detected",60*5);
	    NotImplemented(cmd);
	    SetError(3, 0, 0);
	    return;
	}
    }
    SetError(31, 0, 0);
}


void SetCmdChannel(char *buf, int len)
{
    memcpy(chanbuf[CMD_CHAN], buf, len);
    chanbufp[CMD_CHAN] = 0;
    chanpos[CMD_CHAN] = 0;
    filelen[CMD_CHAN] = len;
    SetError(0, 0, 0);			/* next error */
}

void Init_IECDos(void)
{
    int i;

    for (i = 0; i < NCHAN; i++) {
	flags[i] = 0;
	chfd[i] = -1;
    }
    unit = DEF_UNIT;
#ifdef PRINTER_SUPPORT
    printer_unit = DEF_PRINTER;
#endif
#ifndef __GO32__
    umask(UMask = umask(0));
    UMask ^= 0666;
#endif
    globflags = 0;
    if (FloppyImage) {
	if (MountD64())
	    emu_mode = EMU_MODE_D64;
	else {
	    FloppyImage = NULL;
	    emu_mode = EMU_MODE_FILESYS;
	}
    } else
	emu_mode = EMU_MODE_FILESYS;
    read_the_dir();
    errors[73] = emulver + 5;
    SetError(73, 0, 0);
}

void ReadABlock(int ch)
{
    int bl;
    struct DataBlock *db;
	
    if (chfd[ch] == -4) {       /* Read from directory */
	bl = read1541dirblock(ch);
	if (bl) {
	    filepos[ch] += bl;
	    chanbufp[ch] = 0;
	    filelen[ch] = filepos[ch];
	}
    } else if (emu_mode == EMU_MODE_D64) {
	db = (struct DataBlock *)(chanbuf[ch]);
	bl = AbsoluteSector(db->datanext.track, db->datanext.sector);
	if (bl == -1) {
	    SetError(66, db->datanext.track, db->datanext.sector);
	    chanpos[ch] = filelen[ch] = 0;
	    return;
	}
	memcpy(chanbuf[ch], ImageData + bl * 256, 256);
	if (d64_has_errinfo) {
	    if (ImageErrInfo[bl] != 1) {
		SetError(ErrInfo2Error(ImageErrInfo[bl]), 0, 0);
		chanpos[ch] = filelen[ch] = 0;
		return;
	    }
	}
	if (db->datanext.track == 0) {	/* last sector */
	    filelen[ch] = db->datanext.sector + 1;
	} else {
	    filelen[ch] = 257;
	}
	chanpos[ch] = 2;
	chanbufp[ch] = 2;
    } else if (filelen[ch] > filepos[ch]) {
	bl = filelen[ch] - filepos[ch];
	if (bl > 256)
	    bl = 256;
	read(chfd[ch], chanbuf[ch], bl);
	filepos[ch] += bl;
	chanbufp[ch] = 0;
    }
}

void WriteABlock(int ch)
{
    write(chfd[ch], chanbuf[ch], chanbufp[ch]);
    filepos[ch] += chanbufp[ch];
    chanbufp[ch] = 0;
}

int DoOpenFile(int ch, char *name)
{
    struct stat st;
    int i;
    int fmd;

    /* FIXME : TYPE_L support */

#if defined(JOHNS)
    printf("Open C64 `%s' %d\n", name, ch);	 /* DEBUG */
#endif
    /* Really open the file on disk */
    if (ch == CMD_CHAN) {
	fmod[ch] = 0;
	chfd[ch] = -2;
	if (filenamelen[ch])
	    PerformCommand(filenamebuf[ch], filenamelen[ch]);
	return 0;
    }

    if (ParseFilename(filenamebuf[ch], filenamelen[ch], 1))
	return 1;

    if (!dir && !nbuf[0]) {
	SetError(34, 0, 0);
	return 1;
    }

    if (chanopen) {
	flags[ch] = F_CHANOPEN;
	chfd[ch] = -6;
	filepos[ch] = 0;
	chanpos[ch] = 0;
	filelen[ch] = 256;
	chanbuf[ch][0] = ch;
	SetError(0, 0, 0);
	return 0;
    }
    if (ftype == TYPE_NOTHING) ftype = TYPE_P;
    if (openmode == MODE_NOTHING) openmode = (ch == 1 ? MODE_W : MODE_R);

    if (ch == 0 && ftype == TYPE_P && openmode == MODE_R) { /* always read */
	chfd[ch] = -1;

	if (dir) {
	    if (open1541dir(nbuf, ch)) {
		SetError(71, 0, 0);       /* another dir is open */
		return 1;
	    }
	    flags[ch] = F_READ;
	    filepos[ch] = 0;
	    chanpos[ch] = 0;
	    ReadABlock(ch);
	    SetError(0, 0, 0);
	    return 0;
	}
	if (overwrite) {
	    SetError(33, 0, 0);
	    return 1;
	}
	if (!findfile(ch, nbuf)) {
	    SetError(62, 0, 0);
	    return 1;
	}
	if (c4dhook[findfileidx].mode != ftype) {
	    SetError(64, 0, 0);
	    return 1;
	}
	flags[ch] = F_READ;
	if (emu_mode == EMU_MODE_D64) {
	    chfd[ch] = -5;
	    filepos[ch] = 0;
	    chanpos[ch] = 256;
	    chanbuf[ch][0] = c4dhook[findfileidx].u.d64.starttrack;
	    chanbuf[ch][1] = c4dhook[findfileidx].u.d64.startsector;
	    SetError(0, 0, 0);
	    ReadABlock(ch);
	    return (errorcode != 0);
	}
#ifdef __GO32__
	chfd[ch] = open(unixname[ch], O_RDONLY|BINARY_FILE);
#else
	if ((c4dhook[findfileidx].flags & FLG_PC64) == 0
#ifdef DIRFILE
		&& emu_mode == EMU_MODE_FILESYS
#endif
		&& ftype == TYPE_D)
	    chmod(unixname[ch],0600);
	chfd[ch] = open(unixname[ch], O_RDONLY);
	if ((c4dhook[findfileidx].flags & FLG_PC64) == 0
#ifdef DIRFILE
		&& emu_mode == EMU_MODE_FILESYS
#endif
		&& ftype == TYPE_D)
	    chmod(unixname[ch], 0);
#endif
	fmod[ch] = O_RDONLY;
	if (chfd[ch] != -1)
	    if (fstat(chfd[ch], &st) != -1) {
		filelen[ch] = st.st_size;
		if ((c4dhook[findfileidx].flags & FLG_PC64)) {
		    filelen[ch] -= sizeof(struct pc64entry);
		    lseek(chfd[ch], sizeof(struct pc64entry), SEEK_SET);
		} else if ((c4dhook[findfileidx].flags & FLG_REL) 
#ifdef DIRFILE
			    && emu_mode == EMU_MODE_FILESYS
#endif
						) {
		    filelen[ch] -= 4;
		    lseek(chfd[ch], 4, SEEK_SET);
		}
		filepos[ch] = 0;
		ReadABlock(ch);
		chanpos[ch] = 0;
		SetError(0, 0, 0);
		return 0;
	    }
	SetError(62, 0, 0);
	return 1;
    } else if (ch == 1 && ftype == TYPE_P && openmode == MODE_W) { /* always write */

	if (!overwrite && nbuf[0] == '@') {
	    /* overwrite w/o ':', no longer detected by ParseFileName() to mimic 1541 behaviour */
	    i = 0;
	    if (!nbuf[1]) {
		SetError(34, 0, 0);
		return 1;
	    }
	    do {
		nbuf[i] = nbuf[i + 1];
		i++;
	    } while (nbuf[i]);
	    overwrite = 1;
	}

	if (emu_mode == EMU_MODE_D64) {			/* for now! */
	    SetError(26, 0, 0);	/* write protect */
	    return 1;
	}

	if (strchr(nbuf, '*') || strchr(nbuf, '?')) 	/* has patterns */
	{
	    if (!findfile(ch, nbuf)) {
		SetError(62, 0, 0);
		return 1;
	    }
	    if (!overwrite) {
		SetError(63, 0, 0);
		return 1;
	    }
	    if (c4dhook[findfileidx].mode != ftype) {
		SetError(64, 0, 0);
		return 1;
	    }
	    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
		SetError(26, 0, 0);	/* write protect */
		return 1;
	    }
	} else {
#ifdef DIRFILE
	    if (emu_mode == EMU_MODE_DIRFILE) {
		if (!findfile(ch, nbuf)) {
		    i = MakeDirEntry(nbuf, ftype);
		    if (i == -1) {
			SetError(72, 0, 0);
			return 1;
		    }
		    strcpy(unixname[ch], c4wrhook[i].u.osname);
		    rewrite_dir_file();
		    read_the_dir();
		} else {
		    if (!overwrite) {
			SetError(63, 0, 0);
			return 1;
		    }
		    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
			SetError(26, 0, 0);	/* write protect */
			return 1;
		    }
		}
	    }
	    else
#endif
	    {
	    	if (findfile(ch, nbuf))
		{
		    if (!overwrite) {
			SetError(63, 0, 0);
			return 1;
		    }
		    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
			SetError(26, 0, 0);	/* write protect */
			return 1;
		    }
		} else
		    strcpy(unixname[ch], nbuf);
	    }
	}
#ifdef ALWAYS_SAVE_P00
#ifdef DIRFILE
	if (emu_mode == EMU_MODE_FILESYS)
#endif
	{
	    if (findfileidx != -1)
		unlink(unixname[ch]);
	    strcpy(unixname[ch], MakeP00(nbuf));
	}
#endif
	flags[ch] = F_WRITE;
	if (overwrite)
	    flags[ch] |= F_OVERWRITE;
	i = O_WRONLY|O_CREAT;
	if (overwrite)
	    i |= O_TRUNC;
#ifdef __GO32__
	fmd = 0666;
	chfd[ch] = open(unixname[ch], i|BINARY_FILE, fmd);
#else
#ifdef DIRFILE
	if (emu_mode == EMU_MODE_DIRFILE)
	    fmd = 0;
	else
#endif
#ifdef ALWAYS_SAVE_P00
	    fmd = 0;
#else
	    fmd = mode_2_stat(ftype);
	if (fmd == -1) {
	    SetError(28, 0, 0);
	    return 1;
	}
	if (fmd == -2) {
	    chfd[ch] = open(unixname[ch], i, 0666);
	    fmd = 0;
	} else
#endif
	{
	    fmd |= (0666 & UMask);
	    chfd[ch] = open(unixname[ch], i, fmd);
	}
#endif        
	fmod[ch] = fmd;
	if (chfd[ch] != -1) {
#ifndef ALWAYS_SAVE_P00
	    if (findfileidx != -1 && (c4dhook[findfileidx].flags & FLG_PC64))
#endif
	    {
	    	memset(&pc64header, 0, sizeof(pc64header));
		strcpy(pc64header.detect, PC64ID);
#ifdef ALWAYS_SAVE_P00
		if (findfileidx == -1) {
		    strcpy(pc64header.name, nbuf);
		    pc64header.recordsize = 0;
		} else
#endif
		{
		    strcpy(pc64header.name, c4dhook[findfileidx].name);
		    pc64header.recordsize = c4dhook[findfileidx].recordsize;
		}
		strasc2cbm(pc64header.name);
		if (write(chfd[ch], &pc64header, sizeof(pc64header)) != sizeof(pc64header)) {
		    close(chfd[ch]);
		    chfd[ch] = -1;
		    chanbufp[ch] = 0;
		    chanpos[ch] = 0;
		    filelen[ch] = 0;
		    flags[ch] = 0;
		    SetError(28, 0, 0);
		    return 1;
		}
	    }
	    filelen[ch] = 0;
	    filepos[ch] = 0;
	    chanbufp[ch] = 0;
	    chanpos[ch] = 0;
#ifndef __GO32__
	    chmod(unixname[ch], fmd);
#endif        
	    SetError(0, 0, 0);
	    return 0;
	}
	SetError(28, 0, 0);
	return 1;
    } else {
	if (ftype == TYPE_L) {
	    /* FIXME: TYPE_L fill here:	
	       recordsize: openmode ( or 0 if not specified ) */
	    SetError(50, 0, 0);
	    return 1;
	}
	if (dir) /* open dir with chans other then 0 returns raw BAM data! */
	{
	    SetError(3, 0, 0);
	    return 1;
	}
	/* printf("Open `%s'\n",nbuf);	* DEBUG */
	switch(openmode) {
	    case MODE_W:
	    case MODE_A:

		if (!overwrite && nbuf[0] == '@') {
		    /* overwrite w/o ':', no longer detected by ParseFileName() to mimic 1541 behaviour */
		    i = 0;
		    if (!nbuf[1]) {
			SetError(34, 0, 0);
			return 1;
		    }
		    do {
			nbuf[i] = nbuf[i + 1];
			i++;
		    } while (nbuf[i]);
		    overwrite = 1;
		}

		if (emu_mode == EMU_MODE_D64) {			/* for now! */
		    SetError(26, 0, 0);	/* write protect */
		    return 1;
		}

		if (strchr(nbuf, '*') || strchr(nbuf, '?')) 	/* has patterns */
		{
		    if (!findfile(ch, nbuf)) {
			SetError(62, 0, 0);
			return 1;
		    }
		    if (openmode != MODE_A && !overwrite) {
			SetError(63, 0, 0);
			return 1;
		    }
		    if (c4dhook[findfileidx].mode != ftype) {
			SetError(64, 0, 0);
			return 1;
		    }
		    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
			SetError(26, 0, 0);	/* write protect */
			return 1;
		    }
		} else {
#ifdef DIRFILE
		    if (emu_mode == EMU_MODE_DIRFILE) {
			if (!findfile(ch, nbuf)) {
			    i = MakeDirEntry(nbuf, ftype);
			    if (i == -1) {
				SetError(72, 0, 0);
				return 1;
			    }
			    strcpy(unixname[ch], c4wrhook[i].u.osname);
			    rewrite_dir_file();
			    read_the_dir();
			} else {
			    if (openmode != MODE_A && !overwrite) {
				SetError(63, 0, 0);
				return 1;
			    }
			    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
				SetError(26, 0, 0);	/* write protect */
				return 1;
			    }
			}
		    }
		    else
#endif
		    {
			if (findfile(ch, nbuf))
			{
			    if (openmode != MODE_A && !overwrite) {
				SetError(63, 0, 0);
				return 1;
			    }
			    if ((c4dhook[findfileidx].flags & FLG_LOCK)) {
				SetError(26, 0, 0);	/* write protect */
				return 1;
			    }
			} else
			    strcpy(unixname[ch], nbuf);
		    }
		}
#ifdef ALWAYS_SAVE_P00
#ifdef DIRFILE
		if (emu_mode == EMU_MODE_FILESYS)
#endif
		    if (openmode != MODE_A) {
			if (findfileidx != -1)
			    unlink(unixname[ch]);
			strcpy(unixname[ch], MakeP00(nbuf));
		    }
#endif
		flags[ch] = F_WRITE;
		if (overwrite)
		    flags[ch] |= F_OVERWRITE;
		i = O_WRONLY|O_CREAT;
		if (overwrite) i |= O_TRUNC;
		if (openmode == MODE_A) i |= O_APPEND;
		if ((i & (O_TRUNC|O_APPEND)) == (O_TRUNC|O_APPEND)) {
		    SetError(33, 0, 0);
		    return 1;
		}
#ifdef __GO32__
		fmd = 0666;
		chfd[ch] = open(unixname[ch], i|BINARY_FILE, fmd);
#else
#ifdef DIRFILE
		if (emu_mode == EMU_MODE_DIRFILE)
		    fmd = 0;
		else
#endif
#ifdef ALWAYS_SAVE_P00
		{
		    if (openmode != MODE_A && findfileidx == -1)
			fmd = 0;
		    else
			fmd = mode_2_stat(ftype);
		}
#else
		    fmd = mode_2_stat(ftype);
#endif
		if (fmd == -1) {
		    SetError(28, 0, 0);
		    return 1;
		}
		if (fmd == -2) {
		    chfd[ch] = open(unixname[ch], i, 0666);
		    fmd = 0;
		} else {
		    fmd |= (0666 & UMask);
		    chfd[ch] = open(unixname[ch], i, fmd);
		}
#endif        
		fmod[ch] = fmd;
		if (chfd[ch] != -1) {
#ifdef ALWAYS_SAVE_P00
		    if (openmode != MODE_A)
#else
		    if (openmode != MODE_A && findfileidx != -1 && (c4dhook[findfileidx].flags & FLG_PC64))
#endif
		    {
			memset(&pc64header, 0, sizeof(pc64header));
			strcpy(pc64header.detect, PC64ID);
#ifdef ALWAYS_SAVE_P00
			if (findfileidx == -1) {
			    strcpy(pc64header.name, nbuf);
			    pc64header.recordsize = 0;
			} else
#endif
			{
			    strcpy(pc64header.name, c4dhook[findfileidx].name);
			    pc64header.recordsize = c4dhook[findfileidx].recordsize;
			}
			strasc2cbm(pc64header.name);
			if (write(chfd[ch], &pc64header, sizeof(pc64header)) != sizeof(pc64header)) {
			    close(chfd[ch]);
			    chfd[ch] = -1;
			    chanbufp[ch] = 0;
			    chanpos[ch] = 0;
			    filelen[ch] = 0;
			    flags[ch] = 0;
			    SetError(28, 0, 0);
			    return 1;
			}
		    }
		    filelen[ch] = 0;
		    filepos[ch] = 0;
		    chanbufp[ch] = 0;
		    chanpos[ch] = 0;
#ifndef __GO32__
		    chmod(unixname[ch], fmd);
#endif        
		    SetError(0, 0, 0);
		    return 0;
		}
		SetError(28, 0, 0);
		return 1;
		break;
	    case MODE_R:
	    case MODE_M:
	    default:
		if (!findfile(ch, nbuf)) {
		    SetError(62, 0, 0);
		    return 1;
		}
		if (overwrite) {
		    SetError(33, 0, 0);
		    return 1;
		}
		if (c4dhook[findfileidx].mode != ftype) {
		    SetError(64, 0, 0);
		    return 1;
		}
		flags[ch] = F_READ;
		if (emu_mode == EMU_MODE_D64) {
		    chfd[ch] = -5;
		    filepos[ch] = 0;
		    chanpos[ch] = 256;
		    chanbuf[ch][0] = c4dhook[findfileidx].u.d64.starttrack;
		    chanbuf[ch][1] = c4dhook[findfileidx].u.d64.startsector;
		    SetError(0, 0, 0);
		    ReadABlock(ch);
		    return (errorcode != 0);
		}
#ifndef __GO32__
		if (
#ifdef DIRFILE
		    emu_mode == EMU_MODE_FILESYS &&
#endif
					ftype == TYPE_D)
		    chmod(unixname[ch],0600);
#endif
		chfd[ch] = open(unixname[ch], O_RDONLY|BINARY_FILE);
#ifndef __GO32__
		if (
#ifdef DIRFILE
		    emu_mode == EMU_MODE_FILESYS &&
#endif
					ftype == TYPE_D)
		    chmod(unixname[ch],0);
#endif
		if (chfd[ch] != -1)
		    if (fstat(chfd[ch],&st) != -1) {
			filelen[ch] = st.st_size;
			if ((c4dhook[findfileidx].flags & FLG_PC64)) {
			    filelen[ch] -= sizeof(struct pc64entry);
			    lseek(chfd[ch], sizeof(struct pc64entry), SEEK_SET);
			}
			filepos[ch] = 0;
			ReadABlock(ch);
			chanpos[ch] = 0;
			SetError(0, 0, 0);
			return 0;
		    }
		SetError(62, 0, 0);
		return 1;
	}
    }
}

void DoCloseFile(int ch)
{
    /* Write the rest of block */
    
    if (flags[ch] & F_WRITE)
	WriteABlock(ch);
    if (chfd[ch] > 0) {
	close(chfd[ch]);
    } else if (chfd[ch] == -2 ) {
	/* command channel */ /* FIXME: close all other channels !!! */
    } else if (chfd[ch] == -4 ) {
	/* close a directory */
	close1541dir(ch);
    }
    read_the_dir();

    chfd[ch] = -1;
    chanbufp[ch] = 0;
    chanpos[ch] = 0;
    filelen[ch] = 0;
    flags[ch] = 0;
}


void WriteToFile(int ch, int byte)
{
    if (chfd[ch] == -1) {
	if (errorcode == 0)
	    SetError(61, 0, 0);
	IEC_SetStatus(0x03);
	return;
    }
    if (chfd[ch] == -6) {
	if(chanbufp[ch] == 256) {
	    IEC_SetStatus(0x03);
	    return;
	}
	chanbuf[ch][chanbufp[ch]++] = byte;
	chanpos[ch]++;
	return;
    }
    if (ch == CMD_CHAN)  { /* fast opened cmd channel */
	if (byte == '\r') {
	    if (chanbufp[ch])
		PerformCommand(chanbuf[ch], chanbufp[ch]);
	    chanpos[ch] = 0;
	    chanbufp[ch] = 0;
	    return;
	}
	if (chanbufp[ch] == 63) {       /* MAX CMD length */
	    return;
	} 
    }
    chanbuf[ch][chanbufp[ch]++] = byte;
    chanpos[ch]++;
    if (chanbufp[ch] == 256) {
	WriteABlock(ch);
	chanbufp[ch] = 0;
    }
}

int ReadFromFile(int ch)
{
    int byte;

    if (chfd[ch] == -1 ) {
	if (errorcode == 0)
	    SetError(61, 0, 0);
	IEC_SetStatus(0x02);
	return '\r';
    }
    if (chanpos[ch] < filelen[ch]) {
	byte = chanbuf[ch][chanbufp[ch]++];
	chanpos[ch]++;
	if (chanbufp[ch] == 256) {
	    if (chfd[ch] == -4 || chanpos[ch] < filelen[ch])
		ReadABlock(ch);
	}
    } else {
	IEC_SetStatus(0x02);
	byte = '\r';
    }
    if (chanpos[ch] == filelen[ch]) {
	IEC_SetStatus(0x40);
	if (ch == CMD_CHAN) {
	    SetError(0, 0, 0);
	    SetCmdChannel(errorbuf, strlen(errorbuf));
	}
    }
    return byte;
}

#ifdef PRINTER_SUPPORT
/* The Printer Subroutines */

void ClosePrinter(void)
{
    static char prbuf[128];
    close(printer_fd);
    sprintf(prbuf, printer_printcmd, DEF_PRINTFILE);
    system(prbuf);
    printer_open = 0;
    printer_cmd = 10;                           /* reset state */
}

void OpenPrinter(void)
{
    if (printer_open) 
	ClosePrinter();
    strcpy(DEF_PRINTFILE, SET_PRINTFILE);
    mktemp(DEF_PRINTFILE);
    if (printer_cmd == -1)
	printer_fd = open(DEF_PRINTFILE, O_WRONLY|O_CREAT|O_APPEND, 0666);
    else
	printer_fd = open(DEF_PRINTFILE, O_WRONLY|O_CREAT, 0666);
    printer_open = 1;
}

void WriteToPrinter(int byte)
{
    unsigned char c;

    if (printer_cmd != 0 && printer_cmd != -1 && printer_cmd != 7)
	/* sa: 0:CBM-GRAPHIC charset, 7:CBM-Business charset, -1:no sa given! */
	return;
    c = byte & 0xff;
    if (printer_cmd == 7)
	c = cbmtoasc(c);
    if (c == '\r')
	c = '\n';
    write(printer_fd, &c, 1);
}

void FlushPrinter(void)
{
    /* FIXME: add some kind of timeout here to enforce printing of opened without a secondary address */
    /* this routine is called (by unlisten) every time a chunk of such data was written */
}
#endif

/* The C64 Subroutines */

void IEC_Listen(int iec_unit)
{
    IEC_SetStatus(0);
    aktunit = iec_unit;
    if (aktunit != unit) {
#ifdef PRINTER_SUPPORT
	if (aktunit != printer_unit)
	    IEC_SetStatus(0x80);
	else
	    globflags = F_PR_LISTEN;
#else
       IEC_SetStatus(0x80);
#endif
    } else
	globflags = F_INLISTEN;
}

void IEC_SEC_Listen(int iec_sec)
{
    int cmd;

    IEC_SetStatus(0);
#ifdef PRINTER_SUPPORT
    if ((aktunit == printer_unit) && (globflags == F_PR_LISTEN)) {
	printer_cmd = iec_sec & 15;
	cmd = iec_sec >> 4;
	switch (cmd) {
	    case 0xe:           /* file close */
		globflags = F_PR_CLOSE;
		break;
	    case 0x6:           /* write/open */
		OpenPrinter();
		globflags = F_PR_WRITE;
		return;
	    default:
		globflags = 0;
		IEC_SetStatus(0);
		return;
	}
    }
#endif
    if ((aktunit != unit) || ((globflags & F_INLISTEN) == 0) )
	IEC_SetStatus(0x80);
    else {
	channel = iec_sec & 15;
	cmd = iec_sec >> 4;
	switch (cmd) {
	    case 0xf:           /* file eroeffnen */
		filenamelen[channel] = 0;
		filenamebuf[channel][0] = 0;
		globflags = F_INNAMELISTEN;
		break;
	    case 0xe:           /* file schliessen */
		globflags = F_INCLOSE;
		break;
	    case 0x6:           /* write, save */
		if (channel == CMD_CHAN) {
		    if (chfd[channel] == -1) {
			chfd[channel] = -3;
			flags[channel] = F_OPEN;
		    }
		    flags[channel] |= F_WRITE;
		    chanbufp[CMD_CHAN] = 0;
		    chanpos[CMD_CHAN] = 0;
		    filelen[CMD_CHAN] = 0;
		}
		globflags = F_INLISTEN;
		break;
	    default:
		break;
	}
    }
}

void IEC_Write(int byte)
{
    int i;

    IEC_SetStatus(0);
    if (globflags & F_INNAMELISTEN)
    {
	if ((i = filenamelen[channel]) < 64) {
	    filenamebuf[channel][i] = byte;
	    filenamelen[channel]++;
	}
	/* ignore rest of name silently.. */
    }
    else if (globflags & F_INLISTEN)
    {

	if ((flags[channel] & F_OPEN) == 0)
	    IEC_SetStatus(0x02);
	else
	    WriteToFile(channel, byte);
    }
#ifdef PRINTER_SUPPORT
    else if ((globflags & F_PR_WRITE))
    {
	WriteToPrinter(byte);
    }
    else if ((globflags & F_PR_LISTEN))
    {
	if (!printer_open) {
	    printer_cmd = -1;           /* Open ohne secaddr */
	    OpenPrinter();
	}
	globflags = F_PR_WRITE;
	WriteToPrinter(byte);
    }
#endif
    else
	IEC_SetStatus(0x02);
}

void IEC_Unlisten(void)
{
    IEC_SetStatus(0);
    if ((globflags & F_INNAMELISTEN))
    {
	globflags = 0;
	filenamebuf[channel][filenamelen[channel]] = 0;
	if (DoOpenFile(channel, filenamebuf[channel]) == 0)
	    flags[channel] |= F_OPEN;
    }
    else if ((globflags & F_INLISTEN)) {
	if (chfd[channel] < -1) {	/* [fast] opened cmd channel */
	    if (chfd[channel] != -6)	/* chanopen */
	    {
		if (chanbufp[channel])
		    PerformCommand(chanbuf[channel], chanbufp[channel]);     /* should test for length = 0 */
		if (chfd[channel] == -3) {  /* fast opened */
		    chfd[channel] = -1;
		    flags[channel] = 0;
		}
	    }
	}
	globflags = 0;
    }
    else if ((globflags & F_INCLOSE)) {
	DoCloseFile(channel);
	flags[channel] = 0;
	globflags = 0;
    }
#ifdef PRINTER_SUPPORT
    else if ((globflags & F_PR_CLOSE)) {
	ClosePrinter();
	globflags = 0;
    }
    else if ((globflags & F_PR_WRITE)) {
	if (printer_cmd == -1)
	    FlushPrinter();
	globflags = 0;
    }
#endif
    else
	IEC_SetStatus(0x02);
}

void IEC_Talk(int iec_unit)
{
    IEC_SetStatus(0);
    aktunit = iec_unit;
    if (aktunit != unit)
	IEC_SetStatus(0x80);
    else
	globflags = F_INTALK;
}

void IEC_SEC_Talk(int iec_sec)
{
    IEC_SetStatus(0);
    if (aktunit != unit || (globflags & F_INTALK) == 0 )
	IEC_SetStatus(0x80);
    else {
	channel = iec_sec & 15;
	if (channel != CMD_CHAN) {
	    if ((flags[channel] & F_OPEN) == 0) {
		if (errorcode == 0)
		    SetError(61, 0, 0);
		IEC_SetStatus(0x02);
	    }
	} else {
	    if (chfd[channel] == -1) {
		chfd[channel] = -3;     /* fast open */
		flags[channel] = F_OPEN|F_READ;
	    }
	    SetCmdChannel(errorbuf, strlen(errorbuf));
	}
    }
}

void IEC_Untalk(void)
{
    IEC_SetStatus(0);
    if ((globflags & F_INTALK))
	globflags &= ~F_INTALK;
    else
	IEC_SetStatus(0x02);
    if (chfd[channel] == -3) { /* fast opened cmd channel */
	chfd[channel] = -1;
	flags[channel] = 0;
    }
}

int IEC_Read(void)
{
    IEC_SetStatus(0);
    if ((globflags & F_INTALK)) {
	    return ReadFromFile(channel);
    }
    /* printf("Illegal read\n");	* DEBUG */
    IEC_SetStatus(0x02);
    return '\r';
}

#endif
