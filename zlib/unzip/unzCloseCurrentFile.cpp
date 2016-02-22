
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"

/*
  Close the file in zip opened with unzipOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
ZEXTERN int ZEXPORT unzCloseCurrentFile(HUNZFILE file)
{
	int err = UNZ_OK;
	LPUNZ_INTERNAL s;
	FILE_IN_ZIP_READ_INFO_S* pfile_in_zip_read_info;

  if(!file) return UNZ_PARAMERROR;
	s=(LPUNZ_INTERNAL)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if(!pfile_in_zip_read_info) return UNZ_PARAMERROR;


	if(pfile_in_zip_read_info->rest_read_uncompressed == 0)
	{
		if(pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
			err = UNZ_CRCERROR;
	}


	TRYFREE(pfile_in_zip_read_info->read_buffer);
	pfile_in_zip_read_info->read_buffer = NULL;
	if (pfile_in_zip_read_info->stream_initialised)
		inflateEnd(&pfile_in_zip_read_info->stream);

	pfile_in_zip_read_info->stream_initialised = 0;
	TRYFREE(pfile_in_zip_read_info);

    s->pfile_in_zip_read=NULL;

	return err;
}


