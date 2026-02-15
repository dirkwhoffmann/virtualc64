// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/types/Integers.h"

namespace utl {

constexpr isize operator"" _B(unsigned long long v) {
    return static_cast<isize>(v);
}
constexpr isize operator"" _KB(unsigned long long v) {
    return static_cast<isize>(v) << 10;
}
constexpr isize operator"" _MB(unsigned long long v) {
    return static_cast<isize>(v) << 20;
}
constexpr isize operator"" _GB(unsigned long long v) {
    return static_cast<isize>(v) << 30;
}

}
