/*!
 * @header      NeosMouse.cpp
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

NeosMouse::NeosMouse() {
    
    setDescription("NeosMouse");
    debug(3, "    Creating NeosMouse at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &state,           sizeof(state),          CLEAR_ON_RESET },
        { &triggerCycle,    sizeof(triggerCycle),   CLEAR_ON_RESET },
        { &mouseX,          sizeof(mouseX),         CLEAR_ON_RESET },
        { &mouseY,          sizeof(mouseY),         CLEAR_ON_RESET },
        { &latchedX,        sizeof(latchedX),       CLEAR_ON_RESET },
        { &latchedY,        sizeof(latchedY),       CLEAR_ON_RESET },
        { &targetX,         sizeof(targetX),        CLEAR_ON_RESET },
        { &targetY,         sizeof(targetY),        CLEAR_ON_RESET },
        { &deltaX,          sizeof(deltaX),         CLEAR_ON_RESET },
        { &deltaY,          sizeof(deltaY),         CLEAR_ON_RESET },
        { &leftButton,      sizeof(leftButton),     CLEAR_ON_RESET },
        { &rightButton,     sizeof(rightButton),    CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

NeosMouse::~NeosMouse()
{
}

void
NeosMouse::reset()
{
    VirtualComponent::reset();
    Mouse::reset(); 
    shiftX = 127;
    shiftY = 127;
}

void
NeosMouse::setXY(int64_t x, int64_t y)
{
    Mouse::setXY(x / 2, y);
}

uint8_t
NeosMouse::readControlPort()
{
    uint8_t result = leftButton ? 0xE0 : 0xF0;
    
    // Check for time out
    if (state != 0 && c64->cycle > (triggerCycle + 232) /* from VICE */) {
        state = 0;
        latchPosition();
    }
    
    switch (state) {
            
        case 0: // Transmit X_HIGH
            result |= (((uint8_t)deltaX >> 4) & 0x0F);
            break;
            
        case 1: // Transmit X_LOW
            result |= ((uint8_t)deltaX & 0x0F);
            break;
            
        case 2: // Transmit Y_HIGH
            result |= (((uint8_t)deltaY >> 4) & 0x0F);
            break;
            
        case 3: // Transmit Y_LOW
            result |= ((uint8_t)deltaY & 0x0F);
            break;
            
        default:
            assert(false);
    }
    
    return result;
}

void
NeosMouse::risingStrobe(int portNr)
{
    // Perform rising edge state changes
    switch (state) {
        case 0:  /* X_HIGH -> X_LOW */
            state = 1;
            break;
            
        case 2: /* Y_HIGH -> Y_LOW */
            state = 3;
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = c64->cycle;
}

void
NeosMouse::fallingStrobe(int portNr)
{
    // Perform falling edge state changes
    switch (state) {
        case 1:  /* X_LOW -> Y_HIGH */
            state = 2;
            break;
            
        case 3: /* Y_LOW -> X_HIGH */
            state = 0;
            latchPosition();
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = c64->cycle;
}

void
NeosMouse::latchPosition()
{
    int64_t dx = MAX(MIN((latchedX - mouseX), 127), -128);
    int64_t dy = MAX(MIN((mouseY - latchedY), 127), -128);
    
    deltaX = (uint8_t)dx;
    deltaY = (uint8_t)dy;
    
    latchedX = mouseX;
    latchedY = mouseY;
}


