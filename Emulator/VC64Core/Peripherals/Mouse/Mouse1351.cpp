// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Mouse1351.h"
#include "C64.h"

#include <algorithm>

namespace vc64 {

void
Mouse1351::_didReset(bool hard)
{
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
}

u8
Mouse1351::readPotX() const
{
    return mouseXBits();
}

u8
Mouse1351::readPotY() const
{
    return mouseYBits();
}

u8
Mouse1351::readControlPort() const
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
    if (targetX < mouseX) mouseX -= std::min(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += std::min(targetX - mouseX, shiftX);
}

void
Mouse1351::executeY(i64 targetY)
{
    targetY /= dividerY;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetY - mouseY) / 8 > shiftY) mouseY = targetY;
    
    // Move mouse coordinates towards target coordinates
    if (targetY < mouseY) mouseY -= std::min(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += std::min(targetY - mouseY, shiftY);
}

}
