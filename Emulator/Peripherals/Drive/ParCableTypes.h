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

//
// Enumerations
//

/// Parallel cable type
enum_long(PAR_CABLE_TYPE)
{
    PAR_CABLE_NONE,         ///< No parallel cable attached
    PAR_CABLE_STANDARD,     ///< Standard parallel cable
    PAR_CABLE_DOLPHIN3      ///< DolphinDOS cable
};
typedef PAR_CABLE_TYPE ParCableType;
