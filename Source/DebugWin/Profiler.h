/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef TProfilerH
#define TProfilerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <vector>
#include "z80.h"

//---------------------------------------------------------------------------
class TProfiler : public TForm
{
__published:	// IDE-managed Components
        TListView *ListViewProfileSamples;
        TButton *ButtonNew;
        TButton *ButtonDelete;
        TButton *ButtonEdit;
        TButton *ButtonReset;
        TButton *ButtonRefresh;
        TButton *ButtonPlot;
        TTimer *Timer;
        TCheckBox *AutoRefresh;
        void __fastcall ButtonNewClick(TObject *Sender);
        void __fastcall ButtonEditClick(TObject *Sender);
        void __fastcall ButtonResetClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
        void __fastcall ButtonRefreshClick(TObject *Sender);
        void __fastcall ButtonPlotClick(TObject *Sender);
        void __fastcall ListViewProfileSamplesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall ListViewProfileSamplesEditing(TObject *Sender,
          TListItem *Item, bool &AllowEdit);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall AutoRefreshClick(TObject *Sender);
private:	// User declarations
        void __fastcall UpdateItem(TListItem* item, class ProfileDetail& pd);
        void __fastcall SampleEditCompleteImpl(bool valid);
        int Count();

        std::vector<class ProfileDetail> _profileDetails;

        class ProfileDetail* _newPD;
        class TProfileSampleEdit* _pse;

public:		// User declarations
        void __fastcall EnableButtons(bool enabled);
        __fastcall TProfiler(TComponent* Owner);

        static void SampleEditComplete(bool valid);

        void __fastcall DebugTick(processor* z80);

        void __fastcall Refresh();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfiler *Profiler;
//---------------------------------------------------------------------------
#endif
