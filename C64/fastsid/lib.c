/*
 * lib.c - Library functions.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32_UNICODE_SUPPORT
#include <wchar.h>
#endif

#ifdef AMIGA_SUPPORT
#ifndef __USE_INLINE__
#define __USE_INLINE__
#endif
#endif

#if defined(AMIGA_SUPPORT) || defined(__VBCC__)
#include <proto/exec.h>
#ifndef AMIGA_SUPPORT
#define AMIGA_SUPPORT
#endif
#endif

// #include "types.h"
// #include "debug.h"

#if (defined(sun) || defined(__sun)) && !(defined(__SVR4) || defined(__svr4__))
#  ifndef RAND_MAX
#    define RAND_MAX 32767
#  endif
#endif


/*----------------------------------------------------------------------------*/
/* like malloc, but abort on out of memory. */
void *lib_malloc(size_t size)
{
#ifdef LIB_DEBUG
    void *ptr = lib_debug_libc_malloc(size);
#else
    void *ptr = malloc(size);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0) {
        fprintf(stderr, "error: lib_malloc failed\n");
        exit(-1);
    }
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 3);
#endif

#if 0
    /* clear/fill the block - this should only ever be used for debugging! */
    if (ptr) {
        memset(ptr, 0, size);
    }
#endif
    return ptr;
}

#ifdef AMIGA_SUPPORT
void *lib_AllocVec(unsigned long size, unsigned long attributes)
{
#ifdef LIB_DEBUG
    void *ptr;

    if (attributes & MEMF_CLEAR) {
        ptr = lib_debug_libc_calloc(1, size);
    } else {
        ptr = lib_debug_libc_malloc(size);
    }
#else
    void *ptr = AllocVec(size, attributes);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0) {
        fprintf(stderr, "error: lib_AllocVec failed\n");
        exit(-1);
    }
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 1);
#endif

    return ptr;
}

void *lib_AllocMem(unsigned long size, unsigned long attributes)
{
#ifdef LIB_DEBUG
    void *ptr;

    if (attributes & MEMF_CLEAR) {
        ptr = lib_debug_libc_calloc(1, size);
    } else {
        ptr = lib_debug_libc_malloc(size);
    }
#else
    void *ptr = AllocMem(size, attributes);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0) {
        fprintf(stderr, "error: lib_AllocMem failed\n");
        exit(-1);
    }
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 1);
#endif

    return ptr;
}
#endif

/* Like calloc, but abort if not enough memory is available.  */
void *lib_calloc(size_t nmemb, size_t size)
{
#ifdef LIB_DEBUG
    void *ptr = lib_debug_libc_calloc(nmemb, size);
#else
    void *ptr = calloc(nmemb, size);
#endif

#ifndef __OS2__
    if (ptr == NULL && (size * nmemb) > 0) {
        fprintf(stderr, "error: lib_calloc failed\n");
        exit(-1);
    }
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size * nmemb, 1);
#endif

    return ptr;
}

/* Like realloc, but abort if not enough memory is available.  */
void *lib_realloc(void *ptr, size_t size)
{
#ifdef LIB_DEBUG
    void *new_ptr = lib_debug_libc_realloc(ptr, size);
#else
    void *new_ptr = realloc(ptr, size);
#endif

#ifndef __OS2__
    if (new_ptr == NULL) {
        fprintf(stderr, "error: lib_realloc failed\n");
        exit(-1);
    }
#endif
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 0);
    lib_debug_alloc(new_ptr, size, 1);
#endif

    return new_ptr;
}

void lib_free(const void *constptr)
{
    void * ptr = (void*) constptr;
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
#endif

#ifdef LIB_DEBUG
    lib_debug_libc_free(ptr);
#else
    free(ptr);
#endif
}

#ifdef AMIGA_SUPPORT
void lib_FreeVec(void *ptr)
{
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
    lib_debug_libc_free(ptr);
#else
    FreeVec(ptr);
#endif
}

void lib_FreeMem(void *ptr, unsigned long size)
{
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
    lib_debug_libc_free(ptr);
#else
    FreeMem(ptr, size);
#endif
}
#endif

/*----------------------------------------------------------------------------*/

/* Malloc enough space for `str', copy `str' into it and return its
   address.  */
char *lib_stralloc(const char *str)
{
    size_t size;
    char *ptr;

    if (str == NULL) {
        fprintf(stderr, "error: lib_stralloc failed\n");
        exit(-1);
    }

    size = strlen(str) + 1;
    ptr = lib_malloc(size);

    memcpy(ptr, str, size);
    return ptr;
}



/*----------------------------------------------------------------------------*/

/*
    encapsulated random routines to generate random numbers within a given range.

    see http://c-faq.com/lib/randrange.html
*/

/* set random seed for rand() from current time, so things like random startup
   delay are actually random, ie different on each startup, at all. */
void lib_init_rand(void)
{
    srand((unsigned int)time(NULL));
}

unsigned int lib_unsigned_rand(unsigned int min, unsigned int max)
{
    return min + (rand() / ((RAND_MAX / (max - min + 1)) + 1));
}

float lib_float_rand(float min, float max)
{
    return min + ((float)rand() / (((float)RAND_MAX / (max - min + 1.0f)) + 1.0f));
}
