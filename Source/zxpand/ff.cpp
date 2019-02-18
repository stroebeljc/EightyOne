
/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.07e                    (C)ChaN, 2009
/-----------------------------------------------------------------------------
/ Completely hacked to use a local subfolder for EightyOne
/----------------------------------------------------------------------------*/

#include "ff.h"         /* FatFs configurations and declarations */
#include "diskio.h"

#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <direct.h>
#include <fcntl.h>
#include <SYS\Stat.h>

extern loadFileSymbolsProxy(const char*);

struct stat stats;

extern int dirOpen(const char* path);
extern void dirReadNext(void* fileinfo);

// rootpath, curpath; always slash-terminated.
//
const char* zxpandSDCardFolderRoot = NULL;
char curPath[32768] = {0};

char tempPath[32768] = {0};


void AddSlash(char* p)
{
   size_t n = strlen(p);
   if (p[n-1] != '/' && p[n-1] != '\\')
   {
      p[n] = '/';
      p[n+1] = 0;
   }
}


extern void dirGetExeLocation(char* buffer, int bufSize);

void SetRoot(const char* root)
{
   dirGetExeLocation(tempPath, 32768);

   AddSlash(tempPath);
   strcat(tempPath, root);
   AddSlash(tempPath);

   char* t = tempPath;
   for (;*t; ++t)
   {
      if (*t =='\\')
      {
         *t = '/';
      }
   }

   zxpandSDCardFolderRoot = strdup(tempPath);
   strcpy(curPath, zxpandSDCardFolderRoot);
}

const char*createFullPath(const char*path)
{
   strcpy(tempPath, curPath);
   strcat(tempPath, path);
   return tempPath;
}

char *realpath(const char *path, char *resolved_path) 
{
	char buf[32768];
   int len;
	size_t idx;
	
	_fullpath(buf,path,32768);
	
	len=strlen(buf);
	
	if (len == 0 || len > 32768 - 1)
		strcpy(resolved_path, path);
	else 
		strcpy(resolved_path, buf);
 
	// Replace backslashes with forward slashes so the
	// rest of the code behaves correctly.
	for (idx = 0; idx < strlen(resolved_path); idx++) 
	{
		if (resolved_path[idx] == '\\') 
			resolved_path[idx] = '/';
	}
  
	return resolved_path;
}

void mem_cpy (void* dst, const void* src, int cnt) {
   char *d = (char*)dst;
   const char *s = (const char *)src;
   while (cnt--) *d++ = *s++;
}

char chk_chr (const char* str, char chr) {
   while (*str && *str != chr) str++;
   return *str;
}


FRESULT cvtERRNO(void)
{
   switch(errno)
   {
   case EACCES: // Tried to open read-only file for writing, file's sharing mode does not allow specified operations, or given path is directory.
      return FR_DENIED;

   case EEXIST: // _O_CREAT and _O_EXCL flags specified, but filename already exists.
      return FR_EXIST;

   case EINVAL: // Invalid oflag or pmode argument.
      return FR_INVALID_OBJECT;

   case ENOENT: // File or path not found.
      return FR_NO_PATH;

   default:
      return FR_INT_ERR;
   }
}

void SetHandle(FIL* fp, int handle)
{
   fp->fs = (FATFS*)handle;
}

int GetHandle(FIL* fp)
{
   return (int)fp->fs;
}








/*-----------------------------------------------------------------------*/
/* Helper                                                                */
/*-----------------------------------------------------------------------*/

void get_fileinfo_special(struct _FILINFO_ *fi)
{
   fi->fsize = stats.st_size;
   if (!(stats.st_mode & _S_IWRITE))
   {
      fi->fattrib = AM_RDO;
   }
   else
   {
      fi->fattrib = 0;
   }
}






/*-----------------------------------------------------------------------*/
/* Mount a logical filesystem                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
   BYTE vol,      /* Logical drive number to be mounted/unmounted */
   FATFS *fs      /* Pointer to new file system object (NULL for unmount)*/
)
{
   return FR_OK;
}



/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
   FIL *fp,       /* Pointer to the blank file object */
   const XCHAR *path,   /* Pointer to the file name */
   BYTE mode         /* Access mode and file open mode flags */
)
{
   if (GetHandle(fp) != 0)
   {
      _close(GetHandle(fp));
      SetHandle(fp, 0);
   }

   int flags = _O_BINARY;
   if (mode & FA_WRITE)
   {
      flags |= _O_CREAT | _O_TRUNC | _O_RDWR;
      if (mode & FA_CREATE_NEW)
      {
         // fail if exists
         flags |= _O_EXCL;
      }
   }

   // use stat(name,...) instead of fstat(handle,...)
   
   const char* fullNme = createFullPath(path);
   int handle = _open(fullNme, flags);
   if (-1 == handle)
   {
      return cvtERRNO();
   }

   if (-1 == stat(fullNme, &stats))
   {
      _close(handle);
      return cvtERRNO();
   }

   fp->fsize = stats.st_size;
   fp->flag = 0;

   loadFileSymbolsProxy(fullNme);

   SetHandle(fp, handle);
   return FR_OK;
}


FRESULT f_lseek (FIL *fp, DWORD pos)
{
        if (lseek(GetHandle(fp), pos, SEEK_SET) == -1) {
                return cvtERRNO();
        }
        return FR_OK;
}


/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
   FIL *fp,       /* Pointer to the file object */
   void *buff,    /* Pointer to data buffer */
   UINT btr,      /* Number of bytes to read */
   UINT *br    /* Pointer to number of bytes read */
)
{
   int n = _read(GetHandle(fp), buff, btr);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *br = (UINT)n;
   return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
   FIL *fp,       /* Pointer to the file object */
   const void *buff, /* Pointer to the data to be written */
   UINT btw,         /* Number of bytes to write */
   UINT *bw       /* Pointer to number of bytes written */
)
{
   int n = _write(GetHandle(fp), buff, btw);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *bw = (UINT)n;
   return FR_OK;
}



/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
   FIL *fp     /* Pointer to the file object to be closed */
)
{
   _close(GetHandle(fp));
   SetHandle(fp, 0);
   return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Change Current Drive/Directory                                        */
/*-----------------------------------------------------------------------*/

FRESULT f_chdrive (
   BYTE drv    /* Drive number */
)
{
   return FR_OK;
}



FRESULT f_chdir (
   const XCHAR *path /* Pointer to the directory path */
)
{
   char p[32768];

   // need to turn the relative path spec into a real path
   FRESULT	result = FR_NO_PATH;

   // add new directory to current
	sprintf(tempPath,"%s%s",curPath,path);
	
	// Resolve the newpath 
	if(NULL!=realpath(tempPath,p))
	{
      AddSlash(p);

		// Check that the new path is BELOW the base mmcpath
		if(0==strncmp(zxpandSDCardFolderRoot,p,strlen(zxpandSDCardFolderRoot)))
		{
			// And that it exists
			if(0==_access(p,0))
			{
            strcpy(curPath, p);
				result=FR_OK;
			}
		}
	}

   return result;
}







/*-----------------------------------------------------------------------*/
/* Open Directory ready fr reading                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
   DIR *dj,       /* Pointer to directory object to create */
   const XCHAR *path /* Pointer to the directory path */
)
{
   return (FRESULT)dirOpen(createFullPath(path));
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entry in Sequense                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
   DIR *dj,       /* Pointer to the open directory object */
   FILINFO *fno      /* Pointer to file information to return */
)
{
   dirReadNext(fno);
   return FR_OK;
}




FRESULT f_mkdir (const XCHAR* dirname)
{
   FRESULT ret = FR_OK;

   if (_mkdir(createFullPath(dirname)) == -1)
   {
      ret = cvtERRNO();
   }

   return ret;
}




/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
   const XCHAR *path1,    /* Pointer to the file or directory path */
   const XCHAR *path2    /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;

   char tmpp[32768];
   createFullPath(path1);
   strcpy(tmpp, tempPath);

   // a concatenation of the mmc dir & the requested path
   if (rename(tmpp, createFullPath(path2)))
   {
      ret = cvtERRNO();
   }

   return ret;
}



/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
   const XCHAR *path    /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;
   const char* target= createFullPath(path);

   if (-1 == stat(target, &stats))
   {
      return cvtERRNO();
   }

   if (stats.st_mode & S_IFDIR)
   {
      if (_rmdir(target) == -1)
      {
         ret = FR_DENIED;
      }
   }
   else
   {
      // a concatenation of the mmc dir & the requested path
      if (_unlink(target) == -1)
      {
         ret = cvtERRNO();
      }
   }

   return ret;
}
