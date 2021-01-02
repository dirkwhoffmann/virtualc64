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

enum_long(CPDEVICE)
{
    CPDEVICE_NONE,
    CPDEVICE_MOUSE,
    CPDEVICE_JOYSTICK
};
typedef CPDEVICE ControlPortDevice;

inline bool isControlPortDevice(long value) {
    
    return (unsigned long)value <= CPDEVICE_JOYSTICK;
}

inline const char *ControlPortDeviceName(ControlPortDevice value)
{
    switch (value) {
            
        case CPDEVICE_NONE:      return "NONE";
        case CPDEVICE_MOUSE:     return "MOUSE";
        case CPDEVICE_JOYSTICK:  return "JOYSTICK";
    }
    return "???";
}

enum_long(GAME_PAD_ACTION)
{
    PULL_UP,       // Pull the joystick up
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
typedef GAME_PAD_ACTION GamePadAction;

inline bool isGamePadAction(long value) {
    
    return (unsigned long)value <= RELEASE_RIGHT;
}

inline const char *GamePadActionName(GamePadAction value)
{
    switch (value) {
            
        case PULL_UP:        return "PULL_UP";
        case PULL_DOWN:      return "PULL_DOWN";
        case PULL_LEFT:      return "PULL_LEFT";
        case PULL_RIGHT:     return "PULL_RIGHT";
        case PRESS_FIRE:     return "PRESS_FIRE";
        case PRESS_LEFT:     return "PRESS_LEFT";
        case PRESS_RIGHT:    return "PRESS_RIGHT";
        case RELEASE_X:      return "RELEASE_X";
        case RELEASE_Y:      return "RELEASE_Y";
        case RELEASE_XY:     return "RELEASE_XY";
        case RELEASE_FIRE:   return "RELEASE_FIRE";
        case RELEASE_LEFT:   return "RELEASE_LEFT";
        case RELEASE_RIGHT:  return "RELEASE_RIGHT";
    }
    return "???";
}

enum_long(CRTMODE)
{
    CRTMODE_16K,
    CRTMODE_8K,
    CRTMODE_ULTIMAX,
    CRTMODE_OFF
};
typedef CRTMODE CRTMode;

inline bool isCRTMode(long value) {
    
    return (unsigned long)value <= CRTMODE_OFF;
}

inline const char *CRTModeName(CRTMode value)
{
    switch (value) {
            
        case CRTMODE_16K:      return "16K";
        case CRTMODE_8K:       return "8K";
        case CRTMODE_ULTIMAX:  return "ULTIMAX";
        case CRTMODE_OFF:      return "OFF";
    }
    return "???";
}

#endif
