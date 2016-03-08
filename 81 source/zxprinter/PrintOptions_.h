//---------------------------------------------------------------------------

#ifndef PrintOptions_H
#define PrintOptions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TPrintOptions : public TForm
{
__published:	// IDE-managed Components
        TTrackBar *BitRate;
        TTrackBar *LineRate;
        TLabel *BitRateTxt;
        TLabel *LineRateTxt;
        TButton *OK;
        void __fastcall BitRateChange(TObject *Sender);
        void __fastcall LineRateChange(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPrintOptions(TComponent* Owner);
        int BitSpeed, LineSpeed;
};
//---------------------------------------------------------------------------
extern PACKAGE TPrintOptions *PrintOptions;
//---------------------------------------------------------------------------
#endif
