//---------------------------------------------------------------------------

#ifndef TProfilerH
#define TProfilerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <vector>
#include <z80.h>

//---------------------------------------------------------------------------
class TProfiler : public TForm
{
__published:	// IDE-managed Components
        TButton *ButtonNew;
        TButton *ButtonDelete;
        TListView *ListViewProfileSamples;
        TButton *ButtonEdit;
        TButton *ButtonReset;
        void __fastcall ButtonNewClick(TObject *Sender);
        void __fastcall ButtonEditClick(TObject *Sender);
        void __fastcall ButtonResetClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
private:	// User declarations
        std::vector<class ProfileDetail> _profileDetails;
        void __fastcall UpdateItem(TListItem* item, AnsiString tag, class ProfileDetail& pd);

public:		// User declarations
        __fastcall TProfiler(TComponent* Owner);

        void __fastcall DebugTick(processor* z80);
        void __fastcall Refresh();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfiler *Profiler;
//---------------------------------------------------------------------------
#endif
