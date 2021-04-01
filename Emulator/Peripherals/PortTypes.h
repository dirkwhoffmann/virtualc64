// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "PortPublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct ControlPortDeviceEnum : util::Reflection<ControlPortDeviceEnum, ControlPortDevice> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CPDEVICE_COUNT;
    }
    
    static const char *prefix() { return "CPDEVICE"; }
    static const char *key(ControlPortDevice value)
    {
        switch (value) {
                
            case CPDEVICE_NONE:      return "NONE";
            case CPDEVICE_MOUSE:     return "MOUSE";
            case CPDEVICE_JOYSTICK:  return "JOYSTICK";
            case CPDEVICE_COUNT:     return "???";
        }
        return "???";
    }
};

struct GamePadActionEnum : util::Reflection<GamePadActionEnum, GamePadAction> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= RELEASE_RIGHT;
    }
    
    static const char *prefix() { return nullptr; }
    static const char *key(GamePadAction value)
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
};

struct CRTModeEnum : util::Reflection<CRTModeEnum, CRTMode> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CRTMODE_COUNT;
    }
    
    static const char *prefix() { return "CRTMODE"; }
    static const char *key(CRTMode value)
    {
        switch (value) {
                
            case CRTMODE_16K:      return "16K";
            case CRTMODE_8K:       return "8K";
            case CRTMODE_ULTIMAX:  return "ULTIMAX";
            case CRTMODE_OFF:      return "OFF";
            case CRTMODE_COUNT:    return "???";
        }
        return "???";
    }
};
