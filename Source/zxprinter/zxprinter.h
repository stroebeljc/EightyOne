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
 * zxprinter.h
 *
 */

//---------------------------------------------------------------------------

#ifndef zxprinterH
#define zxprinterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <IniFiles.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TZXPrinter : public TForm
{
__published:	// IDE-managed Components
        TImage *DisplayImage;
        TScrollBar *ScrollBar;
        TButton *SaveImage;
        TButton *ClearImage;
        TSaveDialog *SaveDialog;
        TButton *OptionsBtn;
        TPanel *Panel1;
        TLabel *BitRateTxt;
        TLabel *LineRateTxt;
        TTrackBar *BitRate;
        TTrackBar *LineRate;
        TLabel *Label1;
        TComboBox *Style;
        TButton *Feed;
        TTimer *FeedTimer;
        void __fastcall DrawImage(void);
        void __fastcall ScrollBarChange(TObject *Sender);
        void __fastcall ClearImageClick(TObject *Sender);
        void __fastcall SaveImageClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall OptionsBtnClick(TObject *Sender);
        void __fastcall BitRateChange(TObject *Sender);
        void __fastcall LineRateChange(TObject *Sender);
        void __fastcall StyleChange(TObject *Sender);
        void __fastcall FeedMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FeedTimerExpired(TObject *Sender);
        void __fastcall FeedMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FeedClick(TObject *Sender);
private:	// User declarations
        int StaticWidth;
        void ScrollOutput(void);
        void OutputBit(void);
        void OutputLine(void);
        void ClearBitmap(Graphics::TBitmap *Bitmap);
        Graphics::TBitmap *PrinterOutput;
        bool MotorOn, MotorSlow, StylusActive, OnPaper, EncoderWheel;
        int XPos, YPos, Counter1, Counter2;
public:		// User declarations
        __fastcall TZXPrinter(TComponent* Owner);
        void ResetPrinter(void);
        void WritePort(unsigned char Data);
        unsigned char ReadPort(BYTE idleDataBus);
        void ClockTick(int ts);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        TColor Fg,Bg;
        int BitSpeed, LineSpeed;
};
//---------------------------------------------------------------------------
extern PACKAGE TZXPrinter *ZXPrinter;
//---------------------------------------------------------------------------
#endif
