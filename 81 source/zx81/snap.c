
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <sys\stat.h>
#include <ctype.h>

#include "snap.h"
#include "zx81.h"
#include "zx81config.h"
#include "z80\z80.h"

#include "tapecinterface.h"

void load_snap_cpu(FILE *f);
void load_snap_mem(FILE *f);


//BYTE tape_buffer[65536];
//int tape_buffer_len;

char CharTable[]= " ----------\'£$:?()><=+-*/;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void ConvertToAscii(BYTE *from, char *to)
{
        BYTE c;
        int len=0;

        do
        {
                c=*(from++);

                if (c<64) *(to++) = CharTable[c];
                else if (c>=128 && c<192)
                {
                        *(to++) = CharTable[c-128];
                        *to = '\0';
                        return;
                }
                else *(to++) = '-';

                len++;
        } while (len<16);
        *to='\0';
}

void save_tape_buffer(int ptr)
{
        char name[32];
        int len;

        if (ptr>32768) return;

        ConvertToAscii(memory+ptr,name);
        if (!strlen(name)) strcpy(name,"Unnamed File");
        len = (readbyte(16404) + readbyte(16405)*256) - 16392;
        if (len>65535) return;

        tape_new_block(name, len, memory+16393);
}

void load_tape_buffer(int ptr)
{
        char name[32];
        BYTE *file=NULL;
        int len;

        if (ptr>32768)  len=tape_get_pointer(NULL,&file); // LOAD ""
        else
        {
                ConvertToAscii(memory+ptr,name);
                len=tape_get_pointer(name, &file);
        }

        if (len) memcpy(memory+16393, file, len);
        else z80.pc.w = 0x03a6;
}


//int load_file(char *filename)
//{
//        int fptr;
//
//        fptr=open(filename, O_RDONLY | O_BINARY);
//        if (fptr<1) return(errno);
//
//        if ((tape_buffer_len=read(fptr, tape_buffer, 65536))==-1)
//        {
//                int err;
//
//                err=errno;
//                close(fptr);
//                return(err);
//        }
//
//        close(fptr);
//
//        return(0);
//}


char *get_token(FILE *f)
{
        static char buffer[256];
        int buflen;
        char c;

        c=fgetc(f);
        while(isspace(c) && !feof(f)) c=fgetc(f);

        //if (feof(f)) return(NULL);

        buflen=0;
        buffer[buflen++]=c;

        c=fgetc(f);
        while(!isspace(c) && !feof(f) && buflen<255)
        {
                buffer[buflen++]=c;
                c=fgetc(f);
        }

        buffer[buflen]='\0';

        //if (!buflen) return(NULL);
        return(buffer);
}

int hex2dec(char *str)
{
        int num;

        num=0;
        while(*str)
        {
                num=num*16;
                if (*str>='0' && *str<='9') num += *str - '0';
                else if (*str>='a' && *str<='f') num += *str +10 - 'a';
                else if (*str>='A' && *str<='F') num += *str +10 - 'A';
                else return(num);
                str++;
        }
        return(num);
}

void load_snap_cpu(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (!strcmp(tok,"[MEMORY]"))
                {
                        load_snap_mem(f);
                        return;
                }

                if (!strcmp(tok,"PC")) z80.pc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"SP")) z80.sp.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL")) z80.hl.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE")) z80.de.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC")) z80.bc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF")) z80.af.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL_")) z80.hl_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE_")) z80.de_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC_")) z80.bc_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF_")) z80.af_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IX")) z80.ix.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IY")) z80.iy.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IM")) z80.im = hex2dec(get_token(f));
                if (!strcmp(tok,"IF1")) z80.iff1 = hex2dec(get_token(f));
                if (!strcmp(tok,"IF2")) z80.iff2 = hex2dec(get_token(f));
                if (!strcmp(tok,"HT")) z80.halted = hex2dec(get_token(f));
                if (!strcmp(tok,"IR"))
                {
                        int a;

                        a=hex2dec(get_token(f));

                        z80.i = (a>>8) & 255;
                        z80.r = a & 255;
                        z80.r7 = a & 128;
                }
        }
}

void load_snap_mem(FILE *f)
{
        int Addr, Count, Chr;
        char *tok;

        Addr=16384;

        while(!feof(f))
        {
                tok=get_token(f);

                if (!strcmp(tok,"[CPU]"))
                {
                        load_snap_cpu(f);
                        return;
                }
                else if (!strcmp(tok,"MEMRANGE"))
                {
                        Addr=hex2dec(get_token(f));
                        get_token(f);
                }
                else if (*tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=Chr;
                }
                else memory[Addr++]=hex2dec(tok);
        }
}


int load_snap(char *filename)
{
        FILE *f;

        f=fopen(filename,"rt");
        if (!f) return(0);

        while(!feof(f))
        {
                if (!strcmp(get_token(f),"[CPU]")) load_snap_cpu(f);
                if (!strcmp(get_token(f),"[MEMORY]")) load_snap_mem(f);
        }

        fclose(f);
        DebugUpdate();
}



int save_snap(char *filename)
{
        FILE *f;
        int Addr, Count, Chr;

        f=fopen(filename,"wt");
        if (!f) return(1);

        fprintf(f,"[CPU]\n");
        fprintf(f,"PC %04X    SP  %04X\n", z80.pc.w,z80.sp.w);
        fprintf(f,"HL %04X    HL_ %04X\n", z80.hl.w,z80.hl_.w);
        fprintf(f,"DE %04X    DE_ %04X\n", z80.de.w,z80.de_.w);
        fprintf(f,"BC %04X    BC_ %04X\n", z80.bc.w,z80.bc_.w);
        fprintf(f,"AF %04X    AF_ %04X\n", z80.af.w,z80.af_.w);
        fprintf(f,"IX %04X    IY  %04X\n", z80.ix.w,z80.iy.w);
        fprintf(f,"IR %04X\n", (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));

        fprintf(f,"IM %02X      IF1 %02X\n", z80.im, z80.iff1);
        fprintf(f,"HT %02X      IF2 %02X\n", z80.halted, z80.iff2);

        fprintf(f,"\n[MEMORY]\n");

        fprintf(f,"MEMRANGE %04X %04X\n", zx81.ROMTOP+1, zx81.RAMTOP);

        Addr=zx81.ROMTOP+1;

        while(Addr<=zx81.RAMTOP)
        {
                Chr=memory[Addr];
                Count=1;

                while((memory[Addr+Count]==Chr) && ((Addr+Count)<=zx81.RAMTOP))
                        Count++;

                if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
                else fprintf(f,"%02X ",Chr);

                Addr += Count;
        }
        fprintf(f,"\n\n[EOF]\n");
        fclose(f);
        return(0);
}



/*
int load_snap(char *filename)
{
        int fptr;

        fptr=open(filename, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        read(fptr, &z80, sizeof(z80));

        if (read(fptr, memory, 65536)==-1)
        {
                int err;

                err=errno;
                close(fptr);
                return(err);
        }

        close(fptr);

        return(0);
}


int save_snap(char *filename)
{
        int fptr;

        fptr=open(filename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
                                S_IREAD | S_IWRITE);
        if (fptr<1) return(errno);

        write(fptr, &z80, sizeof(z80));

        if (write(fptr, memory, 65536)==-1)
        {
                int err;

                err=errno;
                close(fptr);
                return(err);
        }

        close(fptr);

        return(0);
}
*/

int memory_load(char *filename, int address, int length)
{
        int fptr;
        char file[256];

        strcpy(file, zx81.cwd);
        if (file[strlen(file)-1]!='\\') strcat(file,"\\");
        strcat(file,filename);

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        if (read(fptr, memory+address, length)==-1)
        {
                int err;

                err=errno;
                close(fptr);
                return(err);
        }

        close(fptr);

        return(0);
}

