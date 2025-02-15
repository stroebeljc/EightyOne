/* z80_ed.c: z80 EDxx opcodes
   Copyright (c) 1999-2002 Philip Kendall

   $Id: z80_ed.c,v 1.12 2002/04/10 14:27:33 pak21 Exp $

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

case 0x40:	/* IN B,(C) */
tstates += 1;
IN(B,BC);
break;

case 0x41:	/* OUT (C),B */
tstates += 1;
OUT(BC,B);
break;

case 0x42:	/* SBC HL,BC */
tstates += 7;
SBC16(BC);
break;

case 0x43:	/* LD (nnnn),BC */
LD16_NNRR(C,B);
break;

case 0x44:	
case 0x4c:
case 0x54:
case 0x5c:
case 0x64:
case 0x6c:
case 0x74:
case 0x7c:	/* NEG */
{
  BYTE bytetemp=A;
  A=0;
  SUB(bytetemp);
}
break;

case 0x4d:      /* RETI */

case 0x45:
case 0x55:
case 0x5d:
case 0x65:
case 0x6d:
case 0x75:
case 0x7d:      /* RETN */
IFF1=IFF2;
RET();
break;

case 0x46:
case 0x4e:
case 0x66:
case 0x6e:	/* IM 0 */
IM=0;
break;

case 0x47:	/* LD I,A */
tstates += 1;
AddToMCycle(1);
I=A;
break;

case 0x48:	/* IN C,(C) */
tstates += 1;
IN(C,BC);
break;

case 0x49:	/* OUT (C),C */
tstates += 1;
OUT(BC,C);
break;

case 0x4a:	/* ADC HL,BC */
tstates += 7;
ADC16(BC);
break;

case 0x4b:	/* LD BC,(nnnn) */
LD16_RRNN(C,B);
break;

case 0x4f:	/* LD R,A */
tstates += 1;
AddToMCycle(1);

/* Keep the RZX instruction counter right */
// rzx_instructions_offset += ( R - A );
//debug(0);
R=R7=A;
break;

case 0x50:	/* IN D,(C) */
tstates += 1;
IN(D,BC);
break;

case 0x51:	/* OUT (C),D */
tstates += 1;
OUT(BC,D);
break;

case 0x52:	/* SBC HL,DE */
tstates += 7;
SBC16(DE);
break;

case 0x53:	/* LD (nnnn),DE */
LD16_NNRR(E,D);
break;

case 0x56:
case 0x76:	/* IM 1 */
IM=1;
break;

case 0x57:	/* LD A,I */
tstates += 1;
AddToMCycle(1);
A=I;
F = (BYTE)(( F & FLAG_C ) | sz53_table[A] | ( IFF2 ? FLAG_V : 0 ));
break;

case 0x58:	/* IN E,(C) */
tstates += 1;
IN(E,BC);
break;

case 0x59:	/* OUT (C),E */
tstates += 1;
OUT(BC,E);
break;

case 0x5a:	/* ADC HL,DE */
tstates += 7;
ADC16(DE);
break;

case 0x5b:	/* LD DE,(nnnn) */
LD16_RRNN(E,D);
break;

case 0x5e:
case 0x7e:	/* IM 2 */
IM=2;
break;

case 0x5f:	/* LD A,R */
tstates += 1;
AddToMCycle(1);
A=(BYTE)((R&0x7f) | (R7&0x80));
F = (BYTE)(( F & FLAG_C ) | sz53_table[A] | ( IFF2 ? FLAG_V : 0 ));
break;

case 0x60:	/* IN H,(C) */
tstates += 1;
IN(H,BC);
break;

case 0x61:	/* OUT (C),H */
tstates += 1;
OUT(BC,H);
break;

case 0x62:	/* SBC HL,HL */
tstates += 7;
SBC16(HL);
break;

case 0x63:	/* LD (nnnn),HL */
LD16_NNRR(L,H);
break;

case 0x67:	/* RRD */
{
  BYTE bytetemp=readbyte(HL);
  InsertMCycle(3);
  AddToMCycle(4);
  InsertMCycle(3);
  contend( HL, 7 ); contend( HL, 3 );
  writebyte(HL,  ( A << 4 ) | ( bytetemp >> 4 ) );
  A = (BYTE)(( A & 0xf0 ) | ( bytetemp & 0x0f ));
  F = (BYTE)(( F & FLAG_C ) | sz53p_table[A]);
}
break;

case 0x68:	/* IN L,(C) */
tstates += 1;
IN(L,BC);
break;

case 0x69:	/* OUT (C),L */
tstates += 1;
OUT(BC,L);
break;

case 0x6a:	/* ADC HL,HL */
tstates += 7;
ADC16(HL);
break;

case 0x6b:	/* LD HL,(nnnn) */
LD16_RRNN(L,H);
break;

case 0x6f:	/* RLD */
{
  BYTE bytetemp=readbyte(HL);
  InsertMCycle(3);
  AddToMCycle(4);
  InsertMCycle(3);
  contend( HL, 7 ); contend( HL, 3 );
  writebyte(HL, (bytetemp << 4 ) | ( A & 0x0f ) );
  A = (BYTE)(( A & 0xf0 ) | ( bytetemp >> 4 ));
  F = (BYTE)(( F & FLAG_C ) | sz53p_table[A]);
}
break;

case 0x70:	/* IN F,(C) */
tstates += 1;
{
  BYTE bytetemp;
  IN(bytetemp,BC);
}
break;

case 0x71:	/* OUT (C),0 */
tstates += 1;
OUT(BC,0);
break;

case 0x72:	/* SBC HL,SP */
tstates += 7;
SBC16(SP);
break;

case 0x73:	/* LD (nnnn),SP */
LD16_NNRR(SPL,SPH);
break;

case 0x78:	/* IN A,(C) */
tstates += 1;
IN(A,BC);
break;

case 0x79:	/* OUT (C),A */
tstates += 1;
OUT(BC,A);
break;

case 0x7a:	/* ADC HL,SP */
tstates += 7;
ADC16(SP);
break;

case 0x7b:	/* LD SP,(nnnn) */
LD16_RRNN(SPL,SPH);
StackChange=0;
break;

case 0xa0:	/* LDI */
{
  BYTE bytetemp=readbyte(HL);
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( DE, 3 ); contend( DE, 1 ); contend( DE, 1 );
  BC--;
  writebyte(DE,bytetemp);
  DE++; HL++;
  bytetemp += A;
  F = (BYTE)(( F & ( FLAG_C | FLAG_Z | FLAG_S ) ) | ( BC ? FLAG_V : 0 ) |
    ( bytetemp & FLAG_3 ) | ( (bytetemp & 0x02) ? FLAG_5 : 0 ));
}
break;

case 0xa1:	/* CPI */
{
  BYTE value=readbyte(HL);
  BYTE bytetemp=(BYTE)(A-value);
  BYTE lookup = (BYTE)(( (A & 0x08) >> 3 ) | ( ( (value) & 0x08 ) >> 2 ) |
    ( (bytetemp & 0x08) >> 1 ));
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
  contend( HL, 1 ); contend( HL, 1 );
  HL++; BC--;
  F = (BYTE)(( F & FLAG_C ) | ( BC ? ( FLAG_V | FLAG_N ) : FLAG_N ) |
    halfcarry_sub_table[lookup] | ( bytetemp ? 0 : FLAG_Z ) |
    ( bytetemp & FLAG_S ));
  if(F & FLAG_H) bytetemp--;
  F |= (BYTE)(( bytetemp & FLAG_3 ) | ( (bytetemp&0x02) ? FLAG_5 : 0 ));
}
break;

case 0xa2:	/* INI */
{
  WORD initemp=readport(BC,&tstates);
  AddToMCycle(1);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  InsertMCycle(3);
  tstates += 2; contend_io( BC, 3 ); contend( HL, 3 );
  writebyte(HL,initemp);
  B--; HL++;
  F = (BYTE)((initemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
}
break;

case 0xa3:	/* OUTI */
{
  WORD outitemp=readbyte(HL);
  AddToMCycle(1);
  InsertMCycle(3);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  tstates++; contend( HL, 4 ); contend_io( BC, 3 );
  B--;
  writeport(BC,outitemp,&tstates);
  HL++;
  F = (BYTE)((outitemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
}
break;

case 0xa8:	/* LDD */
{
  BYTE bytetemp=readbyte(HL);
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( DE, 3 ); contend( DE, 1 ); contend( DE, 1 );
  BC--;
  writebyte(DE,bytetemp);
  DE--; HL--;
  bytetemp += A;
  F = (BYTE)(( F & ( FLAG_C | FLAG_Z | FLAG_S ) ) | ( BC ? FLAG_V : 0 ) |
    ( bytetemp & FLAG_3 ) | ( (bytetemp & 0x02) ? FLAG_5 : 0 ));
}
break;

case 0xa9:	/* CPD */
{
  BYTE value=readbyte(HL);
  BYTE bytetemp=(BYTE)(A-value);
  BYTE lookup = (BYTE)(( (A & 0x08) >> 3 ) | ( ( (value) & 0x08 ) >> 2 ) |
    ( (bytetemp & 0x08) >> 1 ));
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
  contend( HL, 1 ); contend( HL, 1 );
  HL--; BC--;
  F = (BYTE)(( F & FLAG_C ) | ( BC ? ( FLAG_V | FLAG_N ) : FLAG_N ) |
    halfcarry_sub_table[lookup] | ( bytetemp ? 0 : FLAG_Z ) |
    ( bytetemp & FLAG_S ));
  if(F & FLAG_H) bytetemp--;
  F |= (BYTE)(( bytetemp & FLAG_3 ) | ( (bytetemp&0x02) ? FLAG_5 : 0 ));
}
break;

case 0xaa:	/* IND */
{
  WORD initemp=readport(BC,&tstates);
  AddToMCycle(1);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  InsertMCycle(3);
  tstates += 2; contend_io( BC, 3 ); contend( HL, 3 );
  writebyte(HL,initemp);
  B--; HL--;
  F = (BYTE)((initemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
}
break;

case 0xab:	/* OUTD */
{
  WORD outitemp=readbyte(HL);
  AddToMCycle(1);
  InsertMCycle(3);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  tstates++; contend( HL, 4 ); contend_io( BC, 3 );
  B--;
  writeport(BC,outitemp,&tstates);
  HL--;
  F = (BYTE)((outitemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
}
break;

case 0xb0:	/* LDIR */
{
  BYTE bytetemp = readbyte( HL );
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( DE, 3 ); contend( DE, 1 ); contend( DE, 1 );
  writebyte(DE,bytetemp);
  HL++; DE++; BC--;
  bytetemp += A;
  F = (BYTE)(( F & ( FLAG_C | FLAG_Z | FLAG_S ) ) | ( BC ? FLAG_V : 0 ) |
    ( bytetemp & FLAG_3 ) | ( (bytetemp & 0x02) ? FLAG_5 : 0 ));
  if(BC) {
    AddToMCycle(5);
    contend( DE, 1 ); contend( DE, 1 ); contend( DE, 1 ); contend( DE, 1 );
    contend( DE, 1 );
    PC-=(WORD)2;
  }
}
break;

case 0xb1:	/* CPIR */
{
  BYTE value=readbyte(HL);
  BYTE bytetemp=(BYTE)(A-value);
  BYTE lookup = (BYTE)(( (A & 0x08) >> 3 ) | ( ( (value) & 0x08 ) >> 2 ) |
    ( (bytetemp & 0x08) >> 1 ));
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
  contend( HL, 1 ); contend( HL, 1 );
  HL++; BC--;
  F = (BYTE)(( F & FLAG_C ) | ( BC ? ( FLAG_V | FLAG_N ) : FLAG_N ) |
    halfcarry_sub_table[lookup] | ( bytetemp ? 0 : FLAG_Z ) |
    ( bytetemp & FLAG_S ));
  if(F & FLAG_H) bytetemp--;
  F |= (BYTE)(( bytetemp & FLAG_3 ) | ( (bytetemp&0x02) ? FLAG_5 : 0 ));
  if( ( F & ( FLAG_V | FLAG_Z ) ) == FLAG_V ) {
    AddToMCycle(5);
    contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
    contend( HL, 1 );
    PC-=(WORD)2;
  }
}
break;
  
case 0xb2:	/* INIR */
{
  WORD initemp=readport(BC,&tstates);
  AddToMCycle(1);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  InsertMCycle(3);
  tstates += 2; contend_io( BC, 3 ); contend( HL, 3 );
  writebyte(HL,initemp);
  B--; HL++;
  F = (BYTE)((initemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
  if(B) {
    AddToMCycle(5);
    contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
    contend( HL, 1 );
    PC-=(WORD)2;
  }
}
break;

case 0xb3:	/* OTIR */
{
  WORD outitemp=readbyte(HL);
  AddToMCycle(1);
  InsertMCycle(3);
  tstates++; contend( HL, 4 );
  B--;
  writeport(BC,outitemp,&tstates);
  HL++;
  F = (BYTE)((outitemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  if(B) {
    AddToMCycle(5);
    contend_io( BC, 1 );
    contend( PC, 1 ); contend( PC, 1 ); contend( PC  , 1 ); contend( PC, 1 );
    contend( PC, 1 ); contend( PC, 1 ); contend( PC-1, 1 );
    PC-=(WORD)2;
  } else {
    contend_io( BC, 3 );
  }
}
break;

case 0xb8:	/* LDDR */
{
  BYTE bytetemp=readbyte(HL);
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( DE, 3 ); contend( DE, 1 ); contend( DE, 1 );
  writebyte(DE,bytetemp);
  HL--; DE--; BC--;
  bytetemp += A;
  F = (BYTE)(( F & ( FLAG_C | FLAG_Z | FLAG_S ) ) | ( BC ? FLAG_V : 0 ) |
    ( bytetemp & FLAG_3 ) | ( (bytetemp & 0x02) ? FLAG_5 : 0 ));
  if(BC) {
    AddToMCycle(5);
    contend( DE, 1 ); contend( DE, 1 ); contend( DE, 1 ); contend( DE, 1 );
    contend( DE, 1 );
    PC-=(WORD)2;
  }
}
break;

case 0xb9:	/* CPDR */
{
  BYTE value=readbyte(HL);
  BYTE bytetemp=(BYTE)(A-value);
  BYTE lookup = (BYTE)(( (A & 0x08) >> 3 ) | ( ( (value) & 0x08 ) >> 2 ) |
    ( (bytetemp & 0x08) >> 1 ));
  InsertMCycle(3);
  InsertMCycle(5);
  contend( HL, 3 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
  contend( HL, 1 ); contend( HL, 1 );
  HL--; BC--;
  F = (BYTE)(( F & FLAG_C ) | ( BC ? ( FLAG_V | FLAG_N ) : FLAG_N ) |
    halfcarry_sub_table[lookup] | ( bytetemp ? 0 : FLAG_Z ) |
    ( bytetemp & FLAG_S ));
  if(F & FLAG_H) bytetemp--;
  F |= (BYTE)(( bytetemp & FLAG_3 ) | ( (bytetemp&0x02) ? FLAG_5 : 0 ));
  if( ( F & ( FLAG_V | FLAG_Z ) ) == FLAG_V ) {
    AddToMCycle(5);
    contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
    contend( HL, 1 );
    PC-=(WORD)2;
  }
}
break;

case 0xba:	/* INDR */
{
  WORD initemp=readport(BC,&tstates);
  AddToMCycle(1);
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  InsertMCycle(3);
  tstates += 2; contend_io( BC, 3 ); contend( HL, 3 );
  writebyte(HL,initemp);
  B--; HL--;
  F = (BYTE)((initemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
  if(B) {
    AddToMCycle(5);
    contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 ); contend( HL, 1 );
    contend( HL, 1 );
    PC-=(WORD)2;
  }
}
break;

case 0xbb:	/* OTDR */
{
  WORD outitemp=readbyte(HL);
  AddToMCycle(1);
  InsertMCycle(3);
  tstates++; contend( HL, 4 );
  B--;
  writeport(BC,outitemp,&tstates);
  HL--;
  F = (BYTE)((outitemp & 0x80 ? FLAG_N : 0 ) | sz53_table[B]);
  /* C,H and P/V flags not implemented */
  InsertMCycle(4);
  inputOutputMCycle = mCycleIndex;
  if(B) {
    AddToMCycle(5);
    contend_io( BC, 1 );
    contend( PC, 1 ); contend( PC, 1 ); contend( PC  , 1 ); contend( PC, 1 );
    contend( PC, 1 ); contend( PC, 1 ); contend( PC-1, 1 );
    PC-=(WORD)2;
  } else {
    contend_io( BC, 3 );
  }
}
break;

case 0xfb:	/* Emulator trap to load .slt data */
//if( settings_current.slt_traps ) {
//
//  if( slt_length[A] ) {
//    WORD base = HL;
//    BYTE *data = slt[A];
//    size_t length = slt_length[A];
//
//    while( length-- ) writebyte( base++, *data++ );
//  }
//
//}
break;

default:	/* All other opcodes are NOPD */
break;
