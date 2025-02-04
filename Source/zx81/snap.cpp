/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
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
 *
 * snap.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <vcl4.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <sys\stat.h>
#include <ctype.h>
#include <string.h>

#include "snap.h"
#include "zx81.h"
#include "zx81config.h"
#include "z80\z80.h"
#include "chroma\chroma.h"
#include "HW_.h"
#include "main_.h"
#include "Artifacts_.h"

extern int lineCounter;
extern int MemotechMode;
extern int QuicksilvaHiResMode;
extern BYTE font[1024];

int save_snap_zx81(char* filename);
int save_snap_ace(char* filename);
void load_snap_cpu(FILE *f);
void load_snap_mem(FILE *f);
void load_snap_zx81(FILE *f);
void load_snap_machine(FILE *f);
void load_snap_sound(FILE *f);
void load_snap_speech(FILE *f);
void load_snap_chrgen(FILE *f);
void load_snap_hires(FILE *f);
void load_snap_colour(FILE *f);
void load_snap_romcartridge(FILE *f);
void load_snap_interfaces(FILE *f);
void load_snap_advanced(FILE* f);
void load_snap_drives(FILE* f);
void ProcessTag(char* tok, FILE* f);
//void InitialiseHardware();

extern void HWSetMachine(int machine, int speccy);
extern void DebugUpdate();

AnsiString ReplaceSpaces(AnsiString text)
{
        return StringReplace(text.c_str(), " ", "_", TReplaceFlags() << rfReplaceAll);
}

char *get_token(FILE *f)
{
        static char buffer[256];
        int buflen;
        char c;

        c=(char)fgetc(f);
        while(isspace(c) && !feof(f)) c=(char)fgetc(f);

        buflen=0;
        buffer[buflen++]=c;

        c=(char)fgetc(f);
        while(!isspace(c) && !feof(f) && buflen<255)
        {
                buffer[buflen++]=c;
                c=(char)fgetc(f);
        }

        buffer[buflen]='\0';

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

void SetComboBox(TComboBox* comboBox, char* text)
{
        AnsiString newText = StringReplace(text, "_", " ", TReplaceFlags() << rfReplaceAll);

        for (int i = 0; i < comboBox->Items->Count; i++)
        {
                if (comboBox->Items->Strings[i] == newText)
                {
                        comboBox->ItemIndex = i;
                        break;
                }
        }
}

void load_snap_cpu(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"PC")) z80.pc.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"SP")) z80.sp.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"HL")) z80.hl.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"DE")) z80.de.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"BC")) z80.bc.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"AF")) z80.af.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"HL_")) z80.hl_.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"DE_")) z80.de_.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"BC_")) z80.bc_.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"AF_")) z80.af_.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"IX")) z80.ix.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"IY")) z80.iy.w = (WORD)hex2dec(get_token(f));
                if (!strcmp(tok,"IM")) z80.im = (BYTE)hex2dec(get_token(f));
                if (!strcmp(tok,"IF1")) z80.iff1 = (BYTE)hex2dec(get_token(f));
                if (!strcmp(tok,"IF2")) z80.iff2 = (BYTE)hex2dec(get_token(f));
                if (!strcmp(tok,"HT")) z80.halted = hex2dec(get_token(f));
                if (!strcmp(tok,"IR"))
                {
                        int a;

                        a=hex2dec(get_token(f));

                        z80.i = (BYTE)((a>>8) & 255);
                        z80.r = (WORD)(a & 255);
                        z80.r7 = (BYTE)(a & 128);
                }
        }
}

void load_snap_sound(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->SoundCardBox, tok);
                }
        }
}

void load_snap_speech(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->SpeechBox, tok);
                }
        }
}

void load_snap_chrgen(FILE *f)
{
        int Addr, Count, Chr;

        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ChrGenBox, tok);

                        if (HW->ChrGenBox->Text == "Quicksilva")
                        {
                                Addr = 0x8400;
                                Form1->QSChrEnable->Visible = true;
                                Form1->QSChrEnable->Enabled = true;
                        }
                }
                else if ((HW->ChrGenBox->Text == "Quicksilva") && !strcmp(tok,"ENABLED"))
                {
                        zx81.enableQSchrgen = (CFGBYTE)hex2dec(get_token(f));
                        Form1->QSChrEnable->Checked = zx81.enableQSchrgen;
                }
                else if ((HW->ChrGenBox->Text == "Quicksilva") && (*tok=='*'))
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));

                        while(Count--)
                        {
                                font[Addr-0x8400] = (BYTE)Chr;

                                if (machine.colour == COLOURCHROMA)
                                {
                                        memory[Addr] = (BYTE)Chr;
                                        memory[Addr + 0x4000] = (BYTE)Chr;
                                        memory[Addr - 0x6000] = (BYTE)Chr;
                                }

                                Addr++;
                        }
                }
                else
                {
                        Chr=hex2dec(tok);
                        font[Addr-0x8400]=(BYTE)Chr;

                        if (machine.colour == COLOURCHROMA)
                        {
                                memory[Addr] = (BYTE)Chr;
                                memory[Addr + 0x4000] = (BYTE)Chr;
                                memory[Addr - 0x6000] = (BYTE)Chr;
                        }
                        Addr++;
                }
        }
}

void load_snap_hires(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->HiResBox, tok);
                        
                        if (!strcmp(tok,"G007")) HW->EnableLowRAM->Checked=true;
                        if (!strcmp(tok,"Memotech")) HW->ProtectROM->Checked=true;
                }
        }
}

void load_snap_romcartridge(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->RomCartridgeBox, tok);
                }
                else if (!strcmp(tok,"PATH"))
                {
                        tok = get_token(f);
                        AnsiString path = StringReplace(tok, "_", " ", TReplaceFlags() << rfReplaceAll);
                        HW->RomCartridgeFileBox->Text = path;
                }
                else if (!strcmp(tok,"CONFIGURATION"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ZXC1ConfigurationBox, tok);
                }
        }
}

void load_snap_interfaces(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"ZXPAND"))
                {
                        HW->SetZXpandState(hex2dec(get_token(f)),true);
                }
                else if (!strcmp(tok,"ZX_PRINTER"))
                {
                        HW->ZXPrinter->Checked = hex2dec(get_token(f));
                }
        }
}

void load_snap_colour(FILE *f)
{
        int Addr, Count, Chr;
        char *tok;
        bool chroma = false;

        Addr=0xC000;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ColourBox, tok);
                        chroma = (HW->ColourBox->Text == "Chroma");
                        bool lambda = (HW->ColourBox->Text == "Lambda");
                        if (chroma)
                        {
                                Form1->ChromaColourEnable->Visible = true;
                                Form1->ChromaColourEnable->Enabled = true;
                                machine.colour = COLOURCHROMA;
                        }
                        else if (lambda)
                        {
                                machine.colour = COLOURLAMBDA;
                        }
                }
                else if (chroma && !strcmp(tok,"CHROMA_MODE"))
                {
                        zx81.chromaMode = (CFGBYTE)hex2dec(get_token(f));
                }
                else if (chroma && !strcmp(tok,"COLOUR_ENABLED"))
                {
                        zx81.chromaColourSwitchOn = (CFGBYTE)hex2dec(get_token(f));
                        Form1->ChromaColourEnable->Checked = zx81.chromaColourSwitchOn;
                }
                else if (chroma && *tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=(BYTE)Chr;
                }
                else if (chroma)
                {
                        memory[Addr++]=(BYTE)hex2dec(tok);
                }
        }
}

AnsiString GetMachine()
{
        AnsiString machineName;

        switch(emulator.machine)
        {
                case MACHINEZX80:
                        machineName = "ZX80";
                        break;
                case MACHINEZX81:
                        machineName = "ZX81";
                        break;
                case MACHINEACE:
                        machineName = "ACE";
                        break;
                case MACHINETS1000:
                        machineName = "1000";
                        break;
                case MACHINETS1500:
                        machineName = "1500";
                        break;
                case MACHINELAMBDA:
                        machineName = "LAMDA";
                        break;
                case MACHINEZX97LE:
                        machineName = "ZX97LE";
                        break;
                case MACHINER470:
                        machineName = "R470";
                        break;
                case MACHINETK85:
                        machineName = "TK85";
                        break;
                default:
                        machineName = "ZX81";
                        break;
        }

        return machineName;
}

void SetMachine(AnsiString machine)
{
        if (machine == "ZX80") HWSetMachine(MACHINEZX80, NULL);
        else if (machine == "ZX81") HWSetMachine(MACHINEZX81, NULL);
        else if (machine == "1500") HWSetMachine(MACHINETS1500, NULL);
        else if (machine == "LAMDA") HWSetMachine(MACHINELAMBDA, NULL);
        else if (machine == "ZX97LE") HWSetMachine(MACHINEZX97LE, NULL);
        else if (machine == "R470") HWSetMachine(MACHINER470, NULL);
        else if (machine == "TK85") HWSetMachine(MACHINETK85, NULL);
        else if (machine == "1000") HWSetMachine(MACHINETS1000, NULL);
}

void load_snap_machine(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"MODEL")) SetMachine(get_token(f));
        }
}

void load_snap_zx81(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"NMI")) nmiGeneratorEnabled = hex2dec(get_token(f));
                else if (!strcmp(tok,"SYNC")) syncOutputWhite = hex2dec(get_token(f));
                else if (!strcmp(tok,"LINE")) lineCounter = hex2dec(get_token(f));
                //Backwards compatibility
                else if (!strcmp(tok,"HSYNC")) syncOutputWhite = hex2dec(get_token(f));
                else if (!strcmp(tok,"ROW")) lineCounter = hex2dec(get_token(f));
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
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"RAM_PACK"))
                {
                        tok = get_token(f);

                        SetComboBox(HW->RamPackBox, tok);
                }
                else if (!strcmp(tok,"MEMRANGE"))
                {
                        Addr=hex2dec(get_token(f));
                        get_token(f);
                }
                else if (!strcmp(tok,"8K_RAM_ENABLED"))
                {
                        HW->EnableLowRAM->Checked=hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"ROM_PROTECTED"))
                {
                        Form1->WriteProtect8KRAM->Checked = hex2dec(get_token(f));
                }
                else if (*tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=(BYTE)Chr;
                }
                else
                {
                        memory[Addr++]=(BYTE)hex2dec(tok);
                }
        }
}

void load_snap_advanced(FILE* f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"ROM"))
                {
                        tok = get_token(f);
                        AnsiString rom = StringReplace(tok, "_", " ", TReplaceFlags() << rfReplaceAll);
                        HW->RomBox->Text = rom;
                }
                else if (!strcmp(tok,"PROTECT_ROM"))
                {
                        HW->ProtectROM->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"M1NOT"))
                {
                        HW->M1Not->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"IMPROVED_WAIT"))
                {
                        HW->ImprovedWait->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"FLOATING_POINT_FIX"))
                {
                        HW->FloatingPointHardwareFix->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"FRAME_RATE_60HZ"))
                {
                        HW->NTSC->Checked = hex2dec(get_token(f));
                }
        }
}

void load_snap_drives(FILE* f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"FDC"))
                {
                        SetComboBox(HW->FDC, get_token(f));
                }
                else if (!strcmp(tok,"IDE"))
                {
                        SetComboBox(HW->IDEBox, get_token(f));
                }
        }
}

void ProcessTag(char* tok, FILE* f)
{             
        if (!strcmp(tok, "[CPU]")) load_snap_cpu(f);
        else if (!strcmp(tok, "[MACHINE]")) load_snap_machine(f);
        else if (!strcmp(tok, "[MEMORY]")) load_snap_mem(f);
        else if (!strcmp(tok, "[ZX81]")) load_snap_zx81(f);
        else if (!strcmp(tok, "[COLOUR]")) load_snap_colour(f);
        else if (!strcmp(tok, "[SOUND]")) load_snap_sound(f);
        else if (!strcmp(tok, "[SPEECH]")) load_snap_speech(f);
        else if (!strcmp(tok, "[CHR$_GENERATOR]")) load_snap_chrgen(f);
        else if (!strcmp(tok, "[HIGH_RESOLUTION]")) load_snap_hires(f);
        else if (!strcmp(tok, "[ROM_CARTRIDGE]")) load_snap_romcartridge(f);
        else if (!strcmp(tok, "[INTERFACES]")) load_snap_interfaces(f);
        else if (!strcmp(tok, "[ADVANCED]")) load_snap_advanced(f);
        else if (!strcmp(tok, "[DRIVES]")) load_snap_drives(f);
}

void load_snap_ace(FILE *f)
{
        int memptr=0x2000;
        unsigned char c;
        int len, eof;

        eof=0;

        while(!eof)
        {
                c=(unsigned char)fgetc(f);

                if (c!=0xED) memory[memptr++]=c;
                else
                {
                        len=fgetc(f);

                        if (!len) eof=1;
                        else
                        {
                                c=(unsigned char)fgetc(f);
                                while(len--) memory[memptr++]=c;
                        }
                }

                if (feof(f)) eof=1;
        }

        int ramTop = memory[0x2081]*256;
        int ramSize = (ramTop - 0x3400) >> 10;
        int ramPackSize = ramSize - machine.baseRamSize;

        zx81.RAMTOP = ramTop-1;
        if (zx81.RAMTOP == -1) zx81.RAMTOP=65535;

        AnsiString ramPackText = IntToStr(ramPackSize) + "K";
        SetComboBox(HW->RamPackBox, ramPackText.c_str());

        memptr=0x2100;

        z80.af.b.l = memory[memptr]; z80.af.b.h = memory[memptr+1]; memptr+=4;
        z80.bc.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de.b.l = memory[memptr]; z80.de.b.h = memory[memptr+1]; memptr+=4;
        z80.hl.b.l = memory[memptr]; z80.hl.b.h = memory[memptr+1]; memptr+=4;
        z80.ix.b.l = memory[memptr]; z80.ix.b.h = memory[memptr+1]; memptr+=4;
        z80.iy.b.l = memory[memptr]; z80.iy.b.h = memory[memptr+1]; memptr+=4;
        z80.sp.b.l = memory[memptr]; z80.sp.b.h = memory[memptr+1]; memptr+=4;
        z80.pc.b.l = memory[memptr]; z80.pc.b.h = memory[memptr+1]; memptr+=4;
        z80.af_.b.l = memory[memptr]; z80.af_.b.h = memory[memptr+1]; memptr+=4;
        z80.bc_.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de_.b.l = memory[memptr]; z80.de_.b.h = memory[memptr+1]; memptr+=4;
        z80.hl_.b.l = memory[memptr]; z80.hl_.b.h = memory[memptr+1]; memptr+=4;

        z80.im = memory[memptr]; memptr+=4;
        z80.iff1 = memory[memptr]; memptr+=4;
        z80.iff2 = memory[memptr]; memptr+=4;
        z80.i = memory[memptr]; memptr+=4;
        z80.r = memory[memptr];
}

int do_load_snap(char *filename, bool resetHardware)
{
        char *p;
        FILE *f;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);

        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f=fopen(filename,"rb");
                if (!f)
                {
                        ShowMessage("Snapshot load failed.");
                        return 0;
                }

                if (resetHardware)
                {
                        HWSetMachine(MACHINEACE, NULL);
                        machine.initialise();
                }
                load_snap_ace(f);
        }
        else
        {
                if (resetHardware) machine.initialise();

                f=fopen(filename,"rt");
                if (!f) return(0);

                while(!feof(f))
                {
                        char* tok = get_token(f);
                        if (tok[0] == '[')
                        {
                                ProcessTag(tok, f);
                        }
                 }
        }

        fclose(f);
        DebugUpdate();
        return(1);
}

int load_snap(char *filename)
{
        // Read in the snapshot into memory and settings (this also resets the computer)
        int ret = do_load_snap(filename, true);
        if (!ret) return ret;

        // Re-apply the settings without a reset
        bool disableResetStatus = true;
        HW->UpdateHardwareSettings(disableResetStatus);

        return true;
}

int save_snap(char *filename)
{
        char *p;
        int f;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);

        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f = save_snap_ace(filename);
        }
        else
        {
                f = save_snap_zx81(filename);
        }

        if (f)
        {
                ShowMessage("Save snapshot failed.");
        }

        return(0);
}

int save_snap_zx81(char *filename)
{
        FILE *f;
        int Addr, Count, Chr;

	f=fopen(filename,"wt");
	if (!f)
	{
	        return -1;
	}

	fprintf(f,"[MACHINE]\n");
	fprintf(f,"MODEL %s\n", GetMachine().c_str());

	fprintf(f,"\n[CPU]\n");
	fprintf(f,"PC %04X    SP  %04X\n", z80.pc.w,z80.sp.w);
	fprintf(f,"HL %04X    HL_ %04X\n", z80.hl.w,z80.hl_.w);
	fprintf(f,"DE %04X    DE_ %04X\n", z80.de.w,z80.de_.w);
	fprintf(f,"BC %04X    BC_ %04X\n", z80.bc.w,z80.bc_.w);
	fprintf(f,"AF %04X    AF_ %04X\n", z80.af.w,z80.af_.w);
	fprintf(f,"IX %04X    IY  %04X\n", z80.ix.w,z80.iy.w);
	fprintf(f,"IR %04X\n", (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));

	fprintf(f,"IM %02X      IF1 %02X\n", z80.im, z80.iff1);
	fprintf(f,"HT %02X      IF2 %02X\n", z80.halted, z80.iff2);

	fprintf(f,"\n[ZX81]\n");
	fprintf(f,"NMI %02X     SYNC %02X\n", nmiGeneratorEnabled, syncOutputWhite);
	fprintf(f,"LINE %03X\n", lineCounter);

	fprintf(f,"\n[MEMORY]\n");
	fprintf(f,"RAM_PACK %S\n", HW->RamPackBox->Text.c_str());
	fprintf(f,"8K_RAM_ENABLED %02X\n", zx81.RAM816k);
	fprintf(f,"ROM_PROTECTED %02X\n", zx81.RAM816kWriteProtected);

	Addr = zx81.RAM816k || zx81.zxpand ? 8192 : zx81.ROMTOP+1;
	int topOfRAM = zx81.RAMTOP;
	if (zx81.zxpand && (topOfRAM < 0xBFFF))
	{
	        topOfRAM = 0xBFFF;
	}
	fprintf(f,"MEMRANGE %04X %04X\n", Addr, topOfRAM);

	while(Addr <= topOfRAM)
	{
		Chr=memory[Addr];
		Count=1;

		while((memory[Addr+Count]==Chr) && ((Addr+Count)<=topOfRAM))
		        Count++;

                if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
		else fprintf(f,"%02X ",Chr);

		Addr += Count;
	}
	fprintf(f, "\n");

	fprintf(f,"\n[ADVANCED]\n");
	fprintf(f,"ROM %s\n", ReplaceSpaces(HW->RomBox->Text).c_str());
	fprintf(f,"PROTECT_ROM %02X\n", machine.protectROM);
	fprintf(f,"M1NOT %02X\n", (zx81.m1not == 0xC000));
	fprintf(f,"IMPROVED_WAIT %02X\n", zx81.improvedWait);
	fprintf(f,"FLOATING_POINT_FIX %02X\n", zx81.FloatingPointHardwareFix);
	fprintf(f,"FRAME_RATE_60HZ %02X\n", machine.NTSC);

	fprintf(f,"\n[SOUND]\n");
	fprintf(f,"TYPE %s\n", ReplaceSpaces(HW->SoundCardBox->Text).c_str());

	fprintf(f,"\n[SPEECH]\n");
	fprintf(f,"TYPE %s\n", ReplaceSpaces(HW->SpeechBox->Text).c_str());

	// Must output this before the Chr$ Generator section
	fprintf(f,"\n[COLOUR]\n");
	fprintf(f,"TYPE %S\n", HW->ColourBox->Text.c_str());

	if (machine.colour == COLOURCHROMA)
	{
                Addr = 0xC000;

		while (Addr <= 0xFFFF)
		{
		        Chr=memory[Addr];
			Count=1;

                        while((memory[Addr+Count]==Chr) && ((Addr+Count)<=0xFFFF))
			        Count++;

                        if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
			else fprintf(f,"%02X ",Chr);

                        Addr += Count;
                }

		fprintf(f,"\nCHROMA_MODE %02X\n", zx81.chromaMode);
		fprintf(f,"COLOUR_ENABLED %02X\n", zx81.chromaColourSwitchOn);
	}

	fprintf(f,"\n[CHR$_GENERATOR]\n");
	fprintf(f,"TYPE %S\n", HW->ChrGenBox->Text.c_str());

	if (zx81.chrgen == CHRGENQS)
	{
		fprintf(f,"ENABLED %02X\n", zx81.enableQSchrgen);

		Addr = 0x8400;

                while (Addr <= 0x8800)
		{
                        Chr=font[Addr-0x8400];
			Count=1;

			while((font[Addr-0x8400+Count]==Chr) && ((Addr+Count)<=0x8800))
			        Count++;

			if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
			else fprintf(f,"%02X ",Chr);

		        Addr += Count;
		}
		fprintf(f, "\n");
	}

	fprintf(f,"\n[HIGH_RESOLUTION]\n");
	fprintf(f,"TYPE %S\n", HW->HiResBox->Text.c_str());

	fprintf(f,"\n[ROM_CARTRIDGE]\n");
        AnsiString type = StringReplace(HW->RomCartridgeBox->Text, " ", "_", TReplaceFlags() << rfReplaceAll);
	fprintf(f,"TYPE %s\n", type.c_str());
	if (HW->RomCartridgeBox->Text != "None")
	{
	        AnsiString path = StringReplace(HW->RomCartridgeFileBox->Text, " ", "_", TReplaceFlags() << rfReplaceAll);
		fprintf(f,"PATH %s\n", path.c_str());
		if (HW->RomCartridgeBox->Text == "ZXC1")
		{
		        fprintf(f,"CONFIGURATION %S\n", HW->ZXC1ConfigurationBox->Text.c_str());
		}
	}

	fprintf(f,"\n[INTERFACES]\n");
	fprintf(f,"ZX_PRINTER %02X\n", machine.zxprinter);
	fprintf(f,"ZXPAND %02X\n", zx81.zxpand);

	fprintf(f,"\n[DRIVES]\n");
	fprintf(f,"FDC %S\n", HW->FDC->Text.c_str());
	fprintf(f,"IDE %S\n", HW->IDEBox->Text.c_str());

        fprintf(f,"\n[EOF]\n");
	fclose(f);

        return 0;
}

int save_snap_ace(char *filename)
{
	FILE *f;
        int Addr, Count, Chr, memptr;

	f=fopen(filename,"wb");
	if (!f)
	{
	        ShowMessage("Save snapshot failed.");
		return -1;
	}

	memptr=0x2000;
	memory[memptr]=0x01; memory[memptr+1]=0x80;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x2080;
	memory[memptr]=0x00; memory[memptr+1]=(BYTE)((zx81.RAMTOP+1)/256);
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0284;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0288;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x028c;
	memory[memptr]=0x03; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0290;
	memory[memptr]=0x03; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0294;
	memory[memptr]=0xfd; memory[memptr+1]=0xfd;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr+=0x0298;
	memory[memptr]=0x01; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr+=0x029c;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x2100;

	memory[memptr] = z80.af.b.l; memory[memptr+1] = z80.af.b.h; memptr+=4;
	memory[memptr] = z80.bc.b.l; memory[memptr+1] = z80.bc.b.h; memptr+=4;
	memory[memptr] = z80.de.b.l; memory[memptr+1] = z80.de.b.h; memptr+=4;
	memory[memptr] = z80.hl.b.l; memory[memptr+1] = z80.hl.b.h; memptr+=4;
	memory[memptr] = z80.ix.b.l; memory[memptr+1] = z80.ix.b.h; memptr+=4;
	memory[memptr] = z80.iy.b.l; memory[memptr+1] = z80.iy.b.h; memptr+=4;
	memory[memptr] = z80.sp.b.l; memory[memptr+1] = z80.sp.b.h; memptr+=4;
	memory[memptr] = z80.pc.b.l; memory[memptr+1] = z80.pc.b.h; memptr+=4;
	memory[memptr] = z80.af_.b.l; memory[memptr+1] = z80.af_.b.h; memptr+=4;
	memory[memptr] = z80.bc_.b.l; memory[memptr+1] = z80.bc.b.h ; memptr+=4;
	memory[memptr] = z80.de_.b.l; memory[memptr+1] = z80.de_.b.h; memptr+=4;
	memory[memptr] = z80.hl_.b.l; memory[memptr+1] = z80.hl_.b.h; memptr+=4;

	memory[memptr] = z80.im ; memptr+=4;
	memory[memptr] = z80.iff1; memptr+=4;
	memory[memptr] = z80.iff2; memptr+=4;
	memory[memptr] = z80.i; memptr+=4;
	memory[memptr] = z80.r; memptr+=4;
	memory[memptr] = 0x80; memory[memptr+1] = 0x00; memory[memptr+2] = 0x00; memory[memptr+3] = 0x00;

	Addr=0x2000;

	while(Addr <= zx81.RAMTOP)
	{
	        Chr=memory[Addr];
		Count=1;

		while((memory[Addr+Count] == Chr) && ((Addr+Count) <= zx81.RAMTOP))
			Count++;

		if (Count>240) Count=240;

		if (Count>3 || Chr==0xed)
		{
			fputc(0xed,f);
		        fputc(Count,f);
		}
		else    Count=1;

		fputc(Chr,f);
		Addr+=Count;
	}

	fputc(0xed,f);
	fputc(0x00,f);

        fclose(f);
        
	return 0;
}

int memoryLoadToAddress(char *filename, void* destAddress, int length)
{
        int fptr;
        char file[256];
        int len;

        if (strchr(filename, '\\') || strchr(filename, '/'))
        {
                strcpy(file, filename);
        }
        else
        {
                strcpy(file, emulator.cwd);
                strcat(file, romsFolder);
                strcat(file, filename);
        }

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<0)
        {
                int err=errno;
                AnsiString errMsg = "ROM load to address failed:\n" + AnsiString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        if ((len=read(fptr, destAddress, length))==-1)
        {
                int err=errno;
                close(fptr);
                AnsiString errMsg = "ROM load to address failed:\n" + AnsiString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        close(fptr);

        return(len);
}

int do_memory_load(char *file, int address, int length, int secondBank)
{
        int fptr;
        int len;

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<0)
        {
                int err=errno;
                AnsiString errMsg = "ROM load failed:\n" + AnsiString(file);
                ShowMessage(errMsg);
                return(err);
        }

        if ((len=read(fptr, memory+address, length))==-1)
        {
                int err=errno;
                close(fptr);
                AnsiString errMsg = "ROM load failed:\n" + AnsiString(file);
                ShowMessage(errMsg);
                return(err);
        }

        if (secondBank)
        {
                if ((len=read(fptr, memory+address, length))==-1)
                {
                        int err=errno;
                        close(fptr);
                        AnsiString errMsg = "ROM load failed:\n" + AnsiString(file);
                        ShowMessage(errMsg);
                        return(err);
                }
        }

        close(fptr);

        return(len);
}

int memory_device_rom_load(char *filename, int address, int length)
{
        char file[256];

        strcpy(file, emulator.cwd);
        strcat(file, filename);

        return do_memory_load(file, address, length, 0);
}

int memory_load(char *filename, int address, int length, int secondBank)
{
        char file[256];

        if (strchr(filename, '\\') || strchr(filename, '/'))
        {
                strcpy(file, filename);
        }
        else
        {
                strcpy(file, emulator.cwd);
                strcat(file, romsFolder);
                strcat(file, filename);
        }

        return do_memory_load(file, address, length, secondBank);
}

int font_load(const char *filename, char *address, int length)
{
        int fptr;
        char file[256];
        int len;

        strcpy(file, emulator.cwd);
        strcat(file, romsFolder);
        strcat(file, filename);

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<0) return(errno);

        if ((len=read(fptr, address, length))==-1)
        {
                int err=errno;
                close(fptr);
                AnsiString errMsg = "Font load failed:\n" + AnsiString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        close(fptr);

        return(len);
}

