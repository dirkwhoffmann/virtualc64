// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"
#include "Mouse1351.h"

Mouse1351::Mouse1351() {
    
    setDescription("Mouse1351");
}

Mouse1351::~Mouse1351()
{
}

void
Mouse1351::reset()
{
    HardwareComponent::reset();
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
}

u8
Mouse1351::readPotX()
{
    return mouseXBits();
}

u8
Mouse1351::readPotY()
{
    return mouseYBits();
}

u8
Mouse1351::readControlPort()
{
    u8 result = 0xFF;
    
    if (leftButton) CLR_BIT(result, 4);
    if (rightButton) CLR_BIT(result, 0);

    return result;
}

void
Mouse1351::executeX(i64 targetX)
{
    targetX /= dividerX;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetX - mouseX) / 8 > shiftX) mouseX = targetX;
    
    // Move mouse coordinates towards target coordinates
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
}

void
Mouse1351::executeY(i64 targetY)
{
    targetY /= dividerY;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetY - mouseY) / 8 > shiftY) mouseY = targetY;
    
    // Move mouse coordinates towards target coordinates
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
}
