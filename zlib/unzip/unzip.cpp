
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"

// get Info about the current file in the zipfile, with internal only info
int unzLocal_GetCurrentFileInfoInternal(
	HUNZFILE file,
  ZIP_CENTRAL_HEADER *pzch,
	LPSTR  pszFileName,
	DWORD  dwFileNameBufSize,
	LPVOID pExtraField,
	DWORD  dwExtraFieldBufSize,
	LPSTR  pszComment,
	DWORD  dwCommentBufSize)
{
	LPUNZ_INTERNAL     s;
  ZIP_CENTRAL_HEADER zch;
	int                err = UNZ_OK;

	if(!file) return UNZ_PARAMERROR;
  s = (LPUNZ_INTERNAL)file;
  // no files stored in zip directory
  if(!s->gi.dwNumberEntry) return UNZ_END_OF_LIST_OF_FILE;

  if(fseek(s->file, s->pos_in_central_dir +
       s->byte_before_the_zipfile, SEEK_SET)) err = UNZ_ERRNO;
  // reading the central header
  if(err == UNZ_OK)
  { if(fread(&zch, sizeof(ZIP_CENTRAL_HEADER), 1, s->file) != 1)
      err = UNZ_ERRNO; }

	// we check the magic
	if(err == UNZ_OK)
  { if(zch.dwMagic != CENTRALHEADERMAGIC) err = UNZ_BADZIPFILE; }

  if(err == UNZ_OK && pzch)
    zmemcpy(pzch, &zch, sizeof(ZIP_CENTRAL_HEADER));

  // reading the file name in central header
  if(err == UNZ_OK && pszFileName &&
     dwFileNameBufSize > (DWORD)zch.wSizeFileName)
  {
    if(fread(pszFileName, zch.wSizeFileName, 1, s->file) != 1)
      err = UNZ_ERRNO;
    if(err == UNZ_OK) pszFileName[zch.wSizeFileName] = 0;
  }
  else // buffer is too small
  { 
    if(fseek(s->file, zch.wSizeFileName, SEEK_CUR))
      err = UNZ_ERRNO;
  }

  // reading the extra field global
  if(err == UNZ_OK && pExtraField &&
     dwExtraFieldBufSize >= (DWORD)zch.wSizeGlobalExtra)
  {
    if(fread(pExtraField, zch.wSizeGlobalExtra, 1, s->file) != 1)
      err = UNZ_ERRNO;
  }
  else // buffer is too small
  { 
    if(fseek(s->file, zch.wSizeGlobalExtra, SEEK_CUR))
      err = UNZ_ERRNO;
  }

  // reading the comment
  if(err == UNZ_OK && pszComment &&
     dwCommentBufSize > (DWORD)zch.wSizeComment)
  {
    if(fread(pszComment, zch.wSizeComment, 1, s->file) != 1)
      err = UNZ_ERRNO;
    if(err == UNZ_OK) pszComment[zch.wSizeComment] = 0;
  }
  else // buffer is too small
  { 
    if(fseek(s->file, zch.wSizeComment, SEEK_CUR))
      err = UNZ_ERRNO;
  }

	return err;
}


/*
  Read the header of the current zipfile
  Check the coherency of the header and info in the end of
  central directory about this file store in *piSizeVar the size
  of extra info in header (filename and size of extra field data)
*/
int unzLocal_CheckCurrentFileCoherencyHeader(
	LPUNZ_INTERNAL s,
	LPUINT         piSizeVar,
	LPDWORD        poffset_local_extrafield,
	LPUINT         psize_local_extrafield)
{
  ZIP_LOCAL_HEADER zlh;
	int              err = UNZ_OK;

  // position of the local header in zip
  if(fseek(s->file, s->cur_file_info.dwPosLocalHeader +
       s->byte_before_the_zipfile, SEEK_SET)) return UNZ_ERRNO;
  // reading the local header
  if(err == UNZ_OK)
  { if(fread(&zlh, sizeof(ZIP_LOCAL_HEADER), 1, s->file) != 1)
      err = UNZ_ERRNO; }

  // checking the magic
  if(err == UNZ_OK)
  { if(zlh.dwMagic != LOCALHEADERMAGIC) err = UNZ_BADZIPFILE; }
  // version
  /*
  if(err == UNZ_OK && zlh.dwVersion != s->s->cur_file_info.wVersion)
    err=UNZ_BADZIPFILE;
  */
  // comparing the compression method in the central header with the local header
  if(err == UNZ_OK && zlh.wMethod != s->cur_file_info.wMethod)
    err=UNZ_BADZIPFILE;
  // valid compression method
  if(err == UNZ_OK && s->cur_file_info.wMethod &&
     s->cur_file_info.wMethod != Z_DEFLATED)
     err = UNZ_BADZIPFILE;
  // valid crc and flag
  if(err == UNZ_OK && zlh.crc32 != s->cur_file_info.crc32 &&
     !(zlh.wFlag & 8)) err = UNZ_BADZIPFILE;
  // compressed size
  if(err == UNZ_OK &&
     zlh.dwCompSize != s->cur_file_info.dwCompSize &&
     !(zlh.wFlag & 8)) err = UNZ_BADZIPFILE;
  // uncompressed size
  if(err == UNZ_OK &&
     zlh.dwUncompSize != s->cur_file_info.dwUncompSize && 
     !(zlh.wFlag & 8)) err = UNZ_BADZIPFILE;
  if(err == UNZ_OK && zlh.wSizeFileName != s->cur_file_info.wSizeFileName)
	  err = UNZ_BADZIPFILE;

  *piSizeVar = (UINT)zlh.wSizeFileName + zlh.wSizeLocalExtra;
  *poffset_local_extrafield = sizeof(ZIP_LOCAL_HEADER) +
    s->cur_file_info.dwPosLocalHeader +
    zlh.wSizeFileName;
  *psize_local_extrafield = (UINT)zlh.wSizeLocalExtra;

  return err;
}
												