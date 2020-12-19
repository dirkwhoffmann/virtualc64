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
#include <stddef.h>
#include <assert.h>

//
// Basic types
//

typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

static_assert(sizeof(i8) == 1,  "i8 size mismatch");
static_assert(sizeof(i16) == 2, "i16 size mismatch");
static_assert(sizeof(i32) == 4, "i32 size mismatch");
static_assert(sizeof(i64) == 8, "i64 size mismatch");
static_assert(sizeof(u8) == 1,  "u8 size mismatch");
static_assert(sizeof(u16) == 2, "u16 size mismatch");
static_assert(sizeof(u32) == 4, "u32 size mismatch");
static_assert(sizeof(u64) == 8, "u64 size mismatch");


//
// Syntactic sugar
//

#define fallthrough


//
// Cycles
//

typedef i64 Cycle;


//
// Floppy drives and disks
//

typedef unsigned Cylinder;
typedef unsigned Head;
typedef unsigned Track;
typedef unsigned Halftrack;
typedef unsigned Sector;
typedef unsigned Block;


//
// Enumerations
//

/* All enumeration types are declared via a special 'longenum' macro to make
 * them easily accessible in Swift. The macro has two definitions, one for the
 * Swift side and one for the C side. On the Swift side, all enums are mapped
 * to type 'long' to make them accessible via the Swift standard type 'Int'.
 * On the C side all enums are mapped to long long. This ensures the same size
 * for all enums, both on 32-bit and 64-bit architectures.
 */

// Definition for Swift
#if defined(__VC64GUI__)
#define longenum(_name) \
typedef enum __attribute__((enum_extensibility(open))) _name : long _name; \
enum _name : long

// Definition for the C
#else
#define longenum(_name) \
enum _name : long long
#endif

#endif
