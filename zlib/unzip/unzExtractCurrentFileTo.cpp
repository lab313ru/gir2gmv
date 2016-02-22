
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"


ZEXTERN int ZEXPORT unzExtractCurrentFileTo(HUNZFILE file,
  LPCSTR lpFilePath, BOOL fOverwrite, UNZCALLBACK pCallback, LPVOID lParam)
{
  LPUNZ_INTERNAL     s;
  HANDLE             hFile;
  LPVOID             pvBuf;
  int                iRead, iExec;
  DWORD              dwWrite, dw;
  DWORD              dwTotal, dwExtFlag, dwDosDate;
  int                err = UNZ_ERRNO;
  FILETIME           ft;

  if(!file || !lpFilePath) return UNZ_PARAMERROR;
  s = (LPUNZ_INTERNAL)file;
  if(!s->current_file_ok) return UNZ_PARAMERROR;

  dwTotal = s->cur_file_info.dwUncompSize;
  // opening the current file in zip
  if(unzOpenCurrentFile(file) != UNZ_OK)
    return UNZ_ERRNO;

  // allocating memory for the operation
  pvBuf = ALLOC(UNZ_BUFSIZE);
  if(!pvBuf) goto EndZipSave3;

  if(fOverwrite) dw = CREATE_ALWAYS;
  else           dw = CREATE_NEW;
  dwExtFlag = LOWORD(s->cur_file_info.dwExternalFlag);
  // creating the uncompressed file
  hFile = CreateFile(lpFilePath, GENERIC_WRITE, 0, NULL, dw,
    dwExtFlag | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  // error
  if(hFile == INVALID_HANDLE_VALUE) goto EndZipSave2;
  // setting the file time
  dwDosDate = s->cur_file_info.dwDosDate;
  if(DosDateTimeToFileTime(HIWORD(dwDosDate), LOWORD(dwDosDate), &ft))
  { SetFileTime(hFile, &ft, &ft, &ft); }

  iExec   = 0;
  dwWrite = 0;
  while(TRUE)
  {
    // reading from the compressed file
    iRead = unzReadCurrentFile(file, pvBuf, UNZ_BUFSIZE);
    // writing uncompressed
    if(iRead > 0)
    { WriteFile(hFile, pvBuf, iRead, &dw, NULL); }
    else if(iRead < 0)
    { goto EndZipSave1; }
    else
    { break; }

    dwWrite += dw;
    if(iExec > 10)
    { 
      if(pCallback)
      { if(pCallback(100 * dwWrite / dwTotal, lParam) != UNZ_OK) break; }
      iExec = 0;
    }
    else iExec++;
  }

  err = UNZ_OK;
EndZipSave1:
  // closing the output file
  CloseHandle(hFile);
EndZipSave2:
  // free the memory used
  TRYFREE(pvBuf);
EndZipSave3:
  // closing the compressed file in zip
  unzCloseCurrentFile(file);

  return err;
}