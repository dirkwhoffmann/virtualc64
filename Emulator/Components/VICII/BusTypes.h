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
/// @file

#pragma once

#include "Aliases.h"
#include "Reflection.hpp"

enum_long(MEMACCESS)
{
    MEMACCESS_R,     // Memory Refresh
    MEMACCESS_I,     // Idle read
    MEMACCESS_C,     // Character access
    MEMACCESS_G,     // Graphics access
    MEMACCESS_P,     // Sprite pointer access
    MEMACCESS_S,     // Sprite data access
    MEMACCESS_COUNT
};
typedef MEMACCESS MemAccess;
