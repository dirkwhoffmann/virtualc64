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
    setDescription("Mouse");
    
    subComponents = vector<HardwareComponent *> {
        
        &mouse1350,
        &mouse1351,
        &mouseNeos
    };
    
    config.model = MOUSE1350;
}

void Mouse::reset()
{
    HardwareComponent::reset();
    targetX = 0;
    targetY = 0;
}
void
Mouse::setModel(MouseModel model)
{
    c64->suspend();

    config.model = model;
    reset();

    c64->resume();
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
            
        case MOUSE1350:
            mouse1350.setLeftMouseButton(value);
            break;
        case MOUSE1351:
            mouse1351.setLeftMouseButton(value);
            break;
        case NEOSMOUSE:
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
            
        case MOUSE1350:
            mouse1350.setRightMouseButton(value);
            break;
        case MOUSE1351:
            mouse1351.setRightMouseButton(value);
            break;
        case NEOSMOUSE:
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
                
            case MOUSE1350:
                return mouse1350.readPotX();
                
            case MOUSE1351:
                mouse1351.executeX(targetX);
                return mouse1351.readPotX();
                
            case NEOSMOUSE:
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
            case MOUSE1350:
                return mouse1350.readPotY();
            case MOUSE1351:
                mouse1351.executeY(targetY);
                return mouse1351.readPotY();
            case NEOSMOUSE:
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
            case MOUSE1350:
                return mouse1350.readControlPort();
            case MOUSE1351:
                return mouse1351.readControlPort();
            case NEOSMOUSE:
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
                
            case MOUSE1350:
                mouse1350.execute(targetX, targetY);
                break;
            case MOUSE1351:
                // Coordinates are updated in readPotX() and readPotY()
                break;
            case NEOSMOUSE:
                // Coordinates are updated in latchPosition()
                break;
            default:
                assert(false);
        }
    }
}
