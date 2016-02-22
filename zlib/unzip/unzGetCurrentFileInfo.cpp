
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"


ZEXTERN int ZEXPORT unzGetCurrentFileInfo(
	HUNZFILE file,
	LPUNZ_FILE_INFO pufi,
	LPSTR szFileName,
	DWORD fileNameBufferSize,
	LPVOID extraField,
	DWORD extraFieldBufferSize,
	LPSTR szComment,
	DWORD commentBufferSize)
{
  ZIP_CENTRAL_HEADER  zch;
  ZIP_CENTRAL_HEADER *pzch;
  int                 err;

  if(pufi) pzch = &zch;
  else     pzch = NULL;

  err = unzLocal_GetCurrentFileInfoInternal(file, pzch,
	  szFileName, fileNameBufferSize, extraField,
    extraFieldBufferSize, szComment, commentBufferSize);
  if(err == UNZ_OK && pufi)
  {
    // copy info
    pufi->wVersion           = zch.wVersion;
    pufi->wVersionNeeded     = zch.wVersionNeeded;
    pufi->wFlag              = zch.wFlag;
    pufi->wMethod            = zch.wMethod;
    pufi->dwDosDate          = zch.dwDosDate;
    pufi->crc32              = zch.crc32;
    pufi->dwCompSize         = zch.dwCompSize;
    pufi->dwUncompSize       = zch.dwUncompSize;
    pufi->wSizeFileName      = zch.wSizeFileName;
    pufi->wSizeGlobalExtra   = zch.wSizeGlobalExtra;
    pufi->wSizeComment       = zch.wSizeComment;
    pufi->wDiskStart         = zch.wDiskStart;
    pufi->wInternalFlag      = zch.wInternalFlag;
    pufi->dwExternalFlag     = zch.dwExternalFlag;
  }

  return err;
}


/*
  Get the global comment string of the ZipFile, in the pszComment buffer.
  dwSizeBuf is the size of the pszComment buffer.
  return the number of byte copied or an error code < 0
*/
ZEXTERN int ZEXPORT unzGetGlobalComment(HUNZFILE file,
                                        LPSTR pszComment,
                                        DWORD dwSizeBuf)
{
	LPUNZ_INTERNAL s;
	DWORD          dwRead;
	
  if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;

  if(dwSizeBuf > s->gi.dwSizeComment)
    dwRead = s->gi.dwSizeComment;
  else
    dwRead = dwSizeBuf - 1;

	if(fseek(s->file, s->central_pos + sizeof(ZIP_END_HEADER),
       SEEK_SET)) return UNZ_ERRNO;
  if(fread(pszComment, dwRead, 1, s->file) != 1) return UNZ_ERRNO;
  pszComment[dwRead] = 0;

  return (int)dwRead;
}


// give the current position in uncompressed data
ZEXTERN long ZEXPORT unztell(HUNZFILE file)
{
	FILE_IN_ZIP_READ_INFO_S *pfile_in_zip_read_info;

  if(!file) return UNZ_PARAMERROR;
  pfile_in_zip_read_info = 
    ((LPUNZ_INTERNAL)file)->pfile_in_zip_read;
	if(!pfile_in_zip_read_info) return UNZ_PARAMERROR;

	return (long)pfile_in_zip_read_info->stream.total_out;
}


/*
  return 1 if the end of file was reached, 0 elsewhere 
*/
ZEXTERN int ZEXPORT unzeof(HUNZFILE file)
{
	LPUNZ_INTERNAL s;
	FILE_IN_ZIP_READ_INFO_S* pfile_in_zip_read_info;
	if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;
	
	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
		return 1;
	else
		return 0;
}



/*
  Read extra field from the current file (opened by unzOpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

  if buf==NULL, it return the size of the extra field that can be read

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
	buf.
  the return value is the number of bytes copied in buf, or (if <0) 
	the error code
*/
ZEXTERN int ZEXPORT unzGetLocalExtraField(
	HUNZFILE file,
	LPVOID buf,
	UINT len)
{
	LPUNZ_INTERNAL s;
	FILE_IN_ZIP_READ_INFO_S* pfile_in_zip_read_info;
	uInt read_now;
	uLong size_to_read;

	if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;

	size_to_read = (pfile_in_zip_read_info->size_local_extrafield - 
				pfile_in_zip_read_info->pos_local_extrafield);

	if (buf==NULL)
		return (int)size_to_read;
	
	if (len>size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len ;

	if (read_now==0)
		return 0;
	
	if (fseek(pfile_in_zip_read_info->file,
              pfile_in_zip_read_info->offset_local_extrafield + 
			  pfile_in_zip_read_info->pos_local_extrafield,SEEK_SET)!=0)
		return UNZ_ERRNO;

	if (fread(buf,(uInt)size_to_read,1,pfile_in_zip_read_info->file)!=1)
		return UNZ_ERRNO;

	return (int)read_now;
}


/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */
ZEXTERN int ZEXPORT unzGetGlobalInfo(HUNZFILE file,
	LPUNZ_GLOBAL_INFO pugi)
{
  if(!file) return UNZ_PARAMERROR;
  zmemcpy(&(((LPUNZ_INTERNAL)file)->gi),
    pugi, sizeof(UNZ_GLOBAL_INFO));
	return UNZ_OK;
}
