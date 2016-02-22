
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"


/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
ZEXTERN int ZEXPORT unzGoToFirstFile(HUNZFILE file)
{
	int            err = UNZ_OK;
	LPUNZ_INTERNAL s;
	
  if(!file) return UNZ_PARAMERROR;

	s = (LPUNZ_INTERNAL)file;
	s->pos_in_central_dir = s->offset_central_dir;
	s->num_file = 0;
	err = unzLocal_GetCurrentFileInfoInternal(file,
    &s->cur_file_info, NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);

	return err;
}


/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
ZEXTERN int ZEXPORT unzGoToNextFile(HUNZFILE file)
{
	LPUNZ_INTERNAL s;	
	int err;

	if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
	if(!s->current_file_ok) return UNZ_END_OF_LIST_OF_FILE;
  if(s->num_file + 1 == s->gi.dwNumberEntry)
		return UNZ_END_OF_LIST_OF_FILE;

  s->pos_in_central_dir += sizeof(ZIP_CENTRAL_HEADER) + s->cur_file_info.wSizeFileName +
    s->cur_file_info.wSizeGlobalExtra + s->cur_file_info.wSizeComment;
	s->num_file++;
	err = unzLocal_GetCurrentFileInfoInternal(file,
    &s->cur_file_info, NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}


/*
  Try locate the file szFileName in the zipfile.
  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/
ZEXTERN int ZEXPORT unzLocateFile(HUNZFILE file,
                                  LPCSTR   szFileName,
                                  BOOL     fCaseSensitive)
{
	LPUNZ_INTERNAL s;
	int            err;
  char           szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
  DWORD          num_fileSaved;
  DWORD          pos_in_central_dirSaved;


	if(!file) return UNZ_PARAMERROR;

  if(lstrlen(szFileName) >= UNZ_MAXFILENAMEINZIP)
    return UNZ_PARAMERROR;

	s = (LPUNZ_INTERNAL)file;
	if(!s->current_file_ok) return UNZ_END_OF_LIST_OF_FILE;

	num_fileSaved = s->num_file;
	pos_in_central_dirSaved = s->pos_in_central_dir;

	err = unzGoToFirstFile(file);

	while(err == UNZ_OK)
	{
		unzGetCurrentFileInfo(file, NULL, szCurrentFileName,
      sizeof(szCurrentFileName) - 1, NULL, 0, NULL, 0);
    if(fCaseSensitive)
    { if(!lstrcmp(szCurrentFileName, szFileName)) return UNZ_OK; }
    else
    { if(!lstrcmpi(szCurrentFileName, szFileName)) return UNZ_OK; }
		err = unzGoToNextFile(file);
	}

	s->num_file = num_fileSaved;
	s->pos_in_central_dir = pos_in_central_dirSaved ;

	return err;
}
