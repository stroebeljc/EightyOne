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

#ifndef ParallelH
#define ParallelH

#ifdef __cplusplus
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "CPort.hpp"
#include <Dialogs.hpp>
#if __CODEGEARC__ >= 0x0620
#include <System.Win.ScktComp.hpp>
#else
#include <ScktComp.hpp>
#endif
#include <IniFiles.hpp>
#include <stdio.h>
//---------------------------------------------------------------------------
#define PORTNOTHING     0
#define PORTFILE        1
#define PORTTCPIP       2
#define PORTRS232       3

class TParallelPort : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TComboBox *ComPortList;
        TPanel *FilePanel;
        TLabel *Label3;
        TEdit *OutputFileEdit;
        TButton *OutputFileButton;
        TPanel *TCPPanel;
        TLabel *Label9;
        TLabel *Label10;
        TEdit *TCPAddress;
        TEdit *TCPPort;
        TPanel *PortPanel;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TComboBox *BaudRate;
        TComboBox *DataBits;
        TComboBox *StopBits;
        TComboBox *Parity;
        TButton *OK;
        TComPort *ComPort;
        TClientSocket *ClientSocket;
        TSaveDialog *SaveDialog;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ComPortListChange(TObject *Sender);
private:	// User declarations
        int Port;
        FILE *OutFile;
public:		// User declarations
        __fastcall TParallelPort(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void SendData(unsigned char Data);
};
//---------------------------------------------------------------------------
extern PACKAGE TParallelPort *ParallelPort;
//---------------------------------------------------------------------------

extern "C" {
#endif

int PrinterBusy(void);
void PrinterWriteData(unsigned char Data);
void PrinterSetStrobe(unsigned char Data);
void PrinterClockTick(int ts);

#ifdef __cplusplus
}
#endif
#endif
