// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Mouse.h"
#include "C64.h"

Mouse::Mouse(C64 &ref, ControlPort& pref) : C64Component(ref), port(pref)
{
    subComponents = std::vector<HardwareComponent *> {
        
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
Mouse::_dump() const
{
    msg("Mouse %lld:\n", port.nr);
    msg("--------\n");
    msg("   targetX : %lld\n", targetX);
    msg("   targetY : %lld\n", targetY);
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
Mouse::setXY(i64 x, i64 y)
{
    targetX = x;
    targetY = y;
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::setLeftButton(bool value)
{
    debug(PORT_DEBUG, "setLeftButton(%d)\n", value);
    
    switch(config.model) {
            
        case MOUSE_C1350: mouse1350.setLeftMouseButton(value); break;
        case MOUSE_C1351: mouse1351.setLeftMouseButton(value); break;
        case MOUSE_NEOS:  mouseNeos.setLeftMouseButton(value); break;
            
        default:
            assert(false);
    }
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::setRightButton(bool value)
{
    debug(PORT_DEBUG, "setRightButton(%d)\n", value);

    switch(config.model) {
            
        case MOUSE_C1350: mouse1350.setRightMouseButton(value); break;
        case MOUSE_C1351: mouse1351.setRightMouseButton(value); break;
        case MOUSE_NEOS:  mouseNeos.setRightMouseButton(value); break;
            
        default:
            assert(false);
    }
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::trigger(GamePadAction event)
{
    assert_enum(GamePadAction, event);

    debug(PORT_DEBUG, "trigger(%lld)\n", event);
    
    switch (event) {

        case PRESS_LEFT: setLeftButton(true); break;
        case RELEASE_LEFT: setLeftButton(false); break;
        case PRESS_RIGHT: setRightButton(true); break;
        case RELEASE_RIGHT: setRightButton(false); break;
            
        default:
            break;
    }
}

void
Mouse::risingStrobe()
{
    mouseNeos.risingStrobe(targetX, targetY);
}

void
Mouse::fallingStrobe()
{
    mouseNeos.fallingStrobe(targetX, targetY);
}

void
Mouse::updatePotX()
{
    if (config.model == MOUSE_C1351) {
        mouse1351.executeX(targetX);
    }
}

void
Mouse::updatePotY()
{
    if (config.model == MOUSE_C1351) {
        mouse1351.executeY(targetY);
    }
}

void updatePotY();

u8
Mouse::readPotX() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readPotX();
        case MOUSE_C1351: return mouse1351.readPotX();
        case MOUSE_NEOS:  return mouseNeos.readPotX();
            
        default:
            assert(false);
            return 0xFF;
    }
}

u8
Mouse::readPotY() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readPotY();
        case MOUSE_C1351: return mouse1351.readPotY();
        case MOUSE_NEOS:  return mouseNeos.readPotY();
            
        default:
            assert(false);
            return 0xFF;
    }
}

void
Mouse::updateControlPort()
{
    if (config.model == MOUSE_NEOS) {
        mouseNeos.updateControlPort(targetX, targetY);
    }
}

u8
Mouse::getControlPort() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readControlPort();
        case MOUSE_C1351: return mouse1351.readControlPort();
        case MOUSE_NEOS:  return mouseNeos.readControlPort();
            
        default:
            assert(false);
            return 0xFF;
    }
}

void
Mouse::execute()
{
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
