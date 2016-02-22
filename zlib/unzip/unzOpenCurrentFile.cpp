
/* UNZIP IO - Rodrigo Cardoso
   requires zLib */

#include "unzLocal.h"

/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
ZEXTERN int ZEXPORT unzOpenCurrentFile(HUNZFILE file)
{
	LPFILE_IN_ZIP_READ_INFO_S pfile_in_zip_read_info;
	LPUNZ_INTERNAL s;
	int   err = UNZ_OK;
	int   Store;
	UINT  iSizeVar;
	DWORD offset_local_extrafield; // offset of the extra field
	UINT  size_local_extrafield;   // size of the extra field

	if(!file) return UNZ_PARAMERROR;
	s = (LPUNZ_INTERNAL)file;
	if(!s->current_file_ok) return UNZ_PARAMERROR;

  if(s->pfile_in_zip_read) unzCloseCurrentFile(file);

	if(unzLocal_CheckCurrentFileCoherencyHeader(s, &iSizeVar,
       &offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
		return UNZ_BADZIPFILE;

	pfile_in_zip_read_info = (FILE_IN_ZIP_READ_INFO_S*)
									    ALLOC(sizeof(FILE_IN_ZIP_READ_INFO_S));
	if (pfile_in_zip_read_info==NULL)
		return UNZ_INTERNALERROR;

	pfile_in_zip_read_info->read_buffer=(char*)ALLOC(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield=0;

	if (pfile_in_zip_read_info->read_buffer==NULL)
	{
		TRYFREE(pfile_in_zip_read_info);
		return UNZ_INTERNALERROR;
	}

	pfile_in_zip_read_info->stream_initialised=0;
	
	if(s->cur_file_info.wMethod &&
     s->cur_file_info.wMethod != Z_DEFLATED) err = UNZ_BADZIPFILE;
	Store = s->cur_file_info.wMethod==0;

	pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc32;
	pfile_in_zip_read_info->crc32=0;
	pfile_in_zip_read_info->compression_method =
            s->cur_file_info.wMethod;
	pfile_in_zip_read_info->file=s->file;
	pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;

    pfile_in_zip_read_info->stream.total_out = 0;

	if (!Store)
	{
	  err = inflateInit(&pfile_in_zip_read_info->stream, -MAX_WBITS);
	  if (err == Z_OK)
	    pfile_in_zip_read_info->stream_initialised=1;
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. 
         * In unzip, i don't wait absolutely Z_STREAM_END because I known the 
         * size of both compressed and uncompressed data
         */
	}
	pfile_in_zip_read_info->rest_read_compressed = 
    s->cur_file_info.dwCompSize;
	pfile_in_zip_read_info->rest_read_uncompressed = 
    s->cur_file_info.dwUncompSize;

	
	pfile_in_zip_read_info->pos_in_zipfile = 
            s->cur_file_info.dwPosLocalHeader + sizeof(ZIP_LOCAL_HEADER) + 
			  iSizeVar;
	
	pfile_in_zip_read_info->stream.avail_in = (uInt)0;


	s->pfile_in_zip_read = pfile_in_zip_read_info;
    return UNZ_OK;
}
