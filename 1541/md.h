/*
 *	The C64 emulator
 *
 *	Copyright 1992-96 by ALE.
 *	written by Lutz Sammer, Andreas Arens, Frank Wuebbeling
 *
 *	Machine dependend header file
 *-----------------------------------------------------------------------------
 * $Id: md.h,v 1.11 1996/07/07 00:14:50 johns Exp root $
 * $Log: md.h,v $
 * Revision 1.11  1996/07/07 00:14:50  johns
 * PCSOLARIS changes from Frank Wuebbel.
 *
 * Revision 1.10  1996/05/12 21:04:23  johns
 * Cleanup, Binary file open added.
 *
 * Revision 1.9  1995/02/13  11:28:07  ari
 * second linux port integration
 *
 * Revision 1.8  1995/02/06  23:56:41  johns
 * GO32 DPMI keyboard support
 *
 * Revision 1.7  1994/12/18  00:32:11  root
 * non-gcc feature macro support, icc support
 *
 * Revision 1.6  1993/08/31  20:30:45  johns
 * go32 initial support
 *
 * Revision 1.5  1993/06/13  12:25:36  johns
 * cleanup
 *
 * Revision 1.4  1993/01/05  12:42:43  johns
 * Checkin before wuebbel patches
 *
 * Revision 1.3  1992/07/28  19:47:42  johns
 * Sparc changes added.
 *
 * Revision 1.2  1992/07/20  04:30:20  johns
 *
 * Revision 1.1  1992/07/11  21:52:24  johns
 * Initial revision
 *
 *-----------------------------------------------------------------------------
 */

#ifdef _M_UNIX
#   ifndef M_UNIX
#	define M_UNIX
#   endif
#endif
#ifdef _M_I386
#   ifndef M_I386
#	define M_I386
#   endif
#endif

/*
**	Machine config
*/
#	define LITTLE_ENDIAN 1

#define inb(port)	do_in_byte(port)
#define outb(port,val)	do_out_byte(port,val)
#define outw(port,val)	do_out_word(port,val)
#define outi(p,l,h)	do_out_word(p,(l)|((h)<<8))
#define LongFill(a,v,l)	memset(a,v,(l)*4)
#define LongCopy(d,s,l)	memcpy(d,s,(l)*4)

/*---------------------------------------------------------------------------*/

/*
**	Fetch 6502 word
*/
#ifdef BIG_ENDIAN
#   define WORD_FETCH(adr)	((adr)[0]|((adr)[1]<<8))
#endif
#ifdef LITTLE_ENDIAN
#   define WORD_FETCH(adr)	(*(unsigned short*)(adr))
#endif

/*---------------------------------------------------------------------------*/

/*
**	Binary file open
*/
#define BINARY_FILE	O_BINARY

/*---------------------------------------------------------------------------*/

#ifndef CONST
#   define CONST
#endif
#ifndef VOLATILE
#   define VOLATILE
#endif
