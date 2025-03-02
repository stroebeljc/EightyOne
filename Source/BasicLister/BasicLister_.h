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

//---------------------------------------------------------------------------

#ifndef BasicLister_H
#define BasicLister_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "IBasicLister.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <IniFiles.hpp>

#include <string>
#include <vector>

//---------------------------------------------------------------------------
class TBasicLister : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBar;
        TStatusBar *StatusBar;
        TToolBar *ToolBar;
        TToolButton *ToolButtonRefresh;
        TToolButton *ToolButton1;
        TToolButton *ToolButtonSettings;
        TToolButton *ToolButton3;
        TToolButton *ToolButtonSave;
        TToolButton *ToolButton4;
        TSaveDialog *SaveDialog;
        TToolButton *ToolButtonLineEnds;
        TToolButton *ToolButton5;
        TToolButton *ToolButtonInfo;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ScrollBarChange(TObject *Sender);
        void __fastcall ToolButtonRefreshClick(TObject *Sender);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ToolButtonSaveClick(TObject *Sender);
        void __fastcall ToolButtonLineEndsClick(TObject *Sender);
        void __fastcall ToolButtonSettingsClick(TObject *Sender);
        void __fastcall ToolButtonInfoClick(TObject *Sender);

private:	// User declarations
        static const int DisplayableRows = 48;
        static const int DisplayableColumns = 32;

        HBITMAP mBitmap;
        HWND mHWND;
        int mLastHighlightedEntryIndex;
        int mLastFilterIndex;
        IBasicLister* mBasicLister;
        int mBMWidth;
        int mBMHeight;
        int mProgramDisplayRows;
        std::vector<LineInfo>* mLines;
        bool mOutputRemTokensAsCharacterCodes;
        bool mOutputStringTokensAsCharacterCodes;
        bool mOutputNonAsciiAsCharacterCodes;
        bool mOutputVariableNamesInLowercase;
        bool mLimitLineLengths;
        bool mOutputFullWidthLineNumbers;
        int mScaling;

        void ClearBitmap();
        void ConstructBitmap();
        bool ExtractLine(int* basicPos, int* displayLength);
        void ExtractProgramDetails();
        void LoadProgram();
        int ProgramSize();
        void UnhighlightRows(int startRow, int endRow);
        void HighlightRows(int startRow, int endRow);
        void HighlightLine(int lineNumber);
        void HighlightEntry(int index);
        void UnhighlightEntry(int index);
        void ColourRows(int startRow, int endRow, bool highlight);
        void DisableButtons();
        void EnableButtons();
        void SaveListingToFile();
        void ConfigureScrollBar();
        void ConfigureStatusBar();
        int FindLineIndex(int lineNumber);
        int FindLineDisplayedOnRow(int row);
        COLORREF GetHighlightColour();
        void GetSaveOptions();
        void SizeWindow();

public:		// User declarations
        __fastcall TBasicLister(TComponent* Owner);
        virtual __fastcall ~TBasicLister();
        void SetBasicLister(IBasicLister* basicLister);
        void SaveSettings(TIniFile* ini);
        void LoadSettings(TIniFile* ini);
        bool ListerAvailable();
        void Refresh(bool keepScrollbarPosition);
        void Clear();
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicLister *BasicLister;
//---------------------------------------------------------------------------
#endif
