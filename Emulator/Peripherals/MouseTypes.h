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

enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,
    MOUSE_C1351,
    MOUSE_NEOS
};
typedef MOUSE_MODEL MouseModel;

inline bool isMouseModel(long value)
{
    return (unsigned long)value <= MOUSE_NEOS;
}

inline const char *MouseModelName(MouseModel value)
{
    switch (value) {
            
        case MOUSE_C1350:  return "C1350";
        case MOUSE_C1351:  return "C1351";
        case MOUSE_NEOS:   return "NEOS";
    }
    return "???";
}

typedef struct
{
    MouseModel model;
}
MouseConfig;

#endif
