// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PORT_TYPES_H
#define _PORT_TYPES_H

//
// Enumerations
//

enum_long(ControlPortDevice)
{
    ControlPortDevice_NONE,
    ControlPortDevice_MOUSE,
    ControlPortDevice_JOYSTICK
};

inline bool isControlPortDevice(long value) {
    
    return (unsigned long)value <= ControlPortDevice_JOYSTICK;
}

inline const char *ControlPortDeviceName(ControlPortDevice value)
{
    assert(isControlPortDevice(value));
    
    switch (value) {
            
        case ControlPortDevice_NONE:      return "NONE";
        case ControlPortDevice_MOUSE:     return "MOUSE";
        case ControlPortDevice_JOYSTICK:  return "JOYSTICK";
        default:                          return "???";
    }
}

enum_long(GamePadAction)
{
    PULL_UP = 0,   // Pull the joystick up
    PULL_DOWN,     // Pull the joystick down
    PULL_LEFT,     // Pull the joystick left
    PULL_RIGHT,    // Pull the joystick right
    PRESS_FIRE,    // Press the joystick button
    PRESS_LEFT,    // Press the left mouse button
    PRESS_RIGHT,   // Press the right mouse button
    RELEASE_X,     // Move back to neutral horizontally
    RELEASE_Y,     // Move back to neutral vertically
    RELEASE_XY,    // Move back to neutral
    RELEASE_FIRE,  // Release the joystick button
    RELEASE_LEFT,  // Release the left mouse button
    RELEASE_RIGHT  // Release the right mouse button
};

inline bool isGamePadAction(long value) {
    
    return (unsigned long)value <= RELEASE_RIGHT;
}

enum_long(CRTMode)
{
    CRTMode_16K,
    CRTMode_8K,
    CRTMode_ULTIMAX,
    CRTMode_OFF
};

inline bool isCRTMode(long value) {
    
    return (unsigned long)value <= CRTMode_OFF;
}

inline const char *CRTModeName(CRTMode value)
{
    assert(isCRTMode(value));
    
    switch (value) {
            
        case CRTMode_16K:      return "16K";
        case CRTMode_8K:       return "8K";
        case CRTMode_ULTIMAX:  return "ULTIMAX";
        case CRTMode_OFF:      return "OFF";
        default:               return "???";
    }
}

#endif
