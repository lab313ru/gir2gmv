#ifndef _H_GIR_MOVIE_
#define _H_GIR_MOVIE_

#include <windows.h>
#include "grom_gir.h"

#define GMOVIE_FILE_TYPE    0x00564D47
#define GMOVIE_FILE_VERSION 1

// this header comes in the begining of the movie file
typedef struct _GMOVIEFILEHDR {
  DWORD    dwMagic;         // identifies the file (must be GMOVIE_FILE_TYPE)
  DWORD    dwVersion;       // file version (GMOVIE_FILE_VERSION)
  DWORD    dwFlags;         // flags for the file
  DWORD    dwFrames;        // total number of input records in the file
  DWORD    dwFramesPerSec;  // frames per second (60 for NTSC, 50 for PALM)
  BYTE     wPlayerType[8];  // player controler type (3 - 3 buttons, 6 - 6)
  UINT     uStateSize;      // state size, this is valid only when the flag GMVF_USESTATE is set, otherwise is zero
  UINT     uSRAMSize;       // size of SRAM saved (only if GMVF_USESRAM flag is set)
} GMOVIEFILEHDR, *LPGMOVIEFILEHDR;

// valid flags for the file
// if the GMVF_USESTATE flag is set the current state data follows the GMOVIEFILEHDR structure
#define GMVF_USESTATE  0x01 // the movie file has a state file (inside it)
#define GMVF_ALLJOYS   0x02 // the file uses eight joysticks input
#define GMVF_USESRAM   0x04 // the SRAM is saved in the file

  // movie valid states
#define MOVIE_FINISHED  0x00
#define MOVIE_PLAYING   0x01
#define MOVIE_RECORDING 0x02
#define MOVIE_ALLJOYS   0x10 // to suport 4 joysticks

#endif
