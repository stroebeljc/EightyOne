//---------------------------------------------------------------------------

#ifndef BasicVariables_H
#define BasicVariables_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <IniFiles.hpp>
#include <Forms.hpp>
#include "IBasicLister.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#define WM_STATUSBAR (WM_USER+1)
#define WM_SCROLLBAR (WM_USER+2)

class TBasicVariables : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar;
        TScrollBar *ScrollBar;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
        void __fastcall ScrollBarChange(TObject *Sender);

private:	// User declarations
        static const int DisplayableRows = 20;

        HBITMAP mBitmap;
        HWND mHWND;

        int mRows;
        IBasicLister* mBasicLister;
        std::vector<VariableInfo>* mVariables;
        int mVariablesDisplayRows;

        int mBMWidth;
        int mBMHeight;
        int mScaling;

        void ClearBitmap();
        void ConstructBitmap();
        void ExtractVariablesDetails();
        void SizeWindow();
        void UnhighlightRow(int row);
        void HighlightRow(int row);
        void ColourRows(int row, bool setornot);
        COLORREF GetHighlightColour();
        void UnhighlightEntry(int index);
        void HighlightEntry(int index);
        int FindVariableDisplayedOnRow(int row);
        int FindHighlightedVariableIndex();
        int TotalVariablesSize();
        int SingleVariableSize(int index);

public:		// User declarations
        __fastcall TBasicVariables(TComponent* Owner);
        virtual __fastcall ~TBasicVariables();
        void __fastcall WMUpdateStatusBar(TMessage &Message);
        void __fastcall WMUpdateScrollBar(TMessage &Message);

 BEGIN_MESSAGE_MAP
   MESSAGE_HANDLER(WM_STATUSBAR, TMessage, WMUpdateStatusBar)
   MESSAGE_HANDLER(WM_SCROLLBAR, TMessage, WMUpdateScrollBar)
 END_MESSAGE_MAP(TForm)

        void __fastcall SetLister(IBasicLister *lister);
        void SaveSettings(TIniFile* ini);
        void LoadSettings(TIniFile* ini);
        void Refresh(void);
        void Clear(void);
        void ShowScale(int scale);
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicVariables *BasicVariables;
//---------------------------------------------------------------------------
#endif
