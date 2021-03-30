// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Macros.h"

#include "config.h"
#include <assert.h>
#include <sys/types.h>

//
// Basic types
//

/*
typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef ssize_t            isize;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef size_t             usize;

static_assert(sizeof(i8)  == 1,  "i8 size mismatch");
static_assert(sizeof(i16) == 2, "i16 size mismatch");
static_assert(sizeof(i32) == 4, "i32 size mismatch");
static_assert(sizeof(i64) == 8, "i64 size mismatch");
static_assert(sizeof(u8)  == 1,  "u8 size mismatch");
static_assert(sizeof(u16) == 2, "u16 size mismatch");
static_assert(sizeof(u32) == 4, "u32 size mismatch");
static_assert(sizeof(u64) == 8, "u64 size mismatch");
*/

//
// Cycles
//

typedef i64 Cycle;


//
// Floppy drives and disks
//

typedef u32 Cylinder;
typedef u32 Head;
typedef u32 Track;
typedef u32 Halftrack;
typedef u32 Sector;
typedef u32 Block;

typedef i32 HeadPos;


//
// Syntactic sugar
//

/* The following keyword is used for documentary purposes only. It is used to
 * mark all methods that use the exception mechanism to signal error conditions
 * instead of returning error codes. It is used in favor of classic throw
 * lists, since the latter cause the compiler to embed unwanted runtime checks
 * in the code.
 */
#define throws


//
// Enumerations
//

/* All enumeration types are declared via special 'enum_<type>' macros to make
 * them easily accessible in Swift. All macros have two definitions, one for
 * the Swift side and one for the C side. Please note that the type mapping for
 * enum_long differs on both sides. On the Swift side, enums of this type are
 * mapped to type 'long' to make them accessible via the Swift standard type
 * 'Int'. On the C side all enums are mapped to long long. This ensures the
 * same size for all enums, both on 32-bit and 64-bit architectures.
 */

/*
#if defined(__VC64GUI__)

// Definition for Swift
#define enum_open(_name, _type) \
typedef enum __attribute__((enum_extensibility(open))) _name : _type _name; \
enum _name : _type

#define enum_long(_name) enum_open(_name, long)
#define enum_int(_name) enum_open(_name, int)
#define enum_byte(_name) enum_open(_name, unsigned char)

#else

// Definition for C
#define enum_long(_name) enum _name : long long
#define enum_int(_name) enum _name : int
#define enum_byte(_name) enum _name : unsigned char

#endif
*/
