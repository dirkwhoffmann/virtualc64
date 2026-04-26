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

#include "utl/abilities/Reflectable.h"
#include <sys/types.h>
#include <stdint.h>

//
// Integers
//

namespace vc64 {

using namespace utl;

}


//
// Pointers
//

#include <memory>
namespace vc64 { using std::unique_ptr, std::make_unique; }


//
// Strings
//

#include <string>
#include <cstring>
namespace vc64 { using std::string; using std::string_view; }


//
// Data structures
//

#include <vector>
namespace vc64 { using std::vector; }


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
