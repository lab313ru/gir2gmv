#ifndef _H_GMV_MOVIE_
#define _H_GMV_MOVIE_

#define TYPEGMV 0
#define TYPEGM2 1
#define TYPEGM2TEXT 2
#define TRACK1 1
#define TRACK2 2
#define TRACK3 4
#define TRACK1_2 (TRACK1 | TRACK2)
#define TRACK1_3 (TRACK1 | TRACK3)
#define TRACK2_3 (TRACK2 | TRACK3)
#define ALL_TRACKS (TRACK1 | TRACK2 | TRACK3)

struct typeMovie
{
    int Status;
    FILE *File;
    int ReadOnly;
    unsigned int LastFrame;
    char FileName[1024];
    int UseState;
    char StateName[1024];
    unsigned int StateFrame;
    char Header[17];
    unsigned int NbRerecords;
    char Note[41];
    int PlayerConfig[8];
    int Version;
    int Ok;
    int StateOk;
    int Vfreq;		//' 0 =60Hz  1=50Hz
    int StateRequired; // ' 0= No savestate required 1=Savestate required
    int TriplePlayerHack; // 0=No triple player hack 1=yes
    unsigned char Type;
    bool Recorded;
    bool ClearSRAM;
    char PhysicalFileName[1024];
};

#endif
