/*!
 * @header      ControlPort.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include "C64.h"

ControlPort::ControlPort(int portNr)
{
    assert(portNr == 1 || portNr == 2);
    
    nr = portNr;
    autofire = false;
    autofireBullets = -3;
    autofireFrequency = 2.5;
    bulletCounter = 0;
    nextAutofireFrame = 0;
    
    setDescription("ControlPort");
    debug(3, "    Creating ControlPort %d at address %p...\n", nr, this);
    
    // Register snapshot items
    /*
    SnapshotItem items[] = {
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
     */
}

ControlPort::~ControlPort()
{
}

void
ControlPort::reset()
{
    VirtualComponent::reset();

    button = false;
    axisX = 0;
    axisY = 0;
}

void
ControlPort::didLoadFromBuffer(uint8_t **buffer)
{
    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;
}

void
ControlPort::dump()
{
    msg("ControlPort port %d\n", nr);
    msg("------------------\n");
    msg("Button:  %s AxisX: %d AxisY: %d\n", button ? "YES" : "NO", axisX, axisY);
    msg("Bitmask: %02X\n", bitmask());
}

void
ControlPort::scheduleNextShot()
{
    nextAutofireFrame = c64->frame +
    (int)(c64->vic.getFramesPerSecond() / (2 * autofireFrequency));
}

void
ControlPort::execute()
{
    if (!autofire || autofireFrequency <= 0.0)
        return;
  
    // Wait until it's time to push or release fire
    if (c64->frame != nextAutofireFrame)
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

uint8_t
ControlPort::bitmask() {
    
    uint8_t result = 0xFF;
    
    if (axisY == -1) CLR_BIT(result, 0);
    if (axisY ==  1) CLR_BIT(result, 1);
    if (axisX == -1) CLR_BIT(result, 2);
    if (axisX ==  1) CLR_BIT(result, 3);
    if (button)      CLR_BIT(result, 4);
    
    uint8_t mouseBits = c64->mouse.readControlPort(nr);
    result &= mouseBits;
    
    return result;
}


