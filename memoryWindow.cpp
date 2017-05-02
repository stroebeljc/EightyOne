//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "z80.h"
#include "zx81.h"
#include "debug.h"
#include "memoryWindow.h"
#include "EditValue_.h"
#include <set>
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMemoryWindow *MemoryWindow;
//---------------------------------------------------------------------------

extern bool directMemoryAccess;

std::set<int> dirtyBird;
std::set<int>::iterator changeCursor;

__fastcall TMemoryWindow::TMemoryWindow(TComponent* Owner)
        : TForm(Owner)
{
        mHWND = this->Handle;

        mOffscreenBitmap = NULL;
        mRowRenderer = NULL;

        SetViewMode(MWVM_TRADITIONAL);

        mCharSize = Canvas->TextExtent(AnsiString("0"));
       	mHeadingHeight = mCharSize.cy + (mCharSize.cy / 2);
}

 __fastcall TMemoryWindow::~TMemoryWindow()
{
        if (mOffscreenBitmap)
        {
                ::DeleteObject(mOffscreenBitmap);
        }
        delete mRowRenderer;
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetBaseAddress(int value)
{
        ScrollBar1->Position = mRowRenderer->mDisplayCellsPerRow * (int)(value / mRowRenderer->mDisplayCellsPerRow);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetViewMode(int mode)
{
        delete mRowRenderer;

        if (mode == MWVM_BYTE)
        {
                mRowRenderer = new ByteRowRenderer;
        }
        else if (mode == MWVM_WORD)
        {
                mRowRenderer = new WordRowRenderer;
        }
        else if (mode == MWVM_BINARY)
        {
                mRowRenderer = new BinaryRowRenderer;
        }
        else
        {
                mRowRenderer = new TraditionalRowRenderer;
        }

        mViewMode = mode;

        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
            mOffscreenBitmap = NULL;
        }

        Invalidate();
}

//---------------------------------------------------------------------------

void RowRenderer::ChooseTextColour(void)
{
        if (mDirty != mLast && mAddress == *mDirty)
        {
                SetTextColor(mCHDC, RGB(200,0,0));
                ++mDirty;
        }
        else
        {
                SetTextColor(mCHDC, GetSysColor(COLOR_WINDOWTEXT));
        }
}

void RowRenderer::AddressOut(void)
{
        SetTextColor(mCHDC, GetSysColor(COLOR_WINDOWTEXT));
        TextOut(mCHDC, 0, mY, AnsiString::IntToHex(mAddress,4).c_str(), 4);
}

bool RowRenderer::ByteAtX(const int x, int& byte)
{
        if (x <= mLMargin || x >= mLMargin + (mDisplayCellsPerRow * mCellWidth))
        {
                return false;
        }

        byte = ((x - mLMargin) / mCellWidth) * mBytesPerCell;
        return true;
}

bool TraditionalRowRenderer::ByteAtX(const int x, int& byte)
{
        int basex = mLMargin + (mDisplayCellsPerRow * mCellWidth) + mCellWidth;

        if (x >= basex && x < basex + (mDisplayCellsPerRow * 8))
        {
                byte = (x - basex) / 8;
        }
        else if (x <= mLMargin || x >= mLMargin + (mDisplayCellsPerRow * mCellWidth))
        {
                return false;
        }
        else
        {
                byte = (x - mLMargin) / mCellWidth;
        }

        return true;
}

void RowRenderer::SetGeometry(int width, const TSize& charSize, int nCharsPerCell)
{
        mKern = charSize.cx / 2;
        mCellWidth = charSize.cx * (nCharsPerCell + 1);
        mLMargin = (charSize.cx * 6);
		
        int availWidth = width - mLMargin - SBARWIDTH;

        mDisplayCellsPerRow = availWidth / mCellWidth;

        if (mDisplayCellsPerRow * mCellWidth + (charSize.cx * nCharsPerCell) < availWidth)
        {
                ++mDisplayCellsPerRow;
        }
}

void ByteRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 2);
}

void WordRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 4);
        mBytesPerCell = 2;
}

void BinaryRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 8);
}

void TraditionalRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 2);

        double availableWidth = width - mLMargin - mCellWidth - SBARWIDTH;

        float cells = 8 + mCellWidth;
        int Rw = int(availableWidth * (8 / cells));
        int Lw = int(availableWidth) - Rw;

        mDisplayCellsPerRow = Lw / mCellWidth;
}

void ByteRowRenderer::RenderRow(void)
{
        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();
                int val = getbyte(mAddress);
                if (mAddress <= 0xFFFF)
                {
                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                AnsiString::IntToHex(val ,2).c_str(), 2);
                }
                ++mAddress;
        }
}

void WordRowRenderer::RenderRow(void)
{
        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();
                int val = getbyte(mAddress) + 256 * getbyte(mAddress+1);
                if (mAddress <= 0xFFFF)
                {
                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                AnsiString::IntToHex(val ,4).c_str(), 4);
                }
                mAddress += 2;
                ++mDirty;
        }
}

void BinaryRowRenderer::RenderRow(void)
{
        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();
                int val = getbyte(mAddress);
                if (mAddress <= 0xFFFF)
                {
                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                Dbg->Bin8(val).c_str(), 8);
                }
                ++mAddress;
        }
}

void TraditionalRowRenderer::RenderRow(void)
{
        HDC another = CreateCompatibleDC(mCHDC);
        HGDIOBJ oldBM = NULL;

        // yuk yuk yuk don't like it
        // options? default cset? machine->ascii mapping?

        if (machine.cset)
        {
                oldBM = SelectObject(another, (HGDIOBJ)((Graphics::TBitmap*)machine.cset)->Handle);
        }

        AddressOut();
        int basex = mLMargin + (mDisplayCellsPerRow * mCellWidth) + mCellWidth;
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();
                int by = getbyte(mAddress);
                if (mAddress <= 0xFFFF)
                {
                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                AnsiString::IntToHex(by, 2).c_str(), 2);

                        if (machine.cset)
                        {
                                int charX = by % 32;
                                int charY = by / 32;
                                BitBlt(mCHDC, x * 8 + basex, 3 + mY, 8, 8,
                                        another, charX * 8, charY * 8, SRCCOPY);
                        }
                }
                ++mAddress;
        }

        SelectObject(another, oldBM);
        DeleteDC(another);
}

void RowRenderer::RenderColumnHeadings(const TSize& charSize)
{
        TextOut(mCHDC, mLMargin + 2 * mKern - charSize.cy, 0, "+", 1);

        int columnInset = ((mCellWidth - charSize.cy) / 2) + mLMargin;

        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
		AnsiString heading = AnsiString::IntToHex(x, 2);
                TextOut(mCHDC, (x * mCellWidth) + columnInset, 0, heading.c_str(), 2);
        }
}

//---------------------------------------------------------------------------


void TMemoryWindow::CreateBitmap(void)
{
        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
        }

        RECT rect;
        ::GetClientRect(mHWND, (LPRECT) &rect);

        mBMWidth = rect.right - rect.left;
        mBMHeight = rect.bottom - rect.top - StatusBar1->Height;
        if (mBMWidth < 1)
                mBMWidth  = 1;
        if (mBMHeight < 1)
                mBMHeight  = 1;

        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);

        mOffscreenBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);
        HGDIOBJ oldbm = SelectObject(chdc, mOffscreenBitmap);

        HGDIOBJ oldfont = SelectObject(chdc, Canvas->Font->Handle);

        FillRect(chdc, &rect, (HBRUSH)(COLOR_BTNFACE+1));

        SetBkColor(chdc, GetSysColor(COLOR_BTNFACE));

        mRowRenderer->SetGeometry(mBMWidth, mCharSize);

        mRows = (mBMHeight - mHeadingHeight) / mCharSize.cy;

        ScrollBar1->SmallChange = mRowRenderer->mDisplayCellsPerRow;

        ScrollBar1->LargeChange = 8 * ScrollBar1->SmallChange;
        bool partRow = ((65536 % ScrollBar1->SmallChange) != 0);
        int max = partRow ? 65536 + ScrollBar1->SmallChange : 65536;
        ScrollBar1->Max = max - (ScrollBar1->SmallChange * mRows * mRowRenderer->BytesPerCell());

        mBaseAddress = mRowRenderer->mDisplayCellsPerRow * (int)(mBaseAddress / mRowRenderer->mDisplayCellsPerRow);

        mRowRenderer->mAddress = mBaseAddress;
        mRowRenderer->mDirty = dirtyBird.lower_bound(mRowRenderer->mAddress);
        mRowRenderer->mLast = dirtyBird.end();
        mRowRenderer->mCHDC = chdc;
        mRowRenderer->mY = mHeadingHeight;

        mRowRenderer->RenderColumnHeadings(mCharSize);

        for (int i = 0; i < mRows; ++i)
        {
                mRowRenderer->RenderRow();
                mRowRenderer->mY += mCharSize.cy;
        }

        SelectObject(chdc, oldfont);
        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::OnEraseBkgnd (TMessage msg)
{
}
//---------------------------------------------------------------------------


void __fastcall TMemoryWindow::UpdateChanges()
{
        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
            mOffscreenBitmap = NULL;
        }

        Invalidate();

        changeCursor = dirtyBird.lower_bound(mBaseAddress);

        bool ena = dirtyBird.size() != 0;
        ButtonFirstChange->Enabled = ena;
        ButtonPrevChange->Enabled = ena;
        ButtonNextChange->Enabled = ena;
        ButtonLastChange->Enabled = ena;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ClearChanges()
{
        dirtyBird.clear();
        UpdateChanges();
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormPaint(TObject *Sender)
{
        if (!mOffscreenBitmap)
        {
                CreateBitmap();
        }

        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mOffscreenBitmap);
        ::BitBlt(hdc, 0, 0, mBMWidth, mBMHeight, chdc, 0, 0, SRCCOPY);
        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormResize(TObject *Sender)
{
        if (mHWND)
        {
                CreateBitmap();
                Invalidate();

				// Required if the form has been resized using the grip control
				GlueButtonsToStatusBar();
        }
}

void __fastcall TMemoryWindow::ScrollBar1Change(TObject *Sender)
{
        int value = ScrollBar1->Position;
        mBaseAddress = mRowRenderer->mDisplayCellsPerRow * (int)(value / mRowRenderer->mDisplayCellsPerRow);

        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
            mOffscreenBitmap = NULL;
        }

        Invalidate();
}
//---------------------------------------------------------------------------

static void UpdateCheckSetMode(TObject *Sender)
{
}

void __fastcall TMemoryWindow::ViewBytes1Click(TObject *Sender)
{
        TMenuItem* item = (TMenuItem*)Sender;
        ViewBytes1->Checked = false;
        ViewWords1->Checked = false;
        ViewBinary1->Checked = false;
        ViewTraditional1->Checked = false;
        item->Checked = true;
        SetViewMode(item->Tag);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetAddress1Click(TObject *Sender)
{
        EditValue->CentreOn(this);

        int v = SetAddress1->Tag;
        if (EditValue->Edit2(v,2))
        {
                SetBaseAddress(v);
        }
}
//---------------------------------------------------------------------------

bool  __fastcall TMemoryWindow::xyToAddress(int xIn, int yIn, int& address)
{
        int x;
        int y = (yIn - mHeadingHeight) / mCharSize.cy;

        if (!mRowRenderer->ByteAtX(xIn, x) || yIn < mHeadingHeight || yIn >= mHeadingHeight + (mRows * mCharSize.cy))
        {
                return false;
        }

        address = mBaseAddress + x + (y * mRowRenderer->mDisplayCellsPerRow * mRowRenderer->BytesPerCell());

        return (address <= 0xFFFF);
}

void __fastcall TMemoryWindow::FormClick(TObject *Sender)
{
        int address;
        TPoint cp = ScreenToClient(Mouse->CursorPos);

        if (xyToAddress(cp.x, cp.y, address))
        {
                EditValue->Top = Mouse->CursorPos.y;
                EditValue->Left = Mouse->CursorPos.x;

                if (mViewMode != MWVM_WORD)
                {
                        int n = getbyte(address);
                        if (EditValue->Edit2(n,1))
                        {
                                setbyte(address,n);
                        }
                }
                else
                {
                        int n = getbyte(address) + 256 * getbyte(address + 1);
                        if (EditValue->Edit2(n,2))
                        {
                                setbyte(address, n & 255);
                                setbyte(address+1, n >> 8);
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        int address;
        AnsiString t;
        if (xyToAddress(X,Y,address))
        {
                t = "$" + AnsiString::IntToHex(address,4);
        }

        StatusBar1->Panels->Items[4]->Text = t;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        JumpTo1->Caption = "Jump To";

        int address = 0;
        bool gotAddress = xyToAddress(X,Y,address);
        if (gotAddress)
        {
                SetAddress1->Tag = address;

                // indirect address

                address = getbyte(address) + 256 * getbyte(address + 1);
                JumpTo1->Caption += " $" + AnsiString::IntToHex(address,4);
                JumpTo1->Tag = address;
         }
        else
        {
                SetAddress1->Tag = mBaseAddress;
        }

        JumpTo1->Enabled = gotAddress;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::JumpTo1Click(TObject *Sender)
{
        SetBaseAddress(JumpTo1->Tag);
}
//---------------------------------------------------------------------------


void __fastcall TMemoryWindow::SetSBButtonPosition(TButton* btn, int idx)
{
        RECT r;
        StatusBar1->Perform(SB_GETRECT, idx, (LPARAM)&r);

		// Detach the button and then re-attach it (required to ensure the button
		// stays in the status bar when the form is resized using the grip control)
        btn->Parent = NULL;		
        btn->Parent = StatusBar1;
		
        btn->Top = r.top;
        btn->Left = r.left;
        btn->Width = r.right - r.left;
        btn->Height = r.bottom - r.top;
}


void __fastcall TMemoryWindow::FormShow(TObject *Sender)
{
        GlueButtonsToStatusBar();
}

void __fastcall TMemoryWindow::GlueButtonsToStatusBar()
{
        // glue the change navigation buttons to the status bar
        //
        SetSBButtonPosition(ButtonFirstChange, 0);
        SetSBButtonPosition(ButtonPrevChange, 1);
        SetSBButtonPosition(ButtonNextChange, 2);
        SetSBButtonPosition(ButtonLastChange, 3);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonChangeMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        const AnsiString btnDescs[] =
        {
                "First", "Previous", "Next", "Last"
        };

        StatusBar1->Panels->Items[4]->Text = btnDescs[((TButton*)Sender)->Tag] + " Change";
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonFirstChangeClick(TObject *Sender)
{
        changeCursor = dirtyBird.begin();
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonPrevChangeClick(TObject *Sender)
{
       // find the change before the one that is greater or equal
       // to the start address
       //
       changeCursor = dirtyBird.lower_bound(mBaseAddress);
        if (changeCursor != dirtyBird.begin())
        {
                --changeCursor;
        }
        SetBaseAddress(*changeCursor);
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonNextChangeClick(TObject *Sender)
{
        // find the next change greater than the base address
        //
        changeCursor = dirtyBird.upper_bound(mBaseAddress);
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonLastChangeClick(TObject *Sender)
{
        changeCursor = dirtyBird.end();
        --changeCursor;
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

