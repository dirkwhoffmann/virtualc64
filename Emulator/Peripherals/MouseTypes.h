// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This file must conform to standard ANSI-C to be compatible with Swift.

#ifndef MOUSE_TYPES_H
#define MOUSE_TYPES_H

typedef enum
{
    MOUSE1350 = 0,
    MOUSE1351,
    NEOSMOUSE
}
MouseModel;

typedef struct
{
    MouseModel model;
}
MouseConfig;

#endif
