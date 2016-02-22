/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zutil.h"

#ifndef NO_DUMMY_DECL
struct internal_state      {int dummy;}; /* for buggy compilers */
#endif

#ifndef STDC
extern void exit OF((int));
#endif

uLong ZEXPORT zlibCompileFlags()
{
    uLong flags;

    flags = 0;
    switch (sizeof(uInt)) {
    case 2:     break;
    case 4:     flags += 1;     break;
    case 8:     flags += 2;     break;
    default:    flags += 3;
    }
    switch (sizeof(uLong)) {
    case 2:     break;
    case 4:     flags += 1 << 2;        break;
    case 8:     flags += 2 << 2;        break;
    default:    flags += 3 << 2;
    }
    switch (sizeof(voidpf)) {
    case 2:     break;
    case 4:     flags += 1 << 4;        break;
    case 8:     flags += 2 << 4;        break;
    default:    flags += 3 << 4;
    }
    switch (sizeof(z_off_t)) {
    case 2:     break;
    case 4:     flags += 1 << 6;        break;
    case 8:     flags += 2 << 6;        break;
    default:    flags += 3 << 6;
    }
#ifdef DEBUG
    flags += 1 << 8;
#endif
#if defined(ASMV) || defined(ASMINF)
    flags += 1 << 9;
#endif
#ifdef ZLIB_WINAPI
    flags += 1 << 10;
#endif
#ifdef BUILDFIXED
    flags += 1 << 12;
#endif
#ifdef DYNAMIC_CRC_TABLE
    flags += 1 << 13;
#endif
#ifdef NO_GZCOMPRESS
    flags += 1 << 16;
#endif
#ifdef NO_GZIP
    flags += 1 << 17;
#endif
#ifdef PKZIP_BUG_WORKAROUND
    flags += 1 << 20;
#endif
#ifdef FASTEST
    flags += 1 << 21;
#endif
#ifdef STDC
#  ifdef NO_vsnprintf
        flags += 1 << 25;
#    ifdef HAS_vsprintf_void
        flags += 1 << 26;
#    endif
#  else
#    ifdef HAS_vsnprintf_void
        flags += 1 << 26;
#    endif
#  endif
#else
        flags += 1 << 24;
#  ifdef NO_snprintf
        flags += 1 << 25;
#    ifdef HAS_sprintf_void
        flags += 1 << 26;
#    endif
#  else
#    ifdef HAS_snprintf_void
        flags += 1 << 26;
#    endif
#  endif
#endif
    return flags;
}

#ifdef DEBUG

#  ifndef verbose
#    define verbose 0
#  endif
int z_verbose = verbose;

void z_error (m)
    char *m;
{
    fprintf(stderr, "%s\n", m);
    exit(1);
}
#endif

#if defined(_WIN32_WCE)
    /* does not exist on WCE */
    int errno = 0;
#endif

#ifdef SYS16BIT

#ifdef __TURBOC__
/* Turbo C in 16-bit mode */

/* Turbo C malloc() does not allow dynamic allocation of 64K bytes
 * and farmalloc(64K) returns a pointer with an offset of 8, so we
 * must fix the pointer. Warning: the pointer must be put back to its
 * original form in order to free it, use zcfree().
 */

#define MAX_PTR 10
/* 10*64K = 640K */

local int next_ptr = 0;

typedef struct ptr_table_s {
    voidpf org_ptr;
    voidpf new_ptr;
} ptr_table;

local ptr_table table[MAX_PTR];
/* This table is used to remember the original form of pointers
 * to large buffers (64K). Such pointers are normalized with a zero offset.
 * Since MSDOS is not a preemptive multitasking OS, this table is not
 * protected from concurrent access. This hack doesn't work anyway on
 * a protected system like OS/2. Use Microsoft C instead.
 */


#endif /* __TURBOC__ */


#ifdef M_I86
/* Microsoft C in 16-bit mode */

#if (!defined(_MSC_VER) || (_MSC_VER <= 600))
#  define _halloc  halloc
#  define _hfree   hfree
#endif


#endif /* M_I86 */

#endif /* SYS16BIT */
