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
//---------------------------------------------------------------------------
__fastcall TBasicVariables::TBasicVariables(TComponent* Owner)
        : TForm(Owner),
        mBitmap(NULL),
        mHWND(this->Handle),
        mBasicLister(NULL)
{
        mHWND = this->Handle;
        mVariables = new std::vector<VariableInfo>();

        mToolbarHeight = 0;

        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        SizeWindow();
}

 __fastcall TBasicVariables::~TBasicVariables()
{
        if (mBitmap)
        {
            ::DeleteObject(mBitmap);
        }

        delete mVariables;
}

void TBasicVariables::SizeWindow()
{
        mScaling = SaveBasicListingOptionsForm->GetScalingFator();

        int displayRows = mVariables->size();
        int displayColumns = mBasicLister != NULL ? mBasicLister->GetVarDisplayColumns() : 40;

        mBMWidth = displayColumns * PixelsPerCharacterWidth * mScaling;
        mBMHeight = displayRows * PixelsPerCharacterHeight * mScaling;

        ClientWidth = mBMWidth + 1;
        ClientHeight = mBMHeight + 1;
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

        HDC hdc = (HDC)Canvas->Handle;
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

        HDC hdc = (HDC)Canvas->Handle;
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

                ::BitBlt(hdc, 0, mToolbarHeight, mBMWidth, copyHeight, chdc, 0, 0, SRCCOPY);
        }
        else
        {
                RECT rect;
                rect.left = 0;
                rect.top = mToolbarHeight;
                rect.right = ClientWidth;
                rect.bottom = ClientHeight - mToolbarHeight;
                FillRect(chdc, &rect, (HBRUSH)(COLOR_BTNFACE+1));
        }

        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}
//---------------------------------------------------------------------------

void TBasicVariables::Refresh(void)
{
        if (!Visible) return;

        Invalidate();

        LoadVariables();
}

void TBasicVariables::LoadVariables()
{
        if (mBasicLister != NULL)
        {
                ExtractVariablesDetails();
                ConstructBitmap();
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


