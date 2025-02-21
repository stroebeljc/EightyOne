//---------------------------------------------------------------------------

#include <vcl4.h>
#include <stdio.h>
#pragma hdrstop

#include "ZipFile_.h"
#include "unzip.h"
#include "utils.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TZipFile *ZipFile;
//---------------------------------------------------------------------------
__fastcall TZipFile::TZipFile(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
AnsiString TZipFile::ExpandZIP(AnsiString Path, AnsiString DialogueFilter)
{
        unzFile ZFile;
        FILE *F;
        int error;
        char FileName[512], Filter[512];
        char *Orig, *Dest;
        AnsiString File, Ext;

        // Set the window title to the name of the .zip file

        Caption=Path;


        //Build a list of permitted extensions from the doalog box filter option
        // Start by copying the dialog box filter text iinto a buffer.

        Orig=FileName;
        Dest=Filter;
        Filter[0]='\0';
        strcpy(FileName, DialogueFilter.c_str());

        // Now search the text for a * or a , indicating the start of an extension
        // eg *.TZX or .t81

        while(*Orig!='\0')
        {
                if (Orig[0]=='*' && Orig[1]=='.')
                {
                        // Found an extension, so copy it to dest, terminating when
                        // when we reach a | ; or the EOL.

                        Orig++;
                        while(*Orig!='|' && *Orig!=';' && *Orig!='\0') *(Dest++) = *(Orig++);
                        *(Dest++)='\0';
                }
                else    Orig++;
        }
        *Dest='\0';

        // we should end up turnning something like 'All File (TZX,TAP,T81)|*.tzx;*.tap;*.t81\0'
        // into '.tzx\0.tap\0.t81\0\0'

        Dest=Filter;

        // We don't handle nested .zip files, so if it's in the permitted extension
        // list, remove it.

        while(strlen(Dest))
        {
                if ((!strcmp(Dest,".zip")) || (!strcmp(Dest,".ZIP")))
                {
                        Dest[0]='\0';
                        Dest[1]='\0';
                }
                Dest += strlen(Dest)+1;
        }

        // Empty the ListBox
        while(ListBox->Items->Count) ListBox->Items->Delete(0);

        // Open The zip file
        ZFile=unzOpen(Path.c_str());
        if (!ZFile) return("");

        // Step through the contents of the archive, adding each item to the ListBox

        error=unzGoToFirstFile(ZFile);

        while(error==UNZ_OK)
        {
                // Get filename of archive member
                unzGetCurrentFileInfo(ZFile, NULL, FileName, 256, NULL, 0, NULL, 0);

                File=FileName;
                Ext=FileNameGetExt(File);

                // Check the member file extension matches on of those permitted
                Dest=Filter;
                while(strlen(Dest))
                {
                        if (!stricmp(Ext.c_str(), Dest))
                        {
                                ListBox->Items->Add(FileName); //If it does, add it to the ListBox
                                break;
                        }

                        Dest += strlen(Dest)+1; // Next extension
                }
                error=unzGoToNextFile(ZFile);
        }

        // ???? Seems to be a long way of setting ItemIndex to the last file in the list
        if (ListBox->Items->Count)
        {
                int i;
                ListBox->ItemIndex=0;
                for(i=0;i<ListBox->Items->Count;i++)
                {
                        if (ListBox->Items->Strings[i] == LastFile)
                                ListBox->ItemIndex=i;
                }
        }
        else
        {
                // Couldn't file any files in the zip that match the permitted extensions
                Application->MessageBox(L"Sorry, that archive does not contain any files of the relevant type",L"Error", MB_OK | MB_ICONERROR);
                unzClose(ZFile);
                return("");
        }

        // The archive contains several possible files that match the permitted extensions
        // So pop up a dialog and let the user select one
        if (ListBox->Items->Count > 1)
        {
                Cancelled=true;
                ShowModal();
        }
        else
        {       // Only one item in archive so no need to ask the user
                Cancelled=false;
        }

        if (Cancelled) // User cancelled the dialog, so return to app
        {
                unzClose(ZFile);
                return("");
        }

        File=ListBox->Items->Strings[ListBox->ItemIndex];
        LastFile=File;

        // Locate the selected file in the archive
        error=unzLocateFile(ZFile, File.c_str(), 0);
        if (error==UNZ_OK)
        {
                // Create a path in the temp directory to extract to

                strcpy(FileName, emulator.temppath);
                int position = File.LastDelimiter("/\\");
                if (position > 0)
                {
                        File = File.SubString(position + 1, 256);
                }
                strcat(FileName, File.c_str());

                // Open file for writing, then extract the contents.
                F=fopen(FileName, "wb");
                if (F)
                {
                        char buf[256];
                        int len;

                        error=unzOpenCurrentFile(ZFile);
                        if (error==UNZ_OK)
                        {
                                len=unzReadCurrentFile(ZFile, buf, 256);
                                while(len>0)
                                {
                                        fwrite(buf, 1, len, F);
                                        len=unzReadCurrentFile(ZFile, buf, 256);
                                }
                                unzCloseCurrentFile(ZFile);
                        }

                        fclose(F);
                }
        }

        unzClose(ZFile);

        // If everything os OK, return the filename of the extacted file
        if (error==UNZ_OK)
        {
                File=FileName;
                return(File);
        }
        return(""); // If not, return nothing.
}
void __fastcall TZipFile::OKClick(TObject *Sender)
{
        Cancelled=false;
        Close();
}
//---------------------------------------------------------------------------
void TZipFile::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("ZIPFILE","Top",Top);
        Left = ini->ReadInteger("ZIPFILE","Left",Left);
        Width = ini->ReadInteger("ZIPFILE","Width",Width);
        Height = ini->ReadInteger("ZIPFILE","Height",Height);
        LastFile = ini->ReadString("ZIPFILE","LastFile","");
}

void TZipFile::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("ZIPFILE","Top",Top);
        ini->WriteInteger("ZIPFILE","Left",Left);
        ini->WriteInteger("ZIPFILE","Width",Width);
        ini->WriteInteger("ZIPFILE","Height",Height);
        ini->WriteString("ZIPFILE","LastFile",LastFile);

}
void __fastcall TZipFile::ListBoxDblClick(TObject *Sender)
{
        Cancelled=false;
        Close();
}
//---------------------------------------------------------------------------

