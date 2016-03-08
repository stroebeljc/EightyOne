//---------------------------------------------------------------------------

#ifndef memoryWindowH
#define memoryWindowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <set>
//---------------------------------------------------------------------------

static const int SBARWIDTH = 24;

class RowRenderer
{
public:
        RowRenderer() :
                mBytesPerCell(1)
        {};

        virtual ~RowRenderer(){};

        virtual void RenderRow(void) = 0;
        virtual void SetGeometry(int, TSize&) = 0;

        bool ByteAtX(const int, int&);

        // in
        std::set<int>::iterator mDirty;
        std::set<int>::iterator mLast;
        int mAddress, mY;
        HDC mCHDC;

        // out
        int mDisplayCellsPerRow;

protected:
        void SetGeometry(int, const TSize&, int);

        void ChooseTextColour(void);
        void AddressOut(void);

        int mLMargin, mKern;
        int mCellWidth, mBytesPerCell;
};

class ByteRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class WordRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class BinaryRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class TraditionalRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};


class TMemoryWindow : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBar1;
        TPopupMenu *PopupMenu1;
        TMenuItem *ViewBytes1;
        TMenuItem *ViewWords1;
        TMenuItem *SetAddress1;
        TStatusBar *StatusBar1;
        TMenuItem *JumpTo1;
        TMenuItem *ViewTraditional1;
        TButton *ButtonFirstChange;
        TButton *ButtonPrevChange;
        TButton *ButtonNextChange;
        TButton *ButtonLastChange;
        TMenuItem *ViewBinary1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ScrollBar1Change(TObject *Sender);
        void __fastcall ViewBytes1Click(TObject *Sender);
        void __fastcall SetAddress1Click(TObject *Sender);
        void __fastcall FormClick(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall JumpTo1Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ButtonChangeMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall ButtonPrevChangeClick(TObject *Sender);
        void __fastcall ButtonFirstChangeClick(TObject *Sender);
        void __fastcall ButtonNextChangeClick(TObject *Sender);
        void __fastcall ButtonLastChangeClick(TObject *Sender);

private:
       void __fastcall OnEraseBkgnd (TMessage msg);

  BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_ERASEBKGND, TMessage, OnEraseBkgnd)
  END_MESSAGE_MAP(TForm)

        void __fastcall SetBaseAddress(int value);
        bool __fastcall xyToAddress(int xIn, int yIn, int& address);

        void __fastcall SetSBButtonPosition(TButton* btn, int idx);

        int mRows;
        int mBaseAddress;

        HWND mHWND;
        HBITMAP mOffscreenBitmap;

        int mBMWidth;
        int mBMHeight;

        int mViewMode;
        TSize mCharSize;
        RowRenderer* mRowRenderer;

        void CreateBitmap(void);

public:		// User declarations
        __fastcall TMemoryWindow(TComponent* Owner);
        virtual __fastcall ~TMemoryWindow();

        __property int BaseAddress  = { write=SetBaseAddress };

        enum { MWVM_BYTE, MWVM_WORD, MWVM_BINARY, MWVM_TRADITIONAL };

        void __fastcall SetViewMode(int value);

        void __fastcall UpdateChanges();
        void __fastcall ClearChanges();
};

//---------------------------------------------------------------------------
extern PACKAGE TMemoryWindow *MemoryWindow;
//---------------------------------------------------------------------------
#endif
