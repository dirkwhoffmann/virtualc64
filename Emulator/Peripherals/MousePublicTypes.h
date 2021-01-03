// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This file must conform to standard ANSI-C to be compatible with Swift.

#ifndef MOUSE_PUBLIC_TYPES_H
#define MOUSE_PUBLIC_TYPES_H

enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,
    MOUSE_C1351,
    MOUSE_NEOS,
    MOUSE_COUNT
};
typedef MOUSE_MODEL MouseModel;

typedef struct
{
    MouseModel model;
}
MouseConfig;

#endif
