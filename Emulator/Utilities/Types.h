// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include <sys/types.h>
#include <stdint.h>

//
// Booleans
//

// #include <stdbool.h>


//
// Strings
//

#include <string>
#include <cstring>
using std::string;


//
// Integers
//

// Signed integers
typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;
typedef signed long        isize;

// Unsigned integers
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef unsigned long      usize;

// Signed alternative for the sizeof keyword
#define isizeof(x) (isize)(sizeof(x))


//
// Optionals
//

#include <optional>
using std::optional;


// Wrapper for declaring enums
#define new_enum_generic(_name, _type) \
enum __attribute__((enum_extensibility(open))) _name : _type
#define enum_long(_name) new_enum_generic(_name, long)
#define enum_i8(_name) new_enum_generic(_name, i8)
