#ifndef G_INPUT_H
#define G_INPUT_H

#define DIRECTINPUT_VERSION 0x0500  // for joystick support

#include <dinput.h>
//#include <mmsystem.h>

#ifdef __cplusplus
extern "C" {
#endif

struct K_Def {
  unsigned int  Start, Mode;
  unsigned int  A, B, C;
  unsigned int  X, Y, Z;
  unsigned int  Up, Down, Left, Right;
  unsigned int  HotKey[4];
  unsigned long HotKey_Buttons[4];
  };

#define HKB_UP     0x0001
#define HKB_LEFT   0x0002
#define HKB_RIGHT  0x0004
#define HKB_DOWN   0x0008
#define HKB_A      0x0010
#define HKB_B      0x0020
#define HKB_C      0x0040
#define HKB_X      0x0080
#define HKB_Y      0x0100
#define HKB_Z      0x0200
#define HKB_START  0x0400
#define HKB_MODE   0x0800

typedef struct _CONTROL_KEYS {
  UINT Type;
  UINT Up;
  UINT Down;
  UINT Left;
  UINT Right;
  UINT Start;
  UINT Mode;
  UINT A;
  UINT B;
  UINT C;
  UINT X;
  UINT Y;
  UINT Z;
} CONTROL_KEYS, *PCONTROL_KEYS, *LPCONTROL_KEYS;

extern LPCONTROL_KEYS Control;

extern struct K_Def Keys_Def[8];
extern unsigned char Kaillera_Keys[16];

#ifdef __cplusplus
}
#endif

#endif