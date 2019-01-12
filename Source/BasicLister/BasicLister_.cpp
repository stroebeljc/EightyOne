/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2019 Michael D Wynne
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

#include <vcl.h>
#pragma hdrstop

#include "BasicLister_.h"
#include "zx81config.h"
#include "main_.h"
#include <iostream>
#include <fstream>
#include "BasicListerOptions_.h"
#include "BasicListingFormatInfo_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBasicLister *BasicLister;

enum StatusBarIndex
{
        PanelLines = 0,
        PanelLineInfo
};

//---------------------------------------------------------------------------
__fastcall TBasicLister::TBasicLister(TComponent* Owner)
        : TForm(Owner), mBitmap(NULL), mHWND(this->Handle),
          mLastHighlightedEntryIndex(-1), mLastFilterIndex(1), mBasicLister(NULL)
{
        mLines = new std::vector<LineInfo>();

        int displayAreaHeight = (DisplayableRows * PixelsPerCharacterHeight);
        ClientHeight = ToolBar->Height + StatusBar->Height + displayAreaHeight;

        TIniFile* ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}

__fastcall TBasicLister::~TBasicLister()
{
        ::DeleteObject(mBitmap);
        SetBasicLister(NULL);
        delete mLines;
}

void TBasicLister::SetBasicLister(IBasicLister* basicLister)
{
        mLines->clear();
        
        if (mBasicLister != NULL)
        {
                delete mBasicLister;
        }

        mBasicLister = basicLister;
        
        if (mBasicLister != NULL)
        {
                mBasicLister->PopulateKeywords();
                mBasicLister->SetLines(mLines);
        }
}

bool TBasicLister::ListerAvailable()
{
        return (mBasicLister != NULL);
}

void TBasicLister::ClearBitmap()
{
        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);

        mBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = mBMWidth;
        rect.bottom = mBMHeight;

        mBasicLister->ClearRenderedListing(hdc, mBitmap, rect, ToolButtonLineEnds->Down);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}

void TBasicLister::ExtractProgramDetails()
{
        mBasicLister->ExtractProgramDetails();
}

void TBasicLister::ConstructBitmap()
{
        if (mBitmap)
        {
            ::DeleteObject(mBitmap);
        }

        int displayRows = mBasicLister->GetProgramRows();
        int displayColumns = mBasicLister->GetDisplayColumns();

        mBMWidth = displayColumns * PixelsPerCharacterWidth;
        mBMHeight = displayRows * PixelsPerCharacterHeight;

        ClientWidth = mBMWidth + ScrollBar->Width + 1;

        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);

        mBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = mBMWidth;
        rect.bottom = mBMHeight;

        mBasicLister->RenderListing(chdc, mBitmap, rect, ToolButtonLineEnds->Down);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}

//---------------------------------------------------------------------------

void TBasicLister::UnhighlightRows(int startRow, int endRow)
{
        const bool unhighlight = false;
        ColourRows(startRow, endRow, unhighlight);
}

void TBasicLister::HighlightRows(int startRow, int endRow)
{
        const bool highlight = true;
        ColourRows(startRow, endRow, highlight);
}

void TBasicLister::ColourRows(int startRow, int endRow, bool highlight)
{
        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        COLORREF paperColour = mBasicLister->GetPaperColour();
        COLORREF highlightColour = GetHighlightColour();
        COLORREF findColour = highlight ? paperColour : highlightColour;
        COLORREF replaceColour = highlight ? highlightColour : paperColour;

        int startY = startRow * PixelsPerCharacterHeight;
        int endY = (endRow + 1) * PixelsPerCharacterHeight;

        for (int y = startY; y < endY; y++)
        {
                for (int x = 0; x < mBMWidth; x++)
                {
                        COLORREF pixelColor = GetPixel(chdc, x, y);
                        if (pixelColor == findColour)
                        {
                                SetPixelV(chdc, x, y, replaceColour);
                        }
                }
        }

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}

void TBasicLister::HighlightLine(int lineNumber)
{
        int index = FindLineIndex(lineNumber);
        HighlightEntry(index);
}

void TBasicLister::UnhighlightEntry(int index)
{
        if (index != -1)
        {
                int startRow = (*mLines)[index].startDisplayRow;
                int endRow = startRow + (*mLines)[index].displayRows - 1;
                UnhighlightRows(startRow, endRow);

                StatusBar->Panels->Items[PanelLineInfo]->Text = "";

                Invalidate();
        }

        mLastHighlightedEntryIndex = -1;
}

void TBasicLister::HighlightEntry(int index)
{
        if (index != -1)
        {
                UnhighlightEntry(mLastHighlightedEntryIndex);

                int startRow = (*mLines)[index].startDisplayRow;
                int endRow = startRow + (*mLines)[index].displayRows - 1;
                HighlightRows(startRow, endRow);
        }
        Invalidate();

        mLastHighlightedEntryIndex = index;

        AnsiString lineDetails = "";

        if (index != -1)
        {
                int lineNumber = (*mLines)[index].lineNumber;
                lineDetails += "Line " + AnsiString(lineNumber) + ": ";

                int startAddress = (*mLines)[index].address;
                lineDetails += "$" + AnsiString::IntToHex(startAddress, 4);

                int endAddress = startAddress + (*mLines)[index].lineLength - 1;
                lineDetails += "-$" + AnsiString::IntToHex(endAddress, 4);
        }

        StatusBar->Panels->Items[PanelLineInfo]->Text = lineDetails;
}

int TBasicLister::FindLineIndex(int lineNumber)
{
        int index = mLines->size() - 1;

        while (index >= 0)
        {
                if ((*mLines)[index].lineNumber == lineNumber)
                {
                        break;
                }

                index--;
        }

        return index;
}

COLORREF TBasicLister::GetHighlightColour()
{
        return RGB(255, 255, 132);
}

//---------------------------------------------------------------------------

void __fastcall TBasicLister::FormPaint(TObject *Sender)
{
        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        if (mBasicLister != NULL)
        {
                int programDisplayRows = mBasicLister->GetProgramRows();
                int programDisplayPixels = programDisplayRows * PixelsPerCharacterHeight;
                int copyHeight = (programDisplayPixels > mBMHeight) ? programDisplayPixels : mBMHeight;

                ::BitBlt(hdc, 0, ToolBar->Height - ScrollBar->Position * PixelsPerCharacterHeight, mBMWidth, copyHeight, chdc, 0, 0, SRCCOPY);
        }
        else
        {
                RECT rect;
                rect.left = 0;
                rect.top = ToolBar->Height;
                rect.right = ClientWidth;
                rect.bottom = ClientHeight - ToolBar->Height - StatusBar->Height;
                FillRect(chdc, &rect, (HBRUSH)(COLOR_BTNFACE+1));
        }

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}

//---------------------------------------------------------------------------

void __fastcall TBasicLister::FormShow(TObject *Sender)
{
        LoadProgram();
}
//---------------------------------------------------------------------------

void __fastcall TBasicLister::ScrollBarChange(TObject *Sender)
{
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TBasicLister::ToolButtonRefreshClick(TObject *Sender)
{
        ClearBitmap();
        Invalidate();

        LoadProgram();
}
//---------------------------------------------------------------------------

void TBasicLister::LoadProgram()
{
        TCursor oldCursor = Screen->Cursor;
        Screen->Cursor = crHourGlass;

        DisableButtons();

        mLastHighlightedEntryIndex = -1;
        
        if (mBasicLister != NULL)
        {
                ExtractProgramDetails();
                ConstructBitmap();
        }

        ConfigureStatusBar();
        ConfigureScrollBar();

        Invalidate();

        EnableButtons();

        Screen->Cursor = oldCursor;
}

void TBasicLister::ConfigureScrollBar()
{
        ScrollBar->Min = 0;
        mProgramDisplayRows = mBasicLister != NULL ? mBasicLister->GetProgramRows() : 0;
        bool scrollable = (mProgramDisplayRows > DisplayableRows);
        if (scrollable)
        {
                ScrollBar->Max = mProgramDisplayRows - DisplayableRows;
        }
        else
        {
                ScrollBar->Max = mProgramDisplayRows;
        }

        ScrollBar->SmallChange = 1;
        ScrollBar->LargeChange = DisplayableRows;
        ScrollBar->Position = 0;
        ScrollBar->Enabled = scrollable;
}

void TBasicLister::ConfigureStatusBar()
{
        AnsiString programDetails;

        if (mBasicLister != NULL)
        {
                programDetails += "Lines " + AnsiString(mLines->size());

                int programSize = ProgramSize();
                if (programSize > 0)
                {
                        programDetails += ": $407D-$";
                        programDetails += AnsiString::IntToHex(16509 + programSize - 1, 4);
                }
        }

        StatusBar->Panels->Items[PanelLines]->Text = programDetails;
        StatusBar->Panels->Items[PanelLineInfo]->Text = "";
}

void TBasicLister::DisableButtons()
{
        ToolButtonRefresh->Enabled = false;
        ToolButtonSettings->Enabled = false;
        ToolButtonSave->Enabled = false;
        ToolButtonLineEnds->Enabled = false;
        ToolButtonInfo->Enabled = false;

        ScrollBar->Enabled = false;
}

void TBasicLister::EnableButtons()
{
        ToolButtonRefresh->Enabled = true;
        ToolButtonSettings->Enabled = true;
        bool programLoaded = (ProgramSize() > 0);
        ToolButtonSave->Enabled = programLoaded;
        ToolButtonLineEnds->Enabled = programLoaded;
        ToolButtonInfo->Enabled = true;

        ScrollBar->Enabled = (mProgramDisplayRows > DisplayableRows);
}

int TBasicLister::ProgramSize()
{
        int programSize = 0;

        for (std::vector<LineInfo>::iterator it = mLines->begin(); it != mLines->end(); it++)
        {
                programSize += (*it).lineLength;
        }

        return programSize;
}

//---------------------------------------------------------------------------

int TBasicLister::FindLineDisplayedOnRow(int row)
{
        int index = mLines->size() - 1;

        while (index >= 0)
        {
                int startDisplayRow = (*mLines)[index].startDisplayRow;
                int displayRows = (*mLines)[index].displayRows;
                int endDisplayRow = startDisplayRow + displayRows - 1;
                
                if ((row >= startDisplayRow) && (row <= endDisplayRow))
                {
                        break;
                }

                index--;
        }

        return index;
}

//---------------------------------------------------------------------------

void __fastcall TBasicLister::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        if (Button != mbLeft)
        {
                return;
        }

        int rowWithinClientArea = (Y - ToolBar->Height) / PixelsPerCharacterHeight;
        int row = rowWithinClientArea + ScrollBar->Position;

        int index = FindLineDisplayedOnRow(row);
        if (index != mLastHighlightedEntryIndex)
        {
                HighlightEntry(index);
        }
        else
        {
                UnhighlightEntry(index);
        }

        EnableButtons();
}

//---------------------------------------------------------------------------

void __fastcall TBasicLister::FormMouseWheel(TObject *Sender,
      TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
        if (mProgramDisplayRows > DisplayableRows)
        {
                int currentPos = ScrollBar->Position;
                int newPos = currentPos - (WheelDelta / 120);

                if (newPos < ScrollBar->Min)
                {
                        newPos = ScrollBar->Min;
                }

                if (newPos > ScrollBar->Max)
                {
                        newPos = ScrollBar->Max;
                }

                ScrollBar->Position = newPos;
        }

        Handled = true;
}

//---------------------------------------------------------------------------
       
void __fastcall TBasicLister::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->BasicListerOption->Checked = false;        
}
//---------------------------------------------------------------------------
                    
void __fastcall TBasicLister::ToolButtonSaveClick(TObject *Sender)
{
        SaveListingToFile();
}
//---------------------------------------------------------------------------

void TBasicLister::SaveListingToFile()
{
        if (mBasicLister == NULL)
        {
                return;
        }

        LoadProgram();

        bool programLoaded = (ProgramSize() > 0);
        if (!programLoaded)
        {
                Application->MessageBox("There is no BASIC program loaded.", "Save BASIC Listing", MB_OK);
                return;
        }

        AnsiString machineName = mBasicLister->GetMachineName();
        AnsiString basicFileExtension = mBasicLister->GetBasicFileExtension();

        SaveDialog->Filter = machineName + " BASIC File (*." + basicFileExtension + ")|*." + basicFileExtension + "|Bitmap File (*.bmp)|*.bmp";
        SaveDialog->DefaultExt = basicFileExtension;
        SaveDialog->FilterIndex = mLastFilterIndex;
        SaveDialog->FileName = "";
        SaveDialog->Options << ofOverwritePrompt;
        SaveDialog->Title = "Save BASIC Listing";

        if (!SaveDialog->Execute())
        {
                return;
        }

        mLastFilterIndex = SaveDialog->FilterIndex;

        if (SaveDialog->FilterIndex == 1)
        {
                std::ofstream ofs;
                ofs.open(SaveDialog->FileName.c_str());
                for (std::vector<LineInfo>::iterator it = mLines->begin(); it != mLines->end(); it++)
                {
                        AnsiString lineText = mBasicLister->RenderLineAsText(*it, mOutputRemTokensAsCharacterCodes, mOutputNonAsciiAsCharacterCodes);
                        ofs << lineText.c_str() << '\n';
                }

                ofs.close();
        }
        else
        {
                int highlightIndex = mLastHighlightedEntryIndex;
                UnhighlightEntry(highlightIndex);

                Graphics::TBitmap* tempBitmap = new Graphics::TBitmap();
                tempBitmap->Handle = mBitmap;
                tempBitmap->SaveToFile(SaveDialog->FileName);
                tempBitmap->ReleaseHandle();
                delete tempBitmap;

                HighlightEntry(highlightIndex);
        }
}

void TBasicLister::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("BASICLISTER", "Top", Top);
        ini->WriteInteger("BASICLISTER", "Left", Left);
        ini->WriteBool("BASICLISTER", "ShowLineEnds", ToolButtonLineEnds->Down);
}

void TBasicLister::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("BASICLISTER", "Top", Top);
        Left = ini->ReadInteger("BASICLISTER", "Left", Left);
        ToolButtonLineEnds->Down = ini->ReadBool("BASICLISTER", "ShowLineEnds", ToolButtonLineEnds->Down);

        if (Form1->BasicListerOption->Checked) Show();
}

void __fastcall TBasicLister::ToolButtonLineEndsClick(TObject *Sender)
{
        int scrollPos = ScrollBar->Position;

        ToolButtonLineEnds->Down = !ToolButtonLineEnds->Down;

        int highlightIndex = mLastHighlightedEntryIndex;

        LoadProgram();

        HighlightEntry(highlightIndex);

        EnableButtons();
        
        if (scrollPos <= ScrollBar->Max)
        {
                ScrollBar->Position = scrollPos;
        }
}
//---------------------------------------------------------------------------

void __fastcall TBasicLister::ToolButtonSettingsClick(TObject *Sender)
{
        DisableButtons();

        SaveBasicListingOptionsForm->Left = BasicLister->Left + (BasicLister->Width - SaveBasicListingOptionsForm->Width) / 2;
        SaveBasicListingOptionsForm->Top = BasicLister->Top + (BasicLister->Height - SaveBasicListingOptionsForm->Height) / 2;

        SaveBasicListingOptionsForm->ShowModal();

        mOutputRemTokensAsCharacterCodes = SaveBasicListingOptionsForm->GetOutputTokensAsCharacterCodes();
        mOutputNonAsciiAsCharacterCodes = SaveBasicListingOptionsForm->GetOutputNonAsciiAsCharacterCodes();

        EnableButtons();
}         

void __fastcall TBasicLister::ToolButtonInfoClick(TObject *Sender)
{
        AnsiString machine = mBasicLister->GetMachineName();
        if (machine == "ZX80")
        {
                BasicListingFormatInfoForm->SetActivePage(0);
        }
        else if (machine == "ZX81")
        {
                BasicListingFormatInfoForm->SetActivePage(1);
        }
        else if (machine == "Spectrum")
        {
                BasicListingFormatInfoForm->SetActivePage(2);
        }
        else
        {
                return;
        }

        BasicListingFormatInfoForm->Left = BasicLister->Left + (BasicLister->Width - BasicListingFormatInfoForm->Width) / 2;
        BasicListingFormatInfoForm->Top = BasicLister->Top + (BasicLister->Height - BasicListingFormatInfoForm->Height) / 2;

        DisableButtons();

        BasicListingFormatInfoForm->ShowModal();

        EnableButtons();
}
//---------------------------------------------------------------------------

