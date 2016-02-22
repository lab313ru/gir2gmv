
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#ifndef _ZIPLOCAL_H
#define _ZIPLOCAL_H

#include <stdio.h>
#include "../zutil.h"

#ifndef  VERSIONMADEBY
# define VERSIONMADEBY 0x00 // platform depedent
#endif

#ifndef  ZIP_BUFSIZE
# define ZIP_BUFSIZE 4096
#endif

#ifndef  Z_MAXFILENAMEINZIP
# define Z_MAXFILENAMEINZIP 256
#endif

#ifndef  ALLOC
# define ALLOC(size) HeapAlloc(GetProcessHeap(), 0, size)
#endif
#ifndef  ALLOCZ
# define ALLOCZ(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#endif
#ifndef  REALLOC
# define REALLOC(p, size) HeapReAlloc(GetProcessHeap(), 0, p, size)
#endif
#ifndef  TRYFREE
# define TRYFREE(p) { if(p) HeapFree(GetProcessHeap(), 0, p); }
#endif

#define LOCALHEADERMAGIC    0x04034B50L
#define CENTRALHEADERMAGIC  0x02014B50L
#define ENDHEADERMAGIC      0x06054B50L

#define FLAG_LOCALHEADER_OFFSET 0x06
#define CRC_LOCALHEADER_OFFSET  0x0E


// the structures are unaligned
#include <pshpack2.h>

typedef struct _ZIP_LOCAL_HEADER {
  DWORD       dwMagic;            // magic (must be LOCALHEADERMAGIC)
  WORD        wVersionNeeded;     // version needed to extract, must be 20
  WORD        wFlag;
  WORD        wMethod;
  DWORD       dwDosDate;
  DWORD       crc32;              // crc
  DWORD       dwCompSize;         // compressed size
  DWORD       dwUncompSize;       // uncompressed size
  WORD        wSizeFileName;
  WORD        wSizeLocalExtra;    // size of the local extra field
  // file name (variable lenght)
  // extrafield local (variable lenght)
} ZIP_LOCAL_HEADER;


/* central header: at the end of the zip file, each file stored
   in zip has a local header (before the file) and a central
   header (at the end of the zip) */
typedef struct _ZIP_CENTRAL_HEADER {
  DWORD       dwMagic;            // magic (must be CENTRALHEADERMAGIC)
  WORD        wVersion;           // version info (VERSIONMADEBY)
  WORD        wVersionNeeded;     // must be 20
  WORD        wFlag;
  WORD        wMethod;
  DWORD       dwDosDate;
  DWORD       crc32;              // crc
  DWORD       dwCompSize;         // compressed size
  DWORD       dwUncompSize;       // uncompressed size
  WORD        wSizeFileName;      // size file name (whithout zeros at the end)
  WORD        wSizeGlobalExtra;   // size of the global extra field
  WORD        wSizeComment;       // size of the comment for the local file
  WORD        wDiskStart;         // disk nm start
  WORD        wInternalFlag;      // internal flag
  DWORD       dwExternalFlag;     // external flag
  DWORD       dwPosLocalHeader;   // position of the local header
  // file name whithout the zero at the end (variable lenght)
  // extrafield global (variable lenght)
  // comment (variable lenght)
} ZIP_CENTRAL_HEADER;


// header at the end of the zip file
typedef struct _ZIP_END_HEADER {
  DWORD       dwMagic;            // magic (must be ENDHEADERMAGIC)
  WORD        wDisk;              // number of the disk
  WORD        wDiskCD;            // number of the disk with the start of the central directory
  WORD        wDiskEntries;       // total number of entries in the central dir on this disk
  WORD        wNumberEntries;     // total number of entries in the central dir
  DWORD       dwSizeCentralDir;   // size of the central directory
  DWORD       dwCentralDirPos;    /* offset of start of central directory with
                                     respect to the starting disk number */
  WORD        wSizeGlobalComment; // zipfile comment length
  // comment follows (variable lenght)
} ZIP_END_HEADER;

#include <poppack.h>


typedef struct _PZIP_LOCAL_HEADER : _ZIP_LOCAL_HEADER {
  char         pszFileName[2];
} *PZIP_LOCAL_HEADER, *LPZIP_LOCAL_HEADER;

typedef struct _PZIP_CENTRAL_HEADER : _ZIP_CENTRAL_HEADER {
  char         pszFileName[2];
} *PZIP_CENTRAL_HEADER, *LPZIP_CENTRAL_HEADER;

typedef struct _PZIP_END_HEADER : _ZIP_END_HEADER {
  char         pszComment[2];
} *PZIP_END_HEADER, *LPZIP_END_HEADER;


typedef struct _CENTRAL_DIR {
  UINT                  nHeaders;    // number of ZIP_CENTRAL_HEADER structures
  UINT                  nAllocPtr;   // number of pointers allocated to contain the headers
  LPZIP_CENTRAL_HEADER *ppzch;

  int  __stdcall        Init(void);
  void __stdcall        End(void);
  int  __stdcall        AddHeader(LPZIP_CENTRAL_HEADER);
} CENTRAL_DIR, *PCENTRAL_DIR, *LPCENTRAL_DIR;


typedef struct _CURFILE_INFO {
  Z_STREAM stream;                     // zLib stream structure for inflate
  int      stream_initialised;         // 1 is stream is initialised
  UINT     pos_in_buffered_data;       // last written byte in buffered_data
  DWORD    pos_local_header;           // offset of the header of the file currenty writing
  DWORD    size_centralheader;         // size of the central header for cur file
  DWORD    flag;                       // flag of the file currently writing
  int      method;                     // compression method of file currenty wr
  BYTE     buffered_data[ZIP_BUFSIZE]; // buffer contain compressed data to be write
  DWORD    dosDate;
  DWORD    crc32;
  // central header data for the current file
  LPZIP_CENTRAL_HEADER central_header;
} CURFILE_INFO, *PCURFILE_INFO, *LPCURFILE_INFO;


typedef struct _ZIP_INTERNAL {
  FILE            *filezip;
  CENTRAL_DIR      central_dir;          // datablock with central dir in construction
  int              in_opened_file_inzip; // 1 if a file in the zip is currently write
  CURFILE_INFO     ci;                   // info on the file curretly writing
  DWORD            begin_pos;            // position of the beginning of the HZIPFILE
  DWORD            number_entry;
} ZIP_INTERNAL, *PZIP_INTERNAL, *LPZIP_INTERNAL;


#endif // _ZIP_LOCAL_H
