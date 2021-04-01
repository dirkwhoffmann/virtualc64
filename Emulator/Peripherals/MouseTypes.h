// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,
    MOUSE_C1351,
    MOUSE_NEOS,
    
    MOUSE_COUNT
};
typedef MOUSE_MODEL MouseModel;

#ifdef __cplusplus
struct MouseModelEnum : util::Reflection<MouseModelEnum, MouseModel> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < MOUSE_COUNT;
    }
    
    static const char *prefix() { return "DISK_TYPE"; }
    static const char *key(MouseModel value)
    {
        switch (value) {
                
            case MOUSE_C1350:  return "C1350";
            case MOUSE_C1351:  return "C1351";
            case MOUSE_NEOS:   return "NEOS";
            case MOUSE_COUNT:  return "???";
        }
        return "???";
    }
};
#endif

enum_long(PORT_ID)
{
    PORT_NONE,
    PORT_ONE,
    PORT_TWO,
    
    PORT_COUNT
};
typedef PORT_ID PortId;

#ifdef __cplusplus
struct PortIdEnum : util::Reflection<PortIdEnum, PortId> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < PORT_COUNT;
    }
    
    static const char *prefix() { return "PORT"; }
    static const char *key(PortId value)
    {
        switch (value) {
                
            case PORT_NONE:   return "NONE";
            case PORT_ONE:    return "ONE";
            case PORT_TWO:    return "TWO";
            case PORT_COUNT:  return "???";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct
{
    MouseModel model;
}
MouseConfig;
