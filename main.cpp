#include "movie_gir.h"
#include "state_gir.h"
#include "gdinput_gir.h"
#include "gio_gir.h"
#include "zlib.h"

#include "movie_gmv.h"

BYTE g_sram[64 * 1024] = { 0 };
BYTE g_save[MAX_STATE_FILE_LENGHT] = { 0 };

LPCONTROL_KEYS Control = (LPCONTROL_KEYS)(&Controller_1_Type);

// this function is called when the user choose play movie in the menu
int StartPlayMovie(const char *filename, GMOVIEFILEHDR &girHeader, GZFILE &girMovie, LPBYTE save, LPBYTE sram)
{
	GZFILE        file;
	int           i;
	int           status = MOVIE_FINISHED;

	file = gzopen(filename, "rb"); // trying to open the file
	if (!file) return 0;
	// trying to read the file header
	if (gzread(file, &girHeader, sizeof(GMOVIEFILEHDR)) != sizeof(GMOVIEFILEHDR))
	{
		gzclose(file); return 0;
	}
	// check the magic and file version
	if (girHeader.dwMagic != GMOVIE_FILE_TYPE || girHeader.dwVersion != GMOVIE_FILE_VERSION)
	{
		gzclose(file); return 0;
	}
	// if the file has a state, we try to read it
	if (girHeader.dwFlags & GMVF_USESTATE)
	{
		if (gzread(file, save, girHeader.uStateSize) != girHeader.uStateSize)
		{
			gzclose(file); return 0;
		}
	}
	// SRAM
	if (girHeader.dwFlags & GMVF_USESRAM)
	{
		memset(sram, 0, 64 * 1024);
		if (gzread(file, sram, girHeader.uSRAMSize) != girHeader.uSRAMSize)
		{
			gzclose(file); return 0;
		}
	}

	status = MOVIE_PLAYING;
	if (girHeader.dwFlags & GMVF_ALLJOYS) status |= MOVIE_ALLJOYS;
	girMovie = file;
	// saving the original joy configuration and put the new one
	for (i = 0; i < 8; i++)
	{
		Control[i].Type = girHeader.wPlayerType[i];
	}

	return status;           // everything ok, we can start to play the file
}

// stops the movie playing
void StopPlayMovie(int &status, GZFILE &girMovie)
{
	gzclose(girMovie);
	girMovie = NULL;
	status = MOVIE_FINISHED;
}

void MoviePlayingStuff(int &status, GZFILE &girMovie, UINT &frameCount, UINT lastFrame)
{
	int  Pad_Data_Size, i;
	char PadData[8];

	// Check_Misc_Key();  //Quicksave, quickload, slowdown
	if (status & MOVIE_ALLJOYS) Pad_Data_Size = 12;
	else                        Pad_Data_Size = 3;
	// reading the next input from the file
	if (gzread(girMovie, PadData, Pad_Data_Size) != Pad_Data_Size)
	{
		StopPlayMovie(status, girMovie);
		return;
	} // error reading file

	  // you made a mistake here, the controller one is Control[0] not Control[1], fixed
	Control[0].Up = (PadData[0] & 1);
	Control[0].Down = (PadData[0] & 2) >> 1;
	Control[0].Left = (PadData[0] & 4) >> 2;
	Control[0].Right = (PadData[0] & 8) >> 3;
	Control[0].A = (PadData[0] & 16) >> 4;
	Control[0].B = (PadData[0] & 32) >> 5;
	Control[0].C = (PadData[0] & 64) >> 6;
	Control[0].Start = (PadData[0] & 128) >> 7;
	Control[1].Up = (PadData[1] & 1);
	Control[1].Down = (PadData[1] & 2) >> 1;
	Control[1].Left = (PadData[1] & 4) >> 2;
	Control[1].Right = (PadData[1] & 8) >> 3;
	Control[1].A = (PadData[1] & 16) >> 4;
	Control[1].B = (PadData[1] & 32) >> 5;
	Control[1].C = (PadData[1] & 64) >> 6;
	Control[1].Start = (PadData[1] & 128) >> 7;
	Control[0].X = (PadData[2] & 1);
	Control[0].Y = (PadData[2] & 2) >> 1;
	Control[0].Z = (PadData[2] & 4) >> 2;
	Control[0].Mode = (PadData[2] & 8) >> 3;
	Control[1].X = (PadData[2] & 16) >> 4;
	Control[1].Y = (PadData[2] & 32) >> 5;
	Control[1].Z = (PadData[2] & 64) >> 6;
	Control[1].Mode = (PadData[2] & 128) >> 7;

	// this is to suport games that uses more then two joys
	if (status & MOVIE_ALLJOYS)
	{
		for (i = 2; i < 8; i += 2)
		{
			Control[i].Up = (PadData[(i >> 1) * 3] & 1);
			Control[i].Down = (PadData[(i >> 1) * 3] & 2) >> 1;
			Control[i].Left = (PadData[(i >> 1) * 3] & 4) >> 2;
			Control[i].Right = (PadData[(i >> 1) * 3] & 8) >> 3;
			Control[i].A = (PadData[(i >> 1) * 3] & 16) >> 4;
			Control[i].B = (PadData[(i >> 1) * 3] & 32) >> 5;
			Control[i].C = (PadData[(i >> 1) * 3] & 64) >> 6;
			Control[i].Start = (PadData[(i >> 1) * 3] & 128) >> 7;
			Control[i + 1].Up = (PadData[(i >> 1) * 3 + 1] & 1);
			Control[i + 1].Down = (PadData[(i >> 1) * 3 + 1] & 2) >> 1;
			Control[i + 1].Left = (PadData[(i >> 1) * 3 + 1] & 4) >> 2;
			Control[i + 1].Right = (PadData[(i >> 1) * 3 + 1] & 8) >> 3;
			Control[i + 1].A = (PadData[(i >> 1) * 3 + 1] & 16) >> 4;
			Control[i + 1].B = (PadData[(i >> 1) * 3 + 1] & 32) >> 5;
			Control[i + 1].C = (PadData[(i >> 1) * 3 + 1] & 64) >> 6;
			Control[i + 1].Start = (PadData[(i >> 1) * 3 + 1] & 128) >> 7;
			Control[i].X = (PadData[(i >> 1) * 3 + 2] & 1);
			Control[i].Y = (PadData[(i >> 1) * 3 + 2] & 2) >> 1;
			Control[i].Z = (PadData[(i >> 1) * 3 + 2] & 4) >> 2;
			Control[i].Mode = (PadData[(i >> 1) * 3 + 2] & 8) >> 3;
			Control[i + 1].X = (PadData[(i >> 1) * 3 + 2] & 16) >> 4;
			Control[i + 1].Y = (PadData[(i >> 1) * 3 + 2] & 32) >> 5;
			Control[i + 1].Z = (PadData[(i >> 1) * 3 + 2] & 64) >> 6;
			Control[i + 1].Mode = (PadData[(i >> 1) * 3 + 2] & 128) >> 7;
		}
	}

	// end of the movie
	if (frameCount >= lastFrame) StopPlayMovie(status, girMovie);
}

// GMV
void InitMovie(DWORD dwFramesPerSec, typeMovie &gmvMovie)
{
	gmvMovie.File = NULL;
	strcpy(gmvMovie.FileName, "");
	strcpy(gmvMovie.PhysicalFileName, "");
	gmvMovie.LastFrame = 0;
	gmvMovie.NbRerecords = 0;
	strcpy(gmvMovie.Note, "");
	gmvMovie.PlayerConfig[0] = 6;
	gmvMovie.PlayerConfig[1] = 3;
	gmvMovie.ReadOnly = 0;
	strcpy(gmvMovie.StateName, "");
	gmvMovie.Status = 0;
	gmvMovie.UseState = 0;
	gmvMovie.Version = 'A' - '0';
	gmvMovie.Ok = 0;
	gmvMovie.StateFrame = 0;
	gmvMovie.StateOk = 0;
	gmvMovie.Vfreq = (dwFramesPerSec == 60) ? 1 : 0;
	gmvMovie.StateRequired = 0;
	gmvMovie.TriplePlayerHack = 0;
	gmvMovie.Type = TYPEGMV;
	gmvMovie.Recorded = false;
	gmvMovie.ClearSRAM = true;
}

int OpenMovieFile(typeMovie &gmvMovie)
{
	if (!gmvMovie.Ok)
		return 0;

	// use ObtainFile to support loading movies from archives (read-only)
	char LogicalName[1024], PhysicalName[1024];
	strcpy(LogicalName, gmvMovie.FileName);
	strcpy(PhysicalName, gmvMovie.FileName);

	gmvMovie.File = fopen(PhysicalName, "r+b"); // so we can toggle readonly later without re-opening file
	if (!gmvMovie.File)
	{
		gmvMovie.File = fopen(PhysicalName, "rb");
		gmvMovie.ReadOnly = 2; // really read-only
	}
	strncpy(gmvMovie.PhysicalFileName, PhysicalName, 1024);

	if (!gmvMovie.File)
		return 0;

	return 1;
}

void RecordMovie(GMOVIEFILEHDR &girHeader, typeMovie &gmvMovie, const char *outName)
{
	gmvMovie.Status = 0;

	InitMovie(girHeader.dwFramesPerSec, gmvMovie);
	gmvMovie.ReadOnly = 0;
	gmvMovie.Type = TYPEGMV;
	gmvMovie.Version = 'A' - '0';
	strncpy(gmvMovie.FileName, outName, 512);
	strncpy(gmvMovie.Note, "Converted from Gens+ GIR", 40);
	gmvMovie.Note[40] = 0;
	strncpy(gmvMovie.Header, "Gens Movie TESTA", 17);
	gmvMovie.Status = MOVIE_RECORDING;

	if ((Controller_1_Type & 1) == 1)
		gmvMovie.PlayerConfig[0] = 6;
	else
		gmvMovie.PlayerConfig[0] = 3;
	if ((Controller_2_Type & 1) == 1)
		gmvMovie.PlayerConfig[1] = 6;
	else
		gmvMovie.PlayerConfig[1] = 3;

	if (girHeader.dwFlags & GMVF_USESTATE)
	{
		gmvMovie.StateRequired = 1;
	}

	gmvMovie.File = fopen(gmvMovie.FileName, "wb");

	gmvMovie.Ok = 1;
	fseek(gmvMovie.File, 0, SEEK_SET);
	fwrite(gmvMovie.Header, 16, 1, gmvMovie.File);
	fseek(gmvMovie.File, 24, SEEK_SET);
	fwrite(gmvMovie.Note, 40, 1, gmvMovie.File);
	fclose(gmvMovie.File);
	gmvMovie.File = NULL;
	if (OpenMovieFile(gmvMovie) == 0)
	{
		gmvMovie.Status = 0;
		return;
	}

	gmvMovie.NbRerecords = 0;
	gmvMovie.LastFrame = 0;
}

void MovieRecordingStuff(UINT frameCount, typeMovie &gmvMovie)
{
	char PadData[3];

	PadData[0] = Controller_1_Up | (Controller_1_Down << 1) | (Controller_1_Left << 2) | (Controller_1_Right << 3)
		| (Controller_1_A << 4) | (Controller_1_B << 5) | (Controller_1_C << 6) | (Controller_1_Start << 7);
	PadData[1] = Controller_2_Up | (Controller_2_Down << 1) | (Controller_2_Left << 2) | (Controller_2_Right << 3)
		| (Controller_2_A << 4) | (Controller_2_B << 5) | (Controller_2_C << 6) | (Controller_2_Start << 7);
	PadData[2] = Controller_1_X | (Controller_1_Y << 1) | (Controller_1_Z << 2) | (Controller_1_Mode << 3)
		| (Controller_2_X << 4) | (Controller_2_Y << 5) | (Controller_2_Z << 6) | (Controller_2_Mode << 7);
	fseek(gmvMovie.File, 64 + frameCount * 3, SEEK_SET);
	fwrite(PadData, 3, 1, gmvMovie.File);
	
	gmvMovie.LastFrame = frameCount;
}

void WriteMovieHeader(DWORD dwFramesPerSec, typeMovie &gmvMovie)
{
	unsigned char t;

	fseek(gmvMovie.File, 16, SEEK_SET);
	fwrite((char*)&gmvMovie.NbRerecords, sizeof(gmvMovie.NbRerecords), 1, gmvMovie.File);
	if (gmvMovie.Version >= 9)
	{
		fseek(gmvMovie.File, 20, SEEK_SET);
		if ((Controller_1_Type & 1) == 1)
			fputc('6', gmvMovie.File);
		else
			fputc('3', gmvMovie.File);
		if ((Controller_2_Type & 1) == 1)
			fputc('6', gmvMovie.File);
		else
			fputc('3', gmvMovie.File);
		if (gmvMovie.Version >= 'A' - '0')
		{
			fseek(gmvMovie.File, 22, SEEK_SET);
			t = fgetc(gmvMovie.File);
			t = (t & 127) + ((dwFramesPerSec == 60) ? 128 : 0);
			t = (t & 191) + ((gmvMovie.StateRequired) ? 64 : 0);
			t = (t & 223) + ((gmvMovie.TriplePlayerHack) ? 32 : 0);
			fseek(gmvMovie.File, 22, SEEK_SET);
			fputc(t, gmvMovie.File);
		}
		fseek(gmvMovie.File, 24, SEEK_SET);
		fwrite(gmvMovie.Note, 40, 1, gmvMovie.File);
	}
}

int FlushMovieFile(DWORD dwFramesPerSec, typeMovie &gmvMovie)
{
	unsigned int MovieFileLastFrame = 0;
	char * movieData = NULL;

	if (gmvMovie.File == NULL)
		return 0;
	if (gmvMovie.ReadOnly == 0 || gmvMovie.Status == MOVIE_RECORDING)
	{
		WriteMovieHeader(dwFramesPerSec, gmvMovie);
		fseek(gmvMovie.File, 0, SEEK_END);
		MovieFileLastFrame = (ftell(gmvMovie.File) - 64) / 3;
		if (MovieFileLastFrame > gmvMovie.LastFrame)
		{
			fseek(gmvMovie.File, 0, SEEK_SET);
			movieData = new char[gmvMovie.LastFrame * 3 + 64];
			fread(movieData, gmvMovie.LastFrame * 3 + 64, 1, gmvMovie.File);
		}
	}

	fclose(gmvMovie.File);
	gmvMovie.File = NULL;

	// if necessary, truncate the frame data to match the length of the movie
	if ((MovieFileLastFrame > gmvMovie.LastFrame) && (gmvMovie.ReadOnly == 0 || gmvMovie.Status == MOVIE_RECORDING))
	{
		gmvMovie.File = fopen(gmvMovie.PhysicalFileName, "wb");
		if (gmvMovie.File)
		{
			fseek(gmvMovie.File, 0, SEEK_SET);
			fwrite(movieData, gmvMovie.LastFrame * 3 + 64, 1, gmvMovie.File);
			fclose(gmvMovie.File);
		}
	}

	delete[] movieData;
	return 1;
}

int CloseMovieFile(DWORD dwFramesPerSec, typeMovie &gmvMovie)
{
	if (!FlushMovieFile(dwFramesPerSec, gmvMovie))
		return 0;

	char status = gmvMovie.Status;
	InitMovie(dwFramesPerSec, gmvMovie);
	gmvMovie.StateFrame = status;

	return 1;
}

int main(int argc, char *argv[])
{
	if (argc < 2) return -1;

    char *ext = strrchr(argv[1], '.');

	GMOVIEFILEHDR girHeader;
	GZFILE girMovie = NULL;
	UINT frameCount = 0; // Number of frame elapsed since reset
	UINT lastFrame = 0;

	typeMovie gmvMovie;

	int status = StartPlayMovie(argv[1], girHeader, girMovie, g_save, g_sram);
	
    sprintf(&ext[1], "gmv");
    RecordMovie(girHeader, gmvMovie, argv[1]);

	lastFrame = girHeader.dwFrames;
	while (status != MOVIE_FINISHED)
	{
		MoviePlayingStuff(status, girMovie, frameCount, lastFrame);
		MovieRecordingStuff(frameCount, gmvMovie);
		frameCount++;
	}

	if (girHeader.dwFlags & GMVF_USESTATE)
	{
        sprintf(&ext[1], "gst");
		FILE *gst = fopen(argv[1], "wb");
		fwrite(g_save, 1, girHeader.uStateSize, gst);
		fwrite(&status, 1, sizeof(lastFrame), gst); // special hack for gensRR
		fclose(gst);
	}

	if (girHeader.dwFlags & GMVF_USESRAM)
	{
        sprintf(&ext[1], "srm");
		FILE *srm = fopen(argv[1], "wb");
		fwrite(g_sram, 1, girHeader.uSRAMSize, srm);
		fclose(srm);
	}

	CloseMovieFile(girHeader.dwFramesPerSec, gmvMovie);

	return 0;
}