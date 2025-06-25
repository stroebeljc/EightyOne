//---------------------------------------------------------------------------

#ifndef BasicVariables_H
#define BasicVariables_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IBasicLister.h"
//---------------------------------------------------------------------------


class TBasicVariables : public TForm
{
__published:	// IDE-managed Components
        void __fastcall FormPaint(TObject *Sender);

private:	// User declarations
        HBITMAP mBitmap;
        HWND mHWND;

        int mRows;
        int mToolbarHeight;
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
        void LoadVariables();

public:		// User declarations
        __fastcall TBasicVariables(TComponent* Owner);
        virtual __fastcall ~TBasicVariables();

        void __fastcall SetLister(IBasicLister *lister);
        void Refresh();
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicVariables *BasicVariables;
//---------------------------------------------------------------------------
#endif
