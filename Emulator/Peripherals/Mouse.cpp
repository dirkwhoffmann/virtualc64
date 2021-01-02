// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Mouse::Mouse(C64 &ref) : C64Component(ref)
{
    subComponents = vector<HardwareComponent *> {
        
        &mouse1350,
        &mouse1351,
        &mouseNeos
    };
    
    config.model = MOUSE_C1350;
}

void Mouse::_reset()
{
    RESET_SNAPSHOT_ITEMS

    targetX = 0;
    targetY = 0;
}
void
Mouse::setModel(MouseModel model)
{
    suspend();

    config.model = model;
    _reset();

    resume();
}

void
Mouse::connectMouse(unsigned portNr)
{
    assert(portNr <= 2);
    port = portNr;
}

void
Mouse::setXY(i64 x, i64 y)
{
    targetX = x;
    targetY = y;
}

void
Mouse::setLeftButton(bool value)
{
    switch(config.model) {
            
        case MOUSE_C1350:
            mouse1350.setLeftMouseButton(value);
            break;
        case MOUSE_C1351:
            mouse1351.setLeftMouseButton(value);
            break;
        case MOUSE_NEOS:
            mouseNeos.setLeftMouseButton(value);
            break;
        default:
            assert(false);
    }
}

void
Mouse::setRightButton(bool value)
{
    switch(config.model) {
            
        case MOUSE_C1350:
            mouse1350.setRightMouseButton(value);
            break;
        case MOUSE_C1351:
            mouse1351.setRightMouseButton(value);
            break;
        case MOUSE_NEOS:
            mouseNeos.setRightMouseButton(value);
            break;
        default:
            assert(false);
    }
}

void
Mouse::risingStrobe(int portNr)
{
    mouseNeos.risingStrobe(portNr, targetX, targetY);
}

void
Mouse::fallingStrobe(int portNr)
{
    mouseNeos.fallingStrobe(portNr, targetX, targetY);
}

u8
Mouse::readPotX()
{
    if (port > 0) {
        switch(config.model) {
                
            case MOUSE_C1350:
                return mouse1350.readPotX();
                
            case MOUSE_C1351:
                mouse1351.executeX(targetX);
                return mouse1351.readPotX();
                
            case MOUSE_NEOS:
                return mouseNeos.readPotX();
                
            default:
                assert(false);
        }
    }
    return 0xFF;
}

u8
Mouse::readPotY()
{
    if (port > 0) {
        switch(config.model) {
            case MOUSE_C1350:
                return mouse1350.readPotY();
            case MOUSE_C1351:
                mouse1351.executeY(targetY);
                return mouse1351.readPotY();
            case MOUSE_NEOS:
                return mouseNeos.readPotY();
            default:
                assert(false);
        }
    }
    return 0xFF;
}

u8
Mouse::readControlPort(unsigned portNr)
{    
    if (port == portNr) {
        switch(config.model) {
            case MOUSE_C1350:
                return mouse1350.readControlPort();
            case MOUSE_C1351:
                return mouse1351.readControlPort();
            case MOUSE_NEOS:
                return mouseNeos.readControlPort(targetX, targetY);
            default:
                assert(false);
        }
    }
    return 0xFF;
}

void
Mouse::execute()
{
    if (port) {
        switch(config.model) {
                
            case MOUSE_C1350:
                mouse1350.execute(targetX, targetY);
                break;
            case MOUSE_C1351:
                // Coordinates are updated in readPotX() and readPotY()
                break;
            case MOUSE_NEOS:
                // Coordinates are updated in latchPosition()
                break;
            default:
                assert(false);
        }
    }
}
