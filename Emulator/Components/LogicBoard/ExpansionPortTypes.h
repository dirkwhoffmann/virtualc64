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

#include "Types.h"

//
// Enumerations
//

/// Cartridge mode
enum_long(CRTMODE)
{
    CRTMODE_16K,        //! 16KB cartridge
    CRTMODE_8K,         //! 8KB cartridge
    CRTMODE_ULTIMAX,    //! Ultimax cartridge
    CRTMODE_OFF         //! No cartridge
};
typedef CRTMODE CRTMode;
