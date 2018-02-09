//---------------------------------------------------------------------------

#ifndef ProfilePlot_H
#define ProfilePlot_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TProfilePlot : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBarHorizontal;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ScrollBarHorizontalChange(TObject *Sender);
private:	// User declarations
        class ProfileDetail* _pd;
        void InitScrollbar();
public:		// User declarations
        __fastcall TProfilePlot(TComponent* Owner);

        void __fastcall PlotTGraph(class ProfileDetail* pd);
};
//---------------------------------------------------------------------------
extern PACKAGE TProfilePlot *ProfilePlot;
//---------------------------------------------------------------------------
#endif
