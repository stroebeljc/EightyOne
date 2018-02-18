/*
**	The C64 emulator
**
**	Copyright 1996 by ALE.
**	written by Lutz Sammer.
**
**	Configuration header file.
**
**-----------------------------------------------------------------------------
** $Id: config.h,v 1.4 1996/07/03 23:42:19 ari Exp root $
** $Log: config.h,v $
** Revision 1.4  1996/07/03 23:42:19  ari
** New defines for 1541. DSP and GO32 check removed.
**
** Revision 1.3  1996/06/13 00:08:27  ari
** Moved PCSOLARIS autodetection to here.
**
** Revision 1.2  1996/06/09 19:58:22  johns
** Release 1.12 configuration.
**
** Revision 1.1  1996/05/12 20:50:23  johns
** Initial revision
**
**-----------------------------------------------------------------------------
*/

#ifndef __CONFIG_H__	/* { */
#define __CONFIG_H__

/******************************************************************************
 *	Features config
 *****************************************************************************/

#undef	DEBUG				/* If want debugging */

#define	MONITOR				/* Include monitor */

#define	ALE_RAM				/* ALE ram at D200-D3FF D600-D7FF */

#define	PAL				/* Emulate PAL C64 */

#ifndef PAL
#define	NTSC				/* Emulate NTSC C64 (never checked) */
#endif


/******************************************************************************
 *	Machine config
 *****************************************************************************/

/*
**	Video driver.
**		Choose one of the listed.
*/
#define __GO32__

#define	noX11				/* X11 display */
#define	noSVGALIB			/* linux svga library */

#ifdef __GO32__				/* AUTO DETECTION */
#undef X11				/* go32 vga library */
#undef SVGALIB
#endif

/******************************************************************************
 *	Speed config
 *****************************************************************************/

/*
**	Define this to pass arguments in registers for selected functions.
*/
#define	USE_REGPARM			/* parse args to config as registers */

/*
**	Define this to pass arguments in globals for selected functions.
*/
#undef	GLOBAL_ARGS			/* parse args to config as globals */

/*
**	Define this to test the alternative memory interface.
**		(FIXME: currently not full implemented).
*/
#define	noNEW_MEMORY

/******************************************************************************
 *	VIC config
 *****************************************************************************/

/*
**	Define VIDEOSYNC to include syncronise code.
*/
#define	VIDEOSYNC

/*
**	Define OVERSCAN to emulate overscan area.
*/
#define	noOVERSCAN

/*
**	Controls the default time needed for vic emulation.
**		1	Emulate/Refresh each frame.
**		n	Emulate/Refresh 1/n frame.
*/
#define	VIC_EMULATE_RATE	2	/* recalulation rate */
#define	VIC_REFRESH_RATE	1	/* refresh rate */

/*
**	Control the vic fetch position default.
*/
#define	VIC_FETCH_DEFAULT	13	/* position irq before fetch */
#define	VIC_FETCH_ADD_DEFAULT	2	/* increment line before irq */

/*.............................................................................
 *	VIC X11 sub-config
 *...........................................................................*/

/*
**	How to get the colors for window.
**
**		COLORMAP0	Allocate 16 colors from default colormap.
**				Slowest, but looks better.
**		COLORMAP16	Allocate 16 continuous colors from colormap.
**				Middle, but can fail to alloc colors.
**		COLORMAP256	Allocate 256 colors with own colormap.
**				Fastest, but ugly colormap switches.
**		COLORMAP16bpp	Colormap support for 16bpp X11 server.
*/
#define	noCOLORMAP0			/* use colors get from X11 */
#define	noCOLORMAP16			/* use 16 continuous colors */
#define	noCOLORMAP256			/* use full colormap */
#define	noCOLORMAP16bpp			/* 16bpp X11 server */

/*
**	Sync with X11.
**		Wait until X11 has redrawn window.
**		For slow computers this gives a better X11 handling.
**		For fast computers this cost speed, X11 and C64 emulator
**		aren't running parallel.
*/
#define	XSYNC				/* use XSync call */

/*
**	We need no autorepeat for the emulator.
**	Defines this to turn autorepeat off in c64 window.
*/
#define XREPEATOFF			/* turn of auto repeat */

/*
**	Don't know how to get the setting of the switch ServerNumLock
**	from /etc/XF86Config from the client side.
**	Set this same as in the XFree86 config file.
**	-or-
**	X11 Servers of other machines (SOLARIS 2.4 i86) need this set.
*/
#undef	XSERVER_NUM_LOCK		/* different num-lock handling */

/******************************************************************************
 *	SID config
 *****************************************************************************/

/*
**	Supported sid emulations.
**		Choose one of the listed.
*/
#define	noSID_NONE			/* NO SOUND SUPPORT */
#define	noSID_SPEAKER			/* Use speaker for sid emulation */
#define	noSID_DSP_SPEAKER		/* Use digital speaker for sid emu */
#define	noSID_SPEAKER			/* Use speaker for sid emulation */
#define	noSID_DSP			/* Use dsp for sid emulation */
#define	noSID_OPL3			/* Use synth chip for sid emulation */
#define	noSID_AUTO			/* not supported: future feature */

/******************************************************************************
 *	JOYSTICK config
 *****************************************************************************/

/*
**	Choose one of the below.
*/
#define noGAMEPORT_JOY			/* C64 joystick conn. to game-port */
#define noLP_JOY			/* C64 joystick conn. to lp-port */
#define noPC_ANALOG_JOY			/* PC Analog joystick */

/******************************************************************************
 *	1541 config
 *****************************************************************************/

/*
**	1541 emulation configuration:
**		Enable/Disable features.
*/

#define noDIRFILE			/* 1541 dirfile support (obsolete) */

#define ALWAYS_SAVE_P00			/* 1541 write x00 format */

#define PRINTER_SUPPORT			/* Support iec printer emulation */

/*----------------------------------------
**	Real 1541 emulation configuration:
*/
#define REAL1541			/* Emulate the real 1541 */

/*
**	Serial timeout:
**		in 1/50s (50 is 1s)
**		This time the 1541 and c64 are hold cycle syncron.
*/
#define SERIAL_TIMEOUT	"50"		/* serial timeout */

/*---------------------------------------------------------------------------*/

/*
**	Names of used files.
*/
#ifndef C64DEFAULT			/* Path to the default settings */
#   define C64DEFAULT	"/etc/default/c64"
#endif

#ifndef C64IMAGE			/* Path to the rom images */
#   define C64IMAGE	"romimage.c64"
#endif

#ifndef C64EXROM			/* Path to the external rom image */
#   define C64EXROM	"external.c64"
#endif

#ifndef C64AUTOLOAD			/* File to load at startup */
#   define C64AUTOLOAD	"autoload.c64"
#endif

#ifndef C64LIB				/* Program library database */
#   define C64LIB	"lib.c64"
#endif

#ifndef C64SNAPSHOT			/* File to save snapshot */
#   define C64SNAPSHOT	"snapshot.c64"
#endif

#ifndef R1541IMAGE			/* Path to the 1541 rom images */
#define R1541IMAGE	"1541roms.c64"
#endif

/*---------------------------------------------------------------------------*/

/*
**	Some automatic detections.
*/
#if defined(sun) && defined(i386)
#define PCSOLARIS
#define __USES_AS__			/* automatic detection */
#define	XSERVER_NUM_LOCK		/* different num-lock handling */
#endif

#ifdef __GO32__
#undef PRINTER_SUPPORT
#undef X11				/* go32 vga library */
#undef SVGALIB
#endif

/*---------------------------------------------------------------------------*/

/*
**	Test config combinations.
*/
#if defined(VIDEOSYNC) && defined(SID_DSP)
// That's not realy true, you only get bad sound.
// #warning "Can't use videosync combined with sid dsp driver"
#endif

#if !defined(__GO32__) && (defined(SID_DSP_SPEAKER))
#warning "DSP SPEAKER not supported on this machine"
#endif

#if !defined(noSID_NONE) && !defined(noSID_SPEAKER) &&			\
    !defined(noSID_DSP_SPEAKER) && !defined(noSID_SPEAKER) &&		\
    !defined(noSID_DSP) && !defined(noSID_OPL3) && !defined(noSID_AUTO)
#warning "One SID driver must be specified!"
#endif
#endif /* } !__CONFIG_H__ */
