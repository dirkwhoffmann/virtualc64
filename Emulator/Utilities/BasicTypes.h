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
// Integers
//

namespace vc64 {

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

}


//
// Strings
//

#include <string>
#include <cstring>
namespace vc64 { using std::string; }


//
// Data structures
//

#include <vector>
namespace vamiga { using std::vector; }


//
// Optionals
//

#include <optional>
namespace vc64 { using std::optional; }


//
// Filesystem
//

#include <filesystem>
namespace vc64 { namespace fs = std::filesystem; }
