// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "types/Integers.h"
#include "types/Exception.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

/* Frequently used standard types are made available inside namespace utl,
 * not in the global namespace. Code that uses them unqualified needs a
 * 'using namespace utl' in its own namespace.
 */
namespace utl {

namespace fs = std::filesystem;

using std::function;
using std::unique_ptr;
using std::make_unique;
using std::optional;
using std::span;
using std::string;
using std::string_view;
using std::vector;

}
