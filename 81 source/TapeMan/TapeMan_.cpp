//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>

#pragma hdrstop

#include "TapeMan_.h"
#include "main_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTape *Tape;
//---------------------------------------------------------------------------
void __fastcall TTape::WMGetMinMaxInfo(TWMGetMinMaxInfo &Message)
{
        Message.MinMaxInfo->ptMinTrackSize.x = 289;
        Message.MinMaxInfo->ptMinTrackSize.y = 161;
        Message.MinMaxInfo->ptMaxTrackSize.x = 289;
        Message.MinMaxInfo->ptMaxTrackSize.y = 768;
}
//---------------------------------------------------------------------------
__fastcall TTape::TTape(TComponent* Owner)
        : TForm(Owner)
{
        int i;

        DragAcceptFiles(Tape->Handle, true);

        //StringGrid->Rows[0]->CommaText=",Name,Length";
        CurrentPosition = 0;
        Changed=false;

        for(i=0; i<MAXFILES; i++) File[i]=NULL;
        //StringGrid->Col[0]->Align= alRight;
        RecalcCurPos();

}
//---------------------------------------------------------------------------
int TTape::LoadBlock(AnsiString filename)
{
        FILE *fptr;

        fptr=fopen(filename.c_str(), "rb");
        if (!fptr) return(false);

        fseek(fptr, 0, SEEK_END);
        NewFileLen=ftell(fptr);
        fseek(fptr, 0 , SEEK_SET);
        if (NewFileLen<32)
        {
                fclose(fptr);
                return(false);
        }

        NewFile=(BYTE *) malloc(NewFileLen);
        fread(NewFile, NewFileLen, 1, fptr);
        fclose(fptr);
        return(true);
}

//---------------------------------------------------------------------------
int TTape::SaveBlock(AnsiString filename,FILE *fp, int Block)
{
        FILE *fptr;
        int l;

        if (fp) fptr=fp;
        else
        {
                fptr=fopen(filename.c_str(), "wb");
                if (!fptr) return(false);
        }

        l=fwrite(File[Block], 1, StringGrid->Cells[2][Block].ToInt(), fptr);
        fflush(fptr);
        fclose(fptr);
        return(true);
}

//---------------------------------------------------------------------------
void __fastcall TTape::InsertBlockClick(TObject *Sender)
{
        AnsiString Name;
        int NewRow,i;

        if (Sender)
        {
                if (!InsertBlockDialog->Execute()) return;
                Name = InsertBlockDialog->FileName;
                if (!LoadBlock(Name)) return;
        }
        else
        {
                if (BlockName=="") Name = FileName;
                else Name=BlockName;
        }

        Changed = true;

        NewRow = StringGrid->Row;
        StringGrid->RowCount++;

        if (StringGrid->Row != (StringGrid->RowCount - 2))
                for (i= StringGrid->RowCount-2; i>=NewRow; i--)
                {
                        StringGrid->Rows[i+1] = StringGrid->Rows[i];
                        File[i+1] = File[i];
                }

        StringGrid->Row = NewRow+1;

        if (NewRow < CurrentPosition) CurrentPosition++;
        RecalcCurPos();

        Name = ExtractFileName(Name);

        for(i=1; i<= Name.Length(); i++) Name[i] = toupper(Name[i]);

        i=Name.AnsiPos(".P"); if (i) Name = Name.SubString(1, i-1);
        i=Name.AnsiPos(".81"); if (i) Name = Name.SubString(1, i-2);

        StringGrid->Rows[NewRow]->Strings[1] = Name;
        StringGrid->Rows[NewRow]->Strings[2] = NewFileLen;
        File[NewRow]=NewFile;

}
//---------------------------------------------------------------------------
void __fastcall TTape::StringGridDblClick(TObject *Sender)
{
        CurrentPosition = StringGrid->Row;
        RecalcCurPos();
        //StringGrid->Rows[CurrentPosition]->Strings[0]=">";

}
//---------------------------------------------------------------------------


void __fastcall TTape::LoadTapeBtnClick(TObject *Sender)
{
        CloseTape();
        if (!LoadTapeDialog->Execute()) return;

        if (LoadTapeFile(LoadTapeDialog->FileName))
        {
                FileName = LoadTapeDialog->FileName;
                Changed=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TTape::ExtractBlockClick(TObject *Sender)
{
        if (StringGrid->Row == (StringGrid->RowCount-1)) return;

        ExtractBlockDialog->FileName = StringGrid->Cells[1][StringGrid->Row];
        ExtractBlockDialog->FileName += ".p";

        if (!ExtractBlockDialog->Execute()) return;

        SaveBlock(ExtractBlockDialog->FileName, 0, StringGrid->Row);

}
//---------------------------------------------------------------------------

void __fastcall TTape::SaveTapeBtnClick(TObject *Sender)
{
        AnsiString Filter = "EightyOne Tape File (T81)|*.T81";

        if (FileName != "") SaveTapeDialog->FileName = RemoveExt(FileName);
        else SaveTapeDialog->FileName = RemoveExt(SaveTapeDialog->FileName);

        if (StringGrid->RowCount==2) Filter += "|.P File|*.P";

        SaveTapeDialog->Filter = Filter;
        SaveTapeDialog->DefaultExt = "T81";
        SaveTapeDialog->FilterIndex=1;


        if (FileName.Length())
                if (toupper(FileName[FileName.Length()]) == 'P')
                {
                        SaveTapeDialog->DefaultExt = "p";
                        SaveTapeDialog->FilterIndex= 2;
                }

        if (!SaveTapeDialog->Execute()) return;

        if (GetExt(SaveTapeDialog->FileName) == ".P")
        {
                SaveBlock(SaveTapeDialog->FileName,0, 0);
                FileName = SaveTapeDialog->FileName;
        }
        else    SaveTapeFile(SaveTapeDialog->FileName);
        Changed=false;
}
//---------------------------------------------------------------------------

void __fastcall TTape::DeleteBlockClick(TObject *Sender)
{
        int i;

        if (StringGrid->Row >= (StringGrid->RowCount-1)) return;

        if (StringGrid->Row < CurrentPosition) CurrentPosition--;
        if (File[StringGrid->Row]) free(File[StringGrid->Row]);

        for(i=StringGrid->Row; i<StringGrid->RowCount; i++)
        {
                StringGrid->Rows[i]=StringGrid->Rows[i+1];
                File[i]=File[i+1];
        }

        StringGrid->RowCount--;
        RecalcCurPos();
        Changed=true;
}
//---------------------------------------------------------------------------

void __fastcall TTape::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void TTape::RecalcCurPos()
{
        int i;

        if (CurrentPosition<0) CurrentPosition=0;
        if (CurrentPosition > StringGrid->RowCount-1)
                CurrentPosition = StringGrid->RowCount-1;

        for(i=0; i<StringGrid->RowCount; i++)
        {
                StringGrid->Cells[0][i]="";
                if (i==CurrentPosition)
                        StringGrid->Cells[0][i]="--------->";
        }

        StringGrid->Cells[1][StringGrid->RowCount-1]=" ";
        StringGrid->Cells[2][StringGrid->RowCount-1]=" ";
}
//---------------------------------------------------------------------------

int TTape::SaveTapeFile(AnsiString FileName)
{
        char header[] = HEADER_ID;
        char fname[32], flen[16];

        FILE *fptr;
        int i;
        int error=0;

        fptr=fopen(FileName.c_str(), "wb");
        if (!fptr) return(false);

        error += fwrite(header, strlen(header), 1, fptr);

        for(i=0; i < (StringGrid->RowCount - 1); i++)
        {
                memset(fname, 0, 32);
                memset(flen, 0, 16);

                strncpy(fname, StringGrid->Cells[1][i].c_str(), 31);
                strncpy(flen, StringGrid->Cells[2][i].c_str(), 15);

                error += fwrite(fname, 32, 1, fptr);
                error += fwrite(flen, 16, 1, fptr);

                error += fwrite(File[i], atoi(flen), 1, fptr);
        }

        fclose(fptr);

        if (error) return(false);
        return(true);
}
//---------------------------------------------------------------------------

int TTape::LoadTapeFile(AnsiString FileName)
{
        AnsiString Ext;

        char header[5];
        char fname[32], flen[16];

        FILE *fptr;
        int i;
        int error=0;
        int length;

        CloseTape();

        if (FileName.Length()<3) return(false);

        Ext=GetExt(FileName);

        if (Ext==".P")
        {
                if (!LoadBlock(FileName)) return(false);
                Tape->FileName = FileName;
                BlockName="";
                InsertBlockClick(NULL);
                RecalcCurPos();
                Changed=false;
                return(true);
        }

        i=-1;

        fptr=fopen(FileName.c_str(), "rb");
        if (!fptr) return(false);

        fread(header, 4, 1, fptr);
        if (strncmp(header, HEADER_ID,4)) return(false);

        do
        {
                memset(fname, 0, 32);
                memset(flen, 0, 16);
                fread(fname, 32, 1, fptr);
                fread (flen, 16, 1, fptr);

                length = atoi(flen);

                if ( (strlen(fname)>29) || (length < 128) || (length > 49152) )
                        break;
                i++;

                StringGrid->RowCount ++;

                StringGrid->Cells[1][i] = fname;
                StringGrid->Cells[2][i] = flen;

                File[i] = (BYTE *) malloc(length);

                fread(File[i], length, 1, fptr);
        } while(!feof(fptr));

        fclose(fptr);
        RecalcCurPos();
        Changed=false;
        return(true);
}

//---------------------------------------------------------------------------

void TTape::Rewind(void)
{
        CurrentPosition=0;
        RecalcCurPos();
}

void TTape::NextFile(void)
{
        CurrentPosition++;
        if (CurrentPosition == StringGrid->RowCount)
                CurrentPosition--;
        RecalcCurPos();
}

BYTE *TTape::GetCurrentFile(void)
{
        return(File[CurrentPosition]);
}

int TTape::GetCurrentFileLen(void)
{
        if (CurrentPosition == (StringGrid->RowCount-1)) return(0);
        return(StringGrid->Cells[2][CurrentPosition].ToInt());
}

bool TTape::FindFile(AnsiString Name)
{
        int p;

        if (StringGrid->RowCount==1) return(false);

        if (Name=="")
        {
                if (CurrentPosition != StringGrid->RowCount-1)
                        return(true);

                if (AutoRewind->Checked)
                {
                        CurrentPosition=0;
                        return(true);
                }

                return(false);
        }

        p=CurrentPosition;

        do
        {
                if ( (p >= (StringGrid->RowCount-1)) && AutoRewind->Checked) p=0;

                if (StringGrid->Cells[1][p] == Name)
                {
                        CurrentPosition=p;
                        RecalcCurPos();
                        return(true);
                }

                p++;

        } while ( (p!=CurrentPosition && AutoRewind->Checked)
                     || (p<(StringGrid->RowCount-1) && !AutoRewind->Checked) );

        CurrentPosition = p;
        RecalcCurPos();
        return(false);
}

void TTape::AddBlock(AnsiString Name, BYTE *ptr, int Len)
{
        int NewRow,i;

        Changed = true;

        NewRow = CurrentPosition;
        StringGrid->RowCount++;

        if (NewRow != (StringGrid->RowCount - 2))
                for (i= StringGrid->RowCount-2; i>=NewRow; i--)
                {
                        StringGrid->Rows[i+1] = StringGrid->Rows[i];
                        File[i+1] = File[i];
                }

        StringGrid->Row = NewRow+1;

        CurrentPosition++;
        RecalcCurPos();

        StringGrid->Cells[1][NewRow]=Name;
        StringGrid->Cells[2][NewRow]=Len;

        File[NewRow]=(BYTE *) malloc(Len);
        memcpy(File[NewRow], ptr, Len);
        Changed=true;
}

void TTape::NewTape(void)
{
        CloseTape();
}

void TTape::CloseTape(void)
{
        int i;

        if (Changed)
        {
                int btn;

                btn=Application->MessageBox("The current tape file has not been saved.\nWould you like to save it before continuing?",
                                 "Save?", MB_YESNO);

                if (btn==IDYES)SaveTapeBtnClick(NULL);
        }

        for(i=0; i<MAXFILES; i++)
                if (File[i])
                {
                        free(File[i]);
                        File[i]=0;
                }

        //StringGrid->Rows[0]->Delete();

        CurrentPosition=0;
        StringGrid->RowCount=1;
        StringGrid->Rows[0]->CommaText=",,";
        RecalcCurPos();
        Changed=false;
}

void __fastcall TTape::FormResize(TObject *Sender)
{
        Width=289;
}
//---------------------------------------------------------------------------

void __fastcall TTape::NewTapeBtnClick(TObject *Sender)
{
        NewTape();
}
//---------------------------------------------------------------------------
AnsiString TTape::RemoveExt(AnsiString Fname)
{
        AnsiString Ext;
        int len,pos;

        len=Fname.Length();

        if (len<3) return(Fname);

        pos=len;
        while(pos)
        {
                if (Fname[pos]=='.') break;
                pos--;
        }

        Ext = Fname.SubString(pos, 1+len-pos);

        if ( (Ext==".p" || Ext==".P" || Ext==".T81") && pos>1)
                Fname = Fname.SubString(1,pos-1);

        return(Fname);
}

AnsiString TTape::GetExt(AnsiString Fname)
{
        AnsiString Ext;
        int len,pos;

        len=Fname.Length();

        if (len<3) return(Fname);

        pos=len;
        while(pos)
        {
                if (Fname[pos]=='.') break;
                pos--;
        }

        Ext = Fname.SubString(pos, 1+len-pos);

        for(pos=1; pos<=Ext.Length(); pos++) Ext[pos]=toupper(Ext[pos]);
        return(Ext);
}
//---------------------------------------------------------------------------

void __fastcall TTape::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->Tape1->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TTape::BlockUpClick(TObject *Sender)
{
        AnsiString row;
        BYTE *file;

        if (StringGrid->Row==0 || StringGrid->Row==StringGrid->RowCount-1) return;

        file=File[StringGrid->Row];
        row=StringGrid->Rows[StringGrid->Row]->CommaText;

        File[StringGrid->Row] = File[StringGrid->Row-1];
        StringGrid->Rows[StringGrid->Row]->CommaText =
                StringGrid->Rows[StringGrid->Row-1]->CommaText;

        StringGrid->Row -= 1;

        File[StringGrid->Row] = file;
        StringGrid->Rows[StringGrid->Row]->CommaText = row;

        Changed=true;
}
//---------------------------------------------------------------------------

void __fastcall TTape::BlockDownClick(TObject *Sender)
{
        AnsiString row;
        BYTE *file;

        if (StringGrid->Row >= StringGrid->RowCount-2) return;

        file=File[StringGrid->Row];
        row=StringGrid->Rows[StringGrid->Row]->CommaText;

        File[StringGrid->Row] = File[StringGrid->Row+1];
        StringGrid->Rows[StringGrid->Row]->CommaText =
                StringGrid->Rows[StringGrid->Row+1]->CommaText;

        StringGrid->Row += 1;

        File[StringGrid->Row] = file;
        StringGrid->Rows[StringGrid->Row]->CommaText = row;

        Changed=true;
}
//---------------------------------------------------------------------------
bool TTape::IsTape(AnsiString FileName)
{
        AnsiString Ext;

        Ext = GetExt(FileName);

        if (Ext==".T81" || Ext==".P" || Ext==".81") return(true);
        return(false);
}
