
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#ifndef _UNZLOCAL_H
#define _UNZLOCAL_H


#include "unzip.h"
#include "zipLocal.h"
#include <stdio.h>

#ifndef  UNZ_BUFSIZE
# define UNZ_BUFSIZE 4096
#endif

#ifndef  UNZ_MAXFILENAMEINZIP
# define UNZ_MAXFILENAMEINZIP MAX_PATH
#endif

/* FILE_IN_ZIP_READ_INFO_S contain internal information about a file in zipfile,
   when reading and decompress it */
typedef struct _FILE_IN_ZIP_READ_INFO_S
{
  LPSTR    read_buffer;          // internal buffer for compressed data
  Z_STREAM stream;               // zLib stream structure for inflate
  DWORD pos_in_zipfile;          // position in byte on the zipfile, for fseek
  DWORD stream_initialised;      // flag set if stream structure is initialised
  DWORD offset_local_extrafield; // offset of the extra field
  UINT  size_local_extrafield;   // size of the extra field
  DWORD pos_local_extrafield;    // position in the extra field in read
  DWORD crc32;                   // crc32 of all data uncompressed
  DWORD crc32_wait;              // crc32 we must obtain after decompress all
  DWORD rest_read_compressed;    // number of byte to be decompressed
  DWORD rest_read_uncompressed;  // number of byte to be obtained after decomp
  FILE *file;                    // io structore of the zipfile
  DWORD compression_method;      // compression method (0==store)
  DWORD byte_before_the_zipfile; // byte before the zipfile, (>0 for sfx)
} FILE_IN_ZIP_READ_INFO_S, *PFILE_IN_ZIP_READ_INFO_S,
  *LPFILE_IN_ZIP_READ_INFO_S;


// UNZ_INTERNAL contain internal information about the zipfile
typedef struct _UNZ_INTERNAL {
  FILE *file;                       // io structore of the zipfile
  UNZ_GLOBAL_INFO gi;               // public global information
  DWORD byte_before_the_zipfile;    // byte before the zipfile, (>0 for sfx)
  DWORD num_file;                   // number of the current file in the zipfile
  DWORD pos_in_central_dir;         // pos of the current file in the central dir
  BOOL  current_file_ok;            // flag about the usability of the current file
  DWORD central_pos;                // position of the beginning of the central dir
  DWORD size_central_dir;           // size of the central directory
  DWORD offset_central_dir;         // offset of start of central directory with
								                    // respect to the starting disk number
  ZIP_CENTRAL_HEADER cur_file_info; // info about the current file in zip
  FILE_IN_ZIP_READ_INFO_S* pfile_in_zip_read;  // structure about the current
	                                             // file if we are decompressing it
} UNZ_INTERNAL;

typedef struct _PUNZ_INTERNAL : _UNZ_INTERNAL {
  char     pszFilePath[4];       // path to the opened file
} *PUNZ_INTERNAL, *LPUNZ_INTERNAL;


// internal functions (for internal use only)
DWORD unzLocal_GetEndHeader(FILE *, ZIP_END_HEADER *);
int   unzLocal_GetCurrentFileInfoInternal(HUNZFILE,
  ZIP_CENTRAL_HEADER *, LPSTR, DWORD, LPVOID, DWORD, LPSTR, DWORD);
int   unzLocal_CheckCurrentFileCoherencyHeader(LPUNZ_INTERNAL,
  LPUINT, LPDWORD, LPUINT);
// opens the file for an internal structure already alloc
int   unzLocal_OpenInternal(LPUNZ_INTERNAL s);


#endif // _UNZLOCAL_H