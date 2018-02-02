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
//---------------------------------------------------------------------------
class ProfileDetail
{
public:
        static const int waiting = 0;
        static const int counting = 1;
        static const int done = 2;

        ProfileDetail(int start, int end) {
                _start = start;
                _end = end;
                Reset();
        }

        void Reset()
        {
                _ts = 0;
                _state = waiting;
        }

        int Update(int addr, int t) {
                if (addr == _start) {
                        Reset();
                        _state = counting;
                }

                if (_state == counting) {
                        _ts += t;
                        if (_ts > _max)
                                _max = _ts;
                }

                if (addr == _end) {
                        _state = done;
                }

                return _state;
        }

        int Start() {
                return _start;
        }

        int End() {
                return _end;
        }

        int Min() {
                return min(_max, *std::min_element(history.begin(), history.end()));
        }

        int Max() {
                return max(_max, *std::max_element(history.begin(), history.end()));
        }

        int SampleCount() {
                return history.size();
        }

private:
        int _start, _end;
        int _ts, _max;
        int _state;

        std::vector<int> history;
};

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
private:	// User declarations
        std::vector<ProfileDetail> _profileDetails;
        void __fastcall UpdateItem(TListItem* item, AnsiString tag, ProfileDetail& pd);

public:		// User declarations
        __fastcall TProfiler(TComponent* Owner);

        void __fastcall UpdateProfileDetails(int, int);
};
//---------------------------------------------------------------------------
extern PACKAGE TProfiler *Profiler;
//---------------------------------------------------------------------------
#endif
