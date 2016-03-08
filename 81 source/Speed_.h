//---------------------------------------------------------------------------

#ifndef Speed_H
#define Speed_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSpeed : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TPanel *AccuratePanel;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TTrackBar *AccurateSpeed;
        TPanel *FastPanel;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TTrackBar *FastMode;
        TTrackBar *SlowMode;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Recalc(TObject *Sender);
private:	// User declarations
public:		// User declarations
        int DirtyFastTStates;
        int DirtySlowTStates;
        int AccurateScanlines;

        __fastcall TSpeed(TComponent* Owner);
        void ChangeMode(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TSpeed *Speed;
//---------------------------------------------------------------------------
#endif
