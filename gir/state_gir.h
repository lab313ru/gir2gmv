
#ifndef SAVE_H_
#define SAVE_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GENESIS_STATE_FILE_LENGHT    0x22478
#define GENESIS_STATE_FILE_LENGHT_EX 0x25550
#define SEGACD_STATE_FILE_LENGHT     (0x22500 + 0xE0000)
#define G32X_STATE_FILE_LENGHT       (0x22500 + 0x82A00)
#define SCD32X_STATE_FILE_LENGHT     (0x22500 + 0xE0000 + 0x82A00)
#define MAX_STATE_FILE_LENGHT        SCD32X_STATE_FILE_LENGHT

#define MINISHOT_ID                  0x00534D47
#define MINISHOT_SX                  80
#define MINISHOT_SY                  60
#define MINISHOT_SIZE                (MINISHOT_SX * MINISHOT_SY * 2)

#ifdef __cplusplus
};
#endif

#endif