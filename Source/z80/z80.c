/* z80.c: z80 supplementary functions
   Copyright (c) 1999, 2000, 2002 Philip Kendall
   EightyOne Modifications (c) 2003-2006 Michael D Wynne

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <config.h>
#include <stdio.h>

#include "zx81.h"
#include "zx81config.h"
#include "z80.h"
#include "z80_macros.h"

/* Whether a half carry occured or not can be determined by looking at
   the 3rd bit of the two arguments and the result; these are hashed
   into this table in the form r12, where r is the 3rd bit of the
   result, 1 is the 3rd bit of the 1st argument and 2 is the
   third bit of the 2nd argument; the tables differ for add and subtract
   operations */
BYTE halfcarry_add_table[] = { 0, FLAG_H, FLAG_H, FLAG_H, 0, 0, 0, FLAG_H };
BYTE halfcarry_sub_table[] = { 0, 0, FLAG_H, 0, FLAG_H, 0, FLAG_H, FLAG_H };

/* Similarly, overflow can be determined by looking at the 7th bits; again
   the hash into this table is r12 */
BYTE overflow_add_table[] = { 0, 0, 0, FLAG_V, FLAG_V, 0, 0, 0 };
BYTE overflow_sub_table[] = { 0, FLAG_V, 0, 0, 0, 0, FLAG_V, 0 };

/* Some more tables; initialised in z80_init_tables() */
BYTE sz53_table[0x100]; /* The S, Z, 5 and 3 bits of the lookup value */
BYTE parity_table[0x100]; /* The parity of the lookup value */
BYTE sz53p_table[0x100]; /* OR the above two tables together */

extern int nmiLatched;
extern int interruptLatchEnable;
extern int interruptLine;
extern int databus;
extern int refreshAddr;

extern int StackChange;
extern int StepOutRequested;

extern void InsertMCycle(int cycleLength);

/* This is what everything acts on! */
processor z80;
void (*z80_INTACK)(void);

static void z80_init_tables(void);

/* Set up the z80 emulation */
void z80_init(void)
{
        z80_init_tables();
        z80_INTACK=machine.interruptAckCb;
}

/* Initalise the tables used to set flags */
static void z80_init_tables(void)
{
        int i,j,k;
        BYTE parity;

        for(i=0;i<0x100;i++)
        {
                sz53_table[i]= (BYTE)(i & ( FLAG_3 | FLAG_5 | FLAG_S ));
                j=i; parity=0;
                for(k=0;k<8;k++)
                {
                        parity ^= (BYTE)(j & 1);
                        j >>=1;
                }

                parity_table[i]= (BYTE)(( parity ? 0 : FLAG_P ));
                sz53p_table[i] = sz53_table[i] | parity_table[i];
        }

        sz53_table[0]  |= FLAG_Z;
        sz53p_table[0] |= FLAG_Z;
}

/* Reset the z80 */
void z80_reset( void )
{
        AF =BC =DE =HL =0;
        AF_=BC_=DE_=HL_=WZ_=0;
        IX=IY=0;
        I=R=R7=0;
        SP=PC=0;
        IFF1=IFF2=IM=0;
        z80.halted=0;
        StackChange=0;
        StepOutRequested=0;
        nmiLatched = 0;
        interruptLine = 1;
        refreshAddr = 0;
}

void z80_interrupt(int state)
{
        interruptLine = (state != 0);
}

void z80_databus(int bus)
{
        databus = bus;
}

/* Process a z80 maskable interrupt */
int z80_interrupt_internal(void)
{
        if (IFF1 && interruptLatchEnable!=0)
        {
                z80.halted = 0;

                InsertMCycle(7);
                numberOfM1Cycles=1;
                if (z80_INTACK) z80_INTACK();
                R++;
                R = (WORD)(R & 127);

                IFF1 = 0;
                IFF2 = 0;

                InsertMCycle(3);
                writebyte(--SP, PCH);
                InsertMCycle(3);
                writebyte(--SP, PCL);

                switch (IM)
                {
                        case 0:
                        {
                                // Assume a RST 38h placed on the data bus
                                PC = WZ_ = 0x0038;
                                StackChange += 2;
                                return 13;
                        }
                        case 1:
                        {
                                PC = WZ_ = 0x0038;
                                StackChange += 2;
                                return 13;
                        }
                        case 2:
	                {
	                        W_= I;
                                Z_=(BYTE)databus;
                                InsertMCycle(3);
        	                PCL = readbyte(WZ_++);
                                InsertMCycle(3);
                                PCH = readbyte(WZ_);
                                WZ_=PC;
	                        StackChange += 2;
                                return 19;
        	        }
                        default:
                        {
                                return 12;
                        }
                }
        }

        return 0;
}

/* Trigger a z80 non-maskable interrupt */
void z80_nmi(void)
{
        nmiLatched = 1;
}

/* Process a z80 non-maskable interrupt */
int z80_nmi_internal(void)
{
    if (nmiLatched!=0)
    {
        StackChange += 2;
        IFF1 = 0;
        nmiLatched = 0;

        z80.halted=0;

        InsertMCycle(5);
        numberOfM1Cycles=1;
        R++;
        R = (WORD)(R & 127);

        InsertMCycle(3);
        writebyte(--SP, PCH);
        InsertMCycle(3);
        writebyte(--SP, PCL);

        PC=WZ_=0x0066;

        return 11;
    }
    return 0;
}
