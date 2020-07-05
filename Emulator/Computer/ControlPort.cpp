// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------
 
#include "C64.h"

ControlPort::ControlPort(int portNr, C64 &ref) : C64Component(ref)
{
    assert(portNr == 1 || portNr == 2);
    
    nr = portNr;
    autofire = false;
    autofireBullets = -3;
    autofireFrequency = 2.5;
    bulletCounter = 0;
    nextAutofireFrame = 0;
    
    setDescription("ControlPort");    
}

void
ControlPort::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

    button = false;
    axisX = 0;
    axisY = 0;
}

void
ControlPort::didLoadFromBuffer(u8 **buffer)
{
    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;
}

void
ControlPort::_dump()
{
    msg("ControlPort port %d\n", nr);
    msg("------------------\n");
    msg("Button:  %s AxisX: %d AxisY: %d\n", button ? "YES" : "NO", axisX, axisY);
    msg("Bitmask: %02X\n", bitmask());
}

void
ControlPort::scheduleNextShot()
{
    nextAutofireFrame = vc64.frame +
    (int)(vic.getFramesPerSecond() / (2 * autofireFrequency));
}

void
ControlPort::execute()
{
    if (!autofire || autofireFrequency <= 0.0)
        return;
  
    // Wait until it's time to push or release fire
    if (vc64.frame != nextAutofireFrame)
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

void
ControlPort::trigger(JoystickEvent event)
{
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
            assert(0);
    }
}

void
ControlPort::setAutofire(bool value)
{
    if (!(autofire = value)) {
        button = false;
    }
}

void
ControlPort::setAutofireBullets(int value)
{
    autofireBullets = value;
    if (bulletCounter > 0) {
        bulletCounter = (autofireBullets < 0) ? UINT64_MAX : autofireBullets;
    }
}

u8
ControlPort::bitmask() {
    
    u8 result = 0xFF;
    
    if (axisY == -1) CLR_BIT(result, 0);
    if (axisY ==  1) CLR_BIT(result, 1);
    if (axisX == -1) CLR_BIT(result, 2);
    if (axisX ==  1) CLR_BIT(result, 3);
    if (button)      CLR_BIT(result, 4);
    
    u8 mouseBits = mouse.readControlPort(nr);
    result &= mouseBits;
    
    return result;
}


