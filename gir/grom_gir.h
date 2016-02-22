
#ifndef ROM_H
#define ROM_H

#include <windows.h>
#include "config_gir.h"

#define GENESIS_ROM      0x02
#define _32X_ROM         0x04
#define SMS_ROM          0x06
#define GG_ROM           0x08
#define SEGACD_IMAGE     0x0A
#define SEGACD_32X_IMAGE 0x0C


typedef struct _GENESIS_ROM_INFO {
  char Console_Name[20];
  char Copyright[20];
  char Rom_Name[52];
  char Rom_Name_W[52];
  char Type[4];
  char Version[16];
  UINT Checksum;
  char IO_Support[20];
  UINT Rom_Start_Adress;
  UINT Rom_End_Adress;
  UINT R_Size;
  char Ram_Infos[16];
  UINT Ram_Start_Adress;
  UINT Ram_End_Adress;
  char Modem_Infos[16];
  char Description[44];
  char Countries[4];
} GENESIS_ROM_INFO, *PGENESIS_ROM_INFO, *LPGENESIS_ROM_INFO;

typedef struct _SMS_ROM_INFO {
  int un;
} SMS_ROM_INFO, *PSMS_ROM_INFO, *LPSMS_ROM_INFO;

typedef struct _GROM_INFO {
  int   Type;
  union {
    GENESIS_ROM_INFO Genesis;
    SMS_ROM_INFO     SMS;
  };
} GROM_INFO, *PGROM_INFO, *LPGROM_INFO;


#ifdef __cplusplus
extern "C" {
#endif

extern int File_Type_Index;
extern LPGROM_INFO Game;

#define G_MAX_ROM_NAME (GMAX_PATH - 64)

extern char Rom_Name[G_MAX_ROM_NAME];

#ifdef __cplusplus
};
#endif

#endif
