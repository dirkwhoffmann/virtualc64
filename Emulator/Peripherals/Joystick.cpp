// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

const char *
Joystick::getDescription() const
{
    return port.nr == PORT_ONE ? "Joystick1" : "Joystick2";
}

void
Joystick::_dump() const
{
    msg("ControlPort %lld:\n", port.nr);
    msg("--------------\n");
    msg("   Button : %s\n", button ? "pressed" : "released");
    msg("    axisX : %d axisY: %d\n", axisX, axisY);
    msg("  bitmask : %02X\n", getControlPort());
}

usize
Joystick::didLoadFromBuffer(u8 *buffer)
{
    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;

    return 0;
}

void
Joystick::setAutofire(bool value)
{
    autofire = value;
    
    // Release button immediately if autofire-mode is switches off
    if (value == false) button = false;
}

void
Joystick::setAutofireBullets(int value)
{
    autofireBullets = value;
    
    // Update the bullet counter if we're currently firing
    if (bulletCounter > 0) {
        bulletCounter = (autofireBullets < 0) ? UINT64_MAX : autofireBullets;
    }
}

void
Joystick::scheduleNextShot()
{
    nextAutofireFrame = c64.frame +
    (int)(vic.getFramesPerSecond() / (2 * autofireFrequency));
}

u8
Joystick::getControlPort() const
{
    u8 result = 0xFF;
    
    if (axisY == -1) CLR_BIT(result, 0);
    if (axisY ==  1) CLR_BIT(result, 1);
    if (axisX == -1) CLR_BIT(result, 2);
    if (axisX ==  1) CLR_BIT(result, 3);
    if (button)      CLR_BIT(result, 4);
        
    return result;
}

void
Joystick::trigger(GamePadAction event)
{
    debug(PORT_DEBUG, "Port %lld: %s\n", port.nr, GamePadActionEnum::key(event));
    
    switch (event) {
            
        case PULL_UP:
            axisY = -1;
            break;
        case PULL_DOWN:
            axisY = 1;
            break;
        case PULL_LEFT:
            axisX = -1;
            break;
        case PULL_RIGHT:
            axisX = 1;
            break;
        case PRESS_FIRE:
            
            if (autofire) {
                if (bulletCounter) {
                    // Cease fire
                    bulletCounter = 0;
                    button = false;
                } else {
                    // Load magazine
                    bulletCounter = (autofireBullets < 0) ? UINT64_MAX : autofireBullets;
                    button = true;
                    scheduleNextShot();
                }
            } else {
                button = true;
            }
            break;
        case RELEASE_X:
            axisX = 0;
            break;
        case RELEASE_Y:
            axisY = 0;
            break;
        case RELEASE_XY:
            axisX = 0;
            axisY = 0;
            break;
        case RELEASE_FIRE:
            if (!autofire)
                button = false;
            break;
  
        default:
            assert(false);
    }
    port.device = CPDEVICE_JOYSTICK;
}

void
Joystick::execute()
{
    if (!autofire || autofireFrequency <= 0.0)
        return;
  
    // Wait until it's time to push or release fire
    if ((i64)c64.frame != nextAutofireFrame)
        return;
    
    // Are there any bullets left?
    if (bulletCounter) {
        if (button) {
            button = false;
            bulletCounter--;
        } else {
            button = true;
        }
        scheduleNextShot();
    }
}
