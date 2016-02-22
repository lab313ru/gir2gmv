
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#ifndef _UNZ_H
#define _UNZ_H

#include <windows.h>
#include "../zlib.h"

typedef HANDLE HUNZFILE;

#define UNZ_OK                  0
#define UNZ_END_OF_LIST_OF_FILE -100
#define UNZ_ERRNO               Z_ERRNO
#define UNZ_EOF                 0
#define UNZ_PARAMERROR          -102
#define UNZ_BADZIPFILE          -103
#define UNZ_INTERNALERROR       -104
#define UNZ_CRCERROR            -105


/* unz_global_info structure contain global data about the ZIPfile
   These data comes from the end of central dir */
typedef struct _UNZ_GLOBAL_INFO {
  DWORD dwNumberEntry; // total number of entries in the central dir on this disk
  DWORD dwSizeComment; // size of the global comment of the zipfile
} UNZ_GLOBAL_INFO, *PUNZ_GLOBAL_INFO, *LPUNZ_GLOBAL_INFO;


/* unz_file_info contain information about a file in the zipfile */
typedef struct _UNZ_FILE_INFO
{
  DWORD wVersion;             // version made by
  DWORD wVersionNeeded;       // version needed to extract
  DWORD wFlag;                // general purpose bit flag
  DWORD wMethod;              // compression method
  DWORD dwDosDate;            // last mod file date in Dos fmt
  DWORD crc32;                // crc-32
  DWORD dwCompSize;           // compressed size
  DWORD dwUncompSize;         // uncompressed size
  DWORD wSizeFileName;        // filename length
  DWORD wSizeGlobalExtra;     // extra field length
  DWORD wSizeComment;         // file comment length
  DWORD wDiskStart;           // disk number start
  DWORD wInternalFlag;        // internal file attributes
  DWORD dwExternalFlag;       // external file attributes
} UNZ_FILE_INFO, *PUNZ_FILE_INFO, *LPUNZ_FILE_INFO;


ZEXTERN HUNZFILE ZEXPORT unzOpen(LPCSTR pszFileName);
/*
  Open a Zip file. path contain the full pathname (by example,
     on a Windows NT computer "c:\\zlib\\zlib111.zip" or on an Unix computer
	 "zlib/zlib111.zip".
	 If the zipfile cannot be opened (file don't exist or in not valid), the
	   return value is NULL.
     Else, the return value is a HUNZFILE Handle, usable with other function
	   of this unzip package.
*/

ZEXTERN int ZEXPORT unzClose(HUNZFILE file);
/*
  Close a ZipFile opened with unzipOpen.
  If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
    these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
  return UNZ_OK if there is no problem. */

ZEXTERN int ZEXPORT unzGetGlobalInfo(HUNZFILE file,
					LPUNZ_GLOBAL_INFO pglobal_info);
/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */


ZEXTERN int ZEXPORT unzGetGlobalComment(HUNZFILE file,
										   LPSTR szComment, DWORD uSizeBuf);
/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied or an error code <0
*/


/***************************************************************************/
/* Unzip package allow you browse the directory of the zipfile */

ZEXTERN int ZEXPORT unzGoToFirstFile(HUNZFILE file);
/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/

ZEXTERN int ZEXPORT unzGoToNextFile(HUNZFILE file);
/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/

ZEXTERN int ZEXPORT unzLocateFile(
	HUNZFILE file,
	LPCSTR   szFileName,
	BOOL     fCaseSensitive);
/*
  Try locate the file szFileName in the zipfile.
  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/


ZEXTERN int ZEXPORT unzGetCurrentFileInfo(
	HUNZFILE file,
	LPUNZ_FILE_INFO pfile_info,
	LPSTR szFileName,
	DWORD fileNameBufferSize,
	LPVOID extraField,
	DWORD extraFieldBufferSize,
	LPSTR szComment,
	DWORD commentBufferSize);
/*
  Get Info about the current file
  if pfile_info!=NULL, the *pfile_info structure will contain somes info about
	    the current file
  if szFileName!=NULL, the filemane string will be copied in szFileName
			(fileNameBufferSize is the size of the buffer)
  if extraField!=NULL, the extra field information will be copied in extraField
			(extraFieldBufferSize is the size of the buffer).
			This is the Central-header version of the extra field
  if szComment!=NULL, the comment string of the file will be copied in szComment
			(commentBufferSize is the size of the buffer)
*/

/***************************************************************************/
/* for reading the content of the current zipfile, you can open it, read data
   from it, and close it (you can close it before reading all the file)
   */

ZEXTERN int ZEXPORT unzOpenCurrentFile(HUNZFILE file);
/*
  Open for reading data the current file in the zipfile.
  If there is no error, the return value is UNZ_OK.
*/

ZEXTERN int ZEXPORT unzCloseCurrentFile(HUNZFILE file);
/*
  Close the file in zip opened with unzOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/

												
ZEXTERN int ZEXPORT unzReadCurrentFile(HUNZFILE file, voidp buf,
                                      unsigned len);
/*
  Read bytes from the current file (opened by unzOpenCurrentFile)
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return < 0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/

ZEXTERN long ZEXPORT unztell(HUNZFILE file);
/*
  Give the current position in uncompressed data
*/

ZEXTERN int ZEXPORT unzeof(HUNZFILE file);
/*
  return 1 if the end of file was reached, 0 elsewhere 
*/

ZEXTERN int ZEXPORT unzGetLocalExtraField(HUNZFILE file,
  LPVOID buf, UINT len);
/*
  Read extra field from the current file (opened by unzOpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

  if buf==NULL, it return the size of the extra field

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
	buf.
  the return value is the number of bytes copied in buf, or (if <0) 
	the error code
*/


// used to the function ahead
typedef int (__stdcall* UNZCALLBACK)(int, LPVOID);

/* Extra function, extract the current file in zip to the file
   pointed by path in disk.
   If pCallback != NULL, it must return UNZ_OK if the operation
   must continue. The parameter indicates de percentual of the
   operation is complete. */
ZEXTERN int ZEXPORT unzExtractCurrentFileTo(HUNZFILE file,
  LPCSTR lpFilePath, BOOL fOverwrite, UNZCALLBACK pCallback, LPVOID lParam);

#endif // _UNZ_H
