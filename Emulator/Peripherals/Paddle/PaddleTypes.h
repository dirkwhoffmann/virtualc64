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
enum_long(PADDLE_ORIENTATION)
{
    PADDLE_HORIZONTAL,          ///< Horizontal
    PADDLE_HORIZONTAL_FLIPPED,  ///< Horizontal, reverse direction
    PADDLE_VERTICAL,            ///< Vertical
    PADDLE_VERTICAL_FLIPPED     ///< Vertical, reverse direction
};
typedef PADDLE_ORIENTATION PaddleOrientation;

struct PaddleOrientationEnum : util::Reflection<PaddleOrientationEnum, PaddleOrientation> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = PADDLE_VERTICAL_FLIPPED;

    static const char *prefix() { return nullptr; }
    static const char *_key(long value)
    {
        switch (value) {

            case PADDLE_HORIZONTAL:         return "HORIZONTAL";
            case PADDLE_HORIZONTAL_FLIPPED: return "HORIZONTAL_FLIPPED";
            case PADDLE_VERTICAL:           return "VERTICAL";
            case PADDLE_VERTICAL_FLIPPED:   return "VERTICAL_FLIPPED";
        }
        return "???";
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
