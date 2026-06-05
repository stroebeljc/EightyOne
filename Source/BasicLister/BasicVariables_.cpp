//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "BasicVariables_.h"
#include "BasicListerOptions_.h"
#include "zx81config.h"
#include "main_.h"

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
        mBasicLister(NULL),
        mHighlightedVariableIndex(-1)
{
        mVariables = new std::vector<VariableInfo>();
        mLineDetails = "";
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

        ClientWidth = mBMWidth + (ScrollBar->Visible ? ScrollBar->Width + 1 : 0);
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

        RECT rect;
        ::GetClientRect(mHWND, (LPRECT) &rect);

        mBitmap = ::CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        mBasicLister->ClearRenderedVariablesList(chdc, rect);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
        ReleaseDC(mHWND,hdc);
}

void __fastcall TBasicVariables::WMUpdateScrollBar(TMessage &Message)
{
        ScrollBar->Min = 0;
        int variablesDisplayRows = mBasicLister != NULL ? mBasicLister->GetVariablesRows() : 0;
        bool scrollable = (variablesDisplayRows > DisplayableRows);
        if (scrollable)
        {
                ScrollBar->Max = variablesDisplayRows - DisplayableRows;
        }
        else
        {
                ScrollBar->Max = variablesDisplayRows;
        }

        ScrollBar->SmallChange = 1;
        ScrollBar->LargeChange = DisplayableRows;
        ScrollBar->Enabled = scrollable;
        ScrollBar->Visible = scrollable;
        ScrollBar->Position = 1;
        if (!scrollable) ScrollBar->Position = 0;     // This forces the scroll bar to be disabled
        else ScrollBar->Position = ScrollBar->Max;

        Invalidate();
}

void __fastcall TBasicVariables::WMUpdateStatusBar(TMessage &Message)
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
        StatusBar->Panels->Items[PanelVariableInfo]->Text = mLineDetails;

        SizeWindow();
        Invalidate();
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

        HDC hdc = GetDC(mHWND);
        HDC chdc = CreateCompatibleDC(hdc);

        mBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);

        HGDIOBJ oldbm = SelectObject(chdc, mBitmap);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = mBMWidth;
        rect.bottom = mBMHeight;

        mBasicLister->RenderVariables(chdc, rect, mScaling);

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
        ReleaseDC(mHWND,hdc);
}

void TBasicVariables::UnhighlightEntry()
{
        mLineDetails = "";
}

void TBasicVariables::HighlightEntry()
{
        if (mHighlightedVariableIndex<0) return;

        int row = (*mVariables)[mHighlightedVariableIndex].startDisplayRow;
        HighlightRow(row);

        //int lineNumber = (*mLines)[mHighlightedVariableIndex].lineNumber;
        //mLineDetails = "Line " + AnsiString(lineNumber) + ": ";

        int startAddress = (*mVariables)[mHighlightedVariableIndex].address;
        mLineDetails = "$" + AnsiString::IntToHex(startAddress, 4);

        int endAddress = startAddress - 1 + SingleVariableSize(mHighlightedVariableIndex);
        mLineDetails += "-$" + AnsiString::IntToHex(endAddress, 4);
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

                ::BitBlt(hdc, 0, 0 - ScrollBar->Position * PixelsPerCharacterHeight * mScaling, mBMWidth, copyHeight, chdc, 0, 0, SRCCOPY);
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
                if (index != mHighlightedVariableIndex)
                {
                        mHighlightedVariableIndex = index;
                        HighlightEntry();
                }
                else
                {
                        UnhighlightEntry();
                        mHighlightedVariableIndex = -1;
                }

                ConstructBitmap();
                Invalidate();
        }
}
//---------------------------------------------------------------------------

void __fastcall TBasicVariables::FormMouseWheel(TObject *Sender,
      TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
        if (mBasicLister != NULL  && mBasicLister->GetVariablesRows() > DisplayableRows)
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

int TBasicVariables::HandleUpdateWindow(void *param)
{
        TBasicVariables* self = static_cast<TBasicVariables*>(param);

        self->UpdateWindow();
        return 0;
}

void TBasicVariables::UpdateWindow()
{
        static unsigned int lastSize=0;

        if (mBasicLister != NULL)
        {
                ExtractVariablesDetails();
                if (mVariables->size() > 0)
                {
                        ConstructBitmap();
                        HighlightEntry();
                }
                else
                        ClearBitmap();
        }
        else
                ClearBitmap();

        if (lastSize!=mVariables->size())
        {
                PostMessage(mHWND, WM_SCROLLBAR, 0, 0);
        }

        PostMessage(mHWND, WM_STATUSBAR, 0, 0);
        lastSize=mVariables->size();
        Invalidate();
        BasicVariablesRefreshTimer->Enabled = true;
}

void __fastcall TBasicVariables::BasicVariablesRefreshTimerTimer(
      TObject *Sender)
{
        if (!Visible) return;
        BasicVariablesRefreshTimer->Enabled = false;
        Form1->ThreadPool.EnqueueTask(new TTask(HandleUpdateWindow, (void *)this));
}
//---------------------------------------------------------------------------

void __fastcall TBasicVariables::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        BasicVariablesRefreshTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TBasicVariables::FormShow(TObject *Sender)
{
        BasicVariablesRefreshTimer->Enabled = true;
}
//---------------------------------------------------------------------------

