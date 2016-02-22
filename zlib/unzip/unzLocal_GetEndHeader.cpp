
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"


#define BACKREAD 0x0400

/* Find the end header of the zip file by searching for the end
   header magic. If found, return the position of the begining
   of the structure in the zip file and the values of the header
   in pzeh (if pzeh not NULL), the file pointer remains at the
   begining of the coment. */
DWORD unzLocal_GetEndHeader(FILE *file, ZIP_END_HEADER *pzeh)
{
	DWORD  uSizeFile, uBackRead;
  // maximum size of global comment plus the end header
	DWORD  uMaxBack  = 0xFFFF + sizeof(ZIP_END_HEADER);
	DWORD  uPosFound = 0;
  DWORD  dwBuf;
  long   lReadSize, lReadPos;
  long   i;
  int    ch;

	if(fseek(file, 0, SEEK_END)) return 0;
	uSizeFile = ftell(file); // the file size

  if(uSizeFile < sizeof(ZIP_END_HEADER)) return 0;

	if(uMaxBack > uSizeFile) uMaxBack = uSizeFile;

	uBackRead = sizeof(ZIP_END_HEADER);
  lReadSize = 4;
	while(TRUE)
	{
		lReadPos  = uSizeFile - uBackRead;		
		if(fseek(file, lReadPos, SEEK_SET)) break;
    if(fread(&dwBuf, 3, 1, file) != 1) break;
    dwBuf = dwBuf << 8;

    for(i = 3; i < lReadSize; i++)
    {
      ch = getc(file);
      if(ch == EOF) return 0;
      dwBuf = (dwBuf >> 8) | ((DWORD)(ch & 0xFF) << 24);

      // find the magic!!
      if(dwBuf == ENDHEADERMAGIC)
      {
        // reading the header
        if(pzeh)
        {
          pzeh->dwMagic = dwBuf;
          if(fread(((LPDWORD)pzeh) + 1, sizeof(ZIP_END_HEADER) - 
               sizeof(DWORD), 1, file) != 1) return 0;
        }
        return (lReadPos + i - 3);
      }
    }

    if(uBackRead == uMaxBack) // haven´t found the header
      break;
		else if(uBackRead + BACKREAD > uMaxBack)
      uBackRead = uMaxBack;
		else
      uBackRead += BACKREAD;

    lReadSize = ((BACKREAD + 4) < uBackRead) ?
                 (BACKREAD + 4) : uBackRead;
	}

	return 0;
}

