//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "BasicVariables_.h"
#include "BasicListerOptions_.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBasicVariables *BasicVariables;

enum StatusBarIndex
{
        PanelVariables = 0,
        PanelVariableInfo
};


//---------------------------------------------------------------------------
__fastcall TBasicVariables::TBasicVariables(TComponent* Owner)
        : TForm(Owner),
        mBitmap(NULL),
        mHWND(this->Handle),
        mBasicLister(NULL)
{
        mVariables = new std::vector<VariableInfo>();
}

 __fastcall TBasicVariables::~TBasicVariables()
{
        if (mBitmap)
        {
            ::DeleteObject(mBitmap);
        }

        delete mVariables;
}

void TBasicVariables::ShowScale(int scale)
{
        mScaling = scale;
        SizeWindow();
        Show();
}

void TBasicVariables::SizeWindow()
{
        int totalRows = mBasicLister != NULL ? mBasicLister->GetVariablesRows() : DisplayableRows;
        int displayRows = min(totalRows, DisplayableRows);
        int displayColumns = mBasicLister != NULL ? mBasicLister->GetVarDisplayColumns() : 40;

        mBMWidth = displayColumns * PixelsPerCharacterWidth * mScaling;
        mBMHeight = totalRows * PixelsPerCharacterHeight * mScaling;

        ClientWidth = mBMWidth + ScrollBar->Width + 1;
        ClientHeight = displayRows * PixelsPerCharacterHeight * mScaling + StatusBar->Height + 1;
}

void __fastcall TBasicVariables::SetLister(IBasicLister *lister)
{
        mBasicLister = lister;
        if (mVariables != NULL)
        {
                mVariables->clear();
                if (mBasicLister != NULL)
                {
                        mBasicLister->SetVariables(mVariables);
                }
        }
}

void TBasicVariables::Clear()
{
        if (mVariables->size()==0) return;
        mVariables->clear();

        SizeWindow();
        ClearBitmap();
        Invalidate();
}

void TBasicVariables::ClearBitmap()
{
        if (mBitmap)
        {
            ::DeleteObject(mBitmap);
        }

        HDC hdc = GetDC(mHWND);
        HDC chdc = CreateCompatibleDC(hdc);

        mBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = mBMWidth;
        rect.bottom = mBMHeight;

        mBasicLister->ClearRenderedVariablesList(chdc, mBitmap, rect);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
        ReleaseDC(mHWND,hdc);
}

void TBasicVariables::ConfigureScrollBar()
{
        ScrollBar->Min = 0;
        mVariablesDisplayRows = mBasicLister != NULL ? mBasicLister->GetVariablesRows() : 0;
        bool scrollable = (mVariablesDisplayRows > DisplayableRows);
        if (scrollable)
        {
                ScrollBar->Max = mVariablesDisplayRows - DisplayableRows;
        }
        else
        {
                ScrollBar->Max = mVariablesDisplayRows;
        }

        ScrollBar->SmallChange = 1;
        ScrollBar->LargeChange = DisplayableRows;
        ScrollBar->Position = 1;
        ScrollBar->Position = 0;     // This forces the scroll bar to be disabled
        ScrollBar->Enabled = scrollable;
}

void TBasicVariables::ConfigureStatusBar()
{
        AnsiString variableDetails;

        if (mBasicLister != NULL)
        {
                variableDetails += "Variables " + AnsiString(mVariables->size());

                int variablesSize = TotalVariablesSize();
                if (variablesSize > 0)
                {
                        int variablesStart = mBasicLister->GetVariablesStartAddress();
                        variableDetails += ": $";
                        variableDetails += AnsiString::IntToHex(variablesStart, 4);
                        variableDetails += "-$";
                        variableDetails += AnsiString::IntToHex(variablesStart + variablesSize - 1, 4);
                }
        }

        StatusBar->Panels->Items[PanelVariables]->Text = variableDetails;
        StatusBar->Panels->Items[PanelVariableInfo]->Text = "";
}

int TBasicVariables::TotalVariablesSize()
{
        int variablesSize = 0;

        for (unsigned int i = 0; i < mVariables->size(); i++)
        {
                variablesSize += SingleVariableSize(i);
        }

        return variablesSize;
}

int TBasicVariables::SingleVariableSize(int index)
{
        int size = (*mVariables)[index].overheadLength;

        if ((*mVariables)[index].type == IBasicLister::MultiNumber)
                size += (*mVariables)[index].nameSize;
        else
                size++;

        if ((*mVariables)[index].type == IBasicLister::ForNextControl)
                size += mBasicLister->GetForVariableLength();
        else
                size += (*mVariables)[index].contentLength;

        return size;
}

void TBasicVariables::UnhighlightRow(int row)
{
        const bool unhighlight = false;
        ColourRows(row, unhighlight);
}

void TBasicVariables::HighlightRow(int row)
{
        const bool highlight = true;
        ColourRows(row, highlight);
}

void TBasicVariables::ColourRows(int row, bool setornot)
{
        HDC hdc = GetDC(mHWND);
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        COLORREF paperColour = mBasicLister->GetPaperColour();
        COLORREF highlightColour = GetHighlightColour();
        COLORREF findColour;
        COLORREF replaceColour;

        findColour = setornot ? paperColour : highlightColour;
        replaceColour = setornot ? highlightColour : paperColour;

        int startX = 0;
        int startY = row * PixelsPerCharacterHeight * mScaling;
        int endY = (row + 1) * PixelsPerCharacterHeight * mScaling;

        bool doubleSize = (mScaling == 2);

        for (int y = startY; y < endY; y += mScaling)
        {
                int xWidth = mBMWidth * mScaling;

                for (int x = startX; x < xWidth; x += mScaling)
                {
                        COLORREF pixelColor = GetPixel(chdc, x, y);
                        if (pixelColor == findColour)
                        {
                                SetPixelV(chdc, x, y, replaceColour);
                                if (doubleSize)
                                {
                                        SetPixelV(chdc, x+1, y, replaceColour);
                                        SetPixelV(chdc, x, y+1, replaceColour);
                                        SetPixelV(chdc, x+1, y+1, replaceColour);
                                }
                        }
                }
        }

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
        ReleaseDC(mHWND,hdc);
}


void TBasicVariables::ExtractVariablesDetails()
{
        mBasicLister->ExtractVariablesDetails();
}

void TBasicVariables::ConstructBitmap()
{
        if (mBitmap)
        {
            ::DeleteObject(mBitmap);
        }

        SizeWindow();

        HDC hdc = GetDC(mHWND);
        HDC chdc = CreateCompatibleDC(hdc);

        mBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = mBMWidth;
        rect.bottom = mBMHeight;

        mBasicLister->RenderVariables(chdc, mBitmap, rect, mScaling);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
        ReleaseDC(mHWND,hdc);
}

void TBasicVariables::UnhighlightEntry(int index)
{
        if (index == -1)
        {
                index = FindHighlightedVariableIndex();
        }

        if (index != -1)
        {
                int row = (*mVariables)[index].startDisplayRow;
                UnhighlightRow(row);
                (*mVariables)[index].highLighted = false;

                StatusBar->Panels->Items[PanelVariableInfo]->Text = "";

                Invalidate();
        }
}

void TBasicVariables::HighlightEntry(int index)
{
        if (index != -1)
        {
                UnhighlightEntry(-1);

                int row = (*mVariables)[index].startDisplayRow;
                HighlightRow(row);
                (*mVariables)[index].highLighted = true;
        }
        Invalidate();

        AnsiString lineDetails = "";

        if (index != -1)
        {
                //int lineNumber = (*mLines)[index].lineNumber;
                //lineDetails += "Line " + AnsiString(lineNumber) + ": ";

                int startAddress = (*mVariables)[index].address;
                lineDetails += "$" + AnsiString::IntToHex(startAddress, 4);

                int endAddress = startAddress - 1 + SingleVariableSize(index);
                lineDetails += "-$" + AnsiString::IntToHex(endAddress, 4);
        }

        StatusBar->Panels->Items[PanelVariableInfo]->Text = lineDetails;
}

int TBasicVariables::FindVariableDisplayedOnRow(int row)
{
        int index = mVariables->size() - 1;

        while (index >= 0)
        {
                int displayRow = (*mVariables)[index].startDisplayRow;

                if (row == displayRow)
                {
                        break;
                }

                index--;
        }

        return index;
}

int TBasicVariables::FindHighlightedVariableIndex(void)
{
        int index = mVariables->size() - 1;

        while (index >= 0)
        {
                if ((*mVariables)[index].highLighted)
                {
                        break;
                }

                index--;
        }

        return index;
}


COLORREF TBasicVariables::GetHighlightColour()
{
        return RGB(255, 255, 132);
}


//---------------------------------------------------------------------------

void __fastcall TBasicVariables::FormPaint(TObject *Sender)
{
        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        if (mBasicLister != NULL)
        {
                int variablesDisplayRows = mBasicLister->GetVariablesRows();
                int variablesDisplayPixels = variablesDisplayRows * PixelsPerCharacterHeight * mScaling;
                int copyHeight = (variablesDisplayPixels > mBMHeight) ? variablesDisplayPixels : mBMHeight;

                ::BitBlt(hdc, 0, - ScrollBar->Position * PixelsPerCharacterHeight * mScaling, mBMWidth, copyHeight, chdc, 0, 0, SRCCOPY);
        }
        else
        {
                RECT rect;
                rect.left = 0;
                rect.top = 0;
                rect.right = ClientWidth;
                rect.bottom = ClientHeight - StatusBar->Height;
                FillRect(chdc, &rect, (HBRUSH)(COLOR_BTNFACE+1));
        }

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}
//---------------------------------------------------------------------------

void TBasicVariables::Refresh(bool onLineExec)
{
        if (!Visible) return;

        if (mBasicLister != NULL)
        {
                ExtractVariablesDetails();
                ConstructBitmap();
        }

        ConfigureStatusBar();
        
        if (!onLineExec)
        {
                ConfigureScrollBar();
        }

        Invalidate();
}

void TBasicVariables::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("BASICVARIABLES", "Top", Top);
        ini->WriteInteger("BASICVARIABLES", "Left", Left);
}

void TBasicVariables::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("BASICVARIABLES", "Top", Top);
        Left = ini->ReadInteger("BASICVARIABLES", "Left", Left);
}


void __fastcall TBasicVariables::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        int rowWithinClientArea = Y / (PixelsPerCharacterHeight * mScaling);
        int row = rowWithinClientArea + ScrollBar->Position;

        int index = FindVariableDisplayedOnRow(row);

        if (Button == mbLeft)
        {
                int currentHighlightedIndex = FindHighlightedVariableIndex();
                if (index != currentHighlightedIndex)
                {
                        HighlightEntry(index);
                }
                else
                {
                        UnhighlightEntry(index);
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TBasicVariables::FormMouseWheel(TObject *Sender,
      TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
        if (mVariablesDisplayRows > DisplayableRows)
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
                
                Invalidate();
        }

        Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TBasicVariables::ScrollBarChange(TObject *Sender)
{
        Invalidate();
}
//---------------------------------------------------------------------------

