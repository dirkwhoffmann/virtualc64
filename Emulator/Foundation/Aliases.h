// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ALIASES_H
#define _ALIASES_H

#include <stdint.h>

//
// Basic types
//

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


//
// Cycle counts
//

typedef i64 Cycle;


//
// Enumeration types
//

/* All enumeration types are declared via a special 'longenum' macro to make
 * them easily accessible in Swift.
 */

// Definition for wasm (long = 32 Bit)
#if defined(__EMSCRIPTEN__)
#define longenum(_name) \
typedef enum __attribute__((enum_extensibility(open))) _name : long long _name; \
enum _name : long long

// Definition for clang (long = 64 Bit)
#elif defined(__clang__)
#define longenum(_name) \
typedef enum __attribute__((enum_extensibility(open))) _name : long _name; \
enum _name : long

// Definition for gcc (long = 64 Bit)
#else
#define longenum(_name) \
enum _name : long
#endif

#endif
