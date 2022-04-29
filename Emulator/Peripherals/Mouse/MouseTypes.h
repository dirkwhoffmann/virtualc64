// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
    MOUSE_NEOS
};
typedef MOUSE_MODEL MouseModel;

#ifdef __cplusplus
struct MouseModelEnum : util::Reflection<MouseModelEnum, MouseModel> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = MOUSE_NEOS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
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

enum_long(PORT_ID)
{
    PORT_NONE,
    PORT_ONE,
    PORT_TWO
};
typedef PORT_ID PortId;

#ifdef __cplusplus
struct PortIdEnum : util::Reflection<PortIdEnum, PortId> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = PORT_TWO;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "PORT"; }
    static const char *key(PortId value)
    {
        switch (value) {
                
            case PORT_NONE:   return "NONE";
            case PORT_ONE:    return "ONE";
            case PORT_TWO:    return "TWO";
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
    bool shakeDetection;
    long velocity;
}
MouseConfig;
