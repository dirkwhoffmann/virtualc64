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

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Value mapping scheme
enum class PaddleOrientation : long
{
    HORIZONTAL,          ///< Horizontal
    HORIZONTAL_FLIPPED,  ///< Horizontal, reverse direction
    VERTICAL,            ///< Vertical
    VERTICAL_FLIPPED     ///< Vertical, reverse direction
};

struct PaddleOrientationEnum : util::Reflection<PaddleOrientationEnum, PaddleOrientation> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(PaddleOrientation::VERTICAL_FLIPPED);

    static const char *_key(PaddleOrientation value)
    {
        switch (value) {

            case PaddleOrientation::HORIZONTAL:         return "HORIZONTAL";
            case PaddleOrientation::HORIZONTAL_FLIPPED: return "HORIZONTAL_FLIPPED";
            case PaddleOrientation::VERTICAL:           return "VERTICAL";
            case PaddleOrientation::VERTICAL_FLIPPED:   return "VERTICAL_FLIPPED";
        }
        return "???";
    }
    
    static const char *help(PaddleOrientation value)
    {
        return "";
    }
};

//
// Structures
//

typedef struct
{
    PaddleOrientation orientation;
}
PaddleConfig;

typedef struct
{

}
PaddleInfo;

}
