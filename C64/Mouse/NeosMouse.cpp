/*!
 * @file        NeosMouse.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

NeosMouse::NeosMouse() {
    
    setDescription("NeosMouse");
    debug(3, "    Creating NeosMouse at address %p...\n", this);
}

NeosMouse::~NeosMouse()
{
}

void
NeosMouse::reset()
{
    VirtualComponent::reset();
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    targetX = 0;
    targetY = 0;
    shiftX = 127;
    shiftY = 127;
    dividerX = 512;
    dividerY = 256;

    state = 0;
    triggerCycle = 0;
    latchedX = 0;
    latchedY = 0;
    deltaX = 0;
    deltaY = 0;
}

void
NeosMouse::setXY(int64_t x, int64_t y)
{
    targetX = x / dividerX;
    targetY = y / dividerY;
    
    // Sync mouse coords with target coords if more than 8 shifts would
    // be needed to reach target coords
    if (abs(targetX - mouseX) / 8 > shiftX) {
        mouseX = targetX;
    }
    if (abs(targetY - mouseY) / 8 > shiftY) {
        mouseY = targetY;
    }
}

uint8_t
NeosMouse::readControlPort()
{
    uint8_t result = leftButton ? 0xE0 : 0xF0;
    
    // Check for time out
    if (state != 0 && c64->cpu.cycle > (triggerCycle + 232) /* from VICE */) {
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
NeosMouse::execute()
{
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
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
    triggerCycle = c64->cpu.cycle;
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
    triggerCycle = c64->cpu.cycle;
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

