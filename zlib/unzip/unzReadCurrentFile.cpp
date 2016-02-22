
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"

/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return < 0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
ZEXTERN int ZEXPORT unzReadCurrentFile(HUNZFILE file, voidp buf,
                                      unsigned len)
{
	int    err = UNZ_OK;
	uInt   iRead = 0;
  uInt   uReadThis = UNZ_BUFSIZE;
	LPUNZ_INTERNAL s;
	uLong  uTotalOutBefore, uTotalOutAfter;
	const  Bytef *bufBefore;
	uLong  uOutThis;
	int    flush = Z_SYNC_FLUSH;
	FILE_IN_ZIP_READ_INFO_S *pfile_in_zip_read_info;

  if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
  pfile_in_zip_read_info = s->pfile_in_zip_read;

	if(!pfile_in_zip_read_info) return UNZ_PARAMERROR;

	if(!pfile_in_zip_read_info->read_buffer)
		return UNZ_END_OF_LIST_OF_FILE;
	if(!len) return 0;

	pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;

	pfile_in_zip_read_info->stream.avail_out = (uInt)len;
	
	if(len > pfile_in_zip_read_info->rest_read_uncompressed)
		pfile_in_zip_read_info->stream.avail_out = 
		  (uInt)pfile_in_zip_read_info->rest_read_uncompressed;

	while(pfile_in_zip_read_info->stream.avail_out > 0)
	{
		if(!pfile_in_zip_read_info->stream.avail_in &&
       (pfile_in_zip_read_info->rest_read_compressed > 0))
		{
			if (pfile_in_zip_read_info->rest_read_compressed<uReadThis)
				uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;

      if(!uReadThis) return UNZ_EOF;
			if(fseek(pfile_in_zip_read_info->file,
               pfile_in_zip_read_info->pos_in_zipfile + 
               pfile_in_zip_read_info->byte_before_the_zipfile, SEEK_SET))
				return UNZ_ERRNO;
			if (fread(pfile_in_zip_read_info->read_buffer,uReadThis,1,
                pfile_in_zip_read_info->file) != 1)
				return UNZ_ERRNO;
			pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

			pfile_in_zip_read_info->rest_read_compressed -= uReadThis;
			
			pfile_in_zip_read_info->stream.next_in = 
                (Bytef*)pfile_in_zip_read_info->read_buffer;
			pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
		}

		if(!pfile_in_zip_read_info->compression_method)
		{
			uInt uDoCopy,i ;
			if (pfile_in_zip_read_info->stream.avail_out < 
                            pfile_in_zip_read_info->stream.avail_in)
				uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
			else
				uDoCopy = pfile_in_zip_read_info->stream.avail_in ;
				
			for(i = 0; i < uDoCopy; i++)
				*(pfile_in_zip_read_info->stream.next_out+i) =
        *(pfile_in_zip_read_info->stream.next_in+i);
					
			pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->crc32,
								pfile_in_zip_read_info->stream.next_out,
								uDoCopy);
			pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
			pfile_in_zip_read_info->stream.next_out += uDoCopy;
			pfile_in_zip_read_info->stream.next_in += uDoCopy;
            pfile_in_zip_read_info->stream.total_out += uDoCopy;
			iRead += uDoCopy;
		}
		else
		{
			uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
			bufBefore = pfile_in_zip_read_info->stream.next_out;

			/*
			if ((pfile_in_zip_read_info->rest_read_uncompressed ==
			         pfile_in_zip_read_info->stream.avail_out) &&
				(pfile_in_zip_read_info->rest_read_compressed == 0))
				flush = Z_FINISH;
			*/
			err = inflate(&pfile_in_zip_read_info->stream, flush);

			uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
			uOutThis = uTotalOutAfter-uTotalOutBefore;

			pfile_in_zip_read_info->crc32 = 
        crc32(pfile_in_zip_read_info->crc32, bufBefore,
             (uInt)(uOutThis));

			pfile_in_zip_read_info->rest_read_uncompressed -=
                uOutThis;

			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
            
			if(err == Z_STREAM_END)
        return(iRead == 0) ? UNZ_EOF : iRead;
			if(err != Z_OK)  break;

		}
	}

	if(err == Z_OK) return iRead;

	return err;
}


