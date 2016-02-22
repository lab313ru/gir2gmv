
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"


/* Open a Zip file. path contain the full pathname (by example, on
   a Windows NT computer "c:\\test\\zlib109.zip" or on an Unix computer
   "zlib/zlib109.zip".
	 If the zipfile cannot be opened (file don't exist or in not valid),
   the return value is NULL.
   Else, the return value is a HUNZFILE Handle, usable with other
   function of this unzip package. */
ZEXTERN HUNZFILE ZEXPORT unzOpen(LPCSTR pszFileName)
{
	LPUNZ_INTERNAL s;
	int            err = UNZ_OK;
  int            iPathSize;
  BOOL           fPath;
  char           buf[4];
  LPSTR          pszName;

  // getting the full path to the file
  iPathSize = GetFullPathName(pszFileName, 0, buf, &pszName);
  if(!iPathSize)
  { iPathSize = lstrlen(pszFileName);
    fPath = FALSE; }
  else fPath = TRUE;

  // alloc memory for the internal structure
	s = (LPUNZ_INTERNAL)ALLOCZ(sizeof(UNZ_INTERNAL) + iPathSize + 2);
  if(!s) return NULL;
  // copying the file path to the memory
  if(fPath)
  { 
    iPathSize = GetFullPathName(pszFileName, iPathSize + 1,
      s->pszFilePath, &pszName);
    if(!iPathSize) err = UNZ_ERRNO;
  }
  else
  { zmemcpy(s->pszFilePath, pszFileName, iPathSize);
    s->pszFilePath[iPathSize] = 0; }

  // opening the file
  if(err == UNZ_OK) err = unzLocal_OpenInternal(s);
  if(err != UNZ_OK)
  { TRYFREE(s); s = NULL; }

  return (HUNZFILE)s;
}


/*
  Close a ZipFile opened with unzOpen.
  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
    these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
  return UNZ_OK if there is no problem. */
ZEXTERN int ZEXPORT unzClose(HUNZFILE file)
{
	LPUNZ_INTERNAL s;

	if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
  if(s->pfile_in_zip_read) unzCloseCurrentFile(file);
	fclose(s->file);
	TRYFREE(s);

	return UNZ_OK;
}


// opens the file for an internal structure already alloc
int unzLocal_OpenInternal(LPUNZ_INTERNAL s)
{
	FILE          *file;
  DWORD          central_pos;
  ZIP_END_HEADER zeh;
  int            err = UNZ_OK;

  file = fopen(s->pszFilePath, "rb");
	if(!file) return UNZ_ERRNO;

  // searching for ZIP_END_HEADER structure (the function checks for the magic)
	central_pos = unzLocal_GetEndHeader(file, &zeh);
	if(!central_pos) err = UNZ_ERRNO;

  if(zeh.wDiskEntries != zeh.wNumberEntries || zeh.wDisk ||
     zeh.wDiskCD) err = UNZ_BADZIPFILE;

  if(central_pos < (zeh.dwCentralDirPos + zeh.dwSizeCentralDir) &&
     err == UNZ_OK) err = UNZ_BADZIPFILE;

	if(err == UNZ_OK)
  {
    s->file                    = file;
    s->gi.dwNumberEntry        = zeh.wDiskEntries;
    s->gi.dwSizeComment        = zeh.wSizeGlobalComment;
    s->size_central_dir        = zeh.dwSizeCentralDir;
    s->offset_central_dir      = zeh.dwCentralDirPos;
	  s->byte_before_the_zipfile = central_pos -
      (s->offset_central_dir + s->size_central_dir);
	  s->central_pos             = central_pos;
    s->pfile_in_zip_read       = NULL;
    unzGoToFirstFile((HUNZFILE)s);
  }
  else
	{ fclose(file); }

	return err;	
}