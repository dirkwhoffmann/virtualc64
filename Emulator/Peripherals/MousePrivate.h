// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOUSE_PRIVATE_H
#define MOUSE_PRIVATE_H

//
// Reflection APIs
//

struct MouseModelEnum : Reflection<MouseModelEnum, MouseModel> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= MOUSE_NEOS;
    }
    
    static const char *prefix() { return "DISK_TYPE"; }
    static const char *key(MouseModel value)
    {
        switch (value) {
                
            case MOUSE_C1350:  return "C1350";
            case MOUSE_C1351:  return "C1351";
            case MOUSE_NEOS:   return "NEOS";
        }
        return "???";
    }
};

#endif